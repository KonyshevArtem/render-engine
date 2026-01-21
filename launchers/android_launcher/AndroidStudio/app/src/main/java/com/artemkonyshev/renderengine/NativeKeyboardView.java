package com.artemkonyshev.renderengine;

import android.content.Context;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.TextView;

import androidx.appcompat.widget.AppCompatEditText;

import com.artemkonyshev.engineframework.NativeBridge;

public class NativeKeyboardView extends AppCompatEditText
{
    private static NativeKeyboardView s_Instance;

    private InputMethodManager m_InputMethodManager;

    public NativeKeyboardView(Context context, InputMethodManager inputMethodManager)
    {
        super(context);

        m_InputMethodManager = inputMethodManager;

        setOnEditorActionListener(this::HandleEditorAction);
        setOnFocusChangeListener(this::HandleFocusChange);

        post(() -> {
            ViewGroup.LayoutParams layoutParams = getLayoutParams();
            layoutParams.height = 1;
            setLayoutParams(layoutParams);

            setSingleLine(true);
            setMaxLines(1);
        });

        s_Instance = this;
    }

    public static void ShowKeyboard(String text)
    {
        MainActivity.getInstance().runOnUiThread(() -> s_Instance.ShowKeyboard_Internal(text));
    }

    public static void HideKeyboard(boolean done)
    {
        MainActivity.getInstance().runOnUiThread(() -> s_Instance.HideKeyboard_Internal(done));
    }

    private void ShowKeyboard_Internal(String text)
    {
        setText(text);

        setEnabled(true);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
    }

    private void HideKeyboard_Internal(boolean done)
    {
        NativeBridge.NativeKeyboardFinishEdit(done);

        clearFocus();
    }

    @Override
    protected void onTextChanged(CharSequence text, int start, int lengthBefore, int lengthAfter)
    {
        super.onTextChanged(text, start, lengthBefore, lengthAfter);

        NativeBridge.NativeKeyboardTextChanged(text.toString());
    }

    private boolean HandleEditorAction(TextView view, int actionId, KeyEvent event)
    {
        if (actionId == EditorInfo.IME_ACTION_DONE)
        {
            HideKeyboard(true);
            return true;
        }
        return false;
    }

    private void HandleFocusChange(View view, boolean hasFocus)
    {
        if (hasFocus)
            m_InputMethodManager.showSoftInput(this, 0);
        else
            m_InputMethodManager.hideSoftInputFromWindow(getWindowToken(), 0);
    }

    @Override
    public boolean onKeyPreIme(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK && event.getAction() == KeyEvent.ACTION_UP)
        {
            HideKeyboard(false);
            return true;
        }
        return super.dispatchKeyEvent(event);
    }
}
