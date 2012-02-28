package com.fatherfinder;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

/*
 * Notes: 1/26/12
 * For now the idea is that this activity will be the main activity and will handle
 * spawning other activities/services depending on which device is selected
 * 
 * For now we have the idea that this activity will negotiate with two services
 * the first, a bluetooth comm service, will be passed to the second, a paternity
 * test conducting service. More services may be added in the future.
 * 
 * The idea here is that the comm service will have a common api that will allow for
 * easy refactoring of communication protocoll. Similarly, the test service should 
 * be interchangeable with future tests, without being visible to this or any other
 * activity.
 *
 */

//TODO: Resolve the crash case when Paternity test fails (if the connection is killed while there is a read polling) (read returns null)
//TODO: Add in a settings option to dissalow server/client communications.
/**
 * This is the main activity that conducts the tests and displays the results. 
 * @author skyf
 *
 */
public class ConductTest extends Activity {
	// Debugging
    private static final String TAG = "ConductTest";
    private static final boolean D = true;
    
    // Intent request codes
    private static final int REQUEST_CONNECT_DEVICE_SECURE = 1;
    //private static final int REQUEST_CONNECT_DEVICE_INSECURE = 2;
    private static final int REQUEST_ENABLE_BT = 3; //TODO: Refactor this out
    
    // Types of tests
    private static final int PATERNITY_TEST = 1;

    //TODO: Change these layouts to be buttons that say (conduct test)
    //     and then display the results
    // Layout Views
    private ListView mMessageLogView;
    private Button mStartButton;
    
    //TODO: This is needed to ensure that the bluetooth is turned on. Think about refactoring into service
    // Local Bluetooth adapter
    private BluetoothAdapter mBluetoothAdapter = null;
    // Member object for the chat services
    private BluetoothService mMessageSerivce = null; //TODO: make this an interface
    // Array adapter for the conversation thread
    private ArrayAdapter<String> mMessageLogArrayAdapter; //TODO: remove this it is jsut for testing
    
	/**
	 * @see android.app.Activity#onCreate(Bundle)
	 */
	@Override protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
        if(D) Log.d(TAG, "+++ ON CREATE +++");

        // Set up the window layout
        setContentView(R.layout.main);

        // Get local Bluetooth adapter
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        // If the adapter is null, then Bluetooth is not supported
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
	}
	
	//NOTE: This happens when an activity becomes visible
	@Override
    public void onStart() {
        super.onStart();
        if(D) Log.d(TAG, "++ ON START ++");

        // If BT is not on, request that it be enabled.
        // setupChat() will then be called during onActivityResult
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        // Otherwise, setup the chat session
        } else {
            if (mMessageSerivce == null) setupChat();
        }
    }
	
	@Override
    public synchronized void onResume() {
        super.onResume();
        if(D) Log.e(TAG, "+ ON RESUME +");
        
        // Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if (mMessageSerivce != null) {
            // Only if the state is STATE_NONE, do we know that we haven't started already
            if (mMessageSerivce.getState() == BluetoothService.STATE_NONE) {
              // Start the Bluetooth chat services
              mMessageSerivce.start();
            }
        }
    }
	
	private void setupChat() {
        Log.d(TAG, "setupChat()");

        // Initialize the array adapter for the conversation thread
        // This maps array data to the view
        mMessageLogArrayAdapter = new ArrayAdapter<String>(this, R.layout.message);
        mMessageLogView = (ListView) findViewById(R.id.message_list);
        mMessageLogView.setAdapter(mMessageLogArrayAdapter);

        // Initialize the send button with a listener that for click events
        mStartButton = (Button) findViewById(R.id.button_start);
        mStartButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                doTest( PaternityTestService.TEST_NAME, true ); // Start the paternity test as the client
            }
        });

        // Initialize the BluetoothService to perform bluetooth connections
        mMessageSerivce = new BluetoothService(this, mHandler);
    }
	
	 @Override
    public synchronized void onPause() {
        super.onPause();
        if(D) Log.e(TAG, "- ON PAUSE -");
    }

    @Override
    public void onStop() {
        super.onStop();
        if(D) Log.e(TAG, "-- ON STOP --");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        // Stop the Bluetooth chat services
        if (mMessageSerivce != null) mMessageSerivce.stop();
        if(D) Log.e(TAG, "--- ON DESTROY ---");
        
        // Kill the testing service
        doUnbindTestService();
    }
    
    private void ensureDiscoverable() {
        if(D) Log.d(TAG, "ensure discoverable");
        if (mBluetoothAdapter.getScanMode() !=
            BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
            discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
            startActivity(discoverableIntent);
        }
    }
    
    // CALLBACKS
    
    /**
     * Conducts a test.
     * @param test  Which test is being conducted
     * @param asClient Flag indicating if the test is conducted as client or server
     */
    // This will fire off the desired test service and connect it to the chosen device
    //TODO: Investigate the possibility of DoS since multiple tests will be conducted at the same time
    //         a possible fix could be to check if a test is already running, since they use the same bluetooth connection
    //         this could be done in the handler
    private void doTest(String test, boolean asClient) {
    	if(D) Log.d(TAG, "Starting a paternity test with " + asClient);
    	// TODO: Switch on test & make the values constants
    	// TODO: Make less bluetooth specific perhaps by housing connection information inside
    	//       of the service
        // Check that we're actually connected before trying anything
        if (mMessageSerivce.getState() != BluetoothService.STATE_CONNECTED) {
            Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }

        // TODO: Display an indicator that the test is taking place
        
        // Start a PaternityTest and give it the BluetoothService for communication
        doBindTestService(PaternityTestService.class);
         
        // TODO: This is really tacky and should be cleaned up with some better way perhaps waiting until the service is bound
        //          or we can bind to the service ahead of time (we should probably just bind ahead of time)
        mAsClient = asClient;
        if(mTestService != null)
        	displayResult( mTestService.conductTest(mMessageSerivce, mAsClient) );
    }
    
    /**
     * Displays the results of the test
     */
    //TODO: Just make this a boolean at some point
    private void displayResult(String m) {
    	if(m == null)
    		mMessageLogArrayAdapter.add("Something went wrong. Try again");//TODO: something went wrong tell the user (use a resource string)
    	
    	//TODO: modify the ui
    	
    	////////// TEST CODE //////////////////
    	// Here we just add the result to the message adapter to look at later
    	mMessageLogArrayAdapter.add(m);
    	
    	///////////////////////////////////////
    	
    }

    
    // This handler and associated methods handles displaying information of a connected device to the user
    // TODO: see if this is actually necessary
    // Name of the connected device
    private String mConnectedDeviceName = null;
    
    // ActionBar is a 3.0 thing, will have to find another way if this functionality is desired TODO: remove cruft
    //private final void setStatus(int resId) {
        //final ActionBar actionBar = getActionBar();
        //actionBar.setSubtitle(resId);
    //}
    //private final void setStatus(CharSequence subTitle) {
        //final ActionBar actionBar = getActionBar();
        //actionBar.setSubtitle(subTitle);
    //}

    // The Handler that gets information back from the BluetoothService
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case   BluetoothService.MESSAGE_STATE_CHANGE:
                if(D) Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                switch (msg.arg1) {
                case BluetoothService.STATE_CONNECTED:
                    //setStatus(getString(R.string.title_connected_to, mConnectedDeviceName));
                    break;
                case BluetoothService.STATE_CONNECTING:
                    //setStatus(R.string.title_connecting);
                    break;
                case BluetoothService.STATE_LISTEN:
                case BluetoothService.STATE_NONE:
                	//TODO: Kill test and discconect from service?
                    //setStatus(R.string.title_not_connected);
                    break;
                }
                break;
            case BluetoothService.MESSAGE_READ:
                byte[] readBuf = (byte[]) msg.obj;
                // construct a string from the valid bytes in the buffer
                // TODO: ASK the user if the test is desired.
                // TODO: resolve the issue where both click conduct test at the same time, and two tests are initiated                
                String readMessage = new String(readBuf, 0, msg.arg1);
                if(D) Log.d(TAG, "Received the message: " + readMessage);
                
                String[] parsed_message = readMessage.split(PaternityTestService.SEPERATOR);
                
                if(parsed_message.length > 1 && parsed_message[0].equals(PaternityTestService.START_TEST_MESSAGE)) //TODO: refactor for arbitrary tests
                	doTest(parsed_message[1], false);
                break;
            case BluetoothService.MESSAGE_DEVICE_NAME:
                // save the connected device's name
                mConnectedDeviceName = msg.getData().getString(BluetoothService.DEVICE_NAME);
                Toast.makeText(getApplicationContext(), "Connected to "
                               + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                break;
            case BluetoothService.MESSAGE_TOAST:
                Toast.makeText(getApplicationContext(), msg.getData().getString(BluetoothService.TOAST),
                               Toast.LENGTH_SHORT).show();
                break;
            }
        }
    };
    
    //Called when INTENT is returned
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(D) Log.d(TAG, "onActivityResult " + resultCode);
        switch (requestCode) {
        case REQUEST_CONNECT_DEVICE_SECURE:
            // When DeviceListActivity returns with a device to connect
            if (resultCode == Activity.RESULT_OK) {
                connectDevice(data, true);
            }
            break;
//        case REQUEST_CONNECT_DEVICE_INSECURE:
//            // When DeviceListActivity returns with a device to connect
//            if (resultCode == Activity.RESULT_OK) {
//                connectDevice(data, false);
//            }
//            break;
        case REQUEST_ENABLE_BT:
            // When the request to enable Bluetooth returns
            if (resultCode == Activity.RESULT_OK) {
                // Bluetooth is now enabled, so set up a chat session
                setupChat();
            } else {
                // User did not enable Bluetooth or an error occurred
                Log.d(TAG, "BT not enabled");
                Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.options_menu, menu);
        return true;
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Intent serverIntent = null;
        switch (item.getItemId()) {
        case R.id.secure_connect_scan:
            // Launch the DeviceListActivity to see devices and do scan
            serverIntent = new Intent(this, DeviceList.class);
            startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE_SECURE);
            return true;
// TODO: Toast unimplemented (or remove)
//        case R.id.insecure_connect_scan:
//            // Launch the DeviceListActivity to see devices and do scan
//            serverIntent = new Intent(this, DeviceListActivity.class);
//            startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE_INSECURE);
//            return true;
        case R.id.discoverable:
            // Ensure this device is discoverable by others
            ensureDiscoverable();
            return true;
        }
        return false;
    }
    
    private void connectDevice(Intent data, boolean secure) {
        // Get the device MAC address
        String address = data.getExtras()
            .getString(DeviceList.EXTRA_DEVICE_ADDRESS);
        // Get the BluetoothDevice object
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        // Attempt to connect to the device
        mMessageSerivce.connect(device, secure);
    }
    
    // Handle Connecting to the test service (From docs)
    
    private PaternityTestService mTestService;
    private boolean mIsBound = false;
    private boolean mAsClient;

    private ServiceConnection mTestConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            // This is called when the connection with the service has been
            // established, giving us the service object we can use to
            // interact with the service.  Because we have bound to a explicit
            // service that we know is running in our own process, we can
            // cast its IBinder to a concrete class and directly access it.
            mTestService = ((PaternityTestService.LocalBinder)service).getService();
            
            //TODO: Is this really the best way to display the result?
            displayResult( mTestService.conductTest(mMessageSerivce, mAsClient) );
            
        }

        public void onServiceDisconnected(ComponentName className) {
            // This is called when the connection with the service has been
            // unexpectedly disconnected -- that is, its process crashed.
            // Because it is running in our same process, we should never
            // see this happen.
            mTestService = null;
        }
    };
    
    void doBindTestService(Class<PaternityTestService> klass) {
        // Establish a connection with the service.  We use an explicit
        // class name because we want a specific service implementation that
        // we know will be running in our own process (and thus won't be
        // supporting component replacement by other applications).
        bindService(new Intent(ConductTest.this, 
                klass), mTestConnection, Context.BIND_AUTO_CREATE);
        mIsBound = true;
    }

    void doUnbindTestService() {
        if (mIsBound) {
            // Detach our existing connection.
            unbindService(mTestConnection);
            mIsBound = false;
        }
    }
}
