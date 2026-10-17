#include "markdown_exporter.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string MarkdownExporter::classDiagramToMarkdown(const ClassDiagram& d) {
    std::ostringstream o;
    o << "# Class Diagram\n\n";
    for (const auto& c : d.classes) {
        o << "## Class: " << c.name << "\n\n";
        o << "**Attributes:**\n";
        if (c.attributes.empty()) o << "- (нет)\n";
        for (const auto& a : c.attributes) o << "- " << a << "\n";
        o << "\n**Methods:**\n";
        if (c.methods.empty()) o << "- (нет)\n";
        for (const auto& m : c.methods) o << "- " << m << "\n";
        o << "\n**Relationships:** ";
        if (c.relationships.empty()) o << "(нет)";
        else {
            for (size_t i = 0; i < c.relationships.size(); ++i) {
                if (i) o << ", ";
                o << c.relationships[i];
            }
        }
        o << "\n\n";
    }
    return o.str();
}

std::string MarkdownExporter::sequenceDiagramToMarkdown(const SequenceDiagram& d) {
    std::ostringstream o;
    o << "# Sequence Diagram\n\n```\n";
    for (const auto& s : d.steps) {
        o << s.actor << " -> " << s.target << ": " << s.action << "(";
        for (size_t i = 0; i < s.parameters.size(); ++i) {
            if (i) o << ", ";
            o << s.parameters[i];
        }
        o << ")\n";
    }
    o << "```\n";
    return o.str();
}

void MarkdownExporter::writeFile(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    if (!f) throw std::runtime_error("Не удалось записать файл: " + path);
    f << content;
}