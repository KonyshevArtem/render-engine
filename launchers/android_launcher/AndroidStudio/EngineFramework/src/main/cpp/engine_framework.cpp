#include <jni.h>
#include <android/asset_manager_jni.h>

#include "engine_framework.h"

extern "C" JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_EngineFramework_Initialize(JNIEnv* env, jobject /* this */, jobject assetManager)
{
    EngineFramework::Initialize(AAssetManager_fromJava(env, assetManager), nullptr, "OpenGL");
}

extern "C" JNIEXPORT void JNICALL
Java_com_artemkonyshev_engineframework_EngineFramework_TickMainLoop(JNIEnv* env, jobject /* this */, int width, int height)
{
    EngineFramework::TickMainLoop(width, height);
}
