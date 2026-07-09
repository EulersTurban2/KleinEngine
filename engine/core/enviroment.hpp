#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>
#include <unordered_map>
#include <vector>

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
            bool getBool(const std::string& key, bool defaultValue = false) const;
            // Returns the numbers of a JSON array value, or an empty vector if
            // the key is missing or not an array of numbers.
            std::vector<float> getFloatArray(const std::string& key) const;
            bool hasKey(const std::string& key) const;

        private:
            Environment() = default;
            ~Environment() = default;

            std::unordered_map<std::string, std::string> mSettings;
    };

}

#endif // ENVIRONMENT_HPP
