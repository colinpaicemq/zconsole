import setuptools
from setuptools import setup, Extension
import sysconfig
import os
os.environ['_C89_CCMODE'] = '1'
from setuptools.command.build_ext import build_ext
from setuptools import setup
version = '1.0.0'
# our override code to do the compile
class BuildExt(build_ext):
   def build_extensions(self):
     print(self.compiler.compiler_so)
     if '-fno-strict-aliasing' in self.compiler.compiler_so:
       self.compiler.compiler_so.remove('-fno-strict-aliasing')
     if '-Wa,xplink' in self.compiler.compiler_so:
        self.compiler.compiler_so.remove('-Wa,xplink')
     if '-D_UNIX03_THREADS' in self.compiler.compiler_so:
        self.compiler.compiler_so.remove('-D_UNIX03_THREADS')
     super().build_extensions()
os.environ['_C89_CCMODE'] = '1'
#
# This script needs    export _C89_CCMODE=1
# Otherwise you get FSUM3008  messages

setup(name = 'console',
    version = version,
    description = 'z/OS console interface. Put, and respond to modify and stop request',
    long_description= 'provide interface to z/OS console',
    author='Colin Paice',
    author_email='colinpaice3@gmail.com',
    platforms='z/OS',
    package_dir = {'': '.'},
    packages = ['console'],
    license='Python Software Foundation License',
    keywords=('z/OS console modify stop'),
    python_requires='>=3',
    classifiers = [
        'Development Status :: 4 - Beta',
        'License :: OSI Approved :: Python Software Foundation License',
        'Intended Audience :: Developers',
        'Natural Language :: English',
        'Operating System :: OS Independent',
        'Programming Language :: C',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries :: Python Modules',
        ],  # classifiers
        cmdclass = {'build_ext': BuildExt}, # invoke our code
    ext_modules = [Extension('console.zconsole',['console.c'],
        include_dirs=["//'COLIN.MQ930.SCSQC370'","."],
        extra_compile_args=["-Wc,ASM,SHOWINC,ASMLIB(//'SYS1.MACLIB')",
              "-Wc,LIST(c.lst),SOURCE,NOWARN64,XREF","-Wa,LIST,RENT"],
        extra_link_args=["-Wl,LIST,MAP,DLL","/u/tmp/console/qedit.o"
        ],
        )]
    )
#                                           "/u/tmp/console/cpwto.o",
#       extra_link_args=["-Wl,LIST,MAP,DLL","/u/tmp/console/cpwto.o","/u/tmp/console/qedit.o",
#       "/u/tmp/console/qeditw.o"],
#   ext_modules = [Extension('console.zconsole',['console.c','cpwto.s','qedit.s'],
#       extra_link_args=["-Wl,LIST,MAP,DLL"],
#       extra_link_args=["-Wl,LIST,MAP,DLL","./cpwto.o"],
# tup(... cmdclass = {'build_ext': BuildExt}, ext_modules=[Extension(...)] ...)
#       "/u/tmp/console/qeditw.o"],
