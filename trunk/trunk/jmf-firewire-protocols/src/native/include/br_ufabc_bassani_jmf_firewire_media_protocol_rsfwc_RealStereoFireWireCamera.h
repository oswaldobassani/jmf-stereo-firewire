/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera */

#ifndef _Included_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
#define _Included_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: frameRate */
/* Inaccessible static: width */
/* Inaccessible static: height */
/* Inaccessible static: format */
/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
 * Method:    vendor
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_vendor
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
 * Method:    model
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_model
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
 * Method:    configure
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_configure
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
 * Method:    setCaptureFormat
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_setCaptureFormat
  (JNIEnv *, jobject, jint, jint, jint, jint);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
 * Method:    getFrame
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_getFrame__
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
 * Method:    getFrame
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_getFrame___3B
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
 * Method:    start
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_start
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
 * Method:    stop
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_stop
  (JNIEnv *, jobject);

/*
 * Class:     br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera
 * Method:    cleanUp
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_rsfwc_RealStereoFireWireCamera_cleanUp
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
