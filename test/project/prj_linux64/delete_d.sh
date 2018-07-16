#!bin/sh

basepath=$(cd `dirname $0`; pwd)
cd ${basepath}

sh ../../../project/prj_linux64/delete_d.sh

make clean

