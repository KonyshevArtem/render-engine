package com.artemkonyshev.renderengine;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class RenderEngineGLSurfaceView extends GLSurfaceView
{
    private final RenderEngineGLRenderer m_Renderer;

    public RenderEngineGLSurfaceView(Context context)
    {
        super(context);

        setEGLContextClientVersion(3);

        m_Renderer = new RenderEngineGLRenderer(getContext().getAssets());
        setRenderer(m_Renderer);
    }
}
