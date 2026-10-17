#pragma once
#include "model.h"
#include "json.h"
#include <string>

class SequenceDiagramParser {
public:
    SequenceDiagram parse(const JsonValue& root);
    SequenceDiagram parseFile(const std::string& path);
};