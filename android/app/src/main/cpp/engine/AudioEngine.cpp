#include "engine/AudioEngine.h"

namespace goldenears {

AudioEngine& AudioEngine::instance() {
    static AudioEngine engine;
    return engine;
}

bool AudioEngine::initialize() {
    State expected = State::Idle;
    // If another thread already moved the state, compare_exchange fails and
    // that is fine: the engine is initialized either way.
    state_.compare_exchange_strong(expected, State::Initialized);
    return state_.load() == State::Initialized;
}

AudioEngine::State AudioEngine::state() const noexcept {
    return state_.load();
}

std::string AudioEngine::statusString() const {
    std::string status = "GoldenEars Core v";
    status += kVersion;
    status += " - state: ";
    status += toString(state());
    return status;
}

const char* toString(AudioEngine::State state) noexcept {
    switch (state) {
        case AudioEngine::State::Idle:
            return "Idle";
        case AudioEngine::State::Initialized:
            return "Initialized";
    }
    return "Unknown";
}

}  // namespace goldenears
