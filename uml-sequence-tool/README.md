# UML Sequence Tool

Консольное приложение на C++17 для генерации UML-диаграмм классов и
последовательности из JSON-описаний. Работает в ALT Linux без root,
использует только стандартную библиотеку C++ и POSIX API.

## Сборка

```bash
cd ~/uml-sequence-tool
cmake -S . -B build
cmake --build build -j