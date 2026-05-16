# BabelWires

## Project Structure

The primary application is `babelwires`, which provides a GUI for defining data format conversions.

Main project structure:
* `BaseLib` - General utilities and data structures. E.g. result type, logging, etc.
* `BabelWiresLib` - Type system, data flow processing, management of project state, etc.
* `BabelWiresQtUi` - Qt-based UI for BabelWires.
* `Domains` - Domain-specific libraries
* `Domains/Music` - Music sequence data types and processing.
* `Domains/Music/Plugins` - Plugins providing support for specific music sequence formats, e.g. SMF (standard MIDI files).

Tests:
* Tests are found in various folders named `Tests` throughout the project. Examples:
  - `Tests` - Tests for the core libraries.
  - `Domains/Music/Tests` - Tests for the music domain.
  - `Domains/Music/Plugins/Smf/Tests` - Tests for the SMF plugin.

Other:
* `Domains/Music/Seq2tape*' - Tools for working converting audio data to and from music sequence data.

## Coding style

* Formatting is defined by the clang-format configuration in `.clang-format`.
* Naming conventions:
  - Types (classes, structs, typedefs, type aliases, etc.) are in PascalCase.
  - Functions and variables are in camelCase.
  - Macros are in ALL_CAPS_WITH_UNDERSCORES.
* Prefixes:
  - `m_` - Data members
  - `s_` - Static members
  - `c_` - Constants
  - `g_` - Global variables
* Structs:
  - Structs are treated the same as classes, except that their members are public by default.
  - Struct members are prefixed with `m_` just like class members.

## Commenting style

* Use `///` for documentation comments. No additional mark-up is used within documentation comments.
* For non-documentation comments, use `//`. 
* Documentation comments should state what a function or class is for and how it is intended to be used.
* An implementation node can follow the documentation comment, but should use `//`.
* Files headers alone use `/** */` style comments.
* The `/* */` style should never be used for regular commenting. It is reserved for easily commenting out blocks of code when iterating.

## Error-handling

* Code that can fail should use the `Result` type defined in `BaseLib/Result/result.hpp`. 
  - Code that needs to generate or manipulate errors should use the macros in `BaseLib/Result/resultDSL.hpp`.
* Exceptions or calls to `std::abort` (or similar) are not allowed.

## Logging

* The babelwires::logDebug() method from `BaseLib/Log/debugLogger.hpp` can be used to log debug messages.
  - This can be called from any code.
* Logging non-debug messages for the user requires a UserAdvisoryLogger or UserLogger object from `BaseLib/Log/userLogger.hpp`.
  - These are not globally accessible by design, because most functions do not have enough information about how they are being used to report meaningful messages to the user.
  - A `UserLogger` can be passed to functions that do have enough context.
  - Sometimes such a function returns a result object to for the error case. To prevent double-reporting, a `UserAdvisoryLogger` should be provided instead.
* Do not use `std::cout` or similar for logging purposes.

## Singletons

* Singletons should be avoided as much as possible.
* There is one singleton intended for general use in the codebase: the `IdentifierRegister`.
* Some other facilities are implemented using singletons, but that fact is hidden from callers.

## Dependency Injection and the Context

* Most services are made available to the system using a service-provided type object called the `Context`.
* The Context is initialized in a program's main function.
* Libraries and plugins register their factories, types, etc. with the context in functions named `registerLib` by convention.
* The registerLib function of libraries should be called explicitly be the program's main function.
* In the case of plugins, it's called by `babelwires::openPlugin` (in `PluginSupport/pluginOperations.hpp`).

## TODOs

* High-level TODOs for the framework (including bugs and features) are maintained in `Docs\TODO.md`.
* Domains carry their own TODO list (e.g. `Domains\Music\Docs\TODO.md`)
* Low-level TODOs can be captured in code with `// TODO ` comments.
* When a potential improvement or fix requires some consideration, `// MAYBEDO ` should be used instead.
