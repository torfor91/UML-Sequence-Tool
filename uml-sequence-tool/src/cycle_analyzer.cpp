#include "cycle_analyzer.h"
#include <map>
#include <set>

std::vector<std::vector<std::string>> CycleAnalyzer::findCycles(const ClassDiagram& d) const {
    // Построим граф: класс -> его relationships (только на существующие классы).
    std::map<std::string, std::vector<std::string>> g;
    std::set<std::string> known;
    for (const auto& c : d.classes) known.insert(c.name);
    for (const auto& c : d.classes) {
        for (const auto& r : c.relationships)
            if (known.count(r)) g[c.name].push_back(r);
    }

    std::vector<std::vector<std::string>> cycles;
    std::set<std::vector<std::string>> uniqueCycles;

    enum State { White, Gray, Black };
    std::map<std::string, State> state;
    for (const auto& c : d.classes) state[c.name] = White;

    std::vector<std::string> stack;

    std::function<void(const std::string&)> dfs = [&](const std::string& u) {
        state[u] = Gray;
        stack.push_back(u);
        for (const auto& v : g[u]) {
            if (state[v] == White) {
                dfs(v);
            } else if (state[v] == Gray) {
                // Нашли цикл: от v до u в стеке + возврат к v
                auto it = std::find(stack.begin(), stack.end(), v);
                if (it != stack.end()) {
                    std::vector<std::string> cyc(it, stack.end());
                    cyc.push_back(v);
                    // нормализуем: циклический сдвиг к минимальному элементу
                    auto minIt = std::min_element(cyc.begin(), cyc.end() - 1);
                    std::rotate(cyc.begin(), minIt, cyc.end() - 1);
                    cyc.push_back(cyc.front());
                    if (uniqueCycles.insert(cyc).second) cycles.push_back(cyc);
                }
            }
        }
        stack.pop_back();
        state[u] = Black;
    };

    for (const auto& c : d.classes)
        if (state[c.name] == White) dfs(c.name);

    return cycles;
}