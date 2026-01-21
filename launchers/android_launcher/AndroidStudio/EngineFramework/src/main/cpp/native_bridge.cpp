#include "include/native_bridge.h"
#include <jni.h>

extern "C"
JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_NativeBridge_NativeKeyboardTextChanged(JNIEnv *env,
                                                                              jclass clazz,
                                                                              jstring text) {
    const jchar* textPtr = env->GetStringChars(text, nullptr);
    NativeBridgeAndroid::NativeKeyboardTextChanged(
            reinterpret_cast<const char16_t*>(textPtr),
            env->GetStringLength(text));

    env->ReleaseStringChars(text, textPtr);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_NativeBridge_NativeKeyboardFinishEdit(JNIEnv *env,
                                                                             jclass clazz,
                                                                             jboolean done) {
    NativeBridgeAndroid::NativeKeyboardFinishEdit(done);
}