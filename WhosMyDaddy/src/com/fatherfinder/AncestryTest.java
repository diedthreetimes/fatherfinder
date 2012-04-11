package com.fatherfinder;

import java.util.ArrayList;
import java.util.List;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.util.Log;

/**
 * This class conducts an ancestry test 
 * It handles communication with the PSI-C service, and processing/retrieivng data. 
 * @author skyf
 *
 */

//TODO: This is almost a direct copy of paternity test. Refactor better!!


public class AncestryTest {
	// Debugging
    private static final String TAG = "AncestryTest";
    private static final boolean D = true;
    
    public static final String TEST_NAME = "AncestryTest";
    
    private static AncestryTest ref;
	
	private AncestryTest(){
		// hide the constructor
	}
	
	private static AncestryTest getInstance(){
		if (ref == null)
	        ref = new AncestryTest();		
	    return ref;
	}
	
	//TODO: this is very strange and should probably be refactored (perhaps by not binding directly)
	//        As it stands now there may be an issue if multiple activities conduct tests (this shouldn't happen)
	//        Starting the service ahead of time may be a bit excessive. 
	/**
	 * In order to execute a ancestry test one must first call this method.
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
	
	public static String conductTest(BluetoothService messengerService, boolean isClient){
		return getInstance().doTest(messengerService, isClient);
	}
	
	// Handle Connecting to the test service (From docs)
    
    private PrivateProtocol mTestService;
    private boolean mIsBound = false;

    private ServiceConnection mTestConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            // This is called when the connection with the service has been
            // established, giving us the service object we can use to
            // interact with the service.  Because we have bound to a explicit
            // service that we know is running in our own process, we can
            // cast its IBinder to a concrete class and directly access it.
            mTestService = ((PrivateProtocol.LocalBinder)service).getService();
        }

        public void onServiceDisconnected(ComponentName className) {
            // This is called when the connection with the service has been
            // unexpectedly disconnected -- that is, its proc25ess crashed.
            // Because it is running in our same process, we should never
            // see this happen.
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
                PSI_C.class), mTestConnection, Context.BIND_AUTO_CREATE);
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
		if(D) Log.d(TAG, "Starting a ancestry test with " + isClient);
         
        return mTestService.conductTest(TEST_NAME, messageService, isClient, getGenomePositions());
	}
	
	//TODO: This is going to need to be refactored but for now, we retrieve the random elements here.
	//           The best way to do this is probably to just use a list object backed by a memory mapped file
	private List<String> getGenomePositions(){
    	int seed = 10780; // TODO: For now the randomness is hardcoded
    	
    	Log.e(TAG, "Not Implemented");
    	
    	return null;
    }
}
