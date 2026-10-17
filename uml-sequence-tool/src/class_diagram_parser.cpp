#include "class_diagram_parser.h"
#include <stdexcept>
#include <set>

ClassDiagram ClassDiagramParser::parse(const JsonValue& root) {
    if (!root.isObject())
        throw std::runtime_error("ClassDiagram: корневой элемент должен быть объектом");
    if (root["classes"].type != JsonValue::Type::Array)
        throw std::runtime_error("ClassDiagram: отсутствует массив 'classes'");

    ClassDiagram d;
    std::set<std::string> names;

    const auto& arr = root["classes"].arr;
    for (size_t i = 0; i < arr.size(); ++i) {
        const auto& c = arr[i];
        if (!c.isObject())
            throw std::runtime_error("ClassDiagram: элемент classes[" + std::to_string(i) + "] не объект");

        ClassInfo info;
        if (c["name"].type != JsonValue::Type::String || c["name"].strVal.empty())
            throw std::runtime_error("ClassDiagram: classes[" + std::to_string(i) + "].name отсутствует или пусто");
        info.name = c["name"].strVal;
        if (!names.insert(info.name).second)
            throw std::runtime_error("ClassDiagram: дублирующееся имя класса '" + info.name + "'");

        auto readStrArr = [&](const char* key, std::vector<std::string>& out) {
            const auto& v = c[key];
            if (v.type == JsonValue::Type::Null) return;
            if (!v.isArray())
                throw std::runtime_error(std::string("ClassDiagram: ") + key + " класса " + info.name + " должен быть массивом");
            for (const auto& e : v.arr) {
                if (!e.isString())
                    throw std::runtime_error(std::string("ClassDiagram: элемент в ") + key + " класса " + info.name + " не строка");
                out.push_back(e.strVal);
            }
        };
        readStrArr("attributes", info.attributes);
        readStrArr("methods", info.methods);
        readStrArr("relationships", info.relationships);

        d.classes.push_back(std::move(info));
    }
    return d;
}

ClassDiagram ClassDiagramParser::parseFile(const std::string& path) {
    JsonParser p;
    return parse(p.parseFile(path));
}