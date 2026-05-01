#include "core/timer.hpp"

#include "timer.hpp"

namespace Engine {
    namespace Core {
        std::chrono::time_point<std::chrono::high_resolution_clock> Timer::mStart;
        std::chrono::time_point<std::chrono::high_resolution_clock> Timer::mLast;
        float Timer::mDelta = 0.0f;

        void Timer::init() {
            mStart = std::chrono::high_resolution_clock::now();
            mLast = mStart;
        }

        void Timer::update() {
            auto currentFrameTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> timeElapsed = currentFrameTime - mLast;
            mDelta = timeElapsed.count();
            mLast = currentFrameTime;
        }

        float Timer::getDeltaTime() {
            return mDelta;
        }

        float Timer::getTotalTime() {
            auto currentFrameTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> timeElapsed = currentFrameTime - mStart;
            return timeElapsed.count();
        }
    }
}