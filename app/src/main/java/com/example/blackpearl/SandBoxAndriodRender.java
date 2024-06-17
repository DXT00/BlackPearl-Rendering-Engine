package com.example.blackpearl;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
public class SandBoxAndriodRender implements GLSurfaceView.Renderer{
    private int mSampleType;
    SandBoxNativeRenderer mNativeSandboxRender;
    SandBoxAndriodRender()
    {

    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

    }

    @Override
    public void onDrawFrame(GL10 gl) {
        mNativeSandboxRender.native_Run();
    }

    public void init() {
        //mNativeSandboxRender.native_Init();
    }
    public int getSampleType() {
        return mSampleType;
    }
}
