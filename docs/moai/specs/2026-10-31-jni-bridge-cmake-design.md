# Design: CMake build and JNI bridge for the GoldenEars audio engine (v0.1.0)

Date: 2026-10-31
Status: approved

## Goal

Compile the C++20 audio engine with the Android NDK through Gradle, and prove that Kotlin can call it through JNI. This is the first step before the engine opens AAudio streams and before the bridge becomes a React Native TurboModule.

## Scope

In scope:

- `CMakeLists.txt` that builds `libgoldenears.so` in C++20.
- `externalNativeBuild` configuration in `android/app/build.gradle`.
- A pure C++ `AudioEngine` singleton with no JNI or React Native dependency.
- A separate JNI translation file.
- A Kotlin `object` `com.goldenears.bridge.AudioEngineBridge` with two external functions.
- A startup log line in `MainApplication.onCreate` that proves the native call works.

Out of scope (next iterations):

- Opening an AAudio stream in exclusive mode. This also requires raising `minSdkVersion` from 24 to 26 or higher.
- Oboe dependency.
- TurboModule wrapper for React Native.
- FLAC/WAV decoders, ring buffer, USB driver.

## Decisions

| Decision | Choice | Reason |
| --- | --- | --- |
| Bridge shape | C++ singleton + Kotlin `object` + separate JNI file | One DAC, one engine. No instance handles needed. C++ stays free of platform types (rule 3 of `PROJECT_ARCHITECTURE.md`). |
| Symbol registration | Name-mangled `Java_com_goldenears_bridge_AudioEngineBridge_*` symbols | Less code than `RegisterNatives`. Migrate when the bridge grows. |
| C++ standard | C++20, no compiler extensions | Project requirement. |
| STL | `c++_shared` | React Native already ships `libc++_shared.so`. A static copy would duplicate the STL inside the APK. |
| ABIs | From the existing `reactNativeArchitectures` Gradle property | Single source of truth, already consumed by the React Native Gradle plugin. |
| Audio API | None in this version | User decision. Validates CMake and JNI first. |
| Library load failure | Let `UnsatisfiedLinkError` stop the app at startup | Fail fast and visible during this phase. |

## Components

### `android/app/src/main/cpp/CMakeLists.txt`

- `cmake_minimum_required(VERSION 3.22.1)`, project `goldenears`, language CXX.
- `CMAKE_CXX_STANDARD 20`, required, extensions off.
- Shared library `goldenears` from `engine/AudioEngine.cpp` and `jni/AudioEngineJni.cpp`.
- Include directory: the `cpp/` root, so sources include headers as `engine/AudioEngine.h`.
- Warnings: `-Wall -Wextra -Wpedantic`. Default symbol visibility hidden; `JNIEXPORT` re-exports the JNI entry points.
- Links the NDK `log` library.

### `android/app/build.gradle`

- `defaultConfig.externalNativeBuild.cmake.arguments = ["-DANDROID_STL=c++_shared"]`.
- `android.externalNativeBuild.cmake.path = src/main/cpp/CMakeLists.txt`, `version = "3.22.1"`.
- `packaging.jniLibs.pickFirsts += "**/libc++_shared.so"` to avoid a duplicate-file error when React Native and this library both provide the STL.

### `cpp/engine/AudioEngine.h` and `AudioEngine.cpp`

- Namespace `goldenears`. Class `AudioEngine`.
- `enum class State { Idle, Initialized }`.
- `static AudioEngine& instance()`.
- `bool initialize()`: idempotent, O(1), no IO. Moves state from `Idle` to `Initialized`. Returns `true` when the engine is initialized after the call.
- `State state() const noexcept`.
- `std::string statusString() const`: human-readable status, includes version `0.1.0` and the state name.
- State stored in `std::atomic<State>` so Kotlin can query it from any thread.
- No `jni.h`, no `android/log.h`, no React Native headers.

### `cpp/jni/AudioEngineJni.cpp`

- The only file that includes `jni.h`.
- `Java_com_goldenears_bridge_AudioEngineBridge_getEngineStatus(JNIEnv*, jobject) -> jstring`.
- `Java_com_goldenears_bridge_AudioEngineBridge_initEngine(JNIEnv*, jobject) -> jboolean`.
- Logs with tag `GoldenEarsEngine` through `__android_log_print`.
- The existing JNI code inside `engine/AudioEngine.cpp` moves here.

### `java/com/goldenears/bridge/AudioEngineBridge.kt`

- `object AudioEngineBridge`.
- `init { System.loadLibrary("goldenears") }`.
- `external fun getEngineStatus(): String`.
- `external fun initEngine(): Boolean`.
- Functions are instance functions of the Kotlin object (not `@JvmStatic`), so the JNI signature receives a `jobject`.

### `java/com/goldenears/MainApplication.kt`

- In `onCreate`, after `loadReactNative`, call `AudioEngineBridge.initEngine()` and log the result together with `getEngineStatus()` under tag `GoldenEars`.
- Comment states the threading rule: `initEngine()` may run on the main thread only while it is O(1) and free of IO. When it opens an AAudio stream, the call moves to a dedicated engine thread.

## Data flow

1. Kotlin calls `AudioEngineBridge.initEngine()`.
2. The JVM resolves the symbol in `libgoldenears.so` and calls the JNI function.
3. The JNI function calls `goldenears::AudioEngine::instance().initialize()`.
4. The JNI function converts the C++ result into a `jboolean`.
5. `getEngineStatus()` follows the same path and converts `std::string` into a `jstring` with `NewStringUTF`.

## Error handling

- Missing or incompatible `.so`: `UnsatisfiedLinkError` at class load, app stops at startup.
- `initialize()` cannot fail in this version. It keeps a `bool` return so a later AAudio open failure can report `false` without changing the bridge signature.

## Verification

1. Install Android Studio with SDK platform 37, NDK `27.1.12297006` and CMake `3.22.1`.
2. Run `./gradlew assembleDebug` from `android/`.
3. Confirm `libgoldenears.so` exists for `armeabi-v7a`, `arm64-v8a`, `x86` and `x86_64` under `android/app/build/intermediates/merged_native_libs/debug/`.
4. Install the debug APK on a device or emulator and filter Logcat by tag `GoldenEars`. Expected line: `initEngine=true status=GoldenEars Core v0.1.0 - state: Initialized`.
