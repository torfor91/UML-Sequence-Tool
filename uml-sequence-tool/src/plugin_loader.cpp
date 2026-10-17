#include "plugin_loader.h"
#include "logger.h"
#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

PluginLoader::~PluginLoader() { unloadAll(); }

void PluginLoader::loadAll(const std::string& dir) {
    DIR* d = opendir(dir.c_str());
    if (!d) {
        Logger::instance().warn("Каталог плагинов не найден: " + dir);
        return;
    }
    struct dirent* e;
    while ((e = readdir(d)) != nullptr) {
        std::string name = e->d_name;
        if (name.size() < 3 || name.substr(name.size() - 3) != ".so") continue;
        std::string full = dir + "/" + name;

        void* h = dlopen(full.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!h) {
            Logger::instance().warn(std::string("Не удалось загрузить ") + full + ": " + dlerror());
            continue;
        }
        using CreateFn = DiagramPlugin* (*)();
        CreateFn create = (CreateFn)dlsym(h, "create_plugin");
        if (!create) {
            Logger::instance().warn(std::string("В плагине нет create_plugin: ") + full);
            dlclose(h);
            continue;
        }
        DiagramPlugin* p = create();
        if (!p || !p->export_class_diagram || !p->export_sequence_diagram) {
            Logger::instance().warn(std::string("Плагин вернул некорректный интерфейс: ") + full);
            dlclose(h);
            continue;
        }
        Logger::instance().info(std::string("Загружен плагин: ") + (p->name ? p->name : "?") + " из " + full);
        plugins_.push_back({full, h, p});
    }
    closedir(d);
}

void PluginLoader::unloadAll() {
    for (auto& p : plugins_) {
        if (p.handle) dlclose(p.handle);
    }
    plugins_.clear();
}