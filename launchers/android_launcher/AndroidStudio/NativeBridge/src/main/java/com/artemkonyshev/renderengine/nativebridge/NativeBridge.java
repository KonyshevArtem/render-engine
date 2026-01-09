package com.artemkonyshev.renderengine.nativebridge;

public class NativeBridge
{
    static
    {
        System.loadLibrary("nativebridge");
    }

    public static native void InvokeNativeKeyboardTextChanged(String text, long textFieldPtr, long callbackPtr);
    public static native void InvokeFinishEdit(boolean done, long textFieldPtr, long callbackPtr);
}