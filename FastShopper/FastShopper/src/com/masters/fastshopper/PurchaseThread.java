package com.masters.fastshopper;

import java.io.*;
import java.net.*;
import java.util.*;

import android.os.*;
import android.util.*;

public class PurchaseThread extends Thread {
	public static final int MODE_SEARCH_PRODUCT = 0;
	public static final int MODE_SEND_PURCHASE_LIST = 1;
	public static final int MODE_REQUEST_RESULT = 2;
	public static final int MODE_CANCEL_PURCHASE = 3;
	
	public static boolean mDoCancel = false;
	
	private Handler mHandler;
	private String mAddress;
	private int mPort;
	private String mCode;
	private ArrayList<Product> mProductList;
	private int mMode;

	private PurchaseThread(Handler handler, String address, int port) {
		mHandler = handler;
		mAddress = address;
		mPort = port;
	}
	public PurchaseThread(Handler handler, String address, int port, String code) {
		this(handler, address, port);
		mCode = code;
		mMode = MODE_SEARCH_PRODUCT;
	}
	public PurchaseThread(Handler handler, String address, int port, ArrayList<Product> productList) {
		this(handler, address, port);
		mProductList = productList;
		mMode = MODE_SEND_PURCHASE_LIST;
	}
	public PurchaseThread(Handler handler, String address, int port, boolean doCancel) {
		this(handler, address, port);
		if(!doCancel)
			mMode = MODE_REQUEST_RESULT;
		else
			mMode = MODE_CANCEL_PURCHASE;
	}
	public void run() {
		Socket soc = null;
		switch(mMode) {
		case MODE_SEARCH_PRODUCT :
			try {
				soc = new Socket(InetAddress.getByName(mAddress), mPort);
				BufferedWriter outCode = new BufferedWriter(new OutputStreamWriter(soc.getOutputStream()));
				outCode.write(mCode);
				outCode.flush();
				
				ObjectInputStream inProduct = new ObjectInputStream(new BufferedInputStream(soc.getInputStream()));
				Message msg = mHandler.obtainMessage();
				msg.what = MODE_SEARCH_PRODUCT;
				msg.obj = (Product)inProduct.readObject();
				mHandler.sendMessage(msg);
			} catch(Exception e) {
				Log.d("DBG", "PurchaseThread.MODE_SEARCH_PRODUCT Error: " + e.getMessage());
			} finally {
				try {
					if(soc != null)
						soc.close();
				} catch(Exception e) { }
			}
			break;
		case MODE_SEND_PURCHASE_LIST :
			try {
				soc = new Socket(InetAddress.getByName(mAddress), mPort);
				ObjectOutputStream outList = new ObjectOutputStream(new BufferedOutputStream(soc.getOutputStream()));
				outList.writeObject(mProductList);
				outList.flush();

				new PurchaseThread(mHandler, mAddress, 2402, false).start();
				mHandler.sendEmptyMessage(MODE_SEND_PURCHASE_LIST);
			} catch(Exception e) {
				Log.d("DBG", "PurchaseThread.MODE_SEND_PURCHASE_LIST Error: " + e.getMessage());
			} finally {
				try {
					if(soc != null)
						soc.close();
				} catch(Exception e) { }
			}
			break;
		case MODE_REQUEST_RESULT :
			while(true) {
				try {
					soc = new Socket(InetAddress.getByName(mAddress), mPort);
					DataInputStream inState = new DataInputStream(soc.getInputStream());
					Message msg = mHandler.obtainMessage();
					msg.what = MODE_REQUEST_RESULT;
					msg.arg1 = inState.readInt();
					mHandler.sendMessage(msg);

					if(msg.arg1 != 0 || mDoCancel) {
						mDoCancel = false;
						break;
					}
					Thread.sleep(1000);
				} catch(Exception e) {					
					Log.d("DBG", "PurchaseThread.MODE_REQUEST_RESULT Error: " + e.getMessage());
				} finally {
					try {
						if(soc != null)
							soc.close();
					} catch(Exception e) { }
				}
			}
			break;
		case MODE_CANCEL_PURCHASE :
			try {
				mDoCancel = true;
				
				soc = new Socket(InetAddress.getByName(mAddress), mPort);
				DataOutputStream outState = new DataOutputStream(soc.getOutputStream());
				outState.writeInt(2);
				outState.flush();
				
				mHandler.sendEmptyMessage(MODE_CANCEL_PURCHASE);
			} catch(Exception e) {
				Log.d("DBG", "PurchaseThread.MODE_CANCEL_PURCHASE Error: " + e.getMessage());
			} finally {
				try {
					if(soc != null)
						soc.close();
				} catch(Exception e) { }
			}
			break;
		}
	}
}
