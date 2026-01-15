#include "include/native_bridge.h"
#include <jni.h>

extern "C"
JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_NativeBridge_NativeKeyboardTextChanged(JNIEnv *env,
                                                                              jclass clazz,
                                                                              jstring text) {
    NativeBridgeAndroid::NativeKeyboardTextChanged(env->GetStringUTFChars(text, nullptr));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_NativeBridge_NativeKeyboardFinishEdit(JNIEnv *env,
                                                                             jclass clazz,
                                                                             jboolean done) {
    NativeBridgeAndroid::NativeKeyboardFinishEdit(done);
}