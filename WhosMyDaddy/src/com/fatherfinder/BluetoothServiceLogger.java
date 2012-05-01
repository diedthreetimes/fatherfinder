package com.fatherfinder;

import android.content.Context;
import android.os.Handler;

//TODO: Make this class take as an argument a bluetooth Service instead of extending
public class BluetoothServiceLogger extends BluetoothService {

	private boolean mMeasure;
	
	public BluetoothServiceLogger(Context context, Handler handler) {
		super(context, handler);
	}
	
	public void write(byte[] out){
		
	}
	
	public void measureStart(){
		mMeasure = true;
	}
	
	public void measureStop(){
		mMeasure = false;
	}

}
