#   example use of zconsole
# ////////////////////////////////////////////////////////////////////////
# MIT License
#
# Copyright (c) 2022 Colin Paice
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# ================================================================
#
# This is example code to show the use of putting to the console
# and also waiting for operator requests, start, modify and stop
#
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
###############################################
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
a.cb(ccp,[exit_flag])

x  = zconsole.taskinfo()
print("MAIN taskinfo ",x)
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
#console.cancel(r)
