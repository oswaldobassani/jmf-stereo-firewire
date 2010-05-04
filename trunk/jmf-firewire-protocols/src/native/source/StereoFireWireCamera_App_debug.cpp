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

/* interface  */
#include "StereoFireWireCamera_App.h"

using std::cout;
using std::cin;

#define CAMERA_640x480_8BIT
#ifdef CAMERA_640x480_8BIT
	int format = FORMAT_VGA_NONCOMPRESSED;
	int mode = MODE_640x480_MONO;
	int frameRate = FRAMERATE_15;
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
int numCameras = 0; // Queremos 2
int numNodes;

unsigned int channel;
unsigned int speed;

raw1394handle_t	raw1394Handle_e;
raw1394handle_t raw1394Handle[MAX_CAMERAS];
dc1394_cameracapture dc1394Camera[MAX_CAMERAS];

nodeid_t *camera_nodes;

dc1394_feature_set features;
dc1394_camerainfo info;
dc1394_miscinfo misc_info;
quadlet_t supported_framerates;

int bufferSize = 307200; // 640 x 480

/* declarations for conversions */
quadlet_t qValue;
bayer_pattern_t pattern;

/* Buffers de conversao */
unsigned char* rgbBuffer_left;
unsigned char* rgbBuffer_right;

/* Contagem de frames */
int frame_number = 0;
int MAX_FRAMES = 10;

/* save images */
int SAVE_IMAGES = 0;

/* declarations for file */
char nomeArquivo_left[256];
char nomeArquivo_right[256];

FILE* imagefile_left;
FILE* imagefile_right;

/* Debug */
int DEBUG_LEVEL = 0;
void setDebugLevel(int level)
{
	DEBUG_LEVEL = level;
}

/* ========================== vendor() ================================= */
void vendor(void)
{
	if(DEBUG_LEVEL) printf( "vendor...\n" );
	if(DEBUG_LEVEL) printf( "vendor %s\n", info.vendor);
}

/* ========================== model() ================================== */
void model(void)
{
	if(DEBUG_LEVEL) printf( "model...\n" );
	if(DEBUG_LEVEL) printf( "model %s\n", info.model);
}

/* ========================== setCaptureFormat() ======================= */
void setCaptureFormat(void)
{
	if(DEBUG_LEVEL) printf( "setCaptureFormat...\n" );
	if(DEBUG_LEVEL) printf( "Unable to set properties, not implemented!!!\n\n" );
}

/* ========================== configure() ============================== */
int configure(void)
{
	if(DEBUG_LEVEL) printf( "configure...\n" );
	/*---------------------------------------------------------------------- *
	 *  Open ohci and get dc1394 handle                                      *
	 *-----------------------------------------------------------------------*/
	if(DEBUG_LEVEL) printf( "create_handle...\n" );
	raw1394Handle_e = dc1394_create_handle( nCard );

	if ( raw1394Handle_e == NULL )
	{
		if(DEBUG_LEVEL) printf("Unable to aquire a raw1394 handle\n\n"
			"Please check \n"
			"  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded\n"
			"  - if you have read/write access to /dev/raw1394\n" );
		return 1;
	}

	/*---------------------------------------------------------------------- *
	 *  get the camera nodes and describe them as we find them               *
	 *-----------------------------------------------------------------------*/
	numNodes = raw1394_get_nodecount( raw1394Handle_e );

	if(DEBUG_LEVEL) printf( "get_camera_nodes...\n" );
	// dc1394_get_camera_nodes
	// This returns the available cameras on the bus.
	// If showCameras is set to 1, a description of the found cameras is printed.
	camera_nodes = dc1394_get_camera_nodes( raw1394Handle_e, &numCameras, 1 );
	if(DEBUG_LEVEL) printf( "numCameras %d \n", numCameras );

	if(DEBUG_LEVEL) printf( "destroy_handle...\n" );
	dc1394_destroy_handle( raw1394Handle_e );
	if(DEBUG_LEVEL) printf( "destroy_handle OK...\n" );

	if ( numCameras < 1 )
	{
		if(DEBUG_LEVEL) printf("No cameras found! (%d nodes on the bus)\n"
			"  - could be you need to try a different 1394 device (modify code to fix)\n",
			numNodes );
		cleanup();
		return 1;
	}

	/*---------------------------------------------------------------------- *
	 *  check (each camera) if camera highest node to                        *
	 *  prevent the iso-transfer bug from raw1394 system                     *
	 *-----------------------------------------------------------------------*/
	if(DEBUG_LEVEL) printf( "check_camera_nodes...\n" );
	for ( int i=0; i < numCameras; i++ )
	{
		if(DEBUG_LEVEL) printf( "node %d \n", i);
		if(DEBUG_LEVEL) printf( "node %d - camera node %d (last node %d) \n", i, camera_nodes[i], (numNodes-1) );
		if( camera_nodes[i] == numNodes-1 )
		{
			if(DEBUG_LEVEL) printf( "\n Sorry, your camera is the highest numbered node\n"
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
				"\n" );
			cleanup();
			return 1;
		}
	}

	/* =================================================================== *
	 *                    setup cameras for capture                        *
	 *---------------------------------------------------------------------*/
	if(DEBUG_LEVEL) printf( "setup cameras for capture...\n" );
	for ( int i = 0; i < numCameras; i++ )
	{

		/*------------------------------------------------------------------*
		 *  Open ohci and get dc1394 handle for each camera                 *
		 *------------------------------------------------------------------*/
		if(DEBUG_LEVEL) printf( "dc1394_create_handle...\n" );
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
		 *  get_camera_info for each camera                                      *
		 *-------------------------------------------------------------------*/
		if(DEBUG_LEVEL) printf( "get_camera_info...\n" );
		if( dc1394_get_camera_info( raw1394Handle[i],
				dc1394Camera[i].node,
				&info) !=DC1394_SUCCESS )
		{
			if(DEBUG_LEVEL) printf("unable to get camera_info\n");
		}
		else
		{
			dc1394_print_camera_info( &info );
			if(DEBUG_LEVEL) printf( "\n" );
		}

		/*-------------------------------------------------------------------*
		 *  get_camera_misc_info for each camera                                      *
		 *-------------------------------------------------------------------*/
		if(DEBUG_LEVEL) printf( "get_camera_misc_info...\n" );
		if( dc1394_get_camera_misc_info( raw1394Handle[i],
				dc1394Camera[i].node,
				&misc_info) !=DC1394_SUCCESS )
		{
			if(DEBUG_LEVEL) printf("unable to get camera_misc_info\n");
		}
		else
		{
			if(DEBUG_LEVEL) printf( "Format: \t\t %d (384-FORMAT_VGA_NONCOMPRESSED) \n", misc_info.format );
			if(DEBUG_LEVEL) printf( "Mode: \t\t\t %d (69-MODE_640x480_MONO) \n", misc_info.mode );
			if(DEBUG_LEVEL) printf( "Framerate: \t\t %d (35-FRAMERATE_15 / 36-FRAMERATE_30) \n", misc_info.framerate );
			if(DEBUG_LEVEL) printf( "ISO ON: \\t %d (0-False / 1-True) \n", misc_info.is_iso_on );
			if(DEBUG_LEVEL) printf( "ISO Channel: \t\t %d \n", misc_info.iso_channel );
			if(DEBUG_LEVEL) printf( "ISO Speed: \t\t %d (2-SPEED_400) \n", misc_info.iso_speed );
			if(DEBUG_LEVEL) printf( "Mem Channel Number: \t %d \n", misc_info.mem_channel_number );
			if(DEBUG_LEVEL) printf( "Save Channel: \t\t %d \n", misc_info.save_channel );
			if(DEBUG_LEVEL) printf( "Load Channel: \t\t %d \n", misc_info.load_channel );
			if(DEBUG_LEVEL) printf( "BMode Capable: \t\t %d (0-False / 1-True) \n", misc_info.bmode_capable );
			if(DEBUG_LEVEL) printf( "OneShot Capable: \t %d (0-False / 1-True) \n", misc_info.one_shot_capable );
			if(DEBUG_LEVEL) printf( "MultiShot Capable: \t %d (0-False / 1-True) \n", misc_info.multi_shot_capable );
			if(DEBUG_LEVEL) printf( "\n" );
		}

		/*-------------------------------------------------------------------*
		 *  get feature for each camera                                      *
		 *-------------------------------------------------------------------*/
		if(DEBUG_LEVEL) printf( "get_camera_feature_set...\n" );
		if( dc1394_get_camera_feature_set( raw1394Handle[i],
				dc1394Camera[i].node,
				&features ) !=DC1394_SUCCESS )
		{
			if(DEBUG_LEVEL) printf("unable to get feature set\n");
		}
		else
		{
			dc1394_print_feature_set( &features );
			if(DEBUG_LEVEL) printf( "\n" );
		}

		/*-------------------------------------------------------------------*
		 *  get iso_channel_and_speed for each camera                        *
		 *-------------------------------------------------------------------*/
		if(DEBUG_LEVEL) printf( "get_iso_channel_and_speed...\n" );
		if ( dc1394_get_iso_channel_and_speed( raw1394Handle[i],
				dc1394Camera[i].node,
				&channel,
				&speed ) != DC1394_SUCCESS )
		{
			if(DEBUG_LEVEL) printf("unable to get the iso channel number\n");
			cleanup();
			return 1;
		}

		if(DEBUG_LEVEL) printf("Framerate required (int code) %d (35-FRAMERATE_15 / 36-FRAMERATE_30) \n", frameRate);
		if (dc1394_query_supported_framerates(raw1394Handle[i],
				dc1394Camera[i].node,
				format,
				mode,
				&supported_framerates) != DC1394_SUCCESS) {
			if(DEBUG_LEVEL) printf("dc1394_query_supported_framerates() failed.");
		}
		// if(DEBUG_LEVEL) printf("Supported framerates value: \t 0x%x \n", supported_framerates ); //0x78000000
		// if(DEBUG_LEVEL) printf("Supported framerates value: \t %d \n", supported_framerates ); //2013265920
		int supportedFramerate = 0;
		if (supported_framerates & (1U << (31-5)) && frameRate == FRAMERATE_60)
			supportedFramerate = 1;
		else if (supported_framerates & (1U << (31-4)) && frameRate == FRAMERATE_30)
			supportedFramerate = 1;
		else if (supported_framerates & (1U << (31-3)) && frameRate == FRAMERATE_15)
			supportedFramerate = 1;
		else if (supported_framerates & (1U << (31-2)) && frameRate == FRAMERATE_7_5)
			supportedFramerate = 1;
		else if (supported_framerates & (1U << (31-1)) && frameRate == FRAMERATE_3_75)
			supportedFramerate = 1;
		else if (supported_framerates & (1U << (31-0)) && frameRate == FRAMERATE_1_875)
			supportedFramerate = 1;
		if (supportedFramerate) {
			// Framerate suportado
		}else{
			if (supported_framerates & (1U << (31-5)))
				frameRate = FRAMERATE_60;
			else if (supported_framerates & (1U << (31-4)))
				frameRate = FRAMERATE_30;
			else if (supported_framerates & (1U << (31-3)))
				frameRate = FRAMERATE_15;
			else if (supported_framerates & (1U << (31-2)))
				frameRate = FRAMERATE_7_5;
			else if (supported_framerates & (1U << (31-1)))
				frameRate = FRAMERATE_3_75;
			else if (supported_framerates & (1U << (31-0)))
				frameRate = FRAMERATE_1_875;
			else
				if(DEBUG_LEVEL) printf("Couldn't figure out the maximum framerate.\n");
			// if(DEBUG_LEVEL) printf("Frame Rate 'FRAMERATE_30' = %d.\n", FRAMERATE_30);//36
			// if(DEBUG_LEVEL) printf("Frame Rate 'FRAMERATE_15' = %d.\n", FRAMERATE_15);//35
			if(DEBUG_LEVEL) printf("Using framerate (int code) %d \n", frameRate);
		}

		/*-------------------------------------------------------------------*
		 *  dma_setup_capture for each camera                                *
		 *-------------------------------------------------------------------*/
		if(DEBUG_LEVEL) printf( "dma_setup_capture...\n" );
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
			if(DEBUG_LEVEL) printf( "Unable to setup camera-\n"
				"check code above line %d of %s to make sure\n"
				"that the video mode,framerate and format are\n"
				"supported by your camera\n",
				__LINE__,__FILE__);
			cleanup();
			return 1;
		}

		/*---------------------------------------------------------------------*
		 *  query the camera to determine the Bayer pattern                    *
		 *---------------------------------------------------------------------*/
		if(DEBUG_LEVEL) printf( "get Bayer pattern...\n" );
		GetCameraControlRegister( raw1394Handle[i],
				camera_nodes[i],
				0x1040,		/* Bayer Tile Mapping register */
				&qValue );

		pattern = BAYER_PATTERN_BGGR;
		switch( qValue )
		{
		case 0x42474752:	/* BGGR */
			pattern = BAYER_PATTERN_BGGR;
			break;
		case 0x47524247:	/* GRBG */
			pattern = BAYER_PATTERN_GRBG;
			break;
		case 0x52474742:  /* RGGB */
			pattern = BAYER_PATTERN_RGGB;
			break;
		case 0x47425247:	/* GBRG */
			pattern = BAYER_PATTERN_GBRG;
			break;
		case 0x59595959:	/* YYYY = BW */
			if(DEBUG_LEVEL) printf( "Camera is black and white\n" );
			cleanup();
			return 1;
		default:
			if(DEBUG_LEVEL) printf( "Camera BAYER_TILE_MAPPING register has an unexpected value:\n"
				"\t0x%x\n", qValue );
			if(DEBUG_LEVEL) printf( " ... aplicando Bayer pattern RGGB! \n" );
			pattern = BAYER_PATTERN_RGGB;
		}
	}

	/*----------------------------------------------------------------------*
	 *  free_camera_nodes                                                   *
	 *----------------------------------------------------------------------*/
	if(DEBUG_LEVEL) printf( "free_camera_nodes...\n" );
	dc1394_free_camera_nodes(camera_nodes);
	if(DEBUG_LEVEL) printf( "free_camera_nodes OK ...\n" );

	return 0;
}

/* ========================== start() ================================== */
int start(void)
{
	if(DEBUG_LEVEL) printf( "start...\n" );
	if(DEBUG_LEVEL) printf( "setup cameras for capture - step2 - start iso transmission...\n" );
	for ( int i = 0; i < numCameras; i++ )
	{
		/* ================================================================= *
		 *                     start iso transmission                        *
		 *-------------------------------------------------------------------*/
		if(DEBUG_LEVEL) printf( "Starting iso transmission...\n" );
		if ( dc1394_start_iso_transmission( raw1394Handle[i],
				dc1394Camera[i].node ) !=DC1394_SUCCESS )
		{
			perror("unable to start camera iso transmission\n");
			cleanup();
			return 1;
		}
		i++;
	}
	return 0;
}

/* ========================== getFrame() =============================== */
int getFrame(void)
{

	/*---------------------------------------------------------------------*
	 *  criando rbgBuffer		   				       *
	 *---------------------------------------------------------------------*/
	if ( !rgbBuffer_left )
	{
		rgbBuffer_left  = new unsigned char[ 3 * bufferSize ];
		if ( !rgbBuffer_left )
		{
			if(DEBUG_LEVEL) printf( "Could not allocate RGB buffer (left) for color conversion (size = %d)\n", 3 * bufferSize );
			cleanup();
			return 1;
		}
	}
	if ( !rgbBuffer_right )
	{
		rgbBuffer_right = new unsigned char[ 3 * bufferSize ];
		if ( !rgbBuffer_right )
		{
			if(DEBUG_LEVEL) printf( "Could not allocate RGB buffer (right) for color conversion (size = %d)\n", 3 * bufferSize );
			cleanup();
			return 1;
		}
	}

	if(DEBUG_LEVEL) printf( "getFrame...\n" );
	if(DEBUG_LEVEL) printf( "Capturing frames (frame %d) ...\n", frame_number );
	if ( dc1394_dma_multi_capture(dc1394Camera, numCameras)!= DC1394_SUCCESS )
	{
		if(DEBUG_LEVEL) printf( "Capturing frames teste 2 (erro)...\n" );
		if(DEBUG_LEVEL) printf( "Unable to capture a frame\n");
		cleanup();
		return 1;
	}

	/* =================================================================== *
	 *                    setup cameras for capture                        *
	 *---------------------------------------------------------------------*/
	if(DEBUG_LEVEL) printf( "setup cameras for capture...\n" );
	for ( int i = 0; i < numCameras; i++ )
	{
		unsigned char* source = (unsigned char*) dc1394Camera[i].capture_buffer;
		switch( i )
		{
		case 0:	/* Left */
			if(DEBUG_LEVEL) printf( "Capturing frames (camera 0 - left) ...\n" );
			/*---------------------------------------------------------------------*
			 *  convert to color image                                             *
			 *---------------------------------------------------------------------*/
			if(DEBUG_LEVEL) printf( "convert to color image...\n" );
			//Params: imageIn, imageOut, frame_width, frame_height, pattern
			BayerEdgeSense(source, rgbBuffer_left, 640, 480, pattern );
			if(numCameras<2){
				memcpy(&rgbBuffer_right, &rgbBuffer_left, sizeof(rgbBuffer_left));
			}
			if(!SAVE_IMAGES) break;
			/*-----------------------------------------------------------------------
			 *  save image
			 *-----------------------------------------------------------------------*/
			if(DEBUG_LEVEL) printf( "Saving the image...\n" );
			sprintf(nomeArquivo_left, "imagem_left_%d.ppm", frame_number);
			imagefile_left = fopen( nomeArquivo_left, "w" );
			if( imagefile_left == NULL)
			{
				perror("");
				if(DEBUG_LEVEL) printf( "Can't create '%s'\n", nomeArquivo_left );
				cleanup();
				return 1;
			}
			fprintf( imagefile_left, "P6\n%u %u 255\n", 640, 480 );
			fwrite( rgbBuffer_left, 3, bufferSize, imagefile_left );
			fclose( imagefile_left );
			if(DEBUG_LEVEL) printf( "saved image to '%s'\n", nomeArquivo_left );
			break;
		case 1: /* Right */
			if(DEBUG_LEVEL) printf( "Capturing frames (camera 1 - right) ...\n" );
			/*---------------------------------------------------------------------*
			 *  convert to color image                                             *
			 *---------------------------------------------------------------------*/
			if(DEBUG_LEVEL) printf( "convert to color image...\n" );
			//Params: imageIn, imageOut, frame_width, frame_height, pattern
			BayerEdgeSense(source, rgbBuffer_right, 640, 480, pattern );
			if(!SAVE_IMAGES) break;
			/*-----------------------------------------------------------------------
			 *  save image
			 *-----------------------------------------------------------------------*/
			if(DEBUG_LEVEL) printf( "Saving the image...\n" );
			sprintf(nomeArquivo_right, "imagem_right_%d.ppm", frame_number);
			imagefile_right = fopen( nomeArquivo_right, "w" );
			if( imagefile_right == NULL)
			{
				perror("");
				if(DEBUG_LEVEL) printf( "Can't create '%s'\n", nomeArquivo_right );
				cleanup();
				return 1;
			}
			fprintf( imagefile_right, "P6\n%u %u 255\n", 640, 480 );
			fwrite( rgbBuffer_right, 3, bufferSize, imagefile_right );
			fclose( imagefile_right );
			if(DEBUG_LEVEL) printf( "saved image to '%s'\n", nomeArquivo_right );
			break;
		default:
			break;
		}
	}

	if(DEBUG_LEVEL) printf( "done_with_buffer...\n" );
	/* ==================================================================== *
	 *                         done_with_buffer                             *
	 *----------------------------------------------------------------------*/
	for (int i = 0; i < numCameras; i++)
	{
		/*-------------------------------------------------------------------*
		 *  dma_done_with_buffer						  *
		 *-------------------------------------------------------------------*/
		dc1394_dma_done_with_buffer(&dc1394Camera[i]);
	}

	frame_number++;
	return 0;
}

unsigned char* getRgbBuffer_left()
{
	return rgbBuffer_left;
}
unsigned char* getRgbBuffer_right()
{
	return rgbBuffer_right;
}

/* ========================== stop() =================================== */
void stop(void)
{
	if(DEBUG_LEVEL) printf( "stop...\n" );
	/* ==================================================================== *
	 *                         stop data transmission                       *
	 *----------------------------------------------------------------------*/
	for (int i = 0; i < numCameras; i++)
	{
		/*-------------------------------------------------------------------*
		 * stop iso transmission                                             *
		 *-------------------------------------------------------------------*/
		if ( dc1394_stop_iso_transmission( raw1394Handle[i],
				dc1394Camera[i].node ) !=DC1394_SUCCESS )
		{
			if(DEBUG_LEVEL) printf("Couldn't stop the camera?\n");
		}
	}
}

/* ========================== cleanup() ================================ *
 *  called when the program exits and                                    *
 *  destroys the existing connections to the 1394 drivers                *
 *-----------------------------------------------------------------------*/
void cleanup(void)
{
	if(DEBUG_LEVEL) printf( "cleanup...\n" );
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
	int ret = configure();	
	if(ret!=0) return ret;

	vendor();
	model();

	ret = start();
	if(ret!=0) return ret;

	/* =================================================================== *
	 *                       capturing frames				  *
	 *---------------------------------------------------------------------*/
	while (frame_number<MAX_FRAMES) {
		ret = getFrame();
		if(ret!=0) return ret;
	}

	stop();

	/*---------------------------------------------------------------------*
	 *  Close all cameras						  *
	 *---------------------------------------------------------------------*/
	cleanup();
	return 0;
}

