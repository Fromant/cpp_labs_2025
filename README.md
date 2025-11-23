# Simple calculator app
## Operators and functions are loaded from DLLs or SOs

### Build: 
1. `cmake -B ./build -S ./` (on windows you may want to also have `-g Ninja`)
2. `cmake --build ./build`

This will build all the dlls from ./plugins/ and calculator itself

### Running

Simply start the build/CalcApp.exe executable from build directory

### Usage

Calculator will try to calculate any expression that is passed in first line of stdin and then exit
If any exception happened, that will be seen in console:

### Supported syntax
- Calculator comes with some plugins out of the box (see `plugins/` dir): basic math and some functions
- Calculator supports parentheses, binary and unary operators, functions
- Function's arguments should be passed in parentheses: `sin(3.14)`, not `sin 3.14`
- Function may have any positive integer number of arguments that is `size_t` able to save: `max(1,2,3,4,5,...)`
- Operators should be single characters

### Creating your own dll:
To create your very own function or operator you have to create a separate dll
To make it, follow this steps:

1. create a `*.cpp` file in `plugins/` folder
2. include `#include "../src/plugin_interface.h"` in it
3. Define required functional:
   - your function: `anyname`
   - FunctionInfo
   - `PLUGIN_API const FunctionInfo* get_function_info()`
   - If not understood, see example plugins in `plugins/` folder
   - Rebuild the app to create shared library binary from your new .cpp

#### Plugin name restrictions:
- Not empty
- Not be '(', ')', ',' - reserved syntax tokens
- Shouldn't start with number
- Operators may have only 1 char as a name
- Plugins within categories (binary operator, unary operator, function) should not share a name inside a category. So, there may not be 2 unary '-' operators, but may be unary '-' and binary '-' and function "-"

#### What's unary, binary and function?
- If plugin is an operator with arity=2 -> binary operator like `1+2`
- If plugin is an operator with arity=1 -> unary prefix operator `-1`, `(-1+2)`, `-(1+2)`
- if plugin is a function it's implemented as prefix function with any arity: `sin(3.14)`, `pow(2, 4)`