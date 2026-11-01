# PROJECT CONTEXT & ARCHITECTURE: GoldenEars Android Audiophile Player

## 1. Project Overview & Core Goals
This project is a high-fidelity local audio player for Android designed specifically for audiophiles using external USB DAC/Amps.
* **Core Goal:** Achieve **Bit-Perfect Playback** by bypassing Android's audio mixer (AudioFlinger/Resampler). The app must route uncompressed PCM streams directly from FLAC/WAV files to an external USB DAC via USB Audio Class 2.0 or Oboe in `Exclusive Mode`.
* **Primary Features:** Local media scanner, lossless metadata parsing (kHz/bits), USB DAC detection, bit-perfect streaming status indicator, and high-performance React Native UI.

---

## 2. Technology Stack & Directory Structure
* **Frontend UI:** React Native (TypeScript) running on the New Architecture with TurboModules.
* **Android Service Layer:** Kotlin (Android SDK). Package namespace: `com.goldenears`. Handles `ForegroundService`, `MediaSession` notifications, `Scoped Storage` indexing, and `UsbManager` direct permissions.
* **Core Audio Engine:** C++20 compiled via Android NDK and CMake. Utilizes `Oboe` / `AAudio` or custom USB endpoint communication with `dr_flac` for low-latency PCM decoding.
GoldenEars/
├── PROJECT_ARCHITECTURE.md
├── src/                                  # React Native TypeScript UI Layer
│   ├── components/                       # UI components (Player, Visualizers, DAC Badge)
│   ├── hooks/                            # Custom React Hooks
│   ├── services/                         # Native Bridges
│   ├── store/                            # Global State (Zustand)
│   └── types/                            # TypeScript Models
└── android/app/src/main/
├── java/com/goldenears/              # Kotlin Native Layer
│   ├── audio/                        # Foreground Service & MediaSession
│   ├── usb/                          # UsbManager & DAC listeners
│   └── bridge/                       # TurboModules / JNI wrappers
└── cpp/                              # C++ Core Engine (NDK)
├── CMakeLists.txt
├── decoders/                     # dr_flac / dr_wav
├── engine/                       # AudioEngine, RingBuffer
└── usb/                          # Direct USB Driver

---

## 3. Strict Development Rules for AI Assistants
1. **Never use Android Default AudioTrack / MediaPlayer:** Android's high-level Java audio stack forces resampling to 48kHz. All audio streams must flow through the C++ NDK engine or AAudio in Exclusive Mode.
2. **Off-Thread Processing:** File IO, metadata extraction, and PCM frame decoding must never block the React Native JS thread or Android UI thread.
3. **Decoupled Architecture:** Keep C++ logic clean of React Native dependencies. The
