#ifndef PLATFORM_HPP
#define PLATFORM_HPP

namespace Engine {
    namespace Core {
        class Window; // Forward declaration
    }

    namespace Platform {
        class PlatformManager {
        public:
            // Hooks into the native window to intercept OS events
            static void init(Core::Window* window);
        };
    }
}

#endif // PLATFORM_HPP
