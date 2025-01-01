package com.artemkonyshev.renderengine;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;

import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;

import com.artemkonyshev.engineframework.EngineFramework;

public class MainActivity extends Activity
{
    private EngineFramework m_EngineFramework;
    private GLSurfaceView m_GlSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        WindowInsetsControllerCompat insetsController = WindowCompat.getInsetsController(getWindow(), getWindow().getDecorView());
        insetsController.setSystemBarsBehavior(WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
        insetsController.hide(WindowInsetsCompat.Type.systemBars());

        m_EngineFramework = new EngineFramework();
        m_GlSurfaceView = new RenderEngineGLSurfaceView(this, m_EngineFramework);
        setContentView(m_GlSurfaceView);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        int action = event.getActionMasked();
        int pointerIndex = event.getActionIndex();

        for (int i = 0; i < event.getPointerCount(); ++i)
        {
            int id = event.getPointerId(i);
            float x = event.getX(i);
            float y = event.getY(i);

            boolean isSecondaryPointer = i == pointerIndex;
            boolean isSecondaryPointerDown = action == MotionEvent.ACTION_POINTER_DOWN && isSecondaryPointer;
            boolean isSecondaryPointerUp = action == MotionEvent.ACTION_POINTER_UP && isSecondaryPointer;

            if (action == MotionEvent.ACTION_DOWN || isSecondaryPointerDown)
                m_EngineFramework.ProcessTouchDown(id, x, y);
            else if (action == MotionEvent.ACTION_MOVE)
                m_EngineFramework.ProcessTouchMove(id, x ,y);
            else if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_CANCEL || isSecondaryPointerUp)
                m_EngineFramework.ProcessTouchUp(id);
        }

        return true;
    }
}