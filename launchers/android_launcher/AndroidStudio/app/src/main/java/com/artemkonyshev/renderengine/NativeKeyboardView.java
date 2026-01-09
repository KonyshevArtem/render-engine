package com.artemkonyshev.renderengine;

import android.content.Context;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.TextView;

import androidx.appcompat.widget.AppCompatEditText;

import com.artemkonyshev.renderengine.nativebridge.NativeBridge;

public class NativeKeyboardView extends AppCompatEditText
{
    private static NativeKeyboardView s_Instance;

    private InputMethodManager m_InputMethodManager;
    private long m_TextFieldPtr;
    private long m_TextChangedCallbackPtr;
    private long m_FinishEditCallbackPtr;

    public NativeKeyboardView(Context context, InputMethodManager inputMethodManager)
    {
        super(context);

        m_InputMethodManager = inputMethodManager;
        m_TextFieldPtr = 0;
        m_TextChangedCallbackPtr = 0;

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

    public static void ShowKeyboard(String text, long textFieldPtr, long textChangedCallbackPtr, long finishEditCallbackPtr)
    {
        MainActivity.getInstance().runOnUiThread(() -> s_Instance.ShowKeyboard_Internal(text, textFieldPtr, textChangedCallbackPtr, finishEditCallbackPtr));
    }

    public static void HideKeyboard(boolean done)
    {
        MainActivity.getInstance().runOnUiThread(() -> s_Instance.HideKeyboard_Internal(done));
    }

    private void ShowKeyboard_Internal(String text, long textFieldPtr, long textChangedCallbackPtr, long finishEditCallbackPtr)
    {
        m_TextFieldPtr = textFieldPtr;
        m_TextChangedCallbackPtr = textChangedCallbackPtr;
        m_FinishEditCallbackPtr = finishEditCallbackPtr;

        setText(text);

        setEnabled(true);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
    }

    private void HideKeyboard_Internal(boolean done)
    {
        NativeBridge.InvokeFinishEdit(done, m_TextFieldPtr, m_FinishEditCallbackPtr);

        m_TextFieldPtr = 0;
        m_TextChangedCallbackPtr = 0;
        m_FinishEditCallbackPtr = 0;

        clearFocus();
    }

    @Override
    protected void onTextChanged(CharSequence text, int start, int lengthBefore, int lengthAfter)
    {
        super.onTextChanged(text, start, lengthBefore, lengthAfter);

        if (m_TextFieldPtr != 0 && m_TextChangedCallbackPtr != 0)
            NativeBridge.InvokeNativeKeyboardTextChanged(text.toString(), m_TextFieldPtr, m_TextChangedCallbackPtr);
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
