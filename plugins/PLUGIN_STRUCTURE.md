# Every calculator plugin should have 2 functions in ddlexport:
1. `const char* get_function_name()` - возвращает имя функции (`sin`, `cos`). Обращаться в калькуляторе будут именно к ней
2. `double evaluate(double x)` - вычисляет значение функции от аргумента `x` и возвращает результат, или выбрасывает исключение

## All functions in plugin should have `extern "C" __declspec(dllexport)` qualifier
Here's why: 
- `extern "C"` disables name mangling
- `__declspec(dllexport)` exports the symbol from .dll