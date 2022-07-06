import setuptools
from setuptools import setup, Extension
import sysconfig
import os
os.environ['_C89_CCMODE'] = '1'
#
# This script needs    export _C89_CCMODE=1
# Otherwise you get FSUM3008  messages
version = '1.0.0'
setup(name = 'console',
    version = version,
    description = 'z/OS console interface. Put, and respond to modify and stop request',
    long_description= 'provide interface to z/OS console',
    author='Colin Paice',
    author_email='colinpaice3@gmail.com',
#   url='https://dsuch.github.io/pymqi/',
#   download_url='https://pypi.python.org/pypi/pymqi',
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
        'Topic :: Software Development :: Object Brokering',
        ],
#   ext_modules = [Extension('console.zconsole',['console.c','cpwto.s','qedit.s'],
    ext_modules = [Extension('console.zconsole',['console.c'],
        include_dirs=["//'COLIN.MQ930.SCSQC370'","."],
        extra_compile_args=["-Wc,ASM,SHOWINC,ASMLIB(//'SYS1.MACLIB'),LIST(c.lst),SOURCE,WARN64,XREF","-Wa,LIST,RENT"],
        extra_link_args=["-Wl,LIST,MAP,DLL","/u/tmp/console/cpwto.o","/u/tmp/console/qedit.o",
        "/u/tmp/console/qeditw.o"],
        )]
    )
#       extra_link_args=["-Wl,LIST,MAP,DLL"],
#       extra_link_args=["-Wl,LIST,MAP,DLL","./cpwto.o"],
