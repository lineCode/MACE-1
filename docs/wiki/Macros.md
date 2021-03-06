# Macros

There are many different macros that can determine how MACE works.

## Macros that affect how MACE works

| *Macro* | *Effect* |
|--------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| MACE_DEBUG | Does more error checking and more verbose error messages. Automatically defined in Debug configurations. If defined to be 0 before including any MACE headers, will never be defined. |
| MACE_EXPOSE_WINAPI | If defined, classes that use Winapi will have functions to allow for direct access to any Winapi variables. Will not work if MACE_WINAPI is not defined. |
| MACE_EXPOSE_POSIX | If defined, classes that use POSIX will have functions to allow for direct access to any POSIX variables. Will not work if MACE_POSIX is not defined |
| MACE_EXPOSE_OPENGL | If defined, the OpenGL abstraction layer and renderer will be exposed by including MACE.h |
| MACE_EXPOSE_GLFW | If defined, `os::WindowModule` will expose the underlying GLFWwindow |
| MACE_EXPOSE_OPENAL | If defined, `AudioModule` will expose functions to return the OpenAL context |
| MACE_EXPOSE_ALL | If defined, defined all macros starting with MACE_EXPOSE_* |


### Libraries

These macros are automatically defined if the specified library is found. Otherwise, you can manually define them.

* MACE_WINAPI
* MACE_POSIX

#### OpenCV

MACE has optional interoptibility with OpenCV. To link OpenCV with CMake, set the OpenCV_* cache variables when running CMake.

MACE will attempt to automatically detect OpenCV and defined MACE_OPENCV if found.

To manually link OpenCV yourself, define MACE_OPENCV to be 1 before including any MACE header file:

```c++
//specify that OpenCV is linked
#define MACE_OPENCV 1
#include <MACE/MACE.h.
```

MACE will include `<opencv2/opencv.hpp>` if MACE_OPENCV is defined.

Additionally, if CV_VERSION is defined, MACE_OPENCV will also be defined automatically.

To request to never use OpenCV, even if it is included, define MACE_OPENCV to be 0 or false.

```c++
//both of the following lines tell MACE to never use OpenCV
#define MACE_OPENCV 0
#define MACE_OPENCV false
#include <MACE/MACE.h>
```

### Operating Systems

One of these macros will be defined based on the operating system.

* MACE_OSX
* MACE_WINDOWS
* MACE_UNIX

### Compilers

One of the following macros will be defined to their respective compiler versions:

*  MACE_BORLAND 
*  MACE_MINGW 
*  MACE_CLANG 
*  MACE_GCC
*  MACE_HP 
*  MACE_INTEL 
*  MACE_SOLARIS
*  MACE_WATCOM 
*  MACE_CRAY 
*  MACE_MIPSPRO 
*  MACE_PORTLAND 
*  MACE_PATHSCALE 
*  MACE_IBM 

Additionally, if the compiler supports GNU C, `MACE_GNU` will be defined.

## Macros defined by MACE

### Informational macros

| *Macro* | *Meaning* |
|-----------------------------|----------------------------------------------------------------------------------------------------------------|
| MACE_VERSION | Always defined to be the current version of MACE, in the format major.minor.patch |
| MACE_VERSION_MAJOR | Major version number |
| MACE_VERSION_MINOR | Minor version number |
| MACE_VERSION_PATCH | Minor version number |
| MACE_TESTS | Defined if CMake built the testing suite. |
| MACE_DEMOS | Defined if CMake built the demos |
| MACE_POINTER_SIZE | Size of a void* on this system. |
| MACE_32_BIT | Whether this system is 32 bit. Defined if MACE_POINTER_SIZE == 4. |
| MACE_64_BIT | Whether this system is 64 bit. Defined if MACE_POINTER_SIZE == 8. |
| MACE_DYNAMIC_LIBRARY_PREFIX | The prefix for dynamic libraries (such as lib.) Some systems may not have a prefix. |
| MACE_DYNAMIC_LIBRARY_SUFFIX | The suffix for dynamic libraries (such as .so, .dll, or .a.) |
| MACE_LITTLE_ENDIAN | Whether the system uses little endian (significant bit/byte last) |
| MACE_BIG_ENDIAN | Whether the system uses big endian (significant bit/byte first) |


### Utility macros

| *Macro* | *Meaning* |
|---------------------------------|---------------------------------------------------------------------------------------------------------|
| MACE_MAKE_VERSION_NUMBER(major, minor, patch) | Creates a single version number from 3 version numbers. Encodes all 3 numbers into a single 32 bit integer. |
| MACE_MAKE_VERSION_STRING(major, minor, patch) | Creates a token that combines the 3 version numbers into the format "Major.minor.patch". This is not stringified. |
| MACE_FUNCTION_EXPORT | Modifier that tells the compiler that this function should be exported to the global symbol table. Could be empty |
| MACE_FUNCTION_IMPORT | Modifier that tells the compiler that this function will be imported from an exported symbol. Could be empty. |
| MACE_HAS_ATTRIBUTE(attr) | If not defined previously, returns 0 if specified attribute is supported, 1 otherwise. If attributes are not supported by the compiler, always returns 0  |
| MACE_HAS_INCLUDE(incl) | If not defined previously, returns 1 if the system has the specified include file, 0 otherwise. Not all compilers support this. If unsupported by the compiler, always returns 0. |
| MACE_FALLTHROUGH | If not defined previously, if the compiler supports the fallthrough attribute, MACE_FALLTHROUGH is defined to that. Otherwise, defined to nothing  |
| MACE_CONSTEXPR | If not defined previously, defined to be `constexpr` on compilers that `constexpr` is supported, otherwise becomes nothing |
| MACE_DEPRECATED | If not defined previously, Marks a function that is deprecated. Based on the compiler, this is defined to the respective attribute to generate the proper warnings. If not supported, empty. |
| MACE_STATIC_ASSERT(condition, message) | If not defined, defined to be `static_assert`. Can be defined before including MACE headers to change the static assert used by MACE |
| MACE_STRINGIFY(name) | Directly stringifies a macro  |
| MACE_STRINGIFY_NAME(name) | Stringifies a macro name. If the expanded macro is empty, this function returns "" |
| MACE_STRINGIFY_DEFINITION(name) | Stringifies the definition of a macro. If the macro doesn't have a definition, this function returns "" |

