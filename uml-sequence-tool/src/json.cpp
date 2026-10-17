#include "json.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdlib>

const JsonValue& JsonValue::operator[](const std::string& key) const {
    static JsonValue dummy;
    auto it = obj.find(key);
    return it == obj.end() ? dummy : it->second;
}

const JsonValue& JsonValue::operator[](size_t idx) const {
    static JsonValue dummy;
    return idx < arr.size() ? arr[idx] : dummy;
}

size_t JsonValue::size() const {
    if (type == Type::Array) return arr.size();
    if (type == Type::Object) return obj.size();
    return 0;
}

std::string JsonValue::asString() const {
    if (type != Type::String)
        throw std::runtime_error("JSON: значение не является строкой");
    return strVal;
}

void JsonParser::skipWs() {
    while (pos < src.size() && std::isspace((unsigned char)src[pos])) ++pos;
}

void JsonParser::error(const std::string& msg) {
    throw std::runtime_error("JSON parse error at pos " + std::to_string(pos) + ": " + msg);
}

JsonValue JsonParser::parse(const std::string& text) {
    src = text; pos = 0;
    skipWs();
    JsonValue v = parseValue();
    skipWs();
    if (pos != src.size()) error("лишние символы после значения");
    return v;
}

JsonValue JsonParser::parseFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("Не удалось открыть файл: " + path);
    std::stringstream ss; ss << f.rdbuf();
    return parse(ss.str());
}

JsonValue JsonParser::parseValue() {
    skipWs();
    if (pos >= src.size()) error("неожиданный конец");
    char c = src[pos];
    if (c == '{') return parseObject();
    if (c == '[') return parseArray();
    if (c == '"') return parseString();
    if (c == 't' || c == 'f' || c == 'n') return parseBoolNull();
    if (c == '-' || std::isdigit((unsigned char)c)) return parseNumber();
    error(std::string("неожиданный символ '") + c + "'");
}

JsonValue JsonParser::parseObject() {
    JsonValue v; v.type = JsonValue::Type::Object;
    ++pos; // {
    skipWs();
    if (pos < src.size() && src[pos] == '}') { ++pos; return v; }
    while (true) {
        skipWs();
        if (pos >= src.size() || src[pos] != '"') error("ожидался ключ");
        std::string key = parseRawString();
        skipWs();
        if (pos >= src.size() || src[pos] != ':') error("ожидался ':'");
        ++pos;
        v.obj[key] = parseValue();
        skipWs();
        if (pos >= src.size()) error("неожиданный конец объекта");
        if (src[pos] == ',') { ++pos; continue; }
        if (src[pos] == '}') { ++pos; break; }
        error("ожидался ',' или '}'");
    }
    return v;
}

JsonValue JsonParser::parseArray() {
    JsonValue v; v.type = JsonValue::Type::Array;
    ++pos; // [
    skipWs();
    if (pos < src.size() && src[pos] == ']') { ++pos; return v; }
    while (true) {
        v.arr.push_back(parseValue());
        skipWs();
        if (pos >= src.size()) error("неожиданный конец массива");
        if (src[pos] == ',') { ++pos; continue; }
        if (src[pos] == ']') { ++pos; break; }
        error("ожидался ',' или ']'");
    }
    return v;
}

std::string JsonParser::parseRawString() {
    // pos указывает на открывающую кавычку
    ++pos;
    std::string out;
    while (pos < src.size()) {
        char c = src[pos];
        if (c == '"') { ++pos; return out; }
        if (c == '\\') {
            ++pos;
            if (pos >= src.size()) error("незавершённая escape-последовательность");
            char e = src[pos++];
            switch (e) {
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case '/': out += '/'; break;
                case 'n': out += '\n'; break;
                case 't': out += '\t'; break;
                case 'r': out += '\r'; break;
                case 'b': out += '\b'; break;
                case 'f': out += '\f'; break;
                case 'u': {
                    // Простейшая обработка: пропускаем 4 hex-цифры, берём как есть ASCII
                    if (pos + 4 > src.size()) error("некорректный \\u escape");
                    std::string hex = src.substr(pos, 4);
                    pos += 4;
                    unsigned code = (unsigned)std::strtoul(hex.c_str(), nullptr, 16);
                    if (code < 0x80) out += (char)code;
                    else out += '?';
                    break;
                }
                default: error("неизвестный escape-символ");
            }
            continue;
        }
        out += c;
        ++pos;
    }
    error("незавершённая строка");
}

JsonValue JsonParser::parseString() {
    JsonValue v; v.type = JsonValue::Type::String;
    v.strVal = parseRawString();
    return v;
}

JsonValue JsonParser::parseNumber() {
    JsonValue v; v.type = JsonValue::Type::Number;
    size_t start = pos;
    if (src[pos] == '-') ++pos;
    while (pos < src.size() && (std::isdigit((unsigned char)src[pos]) || src[pos]=='.' || src[pos]=='e' || src[pos]=='E' || src[pos]=='+' || src[pos]=='-')) ++pos;
    v.numVal = std::strtod(src.substr(start, pos - start).c_str(), nullptr);
    return v;
}

JsonValue JsonParser::parseBoolNull() {
    JsonValue v;
    if (src.compare(pos, 4, "true") == 0) { pos += 4; v.type = JsonValue::Type::Bool; v.boolVal = true; return v; }
    if (src.compare(pos, 5, "false") == 0) { pos += 5; v.type = JsonValue::Type::Bool; v.boolVal = false; return v; }
    if (src.compare(pos, 4, "null") == 0) { pos += 4; v.type = JsonValue::Type::Null; return v; }
    error("некорректный литерал");
}