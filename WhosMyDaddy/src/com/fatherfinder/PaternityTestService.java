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
    
    public static final String TEST_NAME = "PaternityTest";
    public static final String SEPERATOR = ";;";
    public static final String START_TEST_MESSAGE = "START";
    
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
    /**
     * Conduct the paternity test
     * 
     * If we are the server, we assume the client is already connected. If we are the client we let the server know we are here.
     * @param s A connected bluetooth service
     * @param client Conduct the test as server or client
     * @return The result of the test
     */
    public String conductTest(BluetoothService s, boolean client) {
    	//Switch to synchronous message reading. 
    	s.setReadLoop(false);
    	// The first step should be identifying which test is going to be conducted for now this is determined based on the class
    	
    	//TODO: think about extracting the client and server portions into different messages
    	if(client){
    		// Say hello
    		s.write(START_TEST_MESSAGE + SEPERATOR + TEST_NAME);
    		while(true){
    			String read = s.read();
    			if(read == null){
    				if(D) Log.d(TAG, "Client: Read failed");
    				return null; //TODO: Should we raise? probably not
    			}
    			else if(read == START_TEST_MESSAGE) { //TODO: Should we look for which test was started as well?
    				if(D) Log.d(TAG, "Client: Read succeeded");
    				break;
    			}
    			//else
    				//TODO: something went wrong resend our start test?
    		}
    		s.write("Client says hello: " + Math.random());
    	}
    	else{
    		// Acknowledge the start message
    		s.write(START_TEST_MESSAGE);
    	
    		
    		s.write("Server says hello: " + Math.random());
    	}
    	
    	
    	return s.read();
    }
    
}
