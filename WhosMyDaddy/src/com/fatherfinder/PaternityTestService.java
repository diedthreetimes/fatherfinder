package com.fatherfinder;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

/**
 * This class does all the work for conducting the secure communication
 * with another user. It does not handle any communication directly, but instead
 * relies on a Object that implements an asyncronous write method.
 * @author skyf
 *
 */
public class PaternityTestService extends Service {
	
	/**
	 * @see android.app.Service#onBind(Intent)
	 */
	@Override public IBinder onBind(Intent intent) {
		// TODO Put your code here
		return null;
	}
}
