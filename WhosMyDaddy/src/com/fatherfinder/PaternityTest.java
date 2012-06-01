package com.fatherfinder;

import java.util.ArrayList;
import java.util.List;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.res.Resources;
import android.os.IBinder;
import android.util.Log;

/**
 * This class conducts an actual paternity test 
 * It handles communication with the PSI-C service, and processing/retrieivng data. 
 * @author skyf
 *
 */

//TODO: For now is singleton. Should it be this way? Like this inheritance is weird

//TODO: Investigate bugs that may occur from two tests running at the same time.
//         a possible fix could be to check if a test is already running, since they use the same bluetooth connection
//         this could be done in the handler


public class PaternityTest {
	// Debugging
    private static final String TAG = "PaternityTest";
    private static final boolean D = true;
    
    public static final String TEST_NAME = "PaternityTest";
    
    private static PaternityTest ref;
	
	private PaternityTest(){
		// hide the constructor
	}
	
	private static PaternityTest getInstance(){
		if (ref == null)
	        ref = new PaternityTest();		
	    return ref;
	}
	
	//TODO: this is very strange and should probably be refactored (perhaps by not binding directly)
	//        As it stands now there may be an issue if multiple activities conduct tests (this shouldn't happen)
	//        Starting the service ahead of time may be a bit excessive. 
	/**
	 * In order to execute a paternity test one must first call this method.
	 * And must call stopTest upon exiting.
	 * @param context to contact any necessary services
	 */
	public static void start(Context context){
		getInstance().doStart(context);
	}
	
	/**
	 * You must call stop test when done conducting paternity tests
	 */
	public static void stop(){
		getInstance().doStop();
	}
	

	public synchronized static String conductTest(BluetoothService messengerService, boolean isClient){
		return getInstance().doTest(messengerService, isClient);
	}
	
	// Handle Connecting to the test service (From docs)
    
    private PrivateProtocol mTestService;
    private boolean mIsBound = false;
    private static final int THRESHOLD = 23;

    private ServiceConnection mTestConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            // This is called when the connection with the service has been
            // established, giving us the service object we can use to
            // interact with the service.  Because we have bound to a explicit
            // service that we know is running in our own process, we can
            // cast its IBinder to a concrete class and directly access it.
        	if(D) Log.d(TAG, "Service connected");
            mTestService = ((PrivateProtocol.LocalBinder)service).getService();
        }

        public void onServiceDisconnected(ComponentName className) {
            // This is called when the connection with the service has been
            // unexpectedly disconnected -- that is, its proc25ess crashed.
            // Because it is running in our same process, we should never
            // see this happen.
        	if(D) Log.d(TAG, "Service disconnected");
            mTestService = null;
        }
    };
	private Context mContext;
    
    private void doBindService(Context context) {
        // Establish a connection with the service.  We use an explicit
        // class name because we want a specific service implementation that
        // we know will be running in our own process (and thus won't be
        // supporting component replacement by other applications).
    	mContext = context;
        mContext.bindService(new Intent(context, 
                EC_PSI_C.class), mTestConnection, Context.BIND_AUTO_CREATE);
        mIsBound = true;
    }

    private void doUnbindService() {
        if (mIsBound) {
            // Detach our existing connection.
            mContext.unbindService(mTestConnection);
            mIsBound = false;
            mContext = null;
        }
    }
    
    private void doStart(Context context){
    	doBindService(context);
    	
    }
	
    private void doStop(){
    	doUnbindService();
    }
	
	private String doTest(BluetoothService messageService, boolean isClient){
		//TODO: Crash if we haven't finished binding
		if(D) Log.d(TAG, "Starting a paternity test with " + isClient);
        
		if(mTestService == null){
			Log.e(TAG, "Test service did not initialize!");
			return("An error has occured");//TODO: use string resource
		}
		
		int common_markers = 0;
		try{
			common_markers = Integer.valueOf(mTestService.conductTest(TEST_NAME, messageService, isClient, getMarkerLengths()));
		}catch(NumberFormatException e){
			Log.e(TAG, e.toString() + "\n" + e.getStackTrace()); //TODO: Fix the issue when we get invalid input becasue two tests are started at the same time
		}
        
		//TODO: Make these resources
		if(common_markers > THRESHOLD)
			return "The result of the test indicates that the participants are parent and child.";
		else
			return "The result of the test indicates that the participants are not parent and child.";
	}
	
	private List<String> getMarkerLengths(){
    	//TODO: Implement Reading from SD (in an encyrpted fashion)
    
    	//int[] markerLengths = {1,2,3,4,5};
    	//String[] markerNames = {"Mark1","secondMarker","MarkNumber3", "Mark4", "Mark5"};
    	int[] markerLengths = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
    	
    	// Randomize the inputs
    	markerLengths[1] = (int) (Math.random()*100.0);
    	//markerLengths[2] = (int) (Math.random()*2);
    	//markerLengths[3] = (int) (Math.random()*2);
    	
    	String[] markerNames = {"Mark1","SecondMarker","MarkNumber3","Mark4","Mark5","Mark6","Mark7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25"};
    	List<String> ret = new ArrayList<String>();
    	
    	for( int i = 0; i < markerLengths.length; i++ ){
    		ret.add(markerNames[i].concat( String.valueOf(markerLengths[i]) ));
    	}	
    	
    	return ret;
    }
}
