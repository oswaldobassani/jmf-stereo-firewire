/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera */

#ifndef _Included_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
#define _Included_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: frameRate */
/* Inaccessible static: width */
/* Inaccessible static: height */
/* Inaccessible static: format */
/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
 * Method:    vendor
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_vendor
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
 * Method:    model
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_model
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
 * Method:    configure
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_configure
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
 * Method:    setCaptureFormat
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_setCaptureFormat
  (JNIEnv *, jobject, jint, jint, jint, jint);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
 * Method:    getFrame
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_getFrame__
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
 * Method:    getFrame
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_getFrame___3B
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
 * Method:    start
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_start
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
 * Method:    stop
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_stop
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera
 * Method:    cleanUp
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_sfwc_StereoFireWireCamera_cleanUp
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
