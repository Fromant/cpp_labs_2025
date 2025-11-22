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
