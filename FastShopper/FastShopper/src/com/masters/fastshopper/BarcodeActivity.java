/*
 * Basic no frills app which integrates the ZBar barcode scanner with
 * the camera.
 * 
 * Created by lisah0 on 2012-02-24
 */
package com.masters.fastshopper;

import java.io.*;
import java.net.*;

import android.app.Activity;
import android.content.*;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Handler;
import android.view.Gravity;
import android.widget.FrameLayout;
import android.widget.Button;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.Size;
import android.widget.TextView;

/* Import ZBar Class files */
import net.sourceforge.zbar.ImageScanner;
import net.sourceforge.zbar.Image;
import net.sourceforge.zbar.Symbol;
import net.sourceforge.zbar.Config;

public class BarcodeActivity extends Activity
{
    private Camera mCamera;
    private CameraPreview mPreview;
    private Handler autoFocusHandler;
    private ImageScanner scanner;
    private boolean previewing = true;

    TextView scanText;
    Button scanButton;

    static {
        System.loadLibrary("iconv");
    } 

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        initBarcodeActivity();
    }

    public void onPause() {
        super.onPause();
        releaseCamera();
    }
    
    public void onResume() {
    	super.onResume();
    	if (mCamera == null) {
    		initBarcodeActivity();
        }
    }
    
    private void initBarcodeActivity() {
        setContentView(R.layout.barcode_activity);

        previewing = true;
        autoFocusHandler = new Handler();
        mCamera = getCameraInstance();

        /* Instance barcode scanner */
        scanner = new ImageScanner();
        scanner.setConfig(0, Config.X_DENSITY, 3);
        scanner.setConfig(0, Config.Y_DENSITY, 3);

        mPreview = new CameraPreview(this, mCamera, previewCb, autoFocusCB);
        FrameLayout preview = (FrameLayout)findViewById(R.id.cameraPreview);
        preview.addView(mPreview);

        scanText = (TextView)findViewById(R.id.textScanGuide);
        scanText.setText("바코드를 화면 안에 비춰주세요.");
        scanText.setGravity(Gravity.CENTER_HORIZONTAL);
    }

    /** A safe way to get an instance of the Camera object. */
    public static Camera getCameraInstance(){
        Camera c = null;
        try {
            c = Camera.open();
        } catch (Exception e){
        }
        return c;
    }

    private void releaseCamera() {
        if (mCamera != null) {
            previewing = false;
            mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        }
    }

    private Runnable doAutoFocus = new Runnable() {
        public void run() {
            if (previewing)
                mCamera.autoFocus(autoFocusCB);
        }
    };

    PreviewCallback previewCb = new PreviewCallback() {
        public void onPreviewFrame(byte[] data, Camera camera) {
            Camera.Parameters parameters = camera.getParameters();
            Size size = parameters.getPreviewSize();

            Image barcode = new Image(size.width, size.height, "Y800");
            barcode.setData(data);

            int result = scanner.scanImage(barcode);

            if (result != 0) {
                previewing = false;
                mCamera.setPreviewCallback(null);
                mCamera.stopPreview();

                // 이 부분에서 처리
                Symbol sym = scanner.getResults().iterator().next();
                SharedPreferences pref = getSharedPreferences("BarcodeScan", 0);
                pref.edit().putString("ScannedCode", sym.getData() + '\n').commit();

                sendBroadcast(new Intent().setAction("com.masters.fastshopper.BARCODE_SCAN"));


//                Intent intent = new Intent(BarcodeActivity.this, MainTabActivity.class);
//                intent.setAction("com.masters.fastshopper.action.SelectPurchaseList");
//                intent.putExtra("ScanCode", sym.getData());
//                startActivity(intent);
//                finish();

                
//                for (Symbol sym : syms) {
//                	// Intent로 데이터 전달하는 부분
//                	Intent intent = new Intent();
//                	intent.putExtra("ScanNum",  sym.getData());
//                	setResult(RESULT_OK, intent);
//                	finish();
//                	break;
                	
//                	scanText.setText("barcode result " + sym.getData());
//                 barcodeScanned = true;
                    
//                }
            }
        }
    };

    // Mimic continuous auto-focusing
    AutoFocusCallback autoFocusCB = new AutoFocusCallback() {
        public void onAutoFocus(boolean success, Camera camera) {
            autoFocusHandler.postDelayed(doAutoFocus, 1000);
        }
    };
}