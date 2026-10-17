#pragma once
#include <string>

class Logger {
public:
    static Logger& instance();
    void init(const std::string& path);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
private:
    std::string path_;
    void write(const std::string& level, const std::string& msg);
};