#pragma once
#include "model.h"
#include "json.h"
#include <string>

class ClassDiagramParser {
public:
    // Бросает std::runtime_error с понятным сообщением при ошибке.
    ClassDiagram parse(const JsonValue& root);
    ClassDiagram parseFile(const std::string& path);
};