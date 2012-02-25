package com.fatherfinder;

import java.math.BigInteger;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.security.spec.DSAPublicKeySpec;
import java.security.spec.InvalidKeySpecException;

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

// At the moment we use the built in BigInteger implementation to do our calculations.
// TODO: Benchmark an openssl/gmp version
public class PaternityTestService extends Service {
	// Debugging
    private static final String TAG = "PaternityTestService";
    private static final boolean D = true;
    
    public static final String TEST_NAME = "PaternityTest";
    public static final String SEPERATOR = ";;";
    public static final String START_TEST_MESSAGE = "START";
    public static final String ACK_START_MESSAGE = "ACK_START";
    
    //public/private keys
    private BigInteger p, q, g;
    
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
    public void onCreate(){
    	loadSharedKeys();
    }
    
    @Override //TODO: do we need this? I don't think so since we only allow binding to the service
    public int onStartCommand(Intent intent, int flags, int startId) {
    	if(D)
    		Log.d(TAG, "Received start id " + startId + ": " + intent);
    	
        // We want to ensure all intents are processed
        return START_REDELIVER_INTENT;
    }
	
    // This is the object that receives interactions from clients.
    private final IBinder mBinder = new LocalBinder();
    
	@Override public IBinder onBind(Intent intent) {
		return mBinder;
	}
    	
    // Conduct the Test
    // TODO: just return a boolean (or better yet think about returning a type T when refactoring)
    /**
     * Conduct the paternity test
     * 
     * If we are the server, we assume the client is already connected. If we are the client we let the server know we are here.
     * @param s A connected bluetooth service
     * @param client Conduct the test as server or client
     * @return The result of the test
     */
    public String conductTest(BluetoothService s, boolean client) {
    	String ret = null;
    			
    	//Switch to synchronous message reading. 
    	s.setReadLoop(false); // this may take a message to take affect
    	// The first step should be identifying which test is going to be conducted for now this is determined based on the class
    	
    	try{
	    	//TODO: think about extracting the client and server portions into different functions
    		//TODO: Simplify protocol if messages are guaranteed (they may not be)
	    	if(client){
	    		initiateClient(s);
	    		ret= conductClientTest(s);
	    	}
	    	else{
	    		initiateServer(s);
	    		ret = conductServerTest(s);
	    	}
    	}
    	finally {
    		
    		s.setReadLoop(true); //TODO: make this revert to its previous state
    	}
    	
    	
    	
    	return ret;
    	
    	
    }
    
    
    // Tell the client we heard them
    private void initiateServer(BluetoothService s){
    	if(D) Log.d(TAG, "Server started");
		// Acknowledge the start message
		s.write(ACK_START_MESSAGE); //TODO: we probably don't need any acks look closer
	

		String read;
		
		while(true) {
			read = s.read();
			if(read == null){
				if(D) Log.d(TAG, "Server: Read failed");
				//TODO: Should we raise? yes
			}
			else if(read.equals(START_TEST_MESSAGE + SEPERATOR + TEST_NAME)){ //
				if(D) Log.d(TAG, "Ack not recieved, RE-ACK" + read);
				s.write(ACK_START_MESSAGE);
			}
			else{
				if(D) Log.d(TAG, "Non start recieved: " + read);
				break; //TODO: should we raise here?
			}
		}
		
    }
    
    // Tell the server we are listening
    private void initiateClient(BluetoothService s){
    	// Say hello
    	s.write(START_TEST_MESSAGE + SEPERATOR + TEST_NAME);

    	if(D) Log.d(TAG, "Client is listening");

    	while(true){
    		String read = s.read();
    		if(read == null){
    			if(D) Log.d(TAG, "Client: Read failed");
    			//TODO: Should we raise? yes
    		}
    		else if(read.equals(ACK_START_MESSAGE)) { //TODO: Should we look for which test was started as well?
    			if(D) Log.d(TAG, "Client: Read succeeded");
    			s.write(ACK_START_MESSAGE);
    			break;
    		}
    		else { //We didn't hear the ack resend.
    			if(D) Log.d(TAG, "Garbage was recieved" + read);
    			s.write(START_TEST_MESSAGE + SEPERATOR + TEST_NAME);
    		}

    	}
    }
    
    // Actually perform the test (these will be overides from a testing base class (and can be the same function)
    private String conductClientTest(BluetoothService s){
    	// Compute any precomputation
    	
    	// Generate the Secrets
    	
    	//TODO: Unccoment this section and finish keygen bit
    	//BigInteger rc  = randomRange(q);
    	//BigInteger rc1 = randomRange(q);
    	
    	//Log.d(TAG, "RC: " + rc);
    	//Log.d(TAG, "RC': " + rc1);
    	//Log.d(TAG, "g: " + g);
    	//Log.d(TAG, "q: " + q);
    	//Log.d(TAG, "p: " + p);
    	
    	// Send info to server
    	
    	// Wait for server
    	
    	// Finish computation
    	
    	// Send result
    	
		s.write("Client says hello: " + g);
		return s.read();
    }
    
    private String conductServerTest(BluetoothService s) {
    	// Compute any precomputation
    	
    	// Wait for client
    	
    	// Send final computation and return
    	
    	
		s.write("Server says hello: " + Math.random());
		return s.read();
    }
    
    
    // Utility functions
	// Load the common inputs, p q and g which are:
	// p - a prime number
	// q - the sub prime
	// g - the base (generator)
	private void loadSharedKeys(){
		//TODO: Load these keys from a file
		
		// We probably won't use this, but here is a (rather long) way to generate them
		//TODO: Delete this, just used as a reference (DSA is not supported)
		/*
		KeyPairGenerator kpg;
		try {
			kpg = KeyPairGenerator.getInstance("DSA");
			kpg.initialize(1024);
			KeyPair kp = kpg.genKeyPair();

			KeyFactory fact = KeyFactory.getInstance("DSA");
			DSAPublicKeySpec pub = fact.getKeySpec(kp.getPublic(),
					DSAPublicKeySpec.class);

			p = pub.getP();
			q = pub.getQ();
			g = pub.getG();
		} catch (NoSuchAlgorithmException e) {
			//TODO: What do we do if Service creation fails?
			Log.e(TAG, "DSA key generation is not supported!");
		} catch (InvalidKeySpecException e) {
			Log.e(TAG, "DSA key generation is not supported!");
		}
        */
        
	}
    
    //TODO: Finish implementing this in a way that is compatible with openssh
    private byte [][] getMarkerLengths(){
    	//TODO: Implement Reading from SD (in an encyrpted fashion)
    	int[] markerLengths = {1,2,3,4,5,6,7};
    	String[] markerNames = {"Mark1","SecondMarker","MarkNumber3","Mark4","Mark5","Mark6","Mark7"};
    	byte[][] ret = null;
    	
    	for( String marker : markerNames ){
    		MessageDigest digest = null;
    	    try {
    	        digest = MessageDigest.getInstance("SHA-256");
    	    } catch (NoSuchAlgorithmException e1) {
    	        Log.e(TAG, "SHA-256 is not supported");
    	        return null; // TODO: Raise an error?
    	    }
    	    digest.reset();
    	    
    	    digest.digest(marker.getBytes());
    	    
    	    //Log.i("Eamorr",digest.digest(password.getBytes("UTF-8")).toString());
    	}
    	
    	// First we hash the marker names
    	
    	return ret;
    }
    
    // Calculate a random number between 0 and range
    private BigInteger randomRange(BigInteger range){
    	//TODO: Is there anything else we should fall back on here perhaps openssl bn_range
    	
    	// TODO: Should we be keeping this rand around? 
    	SecureRandom rand = new SecureRandom();
    	BigInteger temp = new BigInteger(range.bitLength(), rand);
    	while(temp.compareTo(range) > 0)
    		temp = new BigInteger(range.bitLength(), rand);
    	return temp;
    	
    }
}
