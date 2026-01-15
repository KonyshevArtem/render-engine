package com.artemkonyshev.engineframework;

public class NativeBridge
{
    static
    {
        System.loadLibrary("engineframework");
    }

    public static native void NativeKeyboardTextChanged(String text);
    public static native void NativeKeyboardFinishEdit(boolean done);
}