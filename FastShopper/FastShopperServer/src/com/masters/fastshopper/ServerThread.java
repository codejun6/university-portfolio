package com.masters.fastshopper;

import java.io.*;
import java.net.*;
import java.util.*;

public class ServerThread extends Thread {
	public static final int MODE_SEARCH_PRODUCT = 0;
	public static final int MODE_ACCEPT_LIST = 1;
	public static final int MODE_CONFIRM_PURCHASE = 2;
	public static final int MODE_CANCEL_PURCHASE = 3;
	
	private ServerSocket server;
	private Socket soc;
	private ProductDBManager pm;
	private UpdateAdaptor view;
	private int mode;
	private int port;
	private int confirmState; // 0: 대기, 1: 확인, 2: 취소
	
	public ServerThread(ProductDBManager pm, UpdateAdaptor view, int mode, int port) {
		this.pm = pm;
		this.view = view;
		this.mode = mode;
		this.port = port;
		confirmState = 0;
	}
	public void setConfirmState(int confirmState) {
		this.confirmState = confirmState;
	}
	public void run() {
		////////////////
//		StringBuilder sb = new StringBuilder();
//		int sumPrice = 0;
//		for(int i = 0; i < pm.getProductList().size(); ++i) {
//			sb.append(pm.getProductList().get(i).toWorkedString());
//			sumPrice += pm.getProductList().get(i).getPurchasePrice();
//		}
//		view.setAction("PrintPurchaseList").setMessage(sb.toString()).setInt(sumPrice).update();
		////////////////
		
		try {
			server = new ServerSocket(port);

			while(true) {
				soc = server.accept();
				switch(mode) {
				case MODE_SEARCH_PRODUCT :
					this.doModeSearchProduct();
					break;
				case MODE_ACCEPT_LIST :
					this.doModeAcceptList();
					break;
				case MODE_CONFIRM_PURCHASE :
					this.doModeConfirmPurchase();
					break;
				case MODE_CANCEL_PURCHASE :
					this.doModeCancelPurchase();
					break;
				}
				soc.close();
			}
		} catch(Exception e) {
			view.setAction("LogPrint").setMessage("소켓 통신 중 문제가 생겼습니다.\n").update();
		}
	}
	
	private void doModeSearchProduct() {
		try {
			BufferedReader inCode = new BufferedReader(new InputStreamReader(soc.getInputStream()));
			String strInCode = inCode.readLine();
			Product searchedProduct = pm.searchProduct(strInCode);
			
			ObjectOutputStream outProduct = new ObjectOutputStream(new BufferedOutputStream(soc.getOutputStream()));
			outProduct.writeObject(searchedProduct);
			outProduct.flush();

			StringBuilder sb = new StringBuilder();
			if(strInCode.equals(""))
				strInCode = "null";
			sb.append("검색 시도: " + strInCode + '\n');
			if(searchedProduct != null)
				sb.append("검색 결과: " + searchedProduct.toString());
			else
				sb.append("검색 결과: null\n");
			view.setAction("Print").setMessage(sb.toString()).update();
		} catch(Exception e) { }
	}
	private void doModeAcceptList() {
		try {
			ObjectInputStream inList = new ObjectInputStream(new BufferedInputStream(soc.getInputStream()));
			ArrayList<Product> acceptedPurchaseList = (ArrayList<Product>)inList.readObject();

			StringBuilder sb = new StringBuilder();
			sb.append("구매 시도:\n");
			for(int i = 0; i < acceptedPurchaseList.size(); ++i)
				sb.append(acceptedPurchaseList.get(i).toString());
			view.setAction("Print").setMessage(sb.toString()).update();
			
			sb = new StringBuilder();
			int sumPrice = 0;
			for(int i = 0; i < acceptedPurchaseList.size(); ++i) {
				sb.append(acceptedPurchaseList.get(i).toWorkedString());
				sumPrice += acceptedPurchaseList.get(i).getPurchasePrice();
			}
			view.setAction("PrintPurchaseList").setMessage(sb.toString()).setInt(sumPrice).update();
			view.setAction("ReadyConfirmation").update();
			view.setAction("LogPrint").setMessage("구매 목록을 받았습니다.\n").update();
		} catch(Exception e) { }
	}
	private void doModeConfirmPurchase() {
		try {
			DataOutputStream outState = new DataOutputStream(soc.getOutputStream());
			outState.writeInt(confirmState);
			outState.flush();
			
			if(confirmState != 0) {
				view.setAction("CompleteConfirmation").setInt(confirmState).update();
				confirmState = 0;
			}
		} catch(Exception e) { }
	}
	private void doModeCancelPurchase() {
		try {
			DataInputStream inState = new DataInputStream(soc.getInputStream());
			if(inState.readInt() == 2) {
				view.setAction("CompleteConfirmation").setInt(2).update();
				confirmState = 0;
			}
		} catch(Exception e) { }
	}
}
