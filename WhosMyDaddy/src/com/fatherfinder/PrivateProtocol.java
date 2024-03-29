package com.fatherfinder;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.util.List;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

/**
 * This class does all the work for conducting the secure communication
 * with another user. It does not handle any communication directly, but instead
 * relies on a Object that extends BluetoothService
 * @author skyf
 *
 */


// This class provides the utilities needed for the other types of protocols
public abstract class PrivateProtocol extends Service {
	// Debugging
    private final String TAG = "PrivateProtocol";
    private final boolean D = false;
    
    // Message Paramaters
    public static final String SEPERATOR = ";;";
    public static final String START_TEST_MESSAGE = "START";
    public static final String ACK_START_MESSAGE = "ACK_START";
    
    protected StopWatch offlineWatch;
    protected StopWatch onlineWatch;
    
    // This is a little ugly, but quick enough for now
    // TODO: Extract benchmarking into another class
    //      It is also worht noting that the benchmarking can not be threaded
    //      The timers are not thread safe
    protected final boolean benchmark = true;
    private final int NUM_TRIALS = 1;
    
    // Exceptions
    @SuppressWarnings("serial")
	class DoubleClientException extends Exception{}
    
	/**
     * Class for clients to access.  Because we know this service always
     * runs in the same process as its clients, we don't need to deal with
     * IPC.
     */
    public class LocalBinder extends Binder {
         PrivateProtocol getService() {
            return PrivateProtocol.this;
        }
    }
    
    @Override
    public void onCreate(){
    	loadSharedKeys();
    	onlineWatch = new StopWatch();
    	offlineWatch = new StopWatch();
    }
    
//    @Override //TODO: do we need this? I don't think so since we only allow binding to the service
//    public int onStartCommand(Intent intent, int flags, int startId) {
//    	if(D)
//    		Log.d(TAG, "Received start id " + startId + ": " + intent);
//    	
//        // We want to ensure all intents are processed
//        return START_REDELIVER_INTENT;
//    }
	
    // This is the object that receives interactions from clients.
    private final IBinder mBinder = new LocalBinder();
    
	@Override public IBinder onBind(Intent intent) {
		return mBinder;
	}
    	
    // Conduct the Test
    /**
     * Perform the Privacy Perserving protocol.
     * 
     * If we are the server, we assume the client is already connected. If we are the client we let the server know we are here.
     * @param s A connected bluetooth service
     * @param client Conduct the test as server or client
     * @param inputSet The data to be intersected
     * @param testName The application specific name for this test
     * @return The result of the test
     */
    public String conductTest(String testName, BluetoothService s, boolean client, List<String> inputSet) {
    	String ret = null;
    	
    	//Switch to synchronous message reading. 
    	s.setReadLoop(false); // this may take a message to take affect
    	
    	int numTrials = 0;
    	onlineWatch.clear();
    	offlineWatch.clear();
    	
    	// TODO: As it stands now conduct test must initialize this properly (this is dangerous)
    	if(benchmark)
    		((BluetoothServiceLogger) s).measureStart();
    	
    	try{
    		do{
		    	//TODO: This loop is a huge hack in order to ensure devices get connected
		    	do{
			    	try{
			    		//TODO: Simplify protocol if messages are guaranteed (they may not be)
				    	if(client){
				    		initiateClient(testName, s);
				    		ret= conductClientTest(s, inputSet);
				    	}
				    	else{
				    		initiateServer(testName, s);
				    		ret = conductServerTest(s, inputSet);
				    	}
			    	}
			    	catch(DoubleClientException e){
			    		Log.i(TAG, "Both users clicked at the same time");
			    		
			    		//TODO: Don't use flow control here
			    		//TODO: one user can be forced to always be the server and thus never learn the result
			    		//          to fix this security issue simply ensure the one who clicks is always the server
			    		client = !client;
			    		
			    	}
			    	
		    	}while(ret == null);
    		}while(benchmark && ++numTrials < NUM_TRIALS);
    		
    		reportTimers(client);
    		
    		if(benchmark){
    			((BluetoothServiceLogger) s).measureStop();
    			Log.i(TAG, "Bytes used: " +((BluetoothServiceLogger) s).bytesUsed() / numTrials);
    			Log.i(TAG, "MegaBytes used: " +((BluetoothServiceLogger) s).megaBytesUsed() / numTrials);
    			Log.i(TAG, "GigaBytes used: " +((BluetoothServiceLogger) s).gigaBytesUsed() / numTrials);
    		}
	    	
    	}
    	finally {
    		
    		s.setReadLoop(true); //TODO: make this revert to its previous state
    	}
    	
    	
    	
    	return ret;
    	
    	
    }
    
    
    // Tell the client we heard them
    private void initiateServer(String testName, BluetoothService s){
    	if(D) Log.d(TAG, "Server started");
		// Acknowledge the start message
		s.write(ACK_START_MESSAGE); //TODO: we probably don't need any acks look closer
	

		String read;
		
		while(true) {
			read = s.readString();
			if(read == null){
				if(D) Log.d(TAG, "Server: Read failed");
				//TODO: Should we raise? yes
			}
			else if(read.equals(START_TEST_MESSAGE + SEPERATOR + testName)){ //
				if(D) Log.d(TAG, "Ack not recieved, RE-ACK" + read);
				//s.write(ACK_START_MESSAGE); //TODO: Will this break things? it was added for easy benchmarking
			}
			else if(read.equals(ACK_START_MESSAGE)){
				break;
			}
			else{
				if(D) Log.d(TAG, "Non start recieved: " + read);
				break; //TODO: should we raise here? Or just continue looping
			}
		}
		
    }
    
    // Tell the server we are listening
    private void initiateClient(String testName, BluetoothService s) throws DoubleClientException{
    	// Say hello
    	s.write(START_TEST_MESSAGE + SEPERATOR + testName);

    	if(D) Log.d(TAG, "Client is listening");

    	while(true){
    		String read = s.readString();
    		if(read == null){
    			if(D) Log.d(TAG, "Client: Read failed");
    			//TODO: Should we raise? yes
    		}
    		else if(read.equals(ACK_START_MESSAGE)) { //TODO: Should we look for which test was started as well?
    			s.write(ACK_START_MESSAGE);
    			break;
    		}
    		// A bit hacky but a way to get around us both clicking start
    		else if(read.equals(START_TEST_MESSAGE + SEPERATOR + testName)){
    			int rand = (int)(Math.random()*1000);
    			Log.i(TAG, "Start recieved from client: sending rand " + rand);
    			s.write(START_TEST_MESSAGE + SEPERATOR + testName); // Tell them we are in the double client state
    			s.write(String.valueOf(rand));
    			
    			while(true){ //TODO: Is this dangerous? this solves if they are equal
	    			while(read.equals(START_TEST_MESSAGE + SEPERATOR + testName)){
	    				read = s.readString();
	    			}
	    			int theirRand = Integer.parseInt(read);
	    			if( rand > theirRand ){
	    				throw new DoubleClientException();
	    			}
	    			break;
    			}
    		}
    		else { //We didn't hear the ack resend. //TODO: is resending dangerous?
    			if(D) Log.d(TAG, "Garbage was recieved" + read);
    			s.write(START_TEST_MESSAGE + SEPERATOR + testName);
    		}

    	}
    }
    
    protected abstract String conductClientTest(BluetoothService s, List<String> input);
    protected abstract String conductServerTest(BluetoothService s, List<String> input);
    
    // Utility functions
    
    
    protected void reportTimers(boolean client){reportTimers(client, NUM_TRIALS);}
    protected void reportTimers(boolean client, int numTrials){
    	Log.i(TAG, "Trials completed: " + numTrials);
    	
    	String cl = client ? "Client" : "Server";
    	Log.i(TAG, cl + " offline phase completed in " + offlineWatch.getElapsedTime()/numTrials + " miliseconds.");
    	Log.i(TAG, cl + " online phase completed in " + onlineWatch.getElapsedTime()/numTrials + " miliseconds.");
    }
    
    protected abstract void loadSharedKeys();
	protected BigInteger hash(byte [] message, byte selector){
		
		// input = selector | message
		byte [] input = new byte[message.length + 1];
		System.arraycopy(message, 0, input, 1, message.length);
		input[0] = selector;
		
		MessageDigest digest = null;
		try {
			digest = MessageDigest.getInstance("SHA-1");
    	} catch (NoSuchAlgorithmException e1) {
    		Log.e(TAG, "SHA-1 is not supported");
    		return null; // TODO: Raise an error?
    	}
		digest.reset();
    	    
		return new BigInteger(digest.digest(input));
	}
    
    // Calculate a random number between 0 and range (exclusive)
	//TODO: Is secure random really secure??
    protected BigInteger randomRange(BigInteger range){
    	//TODO: Is there anything else we should fall back on here perhaps openssl bn_range
    	//         another option is using an AES based key generator (the only algorithim supported by android)
    	
    	// TODO: Should we be keeping this rand around? 
    	SecureRandom rand = new SecureRandom();
    	BigInteger temp = new BigInteger(range.bitLength(), rand);
    	while(temp.compareTo(range) >= 0 || temp.equals(BigInteger.ZERO)){
    		temp = new BigInteger(range.bitLength(), rand);
    	}
    	return temp;
    	
    }
}

