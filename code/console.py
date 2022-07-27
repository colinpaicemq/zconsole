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
# This class is used for zconsole as it can use atexit.register
# to make sure the cancel routine is called to stop the outstanding
# QEDIT wait

import zconsole as zconsole
import atexit   as atexit
class console:
    handle = None
#   def __init__(self):
#      print("console.__init__",a)

    def put(self, a, desc=0, route=0 ):
       zconsole.put(a)

    def console2(self, a, desc=0, route=0):
        zconsole.console2(a)

    def cb(self,a,b):
 #      print("In cb....")
        self.handle =  zconsole.acb(a,b)
        atexit.register(self.cleanup,self.handle)

    def cleanup(self,handle):
        if handle != None:
           zconsole.cancel(self.handle)
