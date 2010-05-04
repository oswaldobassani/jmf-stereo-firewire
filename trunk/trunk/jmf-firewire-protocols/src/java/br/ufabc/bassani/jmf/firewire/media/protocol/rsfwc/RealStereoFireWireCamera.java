package br.ufabc.bassani.jmf.firewire.media.protocol.rsfwc;

import java.awt.Dimension;

import javax.media.Format;
import javax.media.format.RGBFormat;

public class RealStereoFireWireCamera {

	static {
		System.loadLibrary ("RealStereoFireWireCamera");
	}

	static {
		System.loadLibrary ("raw1394");
	}

	static {
		System.loadLibrary ("dc1394_control");
	}

	public RealStereoFireWireCamera () {
		System.out.println ("RealStereoFireWireCamera: constructor");
		//configure();
	}

	public native String vendor();
	public native String model();
	public native void configure();
	public native void setCaptureFormat(int w, int h, int fps, int colorSpace);
	public native byte[] getFrame();
	public native void getFrame(byte[] frame);
	public native boolean start();
	public native boolean stop();
	public native void cleanUp();

	private static float frameRate = 15f;
	private static int width = 640;
	private static int height = 480*2;
	private static Format format;
	
	public static float getFrameRate() {
		return frameRate;
	}

	public static int getHeight() {
		return height;
	}

	public static int getWidth() {
		return width;
	}

	public static Format getFormat() {
		System.out.println ("RealStereoFireWireCamera: getFormat");
		if(format==null){
			Dimension size = new Dimension(width, height);
			int maxDataLength = size.width * size.height;// * 3;
			/*
			 * RGBFormat(java.awt.Dimension size, int maxDataLength, 
			 * 		java.lang.Class dataType, float frameRate,
			 * 		int bitsPerPixel, int red, int green, int blue, 
			 * 		int pixelStride, int lineStride, int flipped, int endian) 
			 */
			format = new RGBFormat(size, maxDataLength,
					Format.intArray,frameRate,
					32, 0x00FF0000, 0x0000FF00, 0x000000FF,
					1, size.width, Format.FALSE, RGBFormat.LITTLE_ENDIAN);
					//24, 3, 2, 1,
					//3, size.width*3, Format.FALSE, RGBFormat.LITTLE_ENDIAN);
		}
		return format;
	}

}
