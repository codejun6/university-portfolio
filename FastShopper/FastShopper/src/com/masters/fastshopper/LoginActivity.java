package com.masters.fastshopper;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;

public class LoginActivity extends Activity {
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		startActivity(new Intent(LoginActivity.this, LoadingActivity.class));
		setContentView(R.layout.login_activity);
		
		((Button)findViewById(R.id.btnLogin)).setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				switch(v.getId()) {
				case R.id.btnLogin:
					startActivity(new Intent(LoginActivity.this, MainTabActivity.class));
					break;
				}
			}
		});
	}
}