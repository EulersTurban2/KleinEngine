#include "enviroment.hpp"
#include "core/logger.hpp"
#include "resources/resource_database.hpp" 
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Engine::Core {

    void Environment::loadConfig(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            LOG_CRITICAL("Failed to open engine environment file at: " + filepath);
            return;
        }

        json j;
        try {
            file >> j;
        } catch(const json::parse_error& e) {
            LOG_CRITICAL("Failed to parse environment JSON file: " + std::string(e.what()));
            return;
        }

        for (auto& [key, value] : j.items()) {
            if (value.is_string()) {
                mSettings[key] = value.get<std::string>();
            } else {
                mSettings[key] = value.dump();
            }
        }
    }

    void Environment::bootResourceDatabase() {
        std::string dbPath = getValue("RESOURCE_DATABASE_PATH", "assets/resources.json");
        std::string appName = getValue("APP_INSTANCE_NAME", "DefaultApp");

        Engine::Resources::ResourceDatabase::getInstance().loadFile(dbPath, appName);
    }

    std::string Environment::getValue(const std::string& key, const std::string& defaultValue) const {
        auto it = mSettings.find(key);
        if (it != mSettings.end()) {
            return it->second;
        }
        return defaultValue;
    }

    bool Environment::hasKey(const std::string& key) const {
        return mSettings.find(key) != mSettings.end();
    }

}