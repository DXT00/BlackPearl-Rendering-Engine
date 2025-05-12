package com.example.blackpearl;

import android.app.NativeActivity;
import android.os.Bundle;
import android.content.res.AssetManager;

public class MainNativeActivity extends NativeActivity {
    static
    {
        System.loadLibrary("SandBoxAndroid");

    }
    public static final int ANDROID_BUILD_VERSION = android.os.Build.VERSION.SDK_INT;
    static MainNativeActivity _activity = null;

    /** AssetManger reference - populated on start up and used when the OBB is packed into the APK */
    private AssetManager			AssetManagerReference;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        // Grab a reference to the asset manager
        AssetManagerReference = this.getAssets();
        nativeSetGlobalActivity();
    }

    public AssetManager AndroidThunkJava_GetAssetManager()
    {
        if(AssetManagerReference == null)
        {
            //Log.debug("No reference to asset manager found!");
        }

        return AssetManagerReference;
    }

    public native void nativeSetGlobalActivity();


}
