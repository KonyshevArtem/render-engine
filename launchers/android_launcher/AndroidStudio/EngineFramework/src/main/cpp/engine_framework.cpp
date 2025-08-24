#include <jni.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>

#include "engine_framework.h"

extern "C" JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_EngineFramework_Initialize(JNIEnv* env, jobject /* this */, jobject surface, jobject assetManager)
{
    EngineFramework::Initialize(AAssetManager_fromJava(env, assetManager), ANativeWindow_fromSurface(env, surface), nullptr, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_EngineFramework_TickMainLoop(JNIEnv* env, jobject /* this */, int width, int height)
{
    EngineFramework::TickMainLoop(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_EngineFramework_ProcessTouchDown(JNIEnv* env, jobject /* this */, jlong touchId, float x, float y)
{
    EngineFramework::ProcessTouchDown(touchId, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_EngineFramework_ProcessTouchMove(JNIEnv* env, jobject /* this */, jlong touchId, float x, float y)
{
    EngineFramework::ProcessTouchMove(touchId, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_EngineFramework_ProcessTouchUp(JNIEnv* env, jobject /* this */, jlong touchId)
{
    EngineFramework::ProcessTouchUp(touchId);
}
