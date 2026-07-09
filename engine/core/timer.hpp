#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

namespace Engine::Core {

    class Timer {
        public:
            static void init();
            static void update();
            static float getDeltaTime();
            static float getTotalTime();

        private:
            static std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
            static std::chrono::time_point<std::chrono::high_resolution_clock> mLast;
            static float mDelta;
    };
}

#endif // TIMER_HPP
