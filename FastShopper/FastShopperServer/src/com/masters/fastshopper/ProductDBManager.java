package com.masters.fastshopper;

import java.io.*;
import java.util.*;

public class ProductDBManager {
	private ArrayList<Product> productList, acceptedProductList, scannedProductList;
	
	public ProductDBManager() {
		productList = new ArrayList<Product>();
		readProduct();
	}
	public ArrayList<Product> getProductList() { return productList; }
	public ArrayList<Product> getAcceptedProductList() { return acceptedProductList; }
	public ArrayList<Product> getScannedProductList() { return scannedProductList; }
	public void setAcceptedProductList(ArrayList<Product> productList) { acceptedProductList = productList; }
	public void setScannedProductList(ArrayList<Product> productList) { scannedProductList = productList; }
	public String toString() {
		StringBuilder sb = new StringBuilder();
		for(int i = 0; i < productList.size(); ++i)
			sb.append(productList.get(i).toString());
		return sb.toString();
	}
	public Product searchProduct(String code) {
		for(int i = 0; i < productList.size(); ++i) {
			if(code.equals(productList.get(i).getCode()))
				return productList.get(i);
		}
		return null;
	}
	public boolean checkProductList(ArrayList<Product> counter, ArrayList<Product> client) {
		if(counter.size() != client.size()) return false;
		int j, size = counter.size();
		for(int i = 0; i < size; ++i) {
			for(j = 0; j < size; ++j) {
				if(counter.get(i).equalsCodeNPurchaseNum(client.get(j)))
					break;
			}
			if(j == counter.size())
				return false;
		}
		return true;
	}

	private void readProduct() {
		try {
			BufferedReader in = new BufferedReader(new InputStreamReader(
					new FileInputStream("c:\\fs_product_data.txt"), "EUC-KR"));
			
			String[] strArr = new String[4];
			String strRead = in.readLine();
			while(strRead != null) {
				StringTokenizer st = new StringTokenizer(strRead, "@");
				for(int i = 0; i < strArr.length; ++i) {
					strArr[i] = st.nextToken();
				}
				productList.add(new Product(strArr[0], strArr[1], Integer.parseInt(strArr[2]), Integer.parseInt(strArr[3])));
				strRead = in.readLine();
			}

			in.close();
		} catch(IOException e) { }
	}
}
