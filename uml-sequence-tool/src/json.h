#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>

// Минималистичный JSON-парсер (без сторонних библиотек).
class JsonValue {
public:
    enum class Type { Null, Bool, Number, String, Array, Object };

    Type type = Type::Null;
    bool boolVal = false;
    double numVal = 0;
    std::string strVal;
    std::vector<JsonValue> arr;
    std::map<std::string, JsonValue> obj;

    const JsonValue& operator[](const std::string& key) const;
    const JsonValue& operator[](size_t idx) const;
    size_t size() const;
    bool isArray() const { return type == Type::Array; }
    bool isObject() const { return type == Type::Object; }
    bool isString() const { return type == Type::String; }
    std::string asString() const;
};

class JsonParser {
public:
    JsonValue parse(const std::string& text);
    JsonValue parseFile(const std::string& path);

private:
    std::string src;
    size_t pos = 0;

    void skipWs();
    JsonValue parseValue();
    JsonValue parseObject();
    JsonValue parseArray();
    JsonValue parseString();
    JsonValue parseNumber();
    JsonValue parseBoolNull();
    std::string parseRawString();
    [[noreturn]] void error(const std::string& msg);
};