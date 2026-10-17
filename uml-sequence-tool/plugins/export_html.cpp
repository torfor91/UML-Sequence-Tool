#include "plugin_api.h"
#include <sstream>
#include <fstream>
#include <cstring>

static bool exportClassHtml(const ClassDiagram* d, const char* dir) {
    std::ostringstream o;
    o << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Class Diagram</title></head><body>\n";
    o << "<h1>Class Diagram</h1>\n";
    for (const auto& c : d->classes) {
        o << "<h2>Class: " << c.name << "</h2>\n";
        o << "<h3>Attributes</h3><ul>";
        for (const auto& a : c.attributes) o << "<li>" << a << "</li>";
        o << "</ul><h3>Methods</h3><ul>";
        for (const auto& m : c.methods) o << "<li>" << m << "</li>";
        o << "</ul><h3>Relationships</h3><p>";
        for (size_t i = 0; i < c.relationships.size(); ++i) {
            if (i) o << ", ";
            o << c.relationships[i];
        }
        o << "</p>\n";
    }
    o << "</body></html>\n";
    std::ofstream f(std::string(dir) + "/class-diagram.html");
    if (!f) return false;
    f << o.str();
    return true;
}

static bool exportSequenceHtml(const SequenceDiagram* d, const char* dir) {
    std::ostringstream o;
    o << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Sequence Diagram</title></head><body>\n";
    o << "<h1>Sequence Diagram</h1>\n<pre>\n";
    for (const auto& s : d->steps) {
        o << s.actor << " -&gt; " << s.target << ": " << s.action << "(";
        for (size_t i = 0; i < s.parameters.size(); ++i) {
            if (i) o << ", ";
            o << s.parameters[i];
        }
        o << ")\n";
    }
    o << "</pre></body></html>\n";
    std::ofstream f(std::string(dir) + "/sequence-diagram.html");
    if (!f) return false;
    f << o.str();
    return true;
}

static DiagramPlugin g_plugin {
    "html", "1.0", &exportClassHtml, &exportSequenceHtml
};

extern "C" DiagramPlugin* create_plugin() { return &g_plugin; }