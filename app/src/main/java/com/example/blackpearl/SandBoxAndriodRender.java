package com.example.blackpearl;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
public class SandBoxAndriodRender implements GLSurfaceView.Renderer{
    private int mSampleType;
    SandBoxNativeRenderer mNativeSandboxRender;
    SandBoxAndriodRender()
    {
        mNativeSandboxRender = new SandBoxNativeRenderer();
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        gl.glClearColor(0.5f,0.5f,0.5f,1.0f);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

    }

    @Override
    public void onDrawFrame(GL10 gl) {

        mNativeSandboxRender.native_Run();
    }
    public void  run() {
        mNativeSandboxRender.native_Run();
    }
    public void init() {
        Log.d("TAG","native_Initxxx");
        mNativeSandboxRender.native_Init();
        int a=1;
        Log.d("TAG","sss");
    }
    public int getSampleType() {
        return mSampleType;
    }
}
