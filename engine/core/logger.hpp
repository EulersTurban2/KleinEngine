#ifndef __LOGGER_HPP
#define __LOGGER_HPP

#define _POSIX_C_SOURCE 200809L

#include <iostream>
#include <fstream>
#include <time.h>

namespace Engine{
    namespace Core{

        enum LOG_LEVEL {
            INFO = 0,
            WARNING = 1,
            ERROR = 2,
            CRITICAL = 3
        };

        class Logger {
            public:
               // Access the one and only instance
                static Logger& get() {
                    static Logger instance; 
                    return instance;
                }

                /**
                 * @brief Postavlja izlazni stream (std::cout, std::ofstream, etc.)
                 * Korisnik mora da osigura da stream ostane validan toko cijelog vremena koriscenja loggera!
                 */
                void setStream(std::ostream& stream) {
                    _log_file = &stream;
                }

                // zabranjujemo kopiranje loggera, jer ne zelimo vise instance loggera
                // zelimo jedan globalan logger koji ce voditi racuna o svemu
                Logger(const Logger&) = delete;
                Logger& operator=(const Logger&) = delete;

                void log(const std::string& msg, LOG_LEVEL level){
                    std::ostream& out = _log_file ? *_log_file : std::cout;
                    std::string level_str = getLevelString(level);
                    
                    // biljezimo vrijeme
                    time_t now = time(NULL);
                    char time_str[26];
                    ctime_r(&now, time_str);

                    std::string tstr_trimmed(time_str);
                    tstr_trimmed.pop_back(); // uklanjamo newline karakter sa kraja
                    out << "[" << level_str << "][" << tstr_trimmed << "]: " << msg << std::endl;
                    if(out.rdbuf() != std::cout.rdbuf()){
                        std::cout << "[" << level_str << "][" << tstr_trimmed << "]: " << msg << std::endl;
                    }
                    if(level == LOG_LEVEL::CRITICAL){
                        out << "Critical error encountered. Terminating application." << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
            
            private:

                std::string getLevelString(LOG_LEVEL level) {
                    switch (level) {
                        case LOG_LEVEL::INFO:       return "INFO";
                        case LOG_LEVEL::WARNING:    return "WARNING";
                        case LOG_LEVEL::ERROR:      return "ERROR";
                        case LOG_LEVEL::CRITICAL :  return "CRITICAL";
                        default:                    return "LOG";
                    }
                }

                Logger(): _log_file(nullptr) {}; // Private constructor
                ~Logger() = default;

                std::ostream* _log_file;

        };
    }
}

// Global convenience macros
#define LOG_INFO(msg)     Engine::Core::Logger::get().log(msg, Engine::Core::LOG_LEVEL::INFO)
#define LOG_WARN(msg)     Engine::Core::Logger::get().log(msg, Engine::Core::LOG_LEVEL::WARNING)
#define LOG_ERROR(msg)    Engine::Core::Logger::get().log(msg, Engine::Core::LOG_LEVEL::ERROR)
#define LOG_CRITICAL(msg) Engine::Core::Logger::get().log(msg, Engine::Core::LOG_LEVEL::CRITICAL)

#endif // __LOGGER_HPP