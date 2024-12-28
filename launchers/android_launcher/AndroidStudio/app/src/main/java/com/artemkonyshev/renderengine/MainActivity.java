package com.artemkonyshev.renderengine;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;

public class MainActivity extends Activity
{
    private GLSurfaceView m_GlSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        m_GlSurfaceView = new RenderEngineGLSurfaceView(this);
        setContentView(m_GlSurfaceView);
    }
}