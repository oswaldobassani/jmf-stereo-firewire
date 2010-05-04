#!/bin/bash

#
# Author: Oswaldo Bassani
#

# -- Error Vars -- 

ERROR_PACOTE_FALTANDO=1
ERROR_LINUX_VERSION=2
# - Outros Erros -
ERROR_COMPILACAO=3
ERROR_LINK=4
ERROR_99=99
EXEC_OK=0

# -- Utility Functions -- 

check_linux_distribution()
{
	# Function.
	cat /etc/*-release | grep -s "Ubuntu" >/dev/null
	if [[ $? > 0 ]] ; then
		echo "(ptBR) Favor instalar o Ubuntu como sistema operacional"
		exit $ERROR_LINUX_VERSION
	else
		echo "(ptBR) Detectado 'Ubuntu'"
		cat /etc/*-release | grep -s "DISTRIB_RELEASE" | awk -F = '{print "Ubuntu version: " $2}'
		# Detalhes podem ser obtidos pelo comando: 'lsb_release -a'
		return
	fi
}

check_package()
{
	# Function.
	# Parameter 1 is the package name
	dpkg --get-selections | grep -s ${1} >/dev/null
	if [[ $? > 0 ]] ; then
		echo "(ptBR) Favor instalar o pacote: '${1}'"
		exit $ERROR_PACOTE_FALTANDO
	else
		echo "(ptBR) Pacote '${1}' instalado."
		return
	fi
}

# -- Check Operational System / Distribution -- 

check_linux_distribution

# (ptBR) -- Pacotes do Ubuntu a serem verificados -- 

# Ubuntu 9.04
# libdc1394-22-dev (new/default)
# libdc1394-13-dev (old)

# Java 5
# (ptBR) Evita erros de swing/awt.

pacotes="libraw1394-dev libdc1394-13-dev sun-java5-jdk"
for pacote in $pacotes; do
	check_package $pacote
done

#
# JAVA Vars
#
echo "Please! Update Java Home var if necessary!"
JAVA_HOME=/usr/lib/jvm/java-1.5.0-sun
JAVA_INCLUDE_DIR=$JAVA_HOME/include
JAVA_INCLUDE_LINUX_DIR=$JAVA_HOME/include/linux

# Clean
echo "Remove binary apps and libraries"
rm -f lib/libFireWireCamera.so
rm -f lib/libStereoFireWireCamera.so
rm -f lib/libRealStereoFireWireCamera.so
rm -f bin/stereoApp

# Build
echo "Compiling projects"

# Codigo em C++
echo " ... libFireWireCamera ..."
g++ -Wall -o lib/libFireWireCamera.so -shared -Wl,-soname,libFireWireCamera.so -Iinclude -Isource -I$JAVA_INCLUDE_DIR  -I$JAVA_INCLUDE_LINUX_DIR source/FireWireCamera.cpp -static -lc -lstdc++ -lraw1394 -ldc1394_control
echo " ... libFireWireCamera ... OK"

echo " ... stereoApp ..."
g++ -Wall -o bin/stereoApp -I$JAVA_INCLUDE_DIR  -I$JAVA_INCLUDE_LINUX_DIR -Iinclude -Iinclude/conversions -Isource -Isource/conversions source/StereoFireWireCamera_App.cpp source/conversions/conversions.cpp -static -lc -lstdc++ -lraw1394 -ldc1394_control
echo " ... stereoApp ... OK"

echo " ... libStereoFireWireCamera ..."
g++ -Wall -o lib/libStereoFireWireCamera.so -shared -Wl,-soname,libStereoFireWireCamera.so -I$JAVA_INCLUDE_DIR  -I$JAVA_INCLUDE_LINUX_DIR -Iinclude -Iinclude/conversions -Isource -Isource/conversions source/StereoFireWireCamera.cpp source/StereoFireWireCamera_App.cpp source/conversions/conversions.cpp -static -lc -lstdc++ -lraw1394 -ldc1394_control
echo " ... libStereoFireWireCamera ... OK"

echo " ... libRealStereoFireWireCamera ..."
g++ -Wall -o lib/libRealStereoFireWireCamera.so -shared -Wl,-soname,libRealStereoFireWireCamera.so -I$JAVA_INCLUDE_DIR  -I$JAVA_INCLUDE_LINUX_DIR -Iinclude -Iinclude/conversions -Isource -Isource/conversions  source/RealStereoFireWireCamera.cpp source/StereoFireWireCamera_App.cpp source/conversions/conversions.cpp -static -lc -lstdc++ -lraw1394 -ldc1394_control
echo " ... libRealStereoFireWireCamera ... OK"

# The END

