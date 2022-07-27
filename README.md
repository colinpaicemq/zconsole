# zconsole
An interface to intercept operator commands START, STOP,MODFY for Python programs on z/OS, and to write to the operator console

## Simple Write to the console

    import zconsole as zconsole 
    zconsole.put("This gets written to the z/OS console") 
    zconsole.put("This gets written to the z/OS console using descr and route",desc=11,route=2)
    
You can use the C run time _console2 but this wilk prefix your message with BPX.. if the userid is not authrorised to BPX.CONSOLE and 
is not a super userid with uid(0).

    zconsole.console2("This gets written to the z/OS console") 
    zconsole.console2("This gets written to the z/OS console using descr and route",desc=11,route=2)
    

## Wait for an operator command
An operator can issue commands like

1.  s PYT,,,'COLINs Data',p=CONSNEW 
1.  STOP  PYT
1.  F PYT,'pass a parameter while running'

To use this you need

    exit_flag = threading.Event() 
    a =console.console() 
    a.cb(ccp,[exit_flag]) 
    
This will call the callback function ccp passing the parameters within the [].

It uses console.py which has a class defined within it.   This is done, so the class object can use the atexit.register.  This registers a function
to be run at shutdown time.    This function then cancels the asynchronous thread.   If this is not done, then the main thread may just hang, or it may abend, saying an attached subtask is still running.
   
This returns data to the call back funtion like (for the Start Command)

    {'rc': 0, 
     'verb': 'Start', 
     'data': '', 
     'console': 'IBMUSER ', 
     'cart': b'\xc9\xe2\xc6\xdb\xc3\x00\xaf;', 
     'consoleid': b'\x03\x00\x00\x0c\x00\xfb8\xc0', 
     'oconsoleid': b'\x03\x00\x00\x0c\x00\xfb8\xc0'
    } 

and for a modify command 

    {'rc': 0, 'verb': 'Modify', 'data': "'ddd'", ... }                    

and from the stop command

    {'rc': 0, 'verb': 'Stop', 'data': ' ',  ... }
    
# A working example
## PYT started task Procedure
    //         PROC P='cons2' 
    //* 
    //  SET PY='/usr/lpp/IBM/cyp/v3r8/pyz/bin/python3' 
    //  SET PR='/u/tmp/console' 
    //* 
    //STEP1   EXEC PGM=BPXBATSL,REGION=0M,TIME=NOLIMIT,MEMLIMIT=NOLIMIT, 
    //  PARM='PGM &PY  &PR/&P..py' 
    //STDOUT   DD SYSOUT=* 
    //STDERR   DD SYSOUT=* 
    //SYSDUMP  DD SYSOUT=* 
    //CEEDUMP  DD SYSOUT=* 
    //STDIN    DD DUMMY 


Start this  JCL to use cons3.py using

    S PYT,p='cons3'
    
## The cons3.py 
### standard imports 
 
    import zconsole as zconsole 
    import console as  console 
    import atexit   as atexit 
    from datetime import datetime 
    import time 
    from   threading import Timer 
    import sys 
    from threading import Event 
    import threading 
    import concurrent.futures    
    
###  The call back function
    def ccp(args,a   ) : 
          global stop   # set this to 1 to stop 
          global global_counter  # increment this 
          x  = zconsole.taskinfo() 
          print("ccp Task info ",x) 
          e = args[1] # [functionName,[parms]) 
          e = e[0]    # event 
          with threadLock: 
              global_counter += 1 
          print("qedit output",a) # display what we received 
          if a["verb"] == "Stop": 
             stop = 1 
          e.set() # post event - wake up main 
    
 1.  This uses a global flag (stop) to tell the main thread that "STOP" was issued
 1.  It also updates a counter, using a threadLock, to ensure there no timing window
 1.  It prints the output
 
 ### The main task
    stop = 0 
    global_counter = 0 
    print("START",flush=True) 
    threadLock = threading.Lock() 
    exit_flag = threading.Event() 
    a =console.console() 
    a.put("Colins write to console") 
    a.put("Output from put,with desc=11 and route=2",desc=11,route=2) 
    a.console2("Write using __console2") 
    a.console2("Write using __console2 desc=11, rout2=2") 
    # set up the call back - passing the routine, and the threading event
    a.cb(ccp,[exit_flag]) 
                                                                         
 ###  The loop
    print("MAIN taskinfo ",zconsole.taskinfo()) 
    for i in range (0,4 ): 
       exit_flag.wait(timeout=30) 
       # did we time out? 
       if (exit_flag.is_set() == False): 
          break 
       print("POST ",datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f'),flush=True) 
       exit_flag.clear() 
       print("GlobalCounter",global_counter) 
       print("stop",stop) 
       if stop == 1: 
          break 
    print("after stop ",stop) 
    
 ##  The operator commands 
 
 1. s PYT,p='cons3' 
 1. f PYT,2oLIN D3ta
 1. p PYT 
 
 This starts, the Python script as a started task. It is passed some data, it is told to shutdown.
 
 ## The output   
 
    START 
    MAIN taskinfo  {'jobname': 'PYT     ', 'ascb': '0x00fbed00', 'tcb': '0x008d5e88', 'tcbttime': 0} 
    ccp Task info  {'jobname': 'PYT     ', 'ascb': '0x00fbed00', 'tcb': '0x008d5cf0', 'tcbttime': 0} 
    
    qedit output {'rc': 0, 'verb': 'Start', 'data': '', 'console': 'IBMUSER ', 'cart': ...
    
    POST  2022-07-27 10:00:42.631067 
    GlobalCounter 1 
    stop 0 
    ccp Task info  {'jobname': 'PYT     ', 'ascb': '0x00fbed00', 'tcb': '0x008d5cf0', 'tcbttime': 0} 
    
    qedit output {'rc': 0, 'verb': 'Modify', 'data': '2OLIN D3TA', 'console': 'IBMUSER ', 'cart': ...
    
    POST  2022-07-27 10:00:46.886143 
    GlobalCounter 2 
    stop 0 
    ccp Task info  {'jobname': 'PYT     ', 'ascb': '0x00fbed00', 'tcb': '0x008d5cf0', 'tcbttime': 0} 
    
    qedit output {'rc': 0, 'verb': 'Stop', 'data': ' ', 'console': 'IBMUSER ', 'cart': ...
    
    POST  2022-07-27 10:00:50.948860 
    GlobalCounter 3 
    stop 1 
    after stop  1 
 
 
