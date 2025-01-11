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

    public native void ProcessTouchDown(long touchId, float x, float y);
    public native void ProcessTouchMove(long touchId, float x, float y);
    public native void ProcessTouchUp(long touchId);
}