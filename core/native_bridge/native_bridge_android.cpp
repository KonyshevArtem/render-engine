#if RENDER_ENGINE_ANDROID

#include "native_bridge_android.h"

#include <jni.h>

namespace NativeBridgeAndroid_Local
{
    JavaVM* s_JavaVM = nullptr;
    JNIEnv* s_Env = nullptr;

    jclass s_NativeBridgeClass;
    jmethodID s_ShowKeyboardMethodID;

    void InitBridge()
    {
        if (s_Env)
            return;

        s_JavaVM->AttachCurrentThread(&s_Env, nullptr);

        s_NativeBridgeClass = s_Env->FindClass("com/artemkonyshev/renderengine/NativeKeyboardView");
        s_ShowKeyboardMethodID = s_Env->GetStaticMethodID(s_NativeBridgeClass, "ShowKeyboard", "(Ljava/lang/String;JJJ)V");
    }
}

jint JNI_OnLoad(JavaVM* javaVM, void* reserved)
{
    NativeBridgeAndroid_Local::s_JavaVM = javaVM;
    return JNI_VERSION_1_6;
}

void NativeBridgeAndroid::ShowNativeKeyboard(const char* text, void* textFieldPtr, void* textChangedCallback, void* finishEditCallback)
{
    NativeBridgeAndroid_Local::InitBridge();

    NativeBridgeAndroid_Local::s_Env->CallStaticVoidMethod(
        NativeBridgeAndroid_Local::s_NativeBridgeClass,
        NativeBridgeAndroid_Local::s_ShowKeyboardMethodID,
        NativeBridgeAndroid_Local::s_Env->NewStringUTF(text),
        textFieldPtr,
        textChangedCallback,
        finishEditCallback);
}

#endif