#pragma once
#include "model.h"
#include <string>

class MarkdownExporter {
public:
    static std::string classDiagramToMarkdown(const ClassDiagram& d);
    static std::string sequenceDiagramToMarkdown(const SequenceDiagram& d);
    static void writeFile(const std::string& path, const std::string& content);
};