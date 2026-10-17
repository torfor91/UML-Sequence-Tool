#include "sequence_diagram_parser.h"
#include <stdexcept>

SequenceDiagram SequenceDiagramParser::parse(const JsonValue& root) {
    if (!root.isObject())
        throw std::runtime_error("SequenceDiagram: корневой элемент должен быть объектом");
    if (root["sequence"].type != JsonValue::Type::Array)
        throw std::runtime_error("SequenceDiagram: отсутствует массив 'sequence'");

    SequenceDiagram d;
    const auto& arr = root["sequence"].arr;
    for (size_t i = 0; i < arr.size(); ++i) {
        const auto& s = arr[i];
        if (!s.isObject())
            throw std::runtime_error("SequenceDiagram: sequence[" + std::to_string(i) + "] не объект");

        SequenceStep step;
        for (const char* key : {"actor", "action", "target"}) {
            const auto& v = s[key];
            if (v.type != JsonValue::Type::String || v.strVal.empty())
                throw std::runtime_error(std::string("SequenceDiagram: sequence[") + std::to_string(i) + "]." + key + " отсутствует или пусто");
        }
        step.actor  = s["actor"].strVal;
        step.action = s["action"].strVal;
        step.target = s["target"].strVal;

        const auto& params = s["parameters"];
        if (params.type != JsonValue::Type::Null) {
            if (!params.isArray())
                throw std::runtime_error("SequenceDiagram: parameters шага " + std::to_string(i) + " должен быть массивом");
            for (const auto& p : params.arr) {
                if (!p.isString())
                    throw std::runtime_error("SequenceDiagram: параметр шага " + std::to_string(i) + " не строка");
                step.parameters.push_back(p.strVal);
            }
        }
        d.steps.push_back(std::move(step));
    }
    return d;
}

SequenceDiagram SequenceDiagramParser::parseFile(const std::string& path) {
    JsonParser p;
    return parse(p.parseFile(path));
}