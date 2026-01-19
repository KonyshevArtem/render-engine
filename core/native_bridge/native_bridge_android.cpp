#if RENDER_ENGINE_ANDROID

#include "native_bridge_android.h"
#include "string_encoding_util.h"

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

void NativeBridgeAndroid::NativeKeyboardTextChanged(const char16_t* text, size_t length)
{
    std::u32string utf32Text = StringEncodingUtil::Utf16ToUtf32(text, length);
    if (NativeBridgeAndroid_Local::s_TextChangedCallback)
        NativeBridgeAndroid_Local::s_TextChangedCallback(reinterpret_cast<const wchar_t*>(utf32Text.c_str()));
}

void NativeBridgeAndroid::NativeKeyboardFinishEdit(bool done)
{
    if (NativeBridgeAndroid_Local::s_FinishEditCallback)
        NativeBridgeAndroid_Local::s_FinishEditCallback(done);
}

void NativeBridgeAndroid::ShowNativeKeyboard(const wchar_t* text, TextChangedCallback textChangedCallback, FinishEditCallback finishEditCallback)
{
    NativeBridgeAndroid_Local::InitBridge();

    NativeBridgeAndroid_Local::s_TextChangedCallback = textChangedCallback;
    NativeBridgeAndroid_Local::s_FinishEditCallback = finishEditCallback;

    std::u16string utf16Text = StringEncodingUtil::Utf32ToUtf16(reinterpret_cast<const char32_t*>(text), wcslen(text));
    jstring javaText = NativeBridgeAndroid_Local::s_Env->NewString(reinterpret_cast<const jchar*>(utf16Text.c_str()), utf16Text.length());

    NativeBridgeAndroid_Local::s_Env->CallStaticVoidMethod(
        NativeBridgeAndroid_Local::s_NativeBridgeClass,
        NativeBridgeAndroid_Local::s_ShowKeyboardMethodID,
        javaText);

    NativeBridgeAndroid_Local::s_Env->DeleteLocalRef(javaText);
}

#endif