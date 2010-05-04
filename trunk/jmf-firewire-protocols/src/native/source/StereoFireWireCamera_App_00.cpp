/* ===================================================================== *
 * ---------------------- PROGRAMA DE CAPTURA -------------------------  *
 * ===================================================================== *
 *  Multiplas cameras usando libdc1394                                   *
 *  Autor: Oswaldo Bassani Neto                                          *
 *  Baseado em: Samuel Perfidio                                          *
 *-----------------------------------------------------------------------*/

/* Libs C++  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

/* Libs libdc1394  */
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>

/* programas auxiliares  */
#include "conversions.h"

using std::cout;
using std::cin;

#define CAMERA_640x480_8BIT
#ifdef CAMERA_640x480_8BIT
	int format = FORMAT_VGA_NONCOMPRESSED;
	int mode = MODE_640x480_MONO;
	int frameRate = FRAMERATE_30;
	int bytesPerPixel = 1;
#endif

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

#define DROP_FRAMES 1
#define MAX_CAMERAS 8
#define NUM_BUFFERS 8

/* declarations for libdc1394 */
int numCameras = 2;
int numNodes;

unsigned int channel;
unsigned int speed;

raw1394handle_t	raw1394Handle_e;
raw1394handle_t raw1394Handle[MAX_CAMERAS];
dc1394_cameracapture dc1394Camera[MAX_CAMERAS];

nodeid_t *camera_nodes;
dc1394_feature_set features;

int bufferSize = 307200; // 640 x 480

/* declarations for conversions */
quadlet_t qValue;
bayer_pattern_t pattern;

/* declarations for file */
const char* saveFileName_left = "image_left.ppm";
const char* saveFileName_right = "image_right.ppm";

FILE* imagefile_left;
FILE* imagefile_right;

/* ========================== vendor() ================================= */
void vendor(void)
{
	printf( "vendor...\n" );
}

/* ========================== model() ================================== */
void model(void)
{
	printf( "model...\n" );
}

/* ========================== setCaptureFormat() ======================= */
void setCaptureFormat(void)
{
	printf( "setCaptureFormat...\n" );
	printf( "Unable to set properties, not implemented!!!\n\n" );
}

/* ========================== configure() ============================== */
void configure(void)
{
	printf( "configure...\n" );
}

/* ========================== start() ================================== */
void start(void)
{
	printf( "start...\n" );
}

/* ========================== getFrame() =============================== */
void getFrame(void)
{
	printf( "getFrame...\n" );
}

/* ========================== stop() =================================== */
void stop(void)
{
	printf( "stop...\n" );
}

/* ========================== cleanup() ================================ *
 *  called when the program exits and                                    *
 *  destroys the existing connections to the 1394 drivers                *
 *-----------------------------------------------------------------------*/
void cleanup(void)
{
	printf( "cleanup...\n" );
	for ( int i=0; i < numCameras; i++ )
	{
		dc1394_dma_unlisten( raw1394Handle[i], &dc1394Camera[i] );
		dc1394_dma_release_camera( raw1394Handle[i], &dc1394Camera[i] );
		dc1394_destroy_handle( raw1394Handle[i] );
	}
}


/* =========================================================================================================================== *
 *                                                  main                                                                       *
 *-----------------------------------------------------------------------------------------------------------------------------*/
int main( int argc, char *argv[] )
{
	/*---------------------------------------------------------------------- *
	 *  Open ohci and get dc1394 handle                                      *
	 *-----------------------------------------------------------------------*/
	printf( "create_handle...\n" );
	raw1394Handle_e = dc1394_create_handle( nCard );

	if ( raw1394Handle_e == NULL )
	{
		fprintf( stderr, "Unable to aquire a raw1394 handle\n\n"
				"Please check \n"
				"  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded\n"
				"  - if you have read/write access to /dev/raw1394\n" );
		return 1;
	}

	/*---------------------------------------------------------------------- *
	 *  get the camera nodes and describe them as we find them               *
	 *-----------------------------------------------------------------------*/
	numNodes = raw1394_get_nodecount( raw1394Handle_e );

	printf( "get_camera_nodes...\n" );
	// dc1394_get_camera_nodes
	// This returns the available cameras on the bus.
	// If showCameras is set to 1, a description of the found cameras is printed.
	camera_nodes = dc1394_get_camera_nodes( raw1394Handle_e, &numCameras, 1 );
	printf( "numCameras\n" );
	cout << numCameras ;
	printf( "\n" );

	printf( "destroy_handle...\n" );
	dc1394_destroy_handle( raw1394Handle_e );

	if ( numCameras < 1 )
	{
		fprintf( stderr,
				"No cameras found! (%d nodes on the bus)\n"
				"  - could be you need to try a different 1394 device (modify code to fix)\n",
				numNodes );
		cleanup();
		return 1;
	}

	/* =================================================================== *
	 *                    setup cameras for capture                        *
	 *---------------------------------------------------------------------*/
	printf( "setup cameras for capture...\n" );
	for ( int i = 0; i < numCameras; i++ )
	{

		/*------------------------------------------------------------------*
		 *  Open ohci and get dc1394 handle for each camera                 *
		 *------------------------------------------------------------------*/
		printf( "dc1394_create_handle...\n" );
		raw1394Handle[i] = dc1394_create_handle( nCard );

		if ( raw1394Handle[i]==NULL )
		{
			perror("Unable to aquire a raw1394 handle\n");
			perror("did you load the drivers?\n");
			cleanup();
			return 1;
		}

		dc1394Camera[i].node = camera_nodes[i];

		/*-------------------------------------------------------------------*
		 *  get feature for each camera                                      *
		 *-------------------------------------------------------------------*/
		printf( "get_camera_feature_set...\n" );
		if( dc1394_get_camera_feature_set( raw1394Handle[i],
				dc1394Camera[i].node,
				&features ) !=DC1394_SUCCESS )
		{
			printf("unable to get feature set\n");
		}
		else
		{
			dc1394_print_feature_set( &features );
			printf( "\n" );
		}

		/*-------------------------------------------------------------------*
		 *  get iso_channel_and_speed for each camera                        *
		 *-------------------------------------------------------------------*/
		printf( "get_iso_channel_and_speed...\n" );
		if ( dc1394_get_iso_channel_and_speed( raw1394Handle[i],
				dc1394Camera[i].node,
				&channel,
				&speed ) != DC1394_SUCCESS )
		{
			printf("unable to get the iso channel number\n");
			cleanup();
			return 1;
		}

		/*-------------------------------------------------------------------*
		 *  dma_setup_capture for each camera                                *
		 *-------------------------------------------------------------------*/
		printf( "dma_setup_capture...\n" );
		if ( dc1394_dma_setup_capture( raw1394Handle[i],
				dc1394Camera[i].node,
				// i+1 /*channel*/,
				i /*channel*/,
				format,
				mode,
				SPEED_400,
				frameRate,
				NUM_BUFFERS,
				DROP_FRAMES,
				device,
				&dc1394Camera[i] ) != DC1394_SUCCESS )
		{
			fprintf( stderr,"Unable to setup camera-\n"
					"check code above line %d of %s to make sure\n"
					"that the video mode,framerate and format are\n"
					"supported by your camera\n",
					__LINE__,__FILE__);
			cleanup();
			return 1;
		}

		printf( "setup cameras for capture - step2 - start iso transmission...\n" );
		/* ================================================================= *
		 *                     start iso transmission                        *
		 *-------------------------------------------------------------------*/
		printf( "Starting iso transmission...\n" );
		if ( dc1394_start_iso_transmission( raw1394Handle[i],
				dc1394Camera[i].node ) !=DC1394_SUCCESS )
		{
			perror("unable to start camera iso transmission\n");
			cleanup();
			return 1;
		}
		i++;
	}

	/*----------------------------------------------------------------------*
	 *  free_camera_nodes                                                   *
	 *----------------------------------------------------------------------*/
	printf( "free_camera_nodes...\n" );
	dc1394_free_camera_nodes(camera_nodes);

	if (numCameras < 1)
	{
		perror("no cameras found :(\n");
		cleanup();
		return 1;
	}

	/*---------------------------------------------------------------------*
	 *  criando rbgBuffer						  *
	 *---------------------------------------------------------------------*/

	unsigned char* rgbBuffer_left  = new unsigned char[ 3 * bufferSize ];
	unsigned char* rgbBuffer_right = new unsigned char[ 3 * bufferSize ];

	if ( !rgbBuffer_left )
	{
		fprintf( stderr, "Could not allocate RGB buffer for color conversion (size = %d)\n", 3 * bufferSize );
		cleanup();
		return 1;
	}
	if ( !rgbBuffer_right )
		{
			fprintf( stderr, "Could not allocate RGB buffer for color conversion (size = %d)\n", 3 * bufferSize );
			cleanup();
			return 1;
		}

	/* =================================================================== *
	 *                       capturing frames				  *
	 *---------------------------------------------------------------------*/

	printf( "numCameras\n" );
	cout<< numCameras ;
	printf( "\n" );

	printf( "Capturing frames...\n" );
	if ( dc1394_dma_multi_capture(dc1394Camera, numCameras)!= DC1394_SUCCESS )
	{
		fprintf( stderr, "Unable to capture a frame\n");
		cleanup();
		return 1;
	}

	/*---------------------------------------------------------------------*
	 *  query the camera to determine the Bayer pattern                    *
	 *---------------------------------------------------------------------*/
	printf( "numCameras\n" );
	cout<< numCameras ;
	printf( "\n" );

	printf( "get Bayer pattern ... force ... BAYER_PATTERN_RGGB \n" );
	pattern = BAYER_PATTERN_RGGB;

	unsigned char* src_left = (unsigned char*) dc1394Camera[0].capture_buffer;
	// unsigned char* src_right = (unsigned char*) dc1394Camera[1].capture_buffer;

	/*---------------------------------------------------------------------*
	 *  convert to color image                                             *
	 *---------------------------------------------------------------------*/

	printf( "convert to color image...\n" );
	//Params: imageIn, imageOut, frame_width, frame_height, pattern
	BayerEdgeSense(src_left, rgbBuffer_left, 640, 480, pattern );
	/*
	BayerEdgeSense(src_right, rgbBuffer_right, 640, 480, pattern );
	*/

	/*-----------------------------------------------------------------------
	 *  save image
	 *-----------------------------------------------------------------------*/
	printf( "Saving the image...\n" );

	imagefile_left = fopen( saveFileName_left, "w" );
	if( imagefile_left == NULL)
	{
		perror("");
		fprintf( stderr, "Can't create '%s'\n", saveFileName_left );
		cleanup();
		return 1;
	}
	fprintf( imagefile_left, "P6\n%u %u 255\n", 640, 480 );
	fwrite( rgbBuffer_left, 3, bufferSize, imagefile_left );
	fclose( imagefile_left );
	printf( "saved image to '%s'\n", saveFileName_left );

	/*
	imagefile_right = fopen( saveFileName_right, "w" );
	if( imagefile_right == NULL)
	{
		perror("");
		fprintf( stderr, "Can't create '%s'\n", saveFileName_right );
		cleanup();
		return 1;
	}
	fprintf( imagefile_right, "P6\n%u %u 255\n", 640, 480 );
	fwrite( rgbBuffer_right, 3, bufferSize, imagefile_right );
	fclose( imagefile_right );
	printf( "saved image to '%s'\n", saveFileName_right );
	*/

	/* ==================================================================== *
	 *                         stop data transmission                       *
	 *----------------------------------------------------------------------*/
	for (int i = 0; i < numCameras; i++)
	{
		/*-------------------------------------------------------------------*
		 *  dma_done_with_buffer						  *
		 *-------------------------------------------------------------------*/
		printf( "done_with_buffer %d\n", i );
		dc1394_dma_done_with_buffer(&dc1394Camera[i]);
		/*-------------------------------------------------------------------*
		 * stop iso transmission                                             *
		 *-------------------------------------------------------------------*/
		printf( "stop_iso_transmission %d\n", i );
		if ( dc1394_stop_iso_transmission( raw1394Handle[i],
				dc1394Camera[i].node ) !=DC1394_SUCCESS )
		{
			printf("Couldn't stop the camera?\n");
		}
	}
	/*---------------------------------------------------------------------*
	 *  Close all cameras						  *
	 *---------------------------------------------------------------------*/
	cleanup();
	return 0;
}

