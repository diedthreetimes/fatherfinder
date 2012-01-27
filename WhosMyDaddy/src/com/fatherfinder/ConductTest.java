package com.fatherfinder;

import android.app.Activity;
import android.os.Bundle;

/**
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
 * @author skyf
 *
 */

public class ConductTest extends Activity {
	/**
	 * @see android.app.Activity#onCreate(Bundle)
	 */
	@Override protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// TODO Put your code here
	}
}
