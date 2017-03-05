package com.masters.fastshopper;

import java.util.*;

import android.app.*;
import android.content.*;
import android.content.DialogInterface.OnCancelListener;
import android.content.DialogInterface.OnDismissListener;
import android.os.*;
import android.text.*;
import android.util.*;
import android.view.*;
import android.view.View.OnClickListener;
import android.widget.*;

public class PurchaseListActivity extends Activity {
	private PurchaseListAdapter mPurchaseListAdapter;
	private ArrayList<Product> mPurchaseList;
	private String mServerAddress;
	private ProgressDialog mPurchaseProgress;
	
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.purchase_list_activity);
	    
	    // 서버 아이피 주소 설정
//	 	mServerAddress = "192.168.0.7";
	    
	    mPurchaseList = new ArrayList<Product>();
	    ((ImageButton)findViewById(R.id.btnPurchase)).setOnClickListener(new OnClickListener() {
	    	public void onClick(View v) {
	    		if(mPurchaseList.isEmpty())
	    			Toast.makeText(PurchaseListActivity.this, "구매할 상품이 없습니다.", Toast.LENGTH_SHORT).show();
	    		else
	    			new PurchaseThread(mHandler, mServerAddress, 2401, mPurchaseList).start();
	    	}
	    });
	    mPurchaseListAdapter = new PurchaseListAdapter(PurchaseListActivity.this, R.layout.purchase_list_item_layout, mPurchaseList);
	    ((ListView)findViewById(R.id.listPurchase)).setAdapter(mPurchaseListAdapter);
	}
	public void onResume() {
		super.onResume();

		SharedPreferences pref = getSharedPreferences("BarcodeScan", 0);
		String strScan = pref.getString("ScannedCode", "");
		if(strScan.equals("") == false) {
			new PurchaseThread(mHandler, mServerAddress, 2400, strScan).start();
			pref.edit().clear().commit();
		}
		
		// 설정탭에서 서버를 설정하던 부분
		pref = getSharedPreferences("SettingServer", 0);
		mServerAddress = pref.getString("ServerAddress", "");

		PurchaseListActivity.this.printTotalPurchasePrice();
	}

	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch(msg.what) {
			case PurchaseThread.MODE_SEARCH_PRODUCT :
				Product productTmp = (Product)msg.obj;
				if(productTmp != null) {
					boolean notExisting = true;
					for(int i = 0; i < mPurchaseList.size(); ++i) {
						if(productTmp.equalsCode(mPurchaseList.get(i))) {
							notExisting = false;
							int excessCase = mPurchaseList.get(i).increasePurchaseNum();
							if(excessCase == Product.PURCHASE_NUM_EXCESS) {
								Toast.makeText(PurchaseListActivity.this, "해당 상품의 개수를 더 이상 늘릴 수 없습니다.", Toast.LENGTH_SHORT).show();
							} else
								Toast.makeText(PurchaseListActivity.this, "해당 상품의 개수가 1개 늘었습니다.", Toast.LENGTH_SHORT).show();
							break;
						}
					}
					if(notExisting) {
						productTmp.setPurchaseNum(1);
						mPurchaseList.add(productTmp);

						Toast.makeText(PurchaseListActivity.this, "해당 상품이 목록에 추가되었습니다.", Toast.LENGTH_SHORT).show();
					}
				} else
					Toast.makeText(PurchaseListActivity.this, "해당 상품은 없는 상품입니다.", Toast.LENGTH_SHORT).show();
				break;
			case PurchaseThread.MODE_SEND_PURCHASE_LIST :
				mPurchaseProgress = ProgressDialog.show(PurchaseListActivity.this, "", "결제 대기 중입니다.", true, true);
				mPurchaseProgress.setOnCancelListener(new OnCancelListener() {
					public void onCancel(DialogInterface dialog) {
						new PurchaseThread(mHandler, mServerAddress, 2403, true).start();
					}
				});
				break;
			case PurchaseThread.MODE_REQUEST_RESULT :
				if(msg.arg1 != 0) {
					if(mPurchaseProgress != null)
						mPurchaseProgress.dismiss();
	
					if(msg.arg1 == 1) {
						mPurchaseList.clear();
						Toast.makeText(PurchaseListActivity.this, "구매가 완료되었습니다.", Toast.LENGTH_SHORT).show();
					} else if(msg.arg1 == 2) {
						Toast.makeText(PurchaseListActivity.this, "구매가 취소되었습니다.", Toast.LENGTH_SHORT).show();
					}
				}
				break;
			case PurchaseThread.MODE_CANCEL_PURCHASE :
				if(mPurchaseProgress != null)
					mPurchaseProgress.dismiss();
				Toast.makeText(PurchaseListActivity.this, "구매를 취소하셨습니다.", Toast.LENGTH_SHORT).show();
				break;
			}
			PurchaseListActivity.this.printTotalPurchasePrice();
			mPurchaseListAdapter.notifyDataSetChanged();
		}
	};

	private class PurchaseListAdapter extends BaseAdapter {
		private Context context;
		private int layout;
		private ArrayList<Product> purchaseList;

		public PurchaseListAdapter(Context context, int layout, ArrayList<Product> purchaseList) {
			this.context = context;
			this.layout = layout;
			this.purchaseList = purchaseList;
		}
		public int getCount() {
			return purchaseList.size();
		}
		public Product getItem(int position) {
			return purchaseList.get(position);
		}
		public long getItemId(int position) {
			return position;
		}
		public View getView(int position, View convertView, ViewGroup parent) {
			if(convertView == null)
				convertView = ((LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE)).inflate(layout, parent, false);

			final int pos = position;

			TextView txtItemTitle = (TextView)convertView.findViewById(R.id.txtItemTitle);
			txtItemTitle.setText(purchaseList.get(position).getName());

			TextView txtItemPrice = (TextView)convertView.findViewById(R.id.txtItemPrice);
			txtItemPrice.setText("가격: " + purchaseList.get(position).getPrice() + "원");
			TextView txtItemNum = (TextView)convertView.findViewById(R.id.txtItemNum);
			txtItemNum.setText("   수량: " + purchaseList.get(position).getPurchaseNum() + "개");
			TextView txtItemNumPrice = (TextView)convertView.findViewById(R.id.txtItemNumPrice);
			txtItemNumPrice.setText("   합계: " + purchaseList.get(position).getPurchasePrice() + "원");

			((ImageButton)convertView.findViewById(R.id.btnItemNumPlus)).setOnClickListener(new OnClickListener() {
				public void onClick(View v) {
					int excessCase = purchaseList.get(pos).increasePurchaseNum();
					if(excessCase == Product.PURCHASE_NUM_EXCESS)
						Toast.makeText(PurchaseListActivity.this, "해당 상품의 개수를 더 이상 늘릴 수 없습니다.", Toast.LENGTH_SHORT).show();
					PurchaseListActivity.this.printTotalPurchasePrice();
					PurchaseListAdapter.this.notifyDataSetChanged();
				}
			});
			((ImageButton)convertView.findViewById(R.id.btnItemNumMinus)).setOnClickListener(new OnClickListener() {
				public void onClick(View v) {
					int removeCase = purchaseList.get(pos).decreasePurchaseNum();
					if(removeCase == Product.PURCHASE_NUM_ZERO) {
						purchaseList.remove(pos);
						Toast.makeText(context, "해당 상품이 삭제되었습니다.", Toast.LENGTH_SHORT).show();
					}
					PurchaseListActivity.this.printTotalPurchasePrice();
					PurchaseListAdapter.this.notifyDataSetChanged();
				}
			});
			((ImageButton)convertView.findViewById(R.id.btnItemRemove)).setOnClickListener(new OnClickListener() {
				public void onClick(View v) {
					purchaseList.remove(pos);
					Toast.makeText(context, "해당 상품이 삭제되었습니다.", Toast.LENGTH_SHORT).show();
					PurchaseListActivity.this.printTotalPurchasePrice();
					PurchaseListAdapter.this.notifyDataSetChanged();
				}
			});

			return convertView;
		}
	}
	
	private void printTotalPurchasePrice() {
		int sum = 0;
		for(int i = 0; i < mPurchaseList.size(); ++i)
			sum += mPurchaseList.get(i).getPurchasePrice();
		((TextView)findViewById(R.id.txtItemTotalPrice)).setText("총 금액: " + sum + "원");
	}
}