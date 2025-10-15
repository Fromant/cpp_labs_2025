# Every calculator plugin should have 2 functions in ddlexport:
1. `const char* get_function_name()` - возвращает имя функции (`sin`, `cos`). Обращаться в калькуляторе будут именно к ней
2. `double evaluate(double x)` - вычисляет значение функции от аргумента `x` и возвращает результат, или выбрасывает исключение