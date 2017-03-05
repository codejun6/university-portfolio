package com.masters.fastshopper;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

class CounterFrame extends JFrame implements ActionListener {
	private static final long serialVersionUID = 1L;
	
	private GridBagLayout layoutGrid;
	private GridBagConstraints layoutGridCon;
	private JPanel panLogArea, panScanArea, panBtn;
	private JScrollPane scrollTextAccept, scrollTextScan, scrollTextLog;
	private JTextArea textAccept, textScan, textLog;
	private JButton btnStartServer, btnLogClear, btnConfirmPurchase, btnCancelPurchase, btnScanClear;
	private JLabel labelAccept, labelScan, labelTotalPrice;

	private ProductDBManager pdbm;
	private ServerThread serverSearchProduct, serverAcceptList, serverConfirmPurchase, serverCancelPurchase;
	private ArrayList<Product> scannedProductList;
	private StringBuilder scannedBarcode;

	public CounterFrame() {
		super("Fast Shopper Counter");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setSize(1000, 800);
//		setResizable(false);
		Dimension dim = Toolkit.getDefaultToolkit().getScreenSize();
        Dimension dimFrame = this.getSize();
        setLocation((int)((dim.getWidth() / 2) - (dimFrame.getWidth() / 2)),
        		(int)((dim.getHeight() / 2) - (dimFrame.getHeight() / 2)));
        
        pdbm = new ProductDBManager();
		serverSearchProduct = new ServerThread(pdbm, adaptor, ServerThread.MODE_SEARCH_PRODUCT, 2400);
		serverAcceptList = new ServerThread(pdbm, adaptor, ServerThread.MODE_ACCEPT_LIST, 2401);
		serverConfirmPurchase = new ServerThread(pdbm, adaptor, ServerThread.MODE_CONFIRM_PURCHASE, 2402);
		serverCancelPurchase = new ServerThread(pdbm, adaptor, ServerThread.MODE_CANCEL_PURCHASE, 2403);
		scannedProductList = new ArrayList<Product>();
		scannedBarcode = new StringBuilder();

		labelAccept = new JLabel("데이터 로그", JLabel.CENTER);
		labelAccept.setFont(new Font(Font.DIALOG, Font.BOLD, 18));
		labelScan = new JLabel("구매 목록", JLabel.CENTER);
		labelScan.setFont(new Font(Font.DIALOG, Font.BOLD, 18));
		
		textAccept = new JTextArea();
		textAccept.setEditable(false);
		scrollTextAccept = new JScrollPane(textAccept);
		textLog = new JTextArea();
		textLog.setEditable(false);
		textLog.setFont(new Font(Font.DIALOG, Font.BOLD, 20));
		scrollTextLog = new JScrollPane(textLog);
		panLogArea = new JPanel();
		layoutGrid = new GridBagLayout();
		layoutGridCon = new GridBagConstraints();
		addGridComp(panLogArea, scrollTextAccept, 1.0, 1.0, 0, 0, 1, 1);
		addGridComp(panLogArea, scrollTextLog, 1.0, 1.0, 0, 1, 1, 1);
		panLogArea.setLayout(layoutGrid);

		textScan = new JTextArea();
		textScan.setEditable(false);
		textScan.setFont(new Font(Font.DIALOG_INPUT, Font.BOLD, 20));
		scrollTextScan = new JScrollPane(textScan);
		labelTotalPrice = new JLabel("  합계: 0원", JLabel.LEFT);
		labelTotalPrice.setFont(new Font(Font.DIALOG_INPUT, Font.BOLD, 30));
		labelTotalPrice.setOpaque(true);
		labelTotalPrice.setBackground(Color.WHITE);
		panScanArea = new JPanel();
		layoutGrid = new GridBagLayout();
		layoutGridCon = new GridBagConstraints();
		addGridComp(panScanArea, scrollTextScan, 1.0, 1.5, 0, 0, 1, 1);
		addGridComp(panScanArea, labelTotalPrice, 1.0, 0.1, 0, 1, 1, 1);
		panScanArea.setLayout(layoutGrid);
		
		btnConfirmPurchase = new JButton("구매 확인");
		btnConfirmPurchase.setFont(new Font(Font.DIALOG, Font.BOLD, 20));
		btnConfirmPurchase.setEnabled(false);
		btnConfirmPurchase.addActionListener(this);
		btnCancelPurchase = new JButton("구매 취소");
		btnCancelPurchase.setFont(new Font(Font.DIALOG, Font.BOLD, 20));
		btnCancelPurchase.setEnabled(false);
		btnCancelPurchase.addActionListener(this);
		btnLogClear = new JButton("로그 지우기");
		btnLogClear.setFont(new Font(Font.DIALOG, Font.BOLD, 20));
		btnLogClear.addActionListener(this);
		btnStartServer = new JButton("서버 시작");
		btnStartServer.setFont(new Font(Font.DIALOG, Font.BOLD, 20));
		btnStartServer.addActionListener(this);
		btnScanClear = new JButton("스캔 목록 초기화");
		btnScanClear.setFont(new Font(Font.DIALOG, Font.BOLD, 20));
		btnScanClear.addActionListener(this);
		panBtn = new JPanel(new GridLayout(4, 1));
		panBtn.add(btnConfirmPurchase);
		panBtn.add(btnCancelPurchase);
//		panBtn.add(btnScanClear);
		panBtn.add(btnLogClear);
		panBtn.add(btnStartServer);

		layoutGrid = new GridBagLayout();
		layoutGridCon = new GridBagConstraints();
		addGridComp(this, labelAccept, 1.0, 0.1, 0, 0, 1, 1);
		addGridComp(this,  labelScan, 1.0, 0.1, 1, 0, 1, 1);
		addGridComp(this, panLogArea, 1.2, 6.0, 0, 1, 1, 1);
		addGridComp(this, panScanArea, 2.0, 6.0, 1, 1, 1, 1);
        addGridComp(this, panBtn, 0.1, 1.0, 2, 0, 1, 2);
		setLayout(layoutGrid);
		setVisible(true);

//		textScan.addKeyListener(new KeyAdapter() {
//			public void keyReleased(KeyEvent e) {
//				Product productTmp;
//				char ch = e.getKeyChar();
//				if(ch == '\n') {
//					productTmp = pdbm.searchProduct(scannedBarcode.toString());
//					if(productTmp != null) {
//						boolean notExisting = true;
//						for(int i = 0; i < scannedProductList.size(); ++i) {
//							if(productTmp.equalsCode(scannedProductList.get(i))) {
//								notExisting = false;
//								int excessCase = scannedProductList.get(i).increasePurchaseNum();
//								if(excessCase == Product.PURCHASE_NUM_EXCESS)
//									textLog.append("해당 상품의 개수를 더 이상 늘릴 수 없습니다.\n");
//								break;
//							}
//						}
//						if(notExisting) {
//							productTmp.setPurchaseNum(1);
//							scannedProductList.add(productTmp);
//						}
//						textScan.setText("");
//						int sum = 0;
//						for(int i = 0; i < scannedProductList.size(); ++i) {
//							textScan.append(scannedProductList.get(i).toWorkedString());
//							sum += scannedProductList.get(i).getPurchasePrice();
//						}
//						pdbm.setScannedProductList(scannedProductList);
//						labelTotalPrice.setText("  합계: " + sum + "원");
//					} else
//						textLog.append("해당 상품은 등록되지 않은 상품입니다.\n");
//					scannedBarcode.setLength(0);
//				} else
//					scannedBarcode.append(ch);
//			}
//		});
		textScan.requestFocus();
	}
	public void actionPerformed(ActionEvent ae) {
		if(ae.getSource() == btnScanClear) {
//			scannedProductList.clear();
//			pdbm.setScannedProductList(scannedProductList);
//			textScan.setText("");
//			labelTotalPrice.setText("  합계: 0원");
		} else if(ae.getSource() == btnConfirmPurchase) {
			serverConfirmPurchase.setConfirmState(1);
		} else if(ae.getSource() == btnCancelPurchase) {
			serverConfirmPurchase.setConfirmState(2);
		} else if(ae.getSource() == btnLogClear) {
			textAccept.setText("");
//			textLog.setText("");
		} else if(ae.getSource() == btnStartServer) {
			try {
				if(serverSearchProduct.isAlive() == false) {
					serverSearchProduct.start();
					textLog.append("상품검색 서버가 열렸습니다.\n");
				} else {
					textLog.append("상품검색 서버가 이미 열려 있습니다.\n");
				}
				if(serverAcceptList.isAlive() == false) {
					serverAcceptList.start();
					textLog.append("상품결제 서버가 열렸습니다.\n");
				} else {
					textLog.append("상품결제 서버가 이미 열려 있습니다.\n");
				}
				if(serverConfirmPurchase.isAlive() == false) {
					serverConfirmPurchase.start();
					serverCancelPurchase.start();
					textLog.append("구매확인 서버가 열렸습니다.\n");
				} else {
					textLog.append("구매확인 서버가 이미 열려 있습니다.\n");
				}
			} catch(Exception e) {
				textLog.append("다른 서버가 이미 열려 있습니다.\n");
			}
			btnStartServer.setEnabled(false);
		}
		textScan.requestFocus();
	}

	private UpdateAdaptor adaptor = new UpdateAdaptor() {
		public void update() {
			if(this.action.equals("Print")) {
				textAccept.append(this.msg);
			} else if(this.action.equals("ReadyConfirmation")) {
				btnConfirmPurchase.setEnabled(true);
				btnCancelPurchase.setEnabled(true);
			} else if(this.action.equals("LogPrint")) {
				textLog.append(this.msg);
			} else if(this.action.equals("CompleteConfirmation")) {
				btnConfirmPurchase.setEnabled(false);
				btnCancelPurchase.setEnabled(false);
				textScan.setText("");
				labelTotalPrice.setText("  합계: 0원");
				
				if(this.arg == 1)
					textAccept.append("구매 확인\n");
				else if(this.arg == 2)
					textAccept.append("구매 취소\n");
			} else if(this.action.equals("PrintPurchaseList")) {
				textScan.setText(this.msg);
				labelTotalPrice.setText("  합계: " + this.arg + "원");
			}
		}
	};
	
	private void addGridComp(Container container, Component comp, double wx, double wy, int x, int y, int width, int height) {
		layoutGridCon.fill = GridBagConstraints.BOTH;
		layoutGridCon.weightx = wx;
		layoutGridCon.weighty = wy;
		layoutGridCon.gridx = x;
		layoutGridCon.gridy = y;
		layoutGridCon.gridwidth = width;
		layoutGridCon.gridheight = height;
		layoutGrid.setConstraints(comp, layoutGridCon);
		container.add(comp);
	}
}

public class FastShopperCounter {
	public static void main(String[] ar) {
		new CounterFrame();
	}
}