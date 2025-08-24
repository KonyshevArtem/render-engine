package com.artemkonyshev.renderengine;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;

import com.artemkonyshev.engineframework.EngineFramework;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RenderEngineGLRenderer implements GLSurfaceView.Renderer
{
    private int m_Width;
    private int m_Height;
    private EngineFramework m_EngineFramework;
    private GLSurfaceView m_SurfaceView;

    public RenderEngineGLRenderer(GLSurfaceView surfaceView, EngineFramework engineFramework)
    {
        m_SurfaceView = surfaceView;
        m_EngineFramework = engineFramework;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        m_EngineFramework.Initialize(m_SurfaceView.getHolder().getSurface(), m_SurfaceView.getContext().getAssets());
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        m_Width = width;
        m_Height = height;
    }

    @Override
    public void onDrawFrame(GL10 gl)
    {
        m_EngineFramework.TickMainLoop(m_Width, m_Height);
    }
}
