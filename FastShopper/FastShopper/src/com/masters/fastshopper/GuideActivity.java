package com.masters.fastshopper;

import java.util.*;

import android.app.Activity;
import android.content.*;
import android.os.Bundle;
import android.view.*;
import android.view.View.OnClickListener;
import android.widget.*;
import android.view.inputmethod.InputMethodManager;

public class GuideActivity extends Activity {
	private GuideListAdapter mGuideListAdapter;
	
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.guide_activity);
   
	    String[] arrCategory = new String[] {
	    		"일회용품", "생활용품", "건어물", "즉석식품", "가공식품", "육류", "신선식품", "과일", "스낵",
	    		"맥도날드", "푸드코트", "침구류", "장난감", "문구류", "주방용품", "차량용품", "가전제품",
	    		"전문식당", "문화용품", "패션잡화"
	    };
	    ImageView[] arrLocation = new ImageView[arrCategory.length];
	    int[] resLocation = new int[] {
	    		R.drawable.img_1st_floor_1, R.drawable.img_1st_floor_2, R.drawable.img_1st_floor_3,
	    		R.drawable.img_1st_floor_4, R.drawable.img_1st_floor_5, R.drawable.img_1st_floor_6,
	    		R.drawable.img_1st_floor_7, R.drawable.img_1st_floor_8, R.drawable.img_1st_floor_9,
	    		R.drawable.img_1st_floor_10, R.drawable.img_1st_floor_11, R.drawable.img_2nd_floor_1,
	    		R.drawable.img_2nd_floor_2, R.drawable.img_2nd_floor_3, R.drawable.img_2nd_floor_4,
	    		R.drawable.img_2nd_floor_5, R.drawable.img_2nd_floor_6, R.drawable.img_2nd_floor_7,
	    		R.drawable.img_2nd_floor_8, R.drawable.img_2nd_floor_9
	    };
//	    for(int i = 0; i < arrLocation.length; ++i) {
//	    	arrLocation[i] = new ImageView(this);
//	    	arrLocation[i].setImageResource(resLocation[i]);
//	    }	    
	    
	    mGuideListAdapter = new GuideListAdapter(this, arrCategory, arrLocation, resLocation);
	    ((ExpandableListView)findViewById(R.id.listProductLocation)).setAdapter(mGuideListAdapter);
	}
	
	private class GuideListAdapter extends BaseExpandableListAdapter {
		private Context context;
		private String[] category;
		private ImageView[] location;
		private int[] resource;

		public GuideListAdapter(Context context, String[] category, ImageView[] location, int[] resource) {
			this.context = context;
			this.category = category;
			this.location = location;
			this.resource = resource;
		}
		public Object getChild(int groupPosition, int childPosition) {
			return location[groupPosition];
		}
		public long getChildId(int groupPosition, int childPosition) {
			return childPosition;
		}
		public View getChildView(int groupPosition, int childPosition,
				boolean isLastChild, View convertView, ViewGroup parent) {
			if(convertView == null) {
				convertView = ((LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE))
						.inflate(R.layout.guide_location_layout, null);
			}
			
			ImageView imgLocation = (ImageView)convertView.findViewById(R.id.imgGuideLocation);
			imgLocation.setImageResource(resource[groupPosition]);
			
			return convertView;
		}
		public int getChildrenCount(int groupPosition) {
			return 1;
		}
		public Object getGroup(int groupPosition) {
			return category[groupPosition];
		}
		public int getGroupCount() {
			return category.length;
		}
		public long getGroupId(int groupPosition) {
			return groupPosition;
		}
		public View getGroupView(int groupPosition, boolean isExpanded,
				View convertView, ViewGroup parent) {
			if(convertView == null) {
				convertView = ((LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE))
						.inflate(android.R.layout.simple_expandable_list_item_2, null);
			}
			
			TextView txtCate = (TextView)convertView.findViewById(android.R.id.text1);
			txtCate.setTextColor(getResources().getColor(android.R.color.black));
			txtCate.setText((String)getGroup(groupPosition));
			
			return convertView;
		}
		public boolean hasStableIds() {
			return true;
		}
		public boolean isChildSelectable(int groupPosition, int childPosition) {
			return false;
		}
	}
}