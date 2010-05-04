/**
 * Autor: Oswaldo Bassani Neto
 *
 * Interface de metodos para o Java por meio do JNI.
 *
 */

/* Libs C++ */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// #include <iostream>

/* Libs libdc1394 */
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>

#include "StereoFireWireCamera_App.h"

/* Interface Java / JNI */
#include "br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera.h"

JNIEXPORT jbyteArray JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_getFrame
(JNIEnv *env, jobject obj) {
	jbyteArray jb;
	jb = env->NewByteArray (640 * 480 * 3);
	getFrame();
	env->SetByteArrayRegion (jb, 0, 640 * 480 * 3, (jbyte*)getRgbBuffer_left());
	return (jb);
}


JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_cleanUp  (JNIEnv *env, jobject obj) {
	cleanup();
}

JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_start (JNIEnv *env, jobject obj) {
	start();
	return JNI_TRUE; /* Everything is okay */
}

JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_stop (JNIEnv *env, jobject obj) {
	stop();
	return (JNI_TRUE);
}

JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_setCaptureFormat (JNIEnv *env, jobject obj, jint w, jint h, jint fps, jint colorSpace) {
	fprintf( stderr, "Unable to set properties, not implemented!!!\n\n");
	return;
}

JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_model (JNIEnv *env, jobject obj) {
	model();
	return env->NewStringUTF ("");
}

JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_vendor (JNIEnv *env, jobject objx) {
	vendor();
	return env->NewStringUTF ("");
}

JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_configure (JNIEnv *env, jobject obj) {
	configure();
}

