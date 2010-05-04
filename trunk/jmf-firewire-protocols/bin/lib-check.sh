#!/bin/bash

LIB_DIR=src/native/lib

check_lib()
{
	# Function.
	# Parameter 1 is the so file name
	echo "Checking .so: '${1}'"
	nm -Ca ${1}.so | grep Java_br
}

cd $LIB_DIR
check_lib libFireWireCamera
check_lib libStereoFireWireCamera
check_lib libRealStereoFireWireCamera
cd -

