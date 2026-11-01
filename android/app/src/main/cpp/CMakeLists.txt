cmake_minimum_required(VERSION 3.22.1)

project(goldenears LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Core engine (pure C++) + JNI translation layer.
# The engine must never include jni.h or React Native headers.
add_library(goldenears SHARED
    engine/AudioEngine.cpp
    jni/AudioEngineJni.cpp
)

# Sources include headers relative to the cpp/ root, e.g. "engine/AudioEngine.h".
target_include_directories(goldenears PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Hidden visibility by default. JNIEXPORT re-exports the JNI entry points.
target_compile_options(goldenears PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -fvisibility=hidden
)

find_library(ANDROID_LOG_LIB log)
target_link_libraries(goldenears PRIVATE ${ANDROID_LOG_LIB})
