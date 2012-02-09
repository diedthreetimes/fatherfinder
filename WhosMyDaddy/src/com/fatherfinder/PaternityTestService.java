package com.fatherfinder;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

/**
 * This class does all the work for conducting the secure communication
 * with another user. It does not handle any communication directly, but instead
 * relies on a Object that implements an BluetoothService's connected thread api
 * @author skyf
 *
 */
public class PaternityTestService extends Service {
	// Debugging
    private static final String TAG = "PaternityTestService";
    private static final boolean D = true;
    
	/**
     * Class for clients to access.  Because we know this service always
     * runs in the same process as its clients, we don't need to deal with
     * IPC.
     */
    public class LocalBinder extends Binder {
        PaternityTestService getService() {
            return PaternityTestService.this;
        }
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
    	if(D)
    		Log.d(TAG, "Received start id " + startId + ": " + intent);
    	
        // We want to ensure all intents are processed
        return START_REDELIVER_INTENT;
    }
	
	@Override public IBinder onBind(Intent intent) {
		// TODO Put your code here
		return mBinder;
	}
	
	// This is the object that receives interactions from clients.
    private final IBinder mBinder = new LocalBinder();
    
    
    // Conduct the Test
    // TODO: just return a boolean
    public String conductTest(BluetoothService s){
    	s.setHandler(mHandler);
    	
    	//TODO: use the service to communicate (probably via a handler)
    	// We may need to restructure the service to block, or we may just want to block here.
    	// The idea being that we do not reply until we here a response, and we do not return a result until we hear a response
    	// a well structured implementation will allow the activity to still be interactive while the test is being conducted
    	// but this is not a requirement
    	
    	// As a side note, the communciation with the bluetoothservice would be better constructed like a socket.
    	// This would rid the use of handlers and allow blocking communication relatively easily.
    	// This will require some refactoring, with the bluetooth service since most of the functionaility will no longer
    	// be necessary for now we just use a test implmentation using handler
    	
    	
    	// The first step should be identifying which test is going to be conducted for now this is determined based on the class
    	
    	
    	//!!!!! TODO:
    	
    	return "HI";
    }
    
    
    //TODO: Remove this (see conductTest)
    // The Handler that gets information back from the BluetoothService
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case BluetoothService.MESSAGE_READ:
                byte[] readBuf = (byte[]) msg.obj;
                // construct a string from the valid bytes in the buffer
                String readMessage = new String(readBuf, 0, msg.arg1);
                //TODO: what do we do with messages as we read them?
                //mConversationArrayAdapter.add(mConnectedDeviceName+":  " + readMessage);
                break;
            case BluetoothService.MESSAGE_DEVICE_NAME:
            	// Display the connected devices name
                Toast.makeText(getApplicationContext(), "Connected to "
                               + msg.getData().getString(BluetoothService.DEVICE_NAME), Toast.LENGTH_SHORT).show();
                break;
            case BluetoothService.MESSAGE_TOAST:
                Toast.makeText(getApplicationContext(), msg.getData().getString(BluetoothService.TOAST),
                               Toast.LENGTH_SHORT).show();
                break;
            }
        }
    };
    
}
