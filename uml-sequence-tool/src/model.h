#pragma once
#include <string>
#include <vector>
#include <map>

struct ClassInfo {
    std::string name;
    std::vector<std::string> attributes;
    std::vector<std::string> methods;
    std::vector<std::string> relationships;
};

struct SequenceStep {
    std::string actor;
    std::string action;
    std::string target;
    std::vector<std::string> parameters;
};

struct ClassDiagram {
    std::vector<ClassInfo> classes;
};

struct SequenceDiagram {
    std::vector<SequenceStep> steps;
};