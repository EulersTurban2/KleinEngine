#ifndef __ENVIRONMENT_HPP
#define __ENVIRONMENT_HPP

#include <string>
#include <unordered_map>

namespace Engine::Core {

    class Environment {
        public:
            static Environment& get() {
                static Environment instance;
                return instance;
            }

            Environment(const Environment&) = delete;
            Environment& operator=(const Environment&) = delete;

            void loadConfig(const std::string& filepath);

            void bootResourceDatabase();

            std::string getValue(const std::string& key, const std::string& defaultValue = "") const;
            bool hasKey(const std::string& key) const;

        private:
            Environment() = default;
            ~Environment() = default;

            std::unordered_map<std::string, std::string> mSettings;
    };

} 

#endif