#ifndef KEYMAP_HPP
#define KEYMAP_HPP

#include <string>
#include <unordered_map>

namespace Engine::Core {

    class Keymap {
    public:
        void bind(const std::string& action, int key) {
            mBindings[action] = key;
        }

        bool hasBinding(const std::string& action) const {
            return mBindings.find(action) != mBindings.end();
        }

        int getKey(const std::string& action) const {
            auto it = mBindings.find(action);
            return it != mBindings.end() ? it->second : -1;
        }

    private:
        std::unordered_map<std::string, int> mBindings;
    };

}

#endif
