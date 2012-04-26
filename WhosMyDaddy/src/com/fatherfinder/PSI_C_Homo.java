package com.fatherfinder;

import java.math.BigInteger;
import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import android.util.Log;

/**
 * Partialy-Homomorphic encryption version of PSI-C based on masked hamming distance
 * @author skyf
 *
 */

//TODO: Think about changing protocol so that both use the same key but mask with random bits
public class PSI_C_Homo extends PSI_C {
	
	// Debugging
    private final String TAG = "PSI_C";
    private final boolean D = true;
    
    private SecureRandom rand;
    
    public void onCreate(){
    	super.onCreate();
    	rand = new SecureRandom();
    }
	
    //TODO: Think about handling the actual encryption here
    //        Also think about moving this somewhere else
    // For now this class stores one encryption
    class Encryption {
    	private BigInteger c1;
    	private BigInteger c2;
    	
    	Encryption(BigInteger one, BigInteger two){
    		c1 = one;
    		c2 = two;
    	}
    	
    	public boolean isEncryptionOfOne(BigInteger key){
    		return c1.modPow(key, p).equals(c2);
    	}
    	
    	/**
    	 * Add other to self
    	 * @param o other
    	 * @return self
    	 */
    	//TODO: should this be unmutable?
    	public Encryption plus(Encryption o){
    		c1 = c1.add(o.c1);
    		c2 = c2.add(o.c2);
    		return this;
    	}
    	
    	/**
    	 * Subtract other from self
    	 * @param o other
    	 * @return self
    	 */
    	//TODO: Should this be unmutable?
    	public Encryption subtract(Encryption o){
    		c1 = c1.subtract(o.c1);
    		c2 = c2.subtract(o.c2);
    		return this;
    	}
    	
    	/**
    	 * Multiply other to self
    	 * @param o other
    	 * @return self
    	 */
    	public Encryption mult(BigInteger o){
    		c1 = c1.multiply(o);
    		c2 = c2.multiply(o);
    		return this;
    	}
    }
    
 // Actually perform the test (these will be overides from a testing base class (and can be the same function)
    protected String conductClientTest(BluetoothService s, List<String> inputs){
    	// OFFLINE PHASE
    	stopwatch.start();
    	BigInteger x  = randomRange(q); // Secret 1
    	
    	BigInteger y = g.modPow(x, p);
    	
    	// C = (c1, c2)
    	List<Encryption> Cs = new ArrayList<Encryption>(); // The set of encryptions of inputs
    	
    	
    	//TODO: In this encryption scheme what are the restrictions on R's
    	BigInteger h,r,c1,c2;
    	for( String input: inputs ){
    		h = hash(input);
    		
    		// Encrypt h
    		
    		//TODO: How big must R be?
    		r = new BigInteger(160, rand);
    		// g^r
    		c1 = g.modPow(r,p);
    		
    		// y^r * g ^ h
    		c2 = y.modPow(r, p).multiply(g.modPow(h,p));
    		
    		Cs.add(new Encryption(c1,c2));
    	}
   
    	stopwatch.stop();
    	Log.i(TAG, "Client offline phase completed in " + stopwatch.getElapsedTime() + " miliseconds.");
    	
    	// ONLINE PHASE
    	stopwatch.start();
    	
    	
    	s.write(y.toByteArray());
    	for( Encryption e : Cs ){
    		s.write(e.c1.toByteArray());
    		s.write(e.c2.toByteArray());
    	}
    	
    	int numCommon = 0;
    	
    	// Get values from the server and process
    	for(int i = 0; i < Cs.size(); i++){

    		if (new Encryption(s.readBigInteger(), s.readBigInteger()).isEncryptionOfOne(x))
    				numCommon++;
    	}
    	
    	// Send result
    	if(D) Log.d(TAG, "Client calculated: " + String.valueOf(numCommon));
    	s.write(String.valueOf(numCommon));
    	
    	stopwatch.stop();
        Log.i(TAG, "Client online phase completed in " + stopwatch.getElapsedTime() + " miliseconds.");
    	
		return String.valueOf(numCommon);
    }
    
    protected String conductServerTest(BluetoothService s, List<String> inputs) {
    	// OFFLINE PHASE
    	stopwatch.start();
    	// Generate the randoms for later along with the first half of the encryption 
    	List<BigInteger> rs = new ArrayList<BigInteger>();  // Randomness for encryption later
    	List<BigInteger> r1s = new ArrayList<BigInteger>(); // Randomness for masking later
    	List<BigInteger> c1s = new ArrayList<BigInteger>(); // First half of encryption 
    	List<BigInteger> hs = new ArrayList<BigInteger>();  // Hashed input

    	BigInteger r;
    	
    	for( String input: inputs ){
    		r = new BigInteger(160, rand);
    		
    		rs.add(r);
    		r1s.add(new BigInteger(80, rand));
    		c1s.add( g.modPow(r,p) );
    		hs.add(hash(input));
    	}
    	
    	stopwatch.stop();
    	Log.i(TAG, "Server offline phase completed in " + stopwatch.getElapsedTime() + " miliseconds.");
    	
    	// ONLINE PHASE
    	stopwatch.start();
    	
    	List<Encryption> es = new ArrayList<Encryption>(); // The set of return encryptions
    	BigInteger y = null;
    	
    	// This code has been reworked to be pipelined. The idea being, we start computation immediately 
    	//   after we receive the clients data. 
    	
    	// Start reading client data
    	y = new BigInteger(s.read());
    
    	BigInteger c2;
    	Encryption se, ce;
    	
    	for(int i = 0; i < hs.size(); i++){
    		// Compute our c2 and encryption
    		c2 = y.modPow(rs.get(i), p).multiply(g.modPow(hs.get(i),p));
    		se = new Encryption(c1s.get(i), c2);
    		
    		// Read a client encryption
    		ce = new Encryption(s.readBigInteger(), s.readBigInteger());
    		
    		// Compute difference and mask
    		es.add( ce.subtract(se).mult(r1s.get(i)) );
    	}
    	Collections.shuffle(es, rand);
    	
    	// Send back to the client
    	for( Encryption e: es ){
    		s.write(e.c1.toByteArray());
    		s.write(e.c2.toByteArray());
    	}
    	
    	stopwatch.stop();
        Log.i(TAG, "Server online phase completed in " + stopwatch.getElapsedTime()+ " miliseconds.");
    	
        // Return the result from the client
		return s.readString();
    }
}
