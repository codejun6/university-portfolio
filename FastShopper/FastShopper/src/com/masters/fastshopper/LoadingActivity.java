package com.masters.fastshopper;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Window;

public class LoadingActivity extends Activity {
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    requestWindowFeature(Window.FEATURE_NO_TITLE);
	    setContentView(R.layout.loading_activity);
	    
	    Handler handler  = new Handler();
	    handler.postDelayed(new Runnable() {
	    	public void run() { finish();	}
	    }, 1500);
	}
}
