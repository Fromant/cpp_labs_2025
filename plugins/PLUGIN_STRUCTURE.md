

# Every calculator plugin should have 2 functions in ddlexport:
1. `PLUGIN_API const FunctionInfo* get_function_info();` - возвращает имя функции или оператора.
2. `PLUGIN API double name_eval(const double* args, size_t count)` - вычисляет значение функции от аргументов и возвращает результат, или выбрасывает исключение

### See src/plugin_interface.h for FunctionInfo and PLUGIN_API definition
- `extern "C"` disables name mangling
- `__declspec(dllexport)` exports the symbol from .dll
