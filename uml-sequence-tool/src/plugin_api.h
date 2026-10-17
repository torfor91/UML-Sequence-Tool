#pragma once
#include "model.h"
#include <string>

// C-совместимый интерфейс плагина. Плагин может экспортировать:
//   extern "C" DiagramPlugin* create_plugin();
//   extern "C" void destroy_plugin(DiagramPlugin*);
struct DiagramPlugin {
    const char* name;      // "html" / "json" / ...
    const char* version;
    // Экспорт диаграмм. path — каталог output/.
    // Возвращает true при успехе.
    bool (*export_class_diagram)(const ClassDiagram* d, const char* path);
    bool (*export_sequence_diagram)(const SequenceDiagram* d, const char* path);
};