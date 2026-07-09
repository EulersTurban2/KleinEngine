#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>
#include <unordered_map>
#include <vector>


/**

    THE ENVIROMENT CLASS

    The Environment class is responsible for loading and managing specific app configurations!
    These configurations include:
        - Resource database path
        - User defined settings! (Handled in the app)

    The class is designed to be a singleton, ensuring that only one instance exists throughout the application lifetime.
    It provides methods to load configurations from a JSON file, retrieve specific configuration values, and boot up the resource database!

    SAMPLE ENVIRONMENT JSON FILE:
    {
        "RESOURCE_DATABASE_PATH": "../resources/resources.json",
        "APP_INSTANCE_NAME": "MyApp",
        "FULLSCREEN": true
    }
    SAMPLE USAGE:
    #include <engine/all.hpp> // has the environment class included, as well as a pointer variable called "env" 
                              // that points to the singleton instance of the Environment class.
    ...
    // Best practice is to have the loading and bootup of the resource database be a seperate function!
    // Load the environment configuration from a JSON file
    void loadEnvironment(const std::string& configFilePath) {
        env.loadConfig(configFilePath);
        env.bootResourceDatabase();
    }
    ... 
    // Now with the enviroment loaded, we can define user-specific configurations and settings, for example FULLSCREEN!
    bool fullscreen = env.getBool("FULLSCREEN", false); // if FULLSCREEN is not defined in the enviroment, it will default to false!
    if (fullscreen) {
        // Set the application to fullscreen mode
    }

*/


namespace Engine::Core {

    class Environment {
        public:
            // Get the singleton instance of the Environment class
            static Environment& get() {
                static Environment instance;
                return instance;
            }

            Environment(const Environment&) = delete;
            Environment& operator=(const Environment&) = delete;

            // Read the give JSON file and load the key-value pairs into the environment settings.
            void loadConfig(const std::string& filepath);

            // Boot the resource database using the loaded configuration!
            void bootResourceDatabase();

            // Gets the string value for the given key, or returns the default value if the key is not found.
            std::string getValue(const std::string& key, const std::string& defaultValue = "") const;

            // Gets the boolean value for the given key, or returns the default value if the key is not found.
            bool getBool(const std::string& key, bool defaultValue = false) const;

            // Returns the numbers of a JSON array value, or an empty vector if the key is missing or not an array of numbers.
            std::vector<float> getFloatArray(const std::string& key) const;

            // Simple checking whether a key exists in the environment settings.
            bool hasKey(const std::string& key) const;

        private:
            Environment() = default;
            ~Environment() = default;
            
            // Internal storage for environment settings as key-value pairs.
            std::unordered_map<std::string, std::string> mSettings;
    };

}

#endif // ENVIRONMENT_HPP
