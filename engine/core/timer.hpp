#ifndef __TIMER_HPP
#define __TIMER_HPP

#include <chrono>

namespace Engine{
    namespace Core {
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
}

#endif