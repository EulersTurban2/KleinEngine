#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <ctime>

namespace Engine::Core {

    // Critical means "unrecoverable for the subsystem that logged it"; the
    // logger itself never terminates the process — callers decide how to react.
    enum class LogLevel {
        Info = 0,
        Warning = 1,
        Error = 2,
        Critical = 3
    };

    class Logger {
        public:
            // Access the one and only instance
            static Logger& get() {
                static Logger instance;
                return instance;
            }

            /**
             * Sets the output stream (std::cout, std::ofstream, etc.).
             * The caller must ensure the stream stays valid for as long as the logger uses it!
             */
            void setStream(std::ostream& stream) {
                mStream = &stream;
            }

            // Copying is forbidden: we want a single global logger.
            Logger(const Logger&) = delete;
            Logger& operator=(const Logger&) = delete;

            void log(const std::string& msg, LogLevel level) {
                std::ostream& out = mStream ? *mStream : std::cout;
                const std::string levelStr = getLevelString(level);

                std::time_t now = std::time(nullptr);
                char timeStr[32] = {};
                std::strftime(timeStr, sizeof(timeStr), "%a %b %d %H:%M:%S %Y", std::localtime(&now));

                out << "[" << levelStr << "][" << timeStr << "]: " << msg << std::endl;
                if (out.rdbuf() != std::cout.rdbuf()) {
                    std::cout << "[" << levelStr << "][" << timeStr << "]: " << msg << std::endl;
                }
            }

        private:
            std::string getLevelString(LogLevel level) {
                switch (level) {
                    case LogLevel::Info:     return "INFO";
                    case LogLevel::Warning:  return "WARNING";
                    case LogLevel::Error:    return "ERROR";
                    case LogLevel::Critical: return "CRITICAL";
                    default:                 return "LOG";
                }
            }

            Logger() : mStream(nullptr) {}
            ~Logger() = default;

            std::ostream* mStream;
    };
}

// Convenience macros
#define LOG_INFO(msg)     Engine::Core::Logger::get().log(msg, Engine::Core::LogLevel::Info)
#define LOG_WARN(msg)     Engine::Core::Logger::get().log(msg, Engine::Core::LogLevel::Warning)
#define LOG_ERROR(msg)    Engine::Core::Logger::get().log(msg, Engine::Core::LogLevel::Error)
#define LOG_CRITICAL(msg) Engine::Core::Logger::get().log(msg, Engine::Core::LogLevel::Critical)

#endif // LOGGER_HPP
