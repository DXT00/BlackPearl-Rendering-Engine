package com.example.blackpearl;

import static android.opengl.GLSurfaceView.RENDERMODE_CONTINUOUSLY;
import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

import static com.example.blackpearl.SandBoxNativeRenderer.SAMPLE_TYPE;
import static com.example.blackpearl.SandBoxNativeRenderer.SAMPLE_TYPE_KEY_BEATING_HEART;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.app.AlertDialog;
import android.content.Intent;
import android.graphics.Bitmap;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.view.ViewTreeObserver;
import android.view.ViewGroup;

import com.example.blackpearl.databinding.ActivityMainBinding;
import com.example.blackpearl.adapter.MyRecyclerViewAdapter;
import java.util.Arrays;

public class MainActivity extends AppCompatActivity implements ViewTreeObserver.OnGlobalLayoutListener, SensorEventListener {

    // Used to load the 'blackpearl' library on application startup.
    static {
        System.loadLibrary("SandBoxAndriod");
    }

    private ViewGroup mRootView;
    private MySurfaceView mGLSurfaceView;


    private ActivityMainBinding binding;

    private SandBoxAndriodRender mSanderBoxRender ;
    private int mSampleSelectedIndex = SAMPLE_TYPE_KEY_BEATING_HEART - SAMPLE_TYPE;
    private SensorManager mSensorManager;
    private static final String[] SAMPLE_TITLES = {
            "DrawTriangle"
            };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
//        tv.setText("Hi BlackPearl");

        mRootView = (ViewGroup) findViewById(R.id.rootView);
        mRootView.getViewTreeObserver().addOnGlobalLayoutListener(this);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mSanderBoxRender = new SandBoxAndriodRender();

        // Example of a call to a native method
        TextView tv = binding.sampleText;
       tv.setText(stringFromJNI());
        Log.d("TAG","before init");
        mSanderBoxRender.init();
        Log.d("TAG","after init");
//        mSanderBoxRender.run();
    }

    @Override
    public void onGlobalLayout() {
        mRootView.getViewTreeObserver().removeOnGlobalLayoutListener(this);
        RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        lp.addRule(RelativeLayout.CENTER_IN_PARENT);
        mGLSurfaceView = new MySurfaceView(this, mSanderBoxRender);
        mRootView.addView(mGLSurfaceView, lp);
        mGLSurfaceView.setRenderMode(RENDERMODE_CONTINUOUSLY);

    }


    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

   }
    @Override
    protected void onResume() {
        super.onResume();
        mSensorManager.registerListener(this,
                mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY),
                SensorManager.SENSOR_DELAY_FASTEST);
//        if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
//            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, PERMISSION_REQUEST_CODE);
//        }
//        ///sdcard/Android/data/com.byteflow.app/files/Download
//        String fileDir = getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath();
//        CommonUtils.copyAssetsDirToSDCard(MainActivity.this, "poly", fileDir + "/model");
//        CommonUtils.copyAssetsDirToSDCard(MainActivity.this, "fonts", fileDir);
//        CommonUtils.copyAssetsDirToSDCard(MainActivity.this, "yuv", fileDir);
    }
    private void showGLSampleDialog()
    {
        final AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = LayoutInflater.from(this);
        final View rootView = inflater.inflate(R.layout.sample_selected_layout, null);

        final AlertDialog dialog = builder.create();

        Button confirmBtn = rootView.findViewById(R.id.confirm_btn);
        confirmBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.cancel();
            }
        });

        final RecyclerView resolutionsListView = rootView.findViewById(R.id.resolution_list_view);

        final MyRecyclerViewAdapter myPreviewSizeViewAdapter = new MyRecyclerViewAdapter(this, Arrays.asList(SAMPLE_TITLES));
        myPreviewSizeViewAdapter.setSelectIndex(mSampleSelectedIndex);
        myPreviewSizeViewAdapter.addOnItemClickListener(new MyRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, int position) {
                mRootView.removeView(mGLSurfaceView);
                RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
                        ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
                lp.addRule(RelativeLayout.CENTER_IN_PARENT);
                mGLSurfaceView = new MySurfaceView(MainActivity.this, mSanderBoxRender);
                mRootView.addView(mGLSurfaceView, lp);

                int selectIndex = myPreviewSizeViewAdapter.getSelectIndex();
                myPreviewSizeViewAdapter.setSelectIndex(position);
                myPreviewSizeViewAdapter.notifyItemChanged(selectIndex);
                myPreviewSizeViewAdapter.notifyItemChanged(position);
                mSampleSelectedIndex = position;
                mGLSurfaceView.setRenderMode(RENDERMODE_WHEN_DIRTY);

                if (mRootView.getWidth() != mGLSurfaceView.getWidth()
                        || mRootView.getHeight() != mGLSurfaceView.getHeight()) {
                    mGLSurfaceView.setAspectRatio(mRootView.getWidth(), mRootView.getHeight());
                }

               // mGLRender.setParamsInt(SAMPLE_TYPE, position + SAMPLE_TYPE, 0);

                int sampleType = position + SAMPLE_TYPE;
                Bitmap tmp;

                mGLSurfaceView.requestRender();

//                if(sampleType != SAMPLE_TYPE_KEY_VISUALIZE_AUDIO && mAudioCollector != null) {
//                    mAudioCollector.unInit();
//                    mAudioCollector = null;
//                }

                dialog.cancel();
            }
        });

        LinearLayoutManager manager = new LinearLayoutManager(this);
        manager.setOrientation(LinearLayoutManager.VERTICAL);
        resolutionsListView.setLayoutManager(manager);

//        resolutionsListView.setAdapter(myPreviewSizeViewAdapter);
//        resolutionsListView.scrollToPosition(mSampleSelectedIndex);

        dialog.show();
        dialog.getWindow().setContentView(rootView);


    }
    /**
     * A native method that is implemented by the 'blackpearl' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    @Override
    public void onSensorChanged(SensorEvent event) {

    }


    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
       // if (id == R.id.action_change_sample) {
            showGLSampleDialog();
      //  }
        return true;
    }
}