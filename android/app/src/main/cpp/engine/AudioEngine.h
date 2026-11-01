#pragma once

#include <atomic>
#include <cstdint>
#include <string>

namespace goldenears {

// Core audio engine singleton.
//
// Rules (see PROJECT_ARCHITECTURE.md):
// - No JNI, Android or React Native dependencies in this class.
// - One engine per process: there is one external DAC and one output path.
class AudioEngine {
public:
    enum class State : std::uint8_t {
        Idle,
        Initialized,
    };

    static constexpr const char* kVersion = "0.1.0";

    static AudioEngine& instance();

    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;
    AudioEngine(AudioEngine&&) = delete;
    AudioEngine& operator=(AudioEngine&&) = delete;

    // Idempotent. Must stay O(1) and free of IO while callers may run it on the
    // main thread. Once this opens an AAudio stream, callers must move to a
    // dedicated engine thread.
    // Returns true when the engine is initialized after the call.
    bool initialize();

    State state() const noexcept;

    // Human-readable status for telemetry, e.g.
    // "GoldenEars Core v0.1.0 - state: Initialized".
    std::string statusString() const;

private:
    AudioEngine() = default;

    std::atomic<State> state_{State::Idle};
};

const char* toString(AudioEngine::State state) noexcept;

}  // namespace goldenears
