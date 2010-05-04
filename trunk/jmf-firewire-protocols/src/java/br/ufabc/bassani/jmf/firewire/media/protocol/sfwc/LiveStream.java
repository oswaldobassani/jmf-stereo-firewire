package br.ufabc.bassani.jmf.firewire.media.protocol.sfwc;

import java.awt.Dimension;
import java.io.IOException;

import javax.media.Buffer;
import javax.media.CaptureDeviceInfo;
import javax.media.Control;
import javax.media.Format;
import javax.media.MediaLocator;
import javax.media.control.FormatControl;
import javax.media.format.AudioFormat;
import javax.media.format.RGBFormat;
import javax.media.protocol.BufferTransferHandler;
import javax.media.protocol.CaptureDevice;
import javax.media.protocol.ContentDescriptor;
import javax.media.protocol.PushBufferStream;

public class LiveStream implements PushBufferStream, Runnable, CaptureDevice {

	protected ContentDescriptor cd = new ContentDescriptor(ContentDescriptor.RAW);
	protected int maxDataLength;
	protected byte [] data;
	protected Dimension size;
	protected RGBFormat rgbFormat;
	protected AudioFormat audioFormat;
	protected boolean started;
	protected Thread thread;
	protected BufferTransferHandler transferHandler;
	protected Control [] controls = new Control[0];
	protected CaptureDeviceInfo cdi;

	protected boolean videoData = true;
	protected StereoFireWireCamera sfwc = new StereoFireWireCamera();
	protected boolean connected = false;

	protected Format[] formats = new Format[1];

	protected float frameRate = StereoFireWireCamera.getFrameRate();
	private int width = StereoFireWireCamera.getWidth();
	private int height = StereoFireWireCamera.getHeight();

	public void connect () {
		System.out.println ("LiveStream: Called connect");
		if (!connected) {
			sfwc.configure ();
			connected = true;
		}
	}

	public void disconnect () {
		System.out.println ("LiveStream: Called disconnect");
		connected = false;
	}

	public CaptureDeviceInfo getCaptureDeviceInfo () {
		System.out.println ("LiveStream: Called getCaptureDeviceInfo");

		return (cdi);
	}

	public FormatControl[] getFormatControls () {
		System.out.println ("LiveStream: Called getFormatControls");

		return null;
	}

	public void setupDeviceCaptureInfo () {
		System.out.println ("LiveStream: Called setupDeviceCaptureInfo");
		size = new Dimension(width, height);
		maxDataLength = size.width * size.height;// * 3;
		formats[0] = StereoFireWireCamera.getFormat();

		cdi = new CaptureDeviceInfo ("PointGrey Dragonfly", new MediaLocator ("sfwc://"), formats);
	}

	public LiveStream() {
		System.out.println ("LiveStream: Called LiveStream");

		if (!connected) {
			sfwc.configure ();
			connected = true;
		}

		size = new Dimension(width, height);
		maxDataLength = size.width * size.height;// *3;
		rgbFormat = (RGBFormat)StereoFireWireCamera.getFormat();

		thread = new Thread(this);
	}

	/***************************************************************************
	 * SourceStream
	 ***************************************************************************/

	public ContentDescriptor getContentDescriptor() {
		return cd;
	}

	public long getContentLength() {
		return LENGTH_UNKNOWN;
	}

	public boolean endOfStream() {
		return false;
	}

	/***************************************************************************
	 * PushBufferStream
	 ***************************************************************************/

	int seqNo = 0;
	double freq = 2.0;

	public Format getFormat() {
		if (videoData){
			System.out.println("LiveStream: getFormat - : "+rgbFormat);
			return rgbFormat;
		}else
			return audioFormat;
	}

	private byte[] inputRaw;
	public void read(Buffer buffer) throws IOException {
		synchronized (this) {
			Object outdata = buffer.getData();
			if (outdata == null || !(outdata.getClass() == Format.intArray) ||
					((int[])outdata).length < maxDataLength) {
				outdata = new int[maxDataLength];
				buffer.setData(outdata); 
			}
			if(inputRaw==null || inputRaw.length!=maxDataLength*3){
				inputRaw = new byte[maxDataLength*3];
			}
			if (videoData) {
				buffer.setFormat( rgbFormat );
				buffer.setTimeStamp( (long) (seqNo * (1000 / frameRate) * 1000000) );
				
				byte[] frame = sfwc.getFrame();
				
				//System.out.println("Frame frame.length = " + frame.length);
				//System.out.println("Frame frame.length = 640 * 480 * " + (frame.length/(640*480)));
				
				System.arraycopy (frame, 0, inputRaw, 0, maxDataLength*3);
				int[] rgbArray = convertToIntArray(inputRaw, width, height);
				System.arraycopy(rgbArray, 0, outdata, 0, maxDataLength);
				
				//System.arraycopy(frame, 0, outdata, 0, maxDataLength);
				
			}
			buffer.setSequenceNumber( seqNo );
			buffer.setLength(maxDataLength);
			buffer.setFlags(0);
			buffer.setHeader( null );
			seqNo++;
		}
	}

	/**
	 * Bayer Pattern RGGB To RGB(32bits, int[], Blue)
	 */

	private int[] bayers;
	private int[] convertToIntArray(byte PixBuf[], int w, int h) {
		if(bayers==null || bayers.length!=w*h){
			bayers = new int[w * h];
		}
		for (int pos=0, off=0; off < bayers.length*3; pos++, off+=3) {
			bayers[pos] = ((((int)PixBuf[off]) & 255)<<16) | ((((int)PixBuf[off+1]) & 255)<<8) | ((((int)PixBuf[off+2]) & 255)<<0);
		}
		return bayers;
	}

	public void setTransferHandler(BufferTransferHandler transferHandler) {
		synchronized (this) {
			this.transferHandler = transferHandler;
			notifyAll();
		}
	}

	void start(boolean started) {
		sfwc.start ();
		synchronized ( this ) {
			this.started = started;
			if (started && !thread.isAlive()) {
				thread = new Thread(this);
				thread.start();
			}
			notifyAll();
		}
	}

	public void start () {
		if (connected) {
			sfwc.start ();
		}
	}

	public void stop () {
		if (connected) {
			sfwc.stop ();
		}
	}

	/***************************************************************************
	 * Runnable
	 ***************************************************************************/

	public void run() {
		while (started) {
			synchronized (this) {
				while (transferHandler == null && started) {
					try {
						wait(1000);
					} catch (InterruptedException ie) {
					}
				}
			}

			if (started && transferHandler != null) {
				transferHandler.transferData(this);
				try {
					//Thread.currentThread().sleep( 10 );
					Thread.sleep( 10 );
				} catch (InterruptedException ise) {
				}
			}
		}
	}

	// Controls
	public Object [] getControls() {
		return controls;
	}

	public Object getControl(String controlType) {
		try {
			Class  cls = Class.forName(controlType);
			Object cs[] = getControls();
			for (int i = 0; i < cs.length; i++) {
				if (cls.isInstance(cs[i]))
					return cs[i];
			}
			return null;

		} catch (Exception e) {
			// no such controlType or such control
			return null;
		}
	}
}
