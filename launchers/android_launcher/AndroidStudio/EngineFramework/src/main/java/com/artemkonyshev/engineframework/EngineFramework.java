package com.artemkonyshev.engineframework;

import android.content.res.AssetManager;
import android.view.Surface;

public class EngineFramework
{
    static
    {
        System.loadLibrary("engineframework");
    }

    public native void Initialize(Surface surface, AssetManager assetManager);
    public native void TickMainLoop(int width, int height);

    public native void ProcessTouchDown(long touchId, float x, float y);
    public native void ProcessTouchMove(long touchId, float x, float y);
    public native void ProcessTouchUp(long touchId);
}