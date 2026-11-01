// JNI translation layer for com.goldenears.bridge.AudioEngineBridge.
//
// This is the only file in the engine that includes jni.h. It converts between
// JVM types and the pure C++ AudioEngine. It must not contain engine logic.

#include <jni.h>
#include <android/log.h>

#include <string>

#include "engine/AudioEngine.h"

namespace {

constexpr const char* kLogTag = "GoldenEarsEngine";

void logInfo(const char* message) {
    __android_log_print(ANDROID_LOG_INFO, kLogTag, "%s", message);
}

}  // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_com_goldenears_bridge_AudioEngineBridge_getEngineStatus(JNIEnv* env, jobject /* thiz */) {
    const std::string status = goldenears::AudioEngine::instance().statusString();
    return env->NewStringUTF(status.c_str());
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_goldenears_bridge_AudioEngineBridge_initEngine(JNIEnv* /* env */, jobject /* thiz */) {
    const bool initialized = goldenears::AudioEngine::instance().initialize();
    logInfo(initialized ? "AudioEngine initialized" : "AudioEngine initialization failed");
    return initialized ? JNI_TRUE : JNI_FALSE;
}
