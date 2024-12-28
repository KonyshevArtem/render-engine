package com.artemkonyshev.engineframework;

import android.content.res.AssetManager;

public class EngineFramework
{
    static
    {
        System.loadLibrary("engineframework");
    }

    public native void Initialize(AssetManager assetManager);
    public native void TickMainLoop(int width, int height);
}