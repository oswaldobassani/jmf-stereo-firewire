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
#include "br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera.h"

//int init = 0;
//jbyteArray jb;

JNIEXPORT jbyteArray JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_getFrame
(JNIEnv *env, jobject obj) {
	jbyteArray jb;
	//if (init==0)
	//{
		jb = env->NewByteArray (640 * 480 * 3 *2);
	//	init = 1;
	//}
	getFrame();
	env->SetByteArrayRegion (jb, 0, 640 * 480 * 3, (jbyte*)getRgbBuffer_left());
	//env->SetByteArrayRegion (jb, 0, 640 * 480 * 3 * 2, (jbyte*)getRgbBuffer_left_right());
	env->SetByteArrayRegion (jb, 640 * 480 * 3, 640 * 480 * 3, (jbyte*)getRgbBuffer_right());
	return (jb);
}


JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_cleanUp  (JNIEnv *env, jobject obj) {
	cleanup();
}

JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_start (JNIEnv *env, jobject obj) {
	start();
	return JNI_TRUE; /* Everything is okay */
}

JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_stop (JNIEnv *env, jobject obj) {
	stop();
	return (JNI_TRUE);
}

JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_setCaptureFormat (JNIEnv *env, jobject obj, jint w, jint h, jint fps, jint colorSpace) {
	fprintf( stderr, "Unable to set properties, not implemented!!!\n\n");
	return;
}

JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_model (JNIEnv *env, jobject obj) {
	model();
	return env->NewStringUTF ("");
}

JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_vendor (JNIEnv *env, jobject objx) {
	vendor();
	return env->NewStringUTF ("");
}

JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_configure (JNIEnv *env, jobject obj) {
	configure();
}

