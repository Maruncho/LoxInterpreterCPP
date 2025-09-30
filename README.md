# LoxInterpreterCPP

A simple translation of the [original](https://github.com/munificent/craftinginterpreters) Java implementation by Bob Nystrom to C++20.

The Lox language specification and its implementation is explained in [his book](https://craftinginterpreters.com/the-lox-language.html).

My implementation uses a simple Mark and Sweep GC as a substitute to the JVM one.

## Build
Because I'm using c++20 modules, the build is a bit tough to do manually. Building with Visual Studio is the easiest and most reliable. If that's not an option, make ChatGPT generate a CMake file.
I didn't include one here, because that introduces dependencies.
Alternatively, you can probably use an AI agent to convert all of the module files and module imports into simple header files and include directives.

## Executable
There is an executable if you have trouble building the repo. You probably don't trust some random executable, though.
