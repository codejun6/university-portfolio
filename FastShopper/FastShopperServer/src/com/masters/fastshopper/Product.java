package com.masters.fastshopper;

import java.io.Serializable;

public class Product implements Serializable {
	private static final long serialVersionUID = 1L;
	
	public static final int PURCHASE_NUM_NORMAL = 0;
	public static final int PURCHASE_NUM_EXCESS = 1;
	public static final int PURCHASE_NUM_ZERO = 2;
	
	private String code;
	private String name;
	private int price;
	private int stockNum;
	private int purchaseNum;
	private int purchasePrice;

	public Product(String code, String name, int price, int stockNum) {
		this.code = code;
		this.name = name;
		this.price = price;
		this.stockNum = stockNum;
		this.purchaseNum = 0;
		this.purchasePrice = 0;
	}
	public String toString() {
		return new String(code + ", " + name + ", " + price + ", " + stockNum + ", " + purchaseNum + ", " + purchasePrice + '\n');
	}
	public String toWorkedString() {
		return new String("[" + code + "]\n" + name + ", 가격: " + price + "원\n재고: "
				+ stockNum + "개, 구매 개수: " + purchaseNum + "개, 합계: " + purchasePrice + "원\n");
	}
	public String getCode() { return code; }
	public String getName() { return name; }
	public int getPrice() { return price; }
	public int getStockNum() { return stockNum; }
	public int getPurchaseNum() { return purchaseNum; }
	public int setPurchaseNum(int purchaseNum) {
		int msg;
		if(purchaseNum > stockNum) {
			this.purchaseNum = stockNum;
			msg = PURCHASE_NUM_EXCESS;
		} else if(purchaseNum <= 0) {
			this.purchaseNum = 1;
			msg = PURCHASE_NUM_ZERO;
		} else {
			this.purchaseNum = purchaseNum;
			msg = PURCHASE_NUM_NORMAL;
		}
		purchasePrice = this.purchaseNum * price;
		return msg;
	}
	public int getPurchasePrice() { return purchasePrice; }
	public int increasePurchaseNum() {
		return setPurchaseNum(purchaseNum+1);
	}
	public int decreasePurchaseNum() {
		return setPurchaseNum(purchaseNum-1);
	}
	public boolean equalsCode(Product product) {
		return this.code.equals(product.getCode());
	}
	public boolean equalsCodeNPurchaseNum(Product product) {
		if(code.equals(product.getCode()) && purchaseNum == product.getPurchaseNum()) return true;
		return false;
	}
}
