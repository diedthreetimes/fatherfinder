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
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

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
// TODO: Benchmark byte[] vs strings for messaging
public class PaternityTestService extends Service {
	// Debugging
    private static final String TAG = "PaternityTestService";
    private static final boolean D = true;
    
    public static final String TEST_NAME = "PaternityTest";
    public static final String SEPERATOR = ";;";
    public static final String START_TEST_MESSAGE = "START";
    public static final String ACK_START_MESSAGE = "ACK_START";
    
    // The number of allowed mismatches
    private static final int ERROR_THRESHOLD = 0;
	
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
    	// OFFLINE PHASE
    	BigInteger rc  = randomRange(q); // Secret 1
    	BigInteger rc1 = randomRange(q); // Secret 2
    	
    	BigInteger x = g.modPow(rc, p);
    	
    	List<BigInteger> ais = new ArrayList<BigInteger>(); // The set {a1,a2,...,ai}
    	for( String marker: getMarkerLengths() ){
    		ais.add(hash(marker).mod(p).modPow(rc1, p));
    	}
   
    	// ONLINE PHASE
    	s.write(x.toString(16));
    	for( BigInteger ai : ais ){
    		s.write(ai.toString(16));
    	}
    	
    	List<BigInteger> bis = new ArrayList<BigInteger>(); // The set {b1,b2,...,bi}
    	List<BigInteger> tsjs = new ArrayList<BigInteger>(); // The set {ts1, ts2, ..., tsj}
    	BigInteger y = null;
    	
    	// Get values from the server
    	y = new BigInteger(s.read(), 16);
    	for(int i = 0; i < ais.size(); i++){
    		bis.add(new BigInteger(s.read(),16));
    	}
    	for(int i = 0; i < ais.size(); i++){
    		tsjs.add(new BigInteger(s.read(),16));
    	}
    	
    	// Finish computation
    	
    	List<BigInteger> tcis = new ArrayList<BigInteger>();
    	for(int i = 0; i < ais.size(); i++){
    		//TODO: Should this be a different hash?
    		// This is the following calculation all mod p
    		// H(y^Rc * bi^(1/Rc') )
    		tcis.add(hash( y.modPow(rc, p).multiply( bis.get(i).modPow(rc1.modInverse(p), p) ) ).mod(p) );
    	}
    	
    	if(D) Log.d(TAG, "Client's first: " + tcis.get(0).toString());
    	if(D) Log.d(TAG, "Client's second: " + tcis.get(1).toString());
    	if(D) Log.d(TAG, "Server's first: " + tsjs.get(0).toString());
    	if(D) Log.d(TAG, "Server's second: " + tsjs.get(1).toString());
    	if(D) Log.d(TAG, "Server's third: "  + tsjs.get(2).toString());
    	
    	// tcis = tcis ^ tsjs (intersection)
    	tcis.retainAll(tsjs);
    	
    	int sharedLengths = tcis.size();
    	
    	// Send result
    	if(D) Log.d(TAG, "Client calculated: " + String.valueOf(sharedLengths));
    	s.write(String.valueOf(sharedLengths));
    	
		return String.valueOf(sharedLengths); //TODO: return a boolean based on ERROR_THRESHOLD
    }
    
    private String conductServerTest(BluetoothService s) {
    	//TODO: Unccoment shuffle and try and figure out why things are not intersecting correctly
    	// OFFLINE PHASE
    	BigInteger rs  = randomRange(q); // Secret 1
    	BigInteger rs1 = randomRange(q); // Secret 2
    	
    	BigInteger y = g.modPow(rs, p);
    	
    	List<BigInteger> ksjs = new ArrayList<BigInteger>(); // The set {ks1,ks2,...,ksi}
    	for( String marker: getMarkerLengths() ){
    		ksjs.add(hash(marker).mod(p).modPow(rs1, p).mod(p));
    	}
    	
    	SecureRandom r = new SecureRandom();
    	//TODO: Collections.shuffle(ksjs, r);
    	
    	// ONLINE PHASE
    	List<BigInteger> ais = new ArrayList<BigInteger>(); // The set {a1,a2,...,ai}
    	BigInteger x = null;
    	
    	// Get values from the client
    	x = new BigInteger(s.read(), 16);
    	for(int i = 0; i < ksjs.size(); i++){
    		ais.add(new BigInteger(s.read(),16));
    	}
    	
    	// Add in our secrets
    	List<BigInteger> bis = new ArrayList<BigInteger>();
    	for(BigInteger ai: ais){
    		bis.add( ai.modPow(rs1, p) );
    	}
    	//TODO: Collections.shuffle(bis, r);
    	
    	List<BigInteger> tsjs = new ArrayList<BigInteger>();
    	for(BigInteger ksj : ksjs){
    		//TODO: Should this be a different hash? Yes
    		// This is the following calculation all mod p
    		// H(x^Rs * ksj )
    		tsjs.add(hash( x.modPow(rs, p).multiply(ksj).mod(p) ).mod(p));
    	}
    	
    	// Send back to the client
    	s.write(y.toString(16));
    	for( BigInteger bi : bis ){
    		s.write(bi.toString(16));
    	}
    	for( BigInteger tsj : tsjs ){
    		s.write(tsj.toString(16));
    	}
    	
    	
		return "Server's result: " + s.read(); // threshold this value
    }
    
    
    // Utility functions
	// Load the common inputs, p q and g which are:
	// p - a prime number
	// q - the sub prime
	// g - the base (generator)
	private void loadSharedKeys(){
		p = new BigInteger("b95b6c851ff243745411a0c901a14c217d429edba65b8a298534731e5c3182bf9806f592611bbf2ded9fc4a1b21acfe685112ec38d6d7c4b4bf28b5bcc636b6c4844fdcf449b002b4bc5143a32e0f7b713097b062683cc7cdaa7adfd6c49b0d897487d4e2d0c94bf0c8cafe11580cb84f14ca7922142503ee0dfc377591233c1", 16);
		q = new BigInteger("d9ad24d2728323f368eac50bb1e1154483d820b7", 16);
		g = new BigInteger("af3ecd5a39c2ec6fd3ebfd44a4e18a422429c3b18ec6a716968f0ea524f1e19a67f7e117211a802eaae551e4b43967b4b63a50ef6d2c31397a845456550eaa89d4fe8959e402e1484139e5ff52187882f25967ad10e294c7980dd678ebb2a592e031e75ada46d1c5af16caebcd86d06430de7e7ba6fb71590d7329ee744977dd", 16);
		
		//TODO: Load these keys from a file
		// TODO: Find a way to generate them?
	}
	
	//TODO: Do H and H' need to be seperate hashes? Yes, do this later using HMAC
	private BigInteger hash(byte [] input){
		MessageDigest digest = null;
		try {
			digest = MessageDigest.getInstance("SHA-256");
    	} catch (NoSuchAlgorithmException e1) {
    		Log.e(TAG, "SHA-256 is not supported");
    		return null; // TODO: Raise an error?
    	}
		digest.reset();
    	    
		return new BigInteger(digest.digest(input));
	}
	private BigInteger hash(String input){
		return hash(input.getBytes());
	}
	private BigInteger hash(BigInteger input){
		return hash(input.toByteArray());
	}
    
    private List<String> getMarkerLengths(){
    	//TODO: Implement Reading from SD (in an encyrpted fashion)
    
    	int[] markerLengths = {1,2,3,4,5};
    	String[] markerNames = {"Mark1","secondMarker","MarkNumber3", "Mark4", "Mark5"};
    	//int[] markerLengths = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
    	//markerLengths[1] = (int) (Math.random()*100.0);
    	//String[] markerNames = {"Mark1","SecondMarker","MarkNumber3","Mark4","Mark5","Mark6","Mark7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25"};
    	List<String> ret = new ArrayList<String>();
    	
    	for( int i = 0; i < markerLengths.length; i++ ){
    		ret.add(markerNames[i].concat( String.valueOf(markerLengths[i]) ));
    	}	
    	
    	return ret;
    }
    
    // Calculate a random number between 0 and range
    //TODO: We can't use securerandom for generating keys. THIS MUST BE CHANGED
    private BigInteger randomRange(BigInteger range){
    	//TODO: Is there anything else we should fall back on here perhaps openssl bn_range
    	//         another option is using an AES based key generator (the only algorithim supported by android)
    	
    	// TODO: Should we be keeping this rand around? 
    	SecureRandom rand = new SecureRandom();
    	BigInteger temp = new BigInteger(range.bitLength(), rand);
    	while(temp.compareTo(range) > 0)
    		temp = new BigInteger(range.bitLength(), rand);
    	return temp;
    	
    }
}
