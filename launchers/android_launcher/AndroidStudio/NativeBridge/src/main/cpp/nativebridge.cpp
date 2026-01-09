#include <jni.h>
#include <string>

typedef void OnNativeKeyboardTextChangedCallback(void*, const char*);
typedef void OnFinishEditCallback(void*, bool);

extern "C"
JNIEXPORT void JNICALL
Java_com_artemkonyshev_renderengine_nativebridge_NativeBridge_InvokeNativeKeyboardTextChanged(JNIEnv *env,
                                                                                              jclass clazz,
                                                                                              jstring text,
                                                                                              jlong textFieldPtr,
                                                                                              jlong callbackPtr)
{
    auto callback = reinterpret_cast<OnNativeKeyboardTextChangedCallback*>(callbackPtr);
    callback(reinterpret_cast<void*>(textFieldPtr), env->GetStringUTFChars(text, nullptr));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artemkonyshev_renderengine_nativebridge_NativeBridge_InvokeFinishEdit(JNIEnv *env,
                                                                               jclass clazz,
                                                                               jboolean done,
                                                                               jlong textFieldPtr,
                                                                               jlong callbackPtr)
{
    auto callback = reinterpret_cast<OnFinishEditCallback*>(callbackPtr);
    callback(reinterpret_cast<void*>(textFieldPtr), done);
}