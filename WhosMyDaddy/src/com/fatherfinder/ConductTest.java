package com.fatherfinder;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
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
    private static final int REQUEST_ENABLE_BT = 3; //TODO: Refactor this out

    //TODO: Change these layouts to be buttons that say (conduct test)
    //     and then display the results
    // Layout Views
    private ListView mMessageLogView;
    private Button mPatButton;
    private Button mAncButton;
    
    //TODO: This is needed to ensure that the bluetooth is turned on. Think about refactoring into service
    // Local Bluetooth adapter
    private BluetoothAdapter mBluetoothAdapter = null;
    // Member object for the communication services
    private BluetoothService mMessageService = null; //TODO: make this an interface
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
            if (mMessageService == null) setupChat();
        }
        
        PaternityTest.start(getBaseContext()); //TODO: Refactor this. What if we start multiple tests (plus this is weird)
        AncestryTest.start(getBaseContext());
	}
	
	@Override
    public synchronized void onResume() {
        super.onResume();
        if(D) Log.e(TAG, "+ ON RESUME +");
        
        // Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if (mMessageService != null) {
            // Only if the state is STATE_NONE, do we know that we haven't started already
            if (mMessageService.getState() == BluetoothService.STATE_NONE) {
              // Start the Bluetooth chat services
              mMessageService.start();
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
        mPatButton = (Button) findViewById(R.id.button_test1);
        mPatButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                doTest( PaternityTest.TEST_NAME, true ); // Start the paternity test as the client
            }
        });
        
        mAncButton = (Button) findViewById(R.id.button_test2);
        mAncButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                doTest( AncestryTest.TEST_NAME, true ); // Start the ancestry test as the client
            }
        });

        // Initialize the BluetoothService to perform bluetooth connections
        mMessageService = new BluetoothService(this, mHandler);
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
        if (mMessageService != null) mMessageService.stop();
        if(D) Log.e(TAG, "--- ON DESTROY ---");
        
        // Kill the testing service
        PaternityTest.stop();
        AncestryTest.stop(); // TODO: Fix this
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
    private void doTest(String test, boolean asClient) {
    	// TODO: Make less bluetooth specific perhaps by housing connection information inside
    	//       of the service
    	
        // Check that we're actually connected before trying anything
        if (mMessageService.getState() != BluetoothService.STATE_CONNECTED) {
            Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }

        // TODO: Display an indicator that the test is taking place
        
        // TODO: think about converting to ints and using a switch
        String result;
        if(test.equals(AncestryTest.TEST_NAME)){
        	result = AncestryTest.conductTest(mMessageService, asClient);
        }
        else if(test.equals(PaternityTest.TEST_NAME)){
        	result = PaternityTest.conductTest(mMessageService, asClient);
        }
        else{
        	Log.e(TAG, "Test name not recognized: " + test);
        	return;
        }
        
        displayResult(result);
    }
    
    /**
     * Displays the results of the test
     */
    private void displayResult(String m) {
    	if(m == null)
    		mMessageLogArrayAdapter.add(getString(R.string.something_went_wrong));//TODO: something went wrong tell the user (use a resource string)
    	
    	////////// TEST CODE //////////////////
    	// Here we just add the result to the message adapter to look at later
    	mMessageLogArrayAdapter.add(m);
    	
    	///////////////////////////////////////
    	
    }

    // Name of the connected device
    private String mConnectedDeviceName = null;
    
    // ActionBar is a 3.0 thing, will have to find another way if this functionality is desired TODO: test on 4.0
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
                // TODO: resolve the issue where both click conduct test at the same time, and two tests are initiated                
                String readMessage = new String(readBuf, 0, msg.arg1);
                if(D) Log.d(TAG, "Received the message: " + readMessage);
                
                String[] parsed_message = readMessage.split(PrivateProtocol.SEPERATOR);
                
                // TODO: Ask the user if conducting the test is ok
                
                // TODO: This is hacky, we shouldn't need to be looking at PrivateProtocol
                
                if(parsed_message.length > 1 && parsed_message[0].equals(PrivateProtocol.START_TEST_MESSAGE)) //TODO: refactor for arbitrary tests
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
        mMessageService.connect(device, secure);
    }
}
