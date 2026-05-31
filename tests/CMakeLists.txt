# Minimum version required is 3.14 because of FetchContent_MakeAvailable.
cmake_minimum_required(VERSION 3.14)
include(FetchContent)
include(GoogleTest)
project(ReflectiveAngelScriptWrapperTests)

set(CMAKE_CXX_FLAGS "-std=c++26 -freflection")

# GoogleTest.
FetchContent_Declare(googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG v1.17.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    ${ReflectiveAngelScriptWrapper_INCLUDE_DIRS}
    ${angelscript_SOURCE_DIR}/add_on/scriptarray/
    ${angelscript_SOURCE_DIR}/add_on/scripthelper/
    ${angelscript_SOURCE_DIR}/add_on/scriptstdstring/
)

target_link_libraries(${PROJECT_NAME}
    gtest
    gmock_main
    ReflectiveAngelScriptWrapper
)

target_sources(${PROJECT_NAME} PRIVATE
    # Static tests go here for immediate feedback:
    "src/Concepts.cpp"
    "src/TypeDecl.cpp"
    "src/FuncDecl.cpp"

    # AngelScript add ons used during testing:
    ${angelscript_SOURCE_DIR}/add_on/scriptarray/scriptarray.cpp
    ${angelscript_SOURCE_DIR}/add_on/scripthelper/scripthelper.cpp
    ${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring.cpp

    # Unit tests:
    "src/OwnedObject.cpp"
    "src/SharedObject.cpp"
    "src/engine/EngineConstruction.cpp"
    "src/engine/EngineGlobalProperties.cpp"
)

# https://cmake.org/cmake/help/latest/module/GoogleTest.html
gtest_discover_tests(${PROJECT_NAME})
