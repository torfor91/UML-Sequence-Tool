#pragma once
#include "plugin_api.h"
#include <string>
#include <vector>

struct LoadedPlugin {
    std::string path;
    void* handle = nullptr;
    DiagramPlugin* plugin = nullptr;
};

class PluginLoader {
public:
    ~PluginLoader();
    // Загружает все *.so из каталога. Ошибки логируются, но не прерывают работу.
    void loadAll(const std::string& dir);
    const std::vector<LoadedPlugin>& plugins() const { return plugins_; }
    void unloadAll();

private:
    std::vector<LoadedPlugin> plugins_;
};