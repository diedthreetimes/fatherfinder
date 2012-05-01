package com.fatherfinder;

import android.content.Context;
import android.os.Handler;

//TODO: Make this class take as an argument a bluetooth Service instead of extending
//        to do this we must have a communication interface
public class BluetoothServiceLogger extends BluetoothService {

	public BluetoothServiceLogger(Context context, Handler handler) {
		super(context, handler);
	}

	private boolean mMeasure = false;
	private long bytes = 0;
	
	
	public void write(String out){
		if(mMeasure){
			bytes += out.getBytes().length;
		}
		super.write(out);
	}
	
	public String read(){
		String r = super.read();
		if(mMeasure){
			bytes += r.getBytes().length;
		}
		return r;
	}
	
	// TODO: Implement other out methods
	
	public void measureStart(){
		mMeasure = true;
	}
	
	public void measureStop(){
		mMeasure = false;
	}
	
	public void measureClear(){
		bytes = 0;
	}
	
	public long bytesUsed(){
		return bytes;
	}
	
	public double megaBytesUsed(){
		return bytes / 1048576.0;
	}
	
	public double gigaBytesUsed(){
		return bytes / 1073741824.0;
	}

}
