package com.goldenears.bridge

/**
 * JNI wrapper over the C++ audio engine (libgoldenears.so).
 *
 * This object only dispatches commands and reads telemetry. It holds no audio
 * logic. The matching native symbols live in cpp/jni/AudioEngineJni.cpp and are
 * named Java_com_goldenears_bridge_AudioEngineBridge_<function>.
 *
 * If the native library is missing or built for the wrong ABI, the first access
 * to this object throws UnsatisfiedLinkError. During this phase that is the
 * intended behavior: the failure must be visible at startup.
 */
object AudioEngineBridge {
    private const val LIBRARY_NAME = "goldenears"

    init {
        System.loadLibrary(LIBRARY_NAME)
    }

    /** Human-readable engine status, e.g. "GoldenEars Core v0.1.0 - state: Initialized". */
    external fun getEngineStatus(): String

    /**
     * Initializes the C++ engine. Idempotent.
     *
     * Threading: this call is O(1) and performs no IO, so it is allowed on the
     * main thread. Once the engine opens an AAudio stream, this call must move
     * to a dedicated engine thread (rule 2 of PROJECT_ARCHITECTURE.md).
     *
     * @return true when the engine is initialized after the call.
     */
    external fun initEngine(): Boolean
}
