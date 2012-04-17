package com.fatherfinder;

import java.math.BigInteger;
import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import android.util.Log;

//TODO: Finish implementing this!!

public class PSI extends AbstractPSIProtocol {
	// Debugging
    private final String TAG = "PSI";
    private final boolean D = true;
    
	@Override
	protected String conductClientTest(BluetoothService s, List<String> input) {
		// OFFLINE PHASE
    	stopwatch.start();
    	BigInteger rc  = randomRange(q); // Secret 1
    	BigInteger rc1 = randomRange(q); // Secret 2
    	
    	BigInteger x = g.modPow(rc, p);
    	
    	List<BigInteger> ais = new ArrayList<BigInteger>(); // The set {a1,a2,...,ai}
    	for( String marker: input ){
    		ais.add(hash(marker).modPow(rc1, p));
    	}
   
    	stopwatch.stop();
    	Log.i(TAG, "Client offline phase completed in " + stopwatch.getElapsedTime() + " miliseconds.");
    	
    	// ONLINE PHASE
    	stopwatch.start();
    	
    	
    	// This code has been pipelined (see note in server code)
    	
    	s.write(x.toByteArray());
    	for( BigInteger ai : ais ){
    		s.write(ai.toByteArray());
    	}
    
    	List<BigInteger> tsjs = new ArrayList<BigInteger>(); // The set {ts1, ts2, ..., tsj}
    	List<BigInteger> tcis = new ArrayList<BigInteger>(); //Will store the clients processed set
    	BigInteger y = null;
    	
    	// Get values from the server and process immediately 
    	y = new BigInteger(s.read());
    	for(int i = 0; i < ais.size(); i++){
    		// This is the following calculation all mod p
    		// H(y^Rc * bi^(1/Rc') )
    		tcis.add(hash( y.modPow(rc, p).multiply((new BigInteger(s.read())).modPow(rc1.modInverse(q), p)).mod(p) ) );
    	}
    	
    	for(int i = 0; i < ais.size(); i++){
    		tsjs.add(new BigInteger(s.read()));
    	}
    	
    	// tcis = tcis ^ tsjs (intersection)
    	//tcis.retainAll(tsjs);
    	
    	//int sharedLengths = tcis.size();
    	
    	// Send result
    	//if(D) Log.d(TAG, "Client calculated: " + String.valueOf(sharedLengths));
    	//s.write(String.valueOf(sharedLengths));
    	
    	//TODO: Write custom intersection and keep hash table around for restructuring result
    	
    	stopwatch.stop();
        Log.i(TAG, "Client online phase completed in " + stopwatch.getElapsedTime() + " miliseconds.");
    	
		//return String.valueOf(sharedLengths);
        return null;
	}

	@Override
	protected String conductServerTest(BluetoothService s, List<String> input) {
		// OFFLINE PHASE
    	stopwatch.start();
    	BigInteger rs  = randomRange(q); // Secret 1
    	BigInteger rs1 = randomRange(q); // Secret 2
    	    	
    	BigInteger y = g.modPow(rs, p);
    	
    	List<BigInteger> ksjs = new ArrayList<BigInteger>(); // The set {ks1,ks2,...,ksi}
    	for( String marker: input ){
    		ksjs.add(hash(marker).modPow(rs1, p));
    	}
    	
    	SecureRandom r = new SecureRandom();
    	Collections.shuffle(ksjs, r);
    	
    	stopwatch.stop();
    	Log.i(TAG, "Server offline phase completed in " + stopwatch.getElapsedTime() + " miliseconds.");
    	
    	// ONLINE PHASE
    	stopwatch.start();
    	
    	List<BigInteger> ais = new ArrayList<BigInteger>(); // The set {a1,a2,...,ai}
    	BigInteger x = null;
    	
    	// Start reading client data
    	x = new BigInteger(s.read());
    	
    	s.write(y.toByteArray());
    	for(int i = 0; i < ksjs.size(); i++){
    		// Read an ai
    		ais.add(new BigInteger(s.read()));
    		
    		// Add our secret and send
    		s.write(ais.get(i).modPow(rs1, p).toByteArray());
    	}
    	
    	for(BigInteger ksj : ksjs){
    		// This is the following calculation all mod p
    		// H(x^Rs * ksj )
    		s.write((hash( x.modPow(rs, p).multiply(ksj).mod(p) ).toByteArray()));
    	}
    	
    	
    	stopwatch.stop();
        Log.i(TAG, "Server online phase completed in " + stopwatch.getElapsedTime()+ " miliseconds.");
    	
		return "Server's result: " + s.readString(); // threshold this value
	}

}