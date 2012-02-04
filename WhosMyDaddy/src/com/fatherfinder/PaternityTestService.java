package com.fatherfinder;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

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
    	//TODO: use the service to communicate (probably via a handler)
    	return "HI";
    }
    
}
