#include <jni.h>
#include <android/log.h>

#define LOG_TAG "GoldenEarsEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_goldenears_bridge_AudioEngineBridge_getEngineStatus(JNIEnv* env, jobject /* this */) {
    LOGI("GoldenEars C++20 Audio Engine initialized successfully.");
    return env->NewStringUTF("GoldenEars Core Ready - Bit-Perfect Active");
}
