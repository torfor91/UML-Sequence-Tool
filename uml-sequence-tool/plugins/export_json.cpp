#include "plugin_api.h"
#include <fstream>
#include <sstream>

static std::string esc(const std::string& s) {
    std::string o;
    for (char c : s) {
        if (c == '"' || c == '\\') { o += '\\'; o += c; }
        else if (c == '\n') o += "\\n";
        else o += c;
    }
    return o;
}

static bool exportClassJson(const ClassDiagram* d, const char* dir) {
    std::ostringstream o;
    o << "{\n  \"classes\": [\n";
    for (size_t i = 0; i < d->classes.size(); ++i) {
        const auto& c = d->classes[i];
        o << "    {\n";
        o << "      \"name\": \"" << esc(c.name) << "\",\n";
        auto arr = [&](const char* key, const std::vector<std::string>& v, bool last) {
            o << "      \"" << key << "\": [";
            for (size_t k = 0; k < v.size(); ++k) {
                if (k) o << ", ";
                o << "\"" << esc(v[k]) << "\"";
            }
            o << "]" << (last ? "\n" : ",\n");
        };
        arr("attributes", c.attributes, false);
        arr("methods", c.methods, false);
        arr("relationships", c.relationships, true);
        o << "    }" << (i + 1 < d->classes.size() ? "," : "") << "\n";
    }
    o << "  ]\n}\n";
    std::ofstream f(std::string(dir) + "/class-diagram.export.json");
    if (!f) return false;
    f << o.str();
    return true;
}

static bool exportSequenceJson(const SequenceDiagram* d, const char* dir) {
    std::ostringstream o;
    o << "{\n  \"sequence\": [\n";
    for (size_t i = 0; i < d->steps.size(); ++i) {
        const auto& s = d->steps[i];
        o << "    {\n";
        o << "      \"actor\": \"" << esc(s.actor) << "\",\n";
        o << "      \"action\": \"" << esc(s.action) << "\",\n";
        o << "      \"target\": \"" << esc(s.target) << "\",\n";
        o << "      \"parameters\": [";
        for (size_t k = 0; k < s.parameters.size(); ++k) {
            if (k) o << ", ";
            o << "\"" << esc(s.parameters[k]) << "\"";
        }
        o << "]\n";
        o << "    }" << (i + 1 < d->steps.size() ? "," : "") << "\n";
    }
    o << "  ]\n}\n";
    std::ofstream f(std::string(dir) + "/sequence-diagram.export.json");
    if (!f) return false;
    f << o.str();
    return true;
}

static DiagramPlugin g_plugin {
    "json", "1.0", &exportClassJson, &exportSequenceJson
};

extern "C" DiagramPlugin* create_plugin() { return &g_plugin; }