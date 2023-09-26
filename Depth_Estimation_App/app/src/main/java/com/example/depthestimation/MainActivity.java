package com.example.depthestimation;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.PopupWindow;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.example.depthestimation.R;
import com.example.depthestimation.databinding.ActivityMainBinding;


public class MainActivity extends AppCompatActivity {
    volatile MainActivity _savedInstance;
    PopupWindow _popupwindow;

    // Used to load the 'onetry' library on application startup.
    static {
        System.loadLibrary("depthestimation");
    }
    static final String TAG = "depthestimation";

    private ActivityMainBinding binding;
    private static final int PERMISSION_REQUEST_CODE_CAMERA = 1;
    SurfaceView msurfaceview;
    SurfaceHolder msurfaceholder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        _savedInstance = this;
//        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);
        
        String [] accessPermissions = new String [] {Manifest.permission.CAMERA, Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE};
        
        boolean needreq = false;
        
        for(String access: accessPermissions){
            int curperm = ActivityCompat.checkSelfPermission(this, access);
            if (curperm != PackageManager.PERMISSION_GRANTED){
                needreq = true;
                break;
            }
        }

        if(needreq){
            ActivityCompat.requestPermissions(this, accessPermissions, PERMISSION_REQUEST_CODE_CAMERA);
        }

//        binding = ActivityMainBinding.inflate(getLayoutInflater());
//        setContentView(binding.getRoot());
//
//        // Example of a call to a native method
//        TextView tv = binding.sampleText;
//        tv.setText(stringFromJNI());
        onCreateJNI(this, getAssets());
        msurfaceview = (SurfaceView) findViewById(R.id.surfaceView);
        msurfaceholder = msurfaceview.getHolder();
        msurfaceholder.addCallback(new SurfaceHolder.Callback() {
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Log.v(TAG, "surfaceChanged format="+format+", width="+width+", height="+ height);
            }
            public void surfaceCreated(SurfaceHolder holder) {
                Log.v(TAG, "surfaceCreated");
                setSurface(holder.getSurface());
            }
            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.v(TAG, "surfaceDestroyed");
            }
        });
        CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
        try
        {
            for (String cameraId : manager.getCameraIdList()) {
                CameraCharacteristics characteristics
                        = manager.getCameraCharacteristics(cameraId);

                Log.d("Img", "INFO_SUPPORTED_HARDWARE_LEVEL " + characteristics.get(CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL));
                Log.d("Img", "INFO_REQUIRED_HARDWARE_LEVEL FULL" + CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_FULL);
                Log.d("Img", "INFO_REQUIRED_HARDWARE_LEVEL 3" + CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_3);
                Log.d("Img", "INFO_REQUIRED_HARDWARE_LEVEL LIMITED" + CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED);
                Log.d("Img", "INFO_REQUIRED_HARDWARE_LEVEL LEGACY" + CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY);

            }
        }
        catch (CameraAccessException e){
            e.printStackTrace();
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    /**
     * A native method that is implemented by the 'onetry' native library,
     * which is packaged with this application.
     */
    public native void onCreateJNI(Activity callerActivity, AssetManager assetManager);
    public native void setSurface(Surface surface);
}