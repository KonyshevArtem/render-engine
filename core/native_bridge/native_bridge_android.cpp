#if RENDER_ENGINE_ANDROID

#include "native_bridge_android.h"

#include <jni.h>

namespace NativeBridgeAndroid_Local
{
    TextChangedCallback s_TextChangedCallback;
    FinishEditCallback s_FinishEditCallback;

    JavaVM* s_JavaVM = nullptr;
    JNIEnv* s_Env = nullptr;

    jclass s_NativeBridgeClass;
    jmethodID s_ShowKeyboardMethodID;

    void InitBridge()
    {
        if (s_Env)
            return;

        s_JavaVM->AttachCurrentThread(&s_Env, nullptr);

        jclass localRef = s_Env->FindClass("com/artemkonyshev/renderengine/NativeKeyboardView");
        s_NativeBridgeClass = reinterpret_cast<jclass>(s_Env->NewGlobalRef(localRef));
        s_Env->DeleteLocalRef(localRef);

        s_ShowKeyboardMethodID = s_Env->GetStaticMethodID(s_NativeBridgeClass, "ShowKeyboard", "(Ljava/lang/String;)V");
    }
}

jint JNI_OnLoad(JavaVM* javaVM, void* reserved)
{
    NativeBridgeAndroid_Local::s_JavaVM = javaVM;
    return JNI_VERSION_1_6;
}

void NativeBridgeAndroid::NativeKeyboardTextChanged(const char *text)
{
    if (NativeBridgeAndroid_Local::s_TextChangedCallback)
        NativeBridgeAndroid_Local::s_TextChangedCallback(text);
}

void NativeBridgeAndroid::NativeKeyboardFinishEdit(bool done)
{
    if (NativeBridgeAndroid_Local::s_FinishEditCallback)
        NativeBridgeAndroid_Local::s_FinishEditCallback(done);
}

void NativeBridgeAndroid::ShowNativeKeyboard(const char* text, TextChangedCallback textChangedCallback, FinishEditCallback finishEditCallback)
{
    NativeBridgeAndroid_Local::InitBridge();

    NativeBridgeAndroid_Local::s_TextChangedCallback = textChangedCallback;
    NativeBridgeAndroid_Local::s_FinishEditCallback = finishEditCallback;

    NativeBridgeAndroid_Local::s_Env->CallStaticVoidMethod(
        NativeBridgeAndroid_Local::s_NativeBridgeClass,
        NativeBridgeAndroid_Local::s_ShowKeyboardMethodID,
        NativeBridgeAndroid_Local::s_Env->NewStringUTF(text));
}

#endif