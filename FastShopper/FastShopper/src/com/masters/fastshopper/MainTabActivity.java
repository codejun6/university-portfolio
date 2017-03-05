package com.masters.fastshopper;

import java.util.*;

import android.app.*;
import android.content.*;
import android.os.Bundle;
import android.view.*;
import android.view.View.*;
import android.view.inputmethod.*;
import android.widget.*;
import android.widget.LinearLayout.*;

public class MainTabActivity extends TabActivity {
	private TabManager mTabManager;
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.main_tab_activity);
		
		startActivity(new Intent(MainTabActivity.this, LoadingActivity.class));

		mTabManager = new TabManager(MainTabActivity.this, getTabHost(), R.id.tabWidget);
		mTabManager.addTab("BarcodeTab", R.drawable.tabbtn_scan_selector, BarcodeActivity.class);
		mTabManager.addTab("PurchaseListTab", R.drawable.tabbtn_list_selector, PurchaseListActivity.class);
		mTabManager.addTab("GuideTab", R.drawable.tabbtn_guide_selector, GuideActivity.class);
		mTabManager.addTab("SettingTab", R.drawable.tabbtn_setting_selector, SettingActivity.class);
		mTabManager.setCurrentTab(0);	
	}
	public void onResume() {
		super.onResume();
		registerReceiver(mBarcodeScanBR, new IntentFilter("com.masters.fastshopper.BARCODE_SCAN"));
	}
	public void onPause() {
		super.onPause();
		unregisterReceiver(mBarcodeScanBR);
	}
	
	private BroadcastReceiver mBarcodeScanBR = new BroadcastReceiver() {
		public void onReceive(Context context, Intent intent) {
			mTabManager.setCurrentTab(1);
		}
	};
	
	private class TabManager {
		private LinearLayout mTabWidgetGroup;
		private ArrayList<Button> mTabButtonList;
		private Context mContext;
		private TabHost mTabHost;

		public TabManager(Context context, TabHost tabHost, int tabWidgetId) {
			mContext = context;
			mTabWidgetGroup = (LinearLayout)findViewById(tabWidgetId);
			mTabButtonList = new ArrayList<Button>();
			mTabHost = tabHost;
		}
		public void addTab(String tabSpec, int bgRes, Class<? extends Activity> activityClass) {
			Button tabButton = new Button(mContext);
			tabButton.setOnClickListener(mTabClickListener);
			tabButton.setBackgroundResource(bgRes);

			LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(0, LayoutParams.MATCH_PARENT, 1);
			mTabHost.addTab(mTabHost.newTabSpec(tabSpec).setIndicator("").setContent(new Intent(mContext, activityClass)));
			mTabButtonList.add(tabButton);
			mTabWidgetGroup.addView(tabButton, params);
		}
		public void setCurrentTab(int index) {
			mTabHost.setCurrentTab(index);
			for (Button tabButton : mTabButtonList) {
				tabButton.setSelected(false);
			}
			mTabButtonList.get(index).setSelected(true);
		}

		private OnClickListener mTabClickListener = new OnClickListener() {
			public void onClick(View v) {
				InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
				imm.hideSoftInputFromWindow(mTabWidgetGroup.getWindowToken(), 0);
				setCurrentTab(mTabButtonList.indexOf(v));
			}
		};
	}
}