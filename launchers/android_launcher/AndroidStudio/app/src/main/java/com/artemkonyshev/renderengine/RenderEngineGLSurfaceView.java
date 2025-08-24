package com.artemkonyshev.renderengine;

import android.content.Context;
import android.opengl.GLSurfaceView;

import com.artemkonyshev.engineframework.EngineFramework;

public class RenderEngineGLSurfaceView extends GLSurfaceView
{
    private final RenderEngineGLRenderer m_Renderer;

    public RenderEngineGLSurfaceView(Context context, EngineFramework engineFramework)
    {
        super(context);

        setEGLContextClientVersion(3);

        m_Renderer = new RenderEngineGLRenderer(this, engineFramework);
        setRenderer(m_Renderer);
    }
}
