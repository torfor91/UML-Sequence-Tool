#include "json.h"
#include "model.h"
#include "class_diagram_parser.h"
#include "sequence_diagram_parser.h"
#include "cycle_analyzer.h"
#include "markdown_exporter.h"
#include "plugin_loader.h"
#include "logger.h"

#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <cstring>
#include <stdexcept>

static void printClassDiagram(const ClassDiagram& d) {
    std::cout << "Class Diagram:\n";
    if (d.classes.empty()) { std::cout << "  (пусто)\n"; return; }
    for (const auto& c : d.classes) {
        std::cout << "  Class: " << c.name << "\n";
        std::cout << "    Attributes: ";
        if (c.attributes.empty()) std::cout << "(нет)";
        else {
            for (size_t i = 0; i < c.attributes.size(); ++i) {
                if (i) std::cout << ", ";
                std::cout << c.attributes[i];
            }
        }
        std::cout << "\n    Methods: ";
        if (c.methods.empty()) std::cout << "(нет)";
        else {
            for (size_t i = 0; i < c.methods.size(); ++i) {
                if (i) std::cout << ", ";
                std::cout << c.methods[i];
            }
        }
        std::cout << "\n    Relationships: ";
        if (c.relationships.empty()) std::cout << "(нет)";
        else {
            for (size_t i = 0; i < c.relationships.size(); ++i) {
                if (i) std::cout << ", ";
                std::cout << c.relationships[i];
            }
        }
        std::cout << "\n\n";
    }
}

static void printSequenceDiagram(const SequenceDiagram& d) {
    std::cout << "Sequence Diagram:\n";
    if (d.steps.empty()) { std::cout << "  (пусто)\n"; return; }
    for (const auto& s : d.steps) {
        std::cout << "  " << s.actor << " -> " << s.target << ": " << s.action << "(";
        for (size_t i = 0; i < s.parameters.size(); ++i) {
            if (i) std::cout << ", ";
            std::cout << s.parameters[i];
        }
        std::cout << ")\n";
    }
    std::cout << "\n";
}

static void ensureDir(const std::string& p) {
    struct stat st{};
    if (stat(p.c_str(), &st) != 0) mkdir(p.c_str(), 0755);
}

int main(int argc, char** argv) {
    std::string base = ".";
    std::string classFile = "data/class-diagram.json";
    std::string seqFile   = "data/sequence-diagram.json";

    // Простые аргументы: --class <path> --seq <path> --base <dir>
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--class" && i + 1 < argc) classFile = argv[++i];
        else if (a == "--seq" && i + 1 < argc) seqFile = argv[++i];
        else if (a == "--base" && i + 1 < argc) base = argv[++i];
        else if (a == "--help") {
            std::cout << "Использование: " << argv[0]
                      << " [--class data/class-diagram.json] [--seq data/sequence-diagram.json] [--base .]\n";
            return 0;
        }
    }

    ensureDir(base + "/logs");
    ensureDir(base + "/output");
    ensureDir(base + "/plugins");

    Logger::instance().init(base + "/logs/app.log");
    Logger::instance().info("Запуск UML Sequence Tool");

    std::cout << "=== UML Sequence Tool ===\n\n";
    std::cout << "Входные файлы:\n  " << classFile << "\n  " << seqFile << "\n\n";

    ClassDiagram cd;
    SequenceDiagram sd;
    bool classOk = true, seqOk = true;

    try {
        ClassDiagramParser cp;
        cd = cp.parseFile(classFile);
        Logger::instance().info("Class diagram загружена: классов = " + std::to_string(cd.classes.size()));
    } catch (const std::exception& e) {
        std::cerr << "Ошибка Class Diagram: " << e.what() << "\n";
        Logger::instance().error(std::string("ClassDiagram: ") + e.what());
        classOk = false;
    }

    try {
        SequenceDiagramParser sp;
        sd = sp.parseFile(seqFile);
        Logger::instance().info("Sequence diagram загружена: шагов = " + std::to_string(sd.steps.size()));
    } catch (const std::exception& e) {
        std::cerr << "Ошибка Sequence Diagram: " << e.what() << "\n";
        Logger::instance().error(std::string("SequenceDiagram: ") + e.what());
        seqOk = false;
    }

    if (classOk) printClassDiagram(cd);
    if (seqOk)   printSequenceDiagram(sd);

    // Анализ циклов
    if (classOk) {
        std::cout << "Анализ циклических зависимостей:\n";
        CycleAnalyzer ca;
        auto cycles = ca.findCycles(cd);
        if (cycles.empty()) {
            std::cout << "  Циклов не обнаружено.\n\n";
            Logger::instance().info("Циклов не обнаружено");
        } else {
            for (const auto& c : cycles) {
                std::cout << "  ВНИМАНИЕ: обнаружен цикл: ";
                for (size_t i = 0; i < c.size(); ++i) {
                    if (i) std::cout << " -> ";
                    std::cout << c[i];
                }
                std::cout << "\n";
                Logger::instance().warn("Обнаружен цикл: " + [&]{
                    std::ostringstream o;
                    for (size_t i = 0; i < c.size(); ++i) { if (i) o << " -> "; o << c[i]; }
                    return o.str();
                }());
            }
            std::cout << "\n";
        }
    }

    // Markdown
    if (classOk) {
        try {
            MarkdownExporter::writeFile(base + "/output/class-diagram.md",
                                        MarkdownExporter::classDiagramToMarkdown(cd));
            Logger::instance().info("Записан output/class-diagram.md");
        } catch (const std::exception& e) {
            std::cerr << "Ошибка записи class-diagram.md: " << e.what() << "\n";
        }
    }
    if (seqOk) {
        try {
            MarkdownExporter::writeFile(base + "/output/sequence-diagram.md",
                                        MarkdownExporter::sequenceDiagramToMarkdown(sd));
            Logger::instance().info("Записан output/sequence-diagram.md");
        } catch (const std::exception& e) {
            std::cerr << "Ошибка записи sequence-diagram.md: " << e.what() << "\n";
        }
    }

    std::cout << "Отчёты сохранены:\n  output/class-diagram.md\n  output/sequence-diagram.md\n\n";

    // Плагины
    {
        PluginLoader pl;
        pl.loadAll(base + "/plugins");
        if (pl.plugins().empty()) {
            std::cout << "Плагины не загружены.\n";
        } else {
            std::cout << "Загружено плагинов: " << pl.plugins().size() << "\n";
            std::string outDir = base + "/output";
            for (const auto& lp : pl.plugins()) {
                bool ok = true;
                if (classOk) ok = lp.plugin->export_class_diagram(&cd, outDir.c_str()) && ok;
                if (seqOk)   ok = lp.plugin->export_sequence_diagram(&sd, outDir.c_str()) && ok;
                std::cout << "  Плагин '" << (lp.plugin->name ? lp.plugin->name : "?")
                          << "' v" << (lp.plugin->version ? lp.plugin->version : "?")
                          << " -> " << (ok ? "OK" : "ОШИБКА") << "\n";
            }
        }
    }

    std::cout << "\nГотово.\n";
    Logger::instance().info("Завершение работы");
    return 0;
}