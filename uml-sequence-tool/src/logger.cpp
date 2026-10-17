#include "logger.h"
#include <fstream>
#include <ctime>
#include <iostream>

Logger& Logger::instance() {
    static Logger l;
    return l;
}

void Logger::init(const std::string& path) {
    path_ = path;
    std::ofstream f(path_, std::ios::trunc);
    f << "=== log started ===\n";
}

void Logger::write(const std::string& level, const std::string& msg) {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    std::string line = std::string("[") + buf + "] [" + level + "] " + msg;
    if (!path_.empty()) {
        std::ofstream f(path_, std::ios::app);
        f << line << "\n";
    }
}

void Logger::info(const std::string& m)  { write("INFO",  m); }
void Logger::warn(const std::string& m)  { write("WARN",  m); std::cerr << "WARN: " << m << "\n"; }
void Logger::error(const std::string& m) { write("ERROR", m); std::cerr << "ERROR: " << m << "\n"; }