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
    private AssetManager m_AssetManager;

    public RenderEngineGLRenderer(AssetManager assetManager)
    {
        m_AssetManager = assetManager;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        m_EngineFramework = new EngineFramework();
        m_EngineFramework.Initialize(m_AssetManager);
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
