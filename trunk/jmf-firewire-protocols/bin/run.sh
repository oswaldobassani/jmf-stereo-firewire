#!/bin/bash

#
# Author: Oswaldo Bassani Neto
#

#
# - Links -
# http://java.sun.com/javase/technologies/desktop/media/jmf/2.1.1/apidocs/
#

usage ()
{
    echo ""
    echo "Parametros:"
    echo "    [-f] [-s] [-r]"
    echo "      f - One Firewire Camera Capture protocol"
    echo "      s - Stereo protocol"
    echo "      r - Real Stereo protocol"
    echo "        - Run jmf registry util for new protocols"
    echo ""
    exit 0
}

single_camera="false"
stereo_camera="false"
realstereo_camera="false"

while getopts "fsrh\?" options; do
  case $options in
    f ) single_camera="true";;
    s ) stereo_camera="true";;
    r ) realstereo_camera="true";;
    h ) usage;;
    \? ) usage;;
    * ) usage;;
  esac
done

if [ $single_camera = 'true' ]; then
    JMF_PROTOCOL="fwc://"
fi

if [ $stereo_camera = 'true' ]; then
    JMF_PROTOCOL="sfwc://"
fi

if [ $realstereo_camera = 'true' ]; then
    JMF_PROTOCOL="rsfwc://"
fi

REG_CLASS=br.ufabc.bassani.jmf.firewire.util.FWCUtilReg

# - JMF Classes -
# JMFInit : Find audio/video devices
# JMFRegistry : Check codecs/plugins/etc

# - Linux extra param -
# EXTRA_PARAM=-Dawt.toolkit=sun.awt.motif.MToolkit
EXTRA_PARAM
#

JAVA_HOME=/usr/lib/jvm/java-6-sun
JAVA_SO_DIR=$JAVA_HOME/jre/lib/i386

LIB_HOME=../LIBs
JMF_HOME=$LIB_HOME/JMF-2.1.1e/lib

CLASS_PATH=$CLASS_PATH:$JMF_HOME:$JMF_HOME/jmf.jar:$JMF_HOME/mediaplayer.jar:$JMF_HOME/multiplayer.jar
CLASS_PATH=$CLASS_PATH:classes

LD_PRELOAD_LIBRARY=$LD_PRELOAD_LIBRARY:$JAVA_SO_DIR:src/native/lib/.:/usr/lib:$JMF_HOME
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JAVA_SO_DIR:src/native/lib/.:/usr/lib:$JMF_HOME

export LD_PRELOAD_LIBRARY
export LD_LIBRARY_PATH

if [ $JMF_PROTOCOL ]; then
    sudo java -classpath $CLASS_PATH -Djava.library.path=$LD_LIBRARY_PATH $EXTRA_PARAM JMStudio $JMF_PROTOCOL
else
    java -classpath $CLASS_PATH -Djava.library.path=$LD_LIBRARY_PATH $EXTRA_PARAM $REG_CLASS
fi

