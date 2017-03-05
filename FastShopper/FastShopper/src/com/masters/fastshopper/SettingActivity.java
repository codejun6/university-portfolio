package com.masters.fastshopper;

import android.app.Activity;
import android.content.*;
import android.os.Bundle;
import android.view.*;
import android.view.View.OnClickListener;
import android.widget.*;
import android.view.inputmethod.InputMethodManager;

public class SettingActivity extends Activity {
	SharedPreferences mPref;
	
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.setting_activity);
	    
	    ((Button)findViewById(R.id.btnSettingServer)).setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				mPref = getSharedPreferences("SettingServer", 0);
				String address = ((EditText)findViewById(R.id.editSettingServer)).getText().toString();
				mPref.edit().putString("ServerAddress", address).commit();
				
				((InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(
						((EditText)findViewById(R.id.editSettingServer)).getWindowToken(), 0);
				Toast.makeText(SettingActivity.this, "서버가 설정되었습니다.", Toast.LENGTH_SHORT).show();
			}
	    });
	}
	public void onResume() {
		super.onResume();
		
		mPref = getSharedPreferences("SettingServer", 0);
		((EditText)findViewById(R.id.editSettingServer)).setText(mPref.getString("ServerAddress", ""));
	}
}