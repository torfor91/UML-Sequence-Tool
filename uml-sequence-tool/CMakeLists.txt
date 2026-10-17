cmake_minimum_required(VERSION 3.16)
project(uml_sequence_tool CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins)

add_executable(uml_tool
    src/main.cpp
    src/json.cpp
    src/class_diagram_parser.cpp
    src/sequence_diagram_parser.cpp
    src/cycle_analyzer.cpp
    src/markdown_exporter.cpp
    src/plugin_loader.cpp
    src/logger.cpp
)
target_include_directories(uml_tool PRIVATE src)
target_link_libraries(uml_tool PRIVATE dl)

# Плагины собираются как shared-библиотеки без префикса "lib"
add_library(export_html SHARED plugins/export_html.cpp)
set_target_properties(export_html PROPERTIES PREFIX "" OUTPUT_NAME "export_html")
target_include_directories(export_html PRIVATE src)

add_library(export_json SHARED plugins/export_json.cpp)
set_target_properties(export_json PROPERTIES PREFIX "" OUTPUT_NAME "export_json")
target_include_directories(export_json PRIVATE src)