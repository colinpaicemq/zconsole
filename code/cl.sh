touch /u/tmp/console/console.c
rm  CEE*
as          -d cpwto.o cpwto.s
as -a       -d qedit.o qedit.s 1> qedit.lst
export _C99_CCMODE=1
python3 setup3.py build bdist_wheel 1>a 2>b
cp ./build/lib.os390-27.00-1090-3.8/console/zconsole.so   .
oedit a b
#python3 cons.*
#oedit CEE*
