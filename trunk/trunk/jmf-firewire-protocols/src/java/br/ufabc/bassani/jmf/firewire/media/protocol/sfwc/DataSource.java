package br.ufabc.bassani.jmf.firewire.media.protocol.sfwc;

import java.io.IOException;

import javax.media.CaptureDeviceInfo;
import javax.media.Format;
import javax.media.MediaLocator;
import javax.media.Time;
import javax.media.control.FormatControl;
import javax.media.protocol.CaptureDevice;
import javax.media.protocol.PushBufferDataSource;
import javax.media.protocol.PushBufferStream;

public class DataSource extends PushBufferDataSource implements CaptureDevice {

	protected Object [] controls = new Object[0];
	protected boolean started = false;
	protected String contentType = "raw";
	protected boolean connected = false;
	protected Time duration = DURATION_UNKNOWN;
	protected LiveStream [] streams = null;
	protected LiveStream stream = null;

	public CaptureDeviceInfo getCaptureDeviceInfo () {
		System.out.println ("DataSource getCaptureDeviceInfo");

		//float frameRate = FireWireCamera.getFrameRate();
		//int width = FireWireCamera.getWidth();
		//int height = FireWireCamera.getHeight();

		//Dimension size = new Dimension(width, height);
		//int maxDataLength = size.width * size.height;
		Format formats[] = new Format[1];

		formats[0] = StereoFireWireCamera.getFormat();

		CaptureDeviceInfo cdi = new CaptureDeviceInfo ("PointGrey Dragonfly", new MediaLocator ("sfwc://"), formats);
		return (cdi);        
	}

	public FormatControl[] getFormatControls () {
		return (null);
	}

	public DataSource() {
	}

	public String getContentType() {
		if (!connected){
			System.err.println("Error: DataSource not connected");
			return null;
		}
		return contentType;
	}

	public void connect() throws IOException {
		if (connected)
			return;
		connected = true;
	}

	public void disconnect() {
		try {
			if (started)
				stop();
		} catch (IOException e) {}
		connected = false;
	}

	public void start() throws IOException {
		// we need to throw error if connect() has not been called
		if (!connected)
			throw new java.lang.Error("DataSource must be connected before it can be started");
		if (started)
			return;
		started = true;
		stream.start(true);
	}

	public void stop() throws IOException {
		if ((!connected) || (!started))
			return;
		started = false;
		stream.start(false);
	}

	public Object[] getControls() {
		return null;
	}

	public Object getControl(String controlType) {
		return null;
	}

	public Time getDuration() {
		return duration;
	}

	public PushBufferStream [] getStreams() {
		if (streams == null) {
			streams = new LiveStream[1];
			stream = streams[0] = new LiveStream();
		}
		return streams;
	}

}
