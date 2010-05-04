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

/* Interface Java / JNI */
#include "br_ufabc_bassani_jmf_firewire_media_protocol_fwc_FireWireCamera.h"

dc1394_cameracapture camera;
int numNodes;
int numCameras;
raw1394handle_t handle;
nodeid_t * camera_nodes;
dc1394_feature_set features;
dc1394_camerainfo info;
dc1394_miscinfo *misc_info;
quadlet_t supported_framerates;

// Bassani: As propriedades neste momento sao fixas.
// Formatos: FORMAT_VGA_NONCOMPRESSED, ...
int format = FORMAT_VGA_NONCOMPRESSED;
// Modos: MODE_320x240_YUV422, MODE_640x480_YUV422, MODE_640x480_MONO, ...
int mode = MODE_640x480_MONO;
// Bytes per Pixel
int bytesPerPixel = 1;
// Tamanhos: 320x240, 640x480, ...
int width = 640;
int height = 480;
// Framerate: esta sendo obtido durante a conexao com a camera.
int framerate;

// Transmition Speed = 400 Mbps
#define TRANSM_SPEED SPEED_400
#define NUM_BUFFERS 8
#define DROP_FRAMES 1

#define USE_CARD_1
#ifdef USE_CARD_0
	int nCard = 0;
	/* Ubuntu 9.04 */
	/*  - Bus (OnBoard se presente) */
	const char* device = "/dev/video1394-0";
	/* Ubuntu 8.04 */
	//const char* device = "/dev/video1394/0";
#endif
#ifdef USE_CARD_1
	int nCard = 1;
	/* Ubuntu 9.04 */
	/*  - Bus (extra) */
	const char* device = "/dev/video1394-1";
	/* Ubuntu 8.04 */
	//const char* device = "/dev/video1394/1";
#endif

JNIEXPORT jbyteArray JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_fwc_FireWireCamera_getFrame
(JNIEnv *env, jobject obj) {
	jbyteArray jb;
	jb = env->NewByteArray (camera.frame_height * camera.frame_width * 2);
	if (dc1394_dma_single_capture (&camera) != DC1394_SUCCESS) {
		printf ("Error at dc1394_dma_single_capture\n");
	}
	env->SetByteArrayRegion (jb, 0, camera.frame_height * camera.frame_width * bytesPerPixel, (jbyte*)camera.capture_buffer);
	dc1394_dma_done_with_buffer(&camera);
	return (jb);
}


JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_fwc_FireWireCamera_cleanUp  (JNIEnv *env, jobject obj) {
	dc1394_dma_unlisten(handle, &camera);
	dc1394_dma_release_camera(handle, &camera);
	dc1394_destroy_handle(handle);
}

JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_fwc_FireWireCamera_start (JNIEnv *env, jobject obj) {
	if (dc1394_start_iso_transmission(handle, camera.node) != DC1394_SUCCESS)
	{
		fprintf( stderr, "unable to start camera iso transmission\n");
		dc1394_dma_release_camera(handle,&camera);
		dc1394_destroy_handle(handle);
		return JNI_FALSE;
	} else {

	}
	return JNI_TRUE; /* Everything is okay */
}

JNIEXPORT jboolean JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_fwc_FireWireCamera_stop (JNIEnv *env, jobject obj) {
	if (dc1394_stop_iso_transmission (handle,camera.node) != DC1394_SUCCESS) {
		printf ("Error in Java_FireWireCamera_stop\n");
		return (JNI_FALSE);
	}
	return (JNI_TRUE);
}

JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_fwc_FireWireCamera_setCaptureFormat (JNIEnv *env, jobject obj, jint w, jint h, jint fps, jint colorSpace) {
	fprintf( stderr, "Unable to set properties, not implemented!!!\n\n");
	return;
}

JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_fwc_FireWireCamera_model (JNIEnv *env, jobject obj) {
	return env->NewStringUTF (info.model);
}

JNIEXPORT jstring JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_fwc_FireWireCamera_vendor (JNIEnv *env, jobject objx) {
	return env->NewStringUTF (info.vendor);
}

JNIEXPORT void JNICALL Java_br_ufabc_bassani_jmf_firewire_media_protocol_fwc_FireWireCamera_configure (JNIEnv *env, jobject obj) {
	int i = 0;
	handle = dc1394_create_handle(nCard);
	if (handle==NULL)
	{
		fprintf( stderr, "Unable to aquire a raw1394 handle\n\n"
				"Please check \n"
				"  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
				"  - if you have read/write access to /dev/raw1394\n\n");
	}
	numNodes = raw1394_get_nodecount(handle);
	camera_nodes = dc1394_get_camera_nodes(handle, &numCameras, 0);
	fflush(stdout);
	if (numCameras<1)
	{
		fprintf( stderr, "no cameras found :(\n");
		dc1394_destroy_handle(handle);
	}
	printf("working with the first camera on the bus\n");
	for (i=0; i<numCameras; i++) {
		if( camera_nodes[i] == numNodes-1)
		{
			fprintf( stderr, "\n"
					"Sorry, your camera is the highest numbered node\n"
					"of the bus, and has therefore become the root node.\n"
					"The root node is responsible for maintaining \n"
					"the timing of isochronous transactions on the IEEE \n"
					"1394 bus.  However, if the root node is not cycle master \n"
					"capable (it doesn't have to be), then isochronous \n"
					"transactions will not work.  The host controller card is \n"
					"cycle master capable, however, most cameras are not.\n"
					"\n"
					"The quick solution is to add the parameter \n"
					"attempt_root=1 when loading the OHCI driver as a \n"
					"module.  So please do (as root):\n"
					"\n"
					"   rmmod ohci1394\n"
					"   insmod ohci1394 attempt_root=1\n"
					"\n"
					"for more information see the FAQ at \n"
					"http://linux1394.sourceforge.net/faq.html#DCbusmgmt\n"
					"\n");
		}
	}

	if (dc1394_query_supported_framerates(handle, camera_nodes[0],
			format,
			mode,
			&supported_framerates) != DC1394_SUCCESS) {
		fprintf(stderr, "dc1394_query_supported_framerates() failed.");
	}

	if (supported_framerates & (1U << (31-5)))
		framerate = FRAMERATE_60;
	else if (supported_framerates & (1U << (31-4)))
		framerate = FRAMERATE_30;
	else if (supported_framerates & (1U << (31-3)))
		framerate = FRAMERATE_15;
	else if (supported_framerates & (1U << (31-2)))
		framerate = FRAMERATE_7_5;
	else if (supported_framerates & (1U << (31-1)))
		framerate = FRAMERATE_3_75;
	else if (supported_framerates & (1U << (31-0)))
		framerate = FRAMERATE_1_875;
	else
		fprintf(stderr, "Couldn't figure out the maximum framerate.\n");

	fprintf(stdout, "Frame Rate 'FRAMERATE_30' = %d.\n", FRAMERATE_30);
	fprintf(stdout, "Frame Rate 'FRAMERATE_15' = %d.\n", FRAMERATE_15);
	fprintf(stdout, "Frame Rate selecionado %d.\n", framerate);

	int channel = 0;

	// Bassani: modificado NUM_BUFFERS >> '4' -> '8'
	if (dc1394_dma_setup_capture(handle,camera_nodes[0],
			channel,
			format,
			mode,
			TRANSM_SPEED,
			framerate, NUM_BUFFERS, DROP_FRAMES, device,
			&camera)!=DC1394_SUCCESS)
	{
		fprintf( stderr,"unable to setup camera-\n"
						"check line %d of %s to make sure\n"
						"that the video mode,framerate and format are\n"
						"supported by your camera\n",
						__LINE__, __FILE__);
		dc1394_dma_release_camera(handle, &camera);
		dc1394_destroy_handle(handle);
	}

}
