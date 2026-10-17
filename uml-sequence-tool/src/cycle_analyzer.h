#pragma once
#include "model.h"
#include <vector>
#include <string>

class CycleAnalyzer {
public:
    // Возвращает список найденных циклов (каждый — как вектор имён, начиная и заканчивая одним узлом).
    std::vector<std::vector<std::string>> findCycles(const ClassDiagram& d) const;
};