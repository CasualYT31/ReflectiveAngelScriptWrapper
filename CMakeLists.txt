# Minimum version required is 3.14 because of FetchContent_MakeAvailable.
cmake_minimum_required(VERSION 3.14)
include(FetchContent)
project(ReflectiveAngelScriptWrapper)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++26 -freflection")

# Autowrapper must be on as we have special handling for the generic call convention that relies on it.
# option(${PROJECT_NAME}_AUTOWRAPPER_ADD_ON "Include the autowrapper add on" OFF)
option(${PROJECT_NAME}_CONTEXT_MANAGER_ADD_ON "Include the context manager add on" OFF)
option(${PROJECT_NAME}_DATETIME_ADD_ON "Include the datetime add on" OFF)
option(${PROJECT_NAME}_DEBUGGER_ADD_ON "Include the debugger add on" OFF)
option(${PROJECT_NAME}_SCRIPT_ANY_ADD_ON "Include the script any add on" OFF)
option(${PROJECT_NAME}_SCRIPT_ARRAY_ADD_ON "Include the array add on" OFF)
option(${PROJECT_NAME}_SCRIPT_BUILDER_ADD_ON "Include the script builder add on" OFF)
option(${PROJECT_NAME}_SCRIPT_DICTIONARY_ADD_ON "Include the script dictionary add on" OFF)
option(${PROJECT_NAME}_SCRIPT_FILE_ADD_ON "Include the script file add on" OFF)
option(${PROJECT_NAME}_SCRIPT_GRID_ADD_ON "Include the script grid add on" OFF)
option(${PROJECT_NAME}_SCRIPT_HANDLE_ADD_ON "Include the script handle add on" OFF)
option(${PROJECT_NAME}_SCRIPT_HELPER_ADD_ON "Include the script helper add on" OFF)
option(${PROJECT_NAME}_SCRIPT_MATH_ADD_ON "Include the script math add on" OFF)
option(${PROJECT_NAME}_SCRIPT_SOCKET_ADD_ON "Include the script socket add on" OFF)
option(${PROJECT_NAME}_SCRIPT_STDSTRING_ADD_ON "Include the script stdstring add on" OFF)
option(${PROJECT_NAME}_SERIALIZER_ADD_ON "Include the serializer add on" OFF)
option(${PROJECT_NAME}_WEAKREF_ADD_ON "Include the weakref add on" OFF)

# AngelScript.
FetchContent_Declare(angelscript
	URL https://www.angelcode.com/angelscript/sdk/files/angelscript_2.38.0.zip
	SOURCE_SUBDIR angelscript/projects/cmake
)
FetchContent_MakeAvailable(angelscript)

# Write include directories here.
set(${PROJECT_NAME}_INCLUDE_DIRS_LIST
    "${PROJECT_SOURCE_DIR}/include"
    ${angelscript_SOURCE_DIR}/angelscript/include
)

# Write source files here.
set(${PROJECT_NAME}_SOURCE_FILES_LIST
    "src/Stub.cpp"
)

# Apply add on options to our include dirs and source files.
# if(${PROJECT_NAME}_AUTOWRAPPER_ADD_ON)
if(TRUE)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/autowrapper)
endif()

if(${PROJECT_NAME}_CONTEXT_MANAGER_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/contextmgr)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/contextmgr/contextmgr.cpp)
endif()

if(${PROJECT_NAME}_DATETIME_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/datetime)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/datetime/datetime.cpp)
endif()

if(${PROJECT_NAME}_DEBUGGER_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/debugger)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/debugger/debugger.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_ANY_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scriptany)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptany/scriptany.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_ARRAY_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scriptarray)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptarray/scriptarray.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_BUILDER_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scriptbuilder)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptbuilder/scriptbuilder.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_DICTIONARY_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scriptdictionary)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptdictionary/scriptdictionary.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_FILE_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scriptfile)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptfile/scriptfile.cpp)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptfile/scriptfilesystem.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_GRID_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scriptgrid)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptgrid/scriptgrid.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_HANDLE_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scripthandle)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scripthandle/scripthandle.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_HELPER_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scripthelper)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scripthelper/scripthelper.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_MATH_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scriptmath)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptmath/scriptmath.cpp)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptmath/scriptmathcomplex.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_SOCKET_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scriptsocket)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptsocket/scriptsocket.cpp)
endif()

if(${PROJECT_NAME}_SCRIPT_STDSTRING_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/scriptstdstring)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring.cpp)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring_utils.cpp)
endif()

if(${PROJECT_NAME}_SERIALIZER_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/serializer)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/serializer/serializer.cpp)
endif()

if(${PROJECT_NAME}_WEAKREF_ADD_ON)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS_LIST ${angelscript_SOURCE_DIR}/add_on/weakref)
    list(APPEND ${PROJECT_NAME}_SOURCE_FILES_LIST ${angelscript_SOURCE_DIR}/add_on/weakref/weakref.cpp)
endif()

add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${${PROJECT_NAME}_INCLUDE_DIRS_LIST}
)

target_link_libraries(${PROJECT_NAME}
    angelscript
)

target_sources(${PROJECT_NAME} PRIVATE
    ${${PROJECT_NAME}_SOURCE_FILES_LIST}
)

add_subdirectory("tests")
