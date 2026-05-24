# ReflectiveAngelScriptWrapper
An experimental C++ wrapper for AngelScript that makes use of the recently-implemented reflection features of C++26.

This README assumes you are already familiar with AngelScript and its features. If you are not, then visit TODO.

The primary goal of this template library is to greatly reduce the registration code required to set up your application interface. Without it, you would have to register all of your classes, global variables, global functions, enums, etc., manually, which is error prone and a headache to maintain.

This wrapper does not aim to put AngelScript in a black box: you can directly access, for example, the engine object, if you need to.

## Todo

1. Write instructions on building GCC 16.1.
1. Avoid using custom CMake build.
1. Write examples as I write features and tests.