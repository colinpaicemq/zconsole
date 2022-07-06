# zconsole
An interface to intercept operator commands START, STOP,MODFY for Python programs on z/OS, and to write to the operator console

## Write to the console

    import zconsole as zconsole 
    zconsole.put("This gets written to the z/OS console") 

## Wait for an operator command
An operator can issue commands like

1.  s PYT,,,'COLINs Data',p=CONSNEW 
1.  STOP  PYT
1.  F PYT,'pass a parameter while running'

To use this you need

    import zconsole as zconsole 
    h  = zconsole.init() 
    yy = zconsole.get(h)
    ...
    zconsole.kill(h)
    
This will suspend the thread until the operator issues a Stop or Modify request.
See below for typical usage

   
This returns data like (for the Start Command)

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
    
## When the zconsole.get is issued it will cause the thread to wait, until the zconsole.kill(handle) or the operator sends a request.

You will want to use this in a ThreadPoolExecutor environment
    
