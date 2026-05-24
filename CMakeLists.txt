# Minimum version required is 3.14 because of FetchContent_MakeAvailable.
cmake_minimum_required(VERSION 3.14)
include(FetchContent)
project(ReflectiveAngelScriptWrapper)

set(CMAKE_CXX_FLAGS "-std=c++26 -freflection")

# AngelScript.
FetchContent_Declare(angelscript
	URL https://www.angelcode.com/angelscript/sdk/files/angelscript_2.38.0.zip
	SOURCE_SUBDIR angelscript/projects/cmake
)
FetchContent_MakeAvailable(angelscript)

# Expose public includes to other projects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS
    "${PROJECT_SOURCE_DIR}/include"
    ${angelscript_SOURCE_DIR}/angelscript/include
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
)

add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${ReflectiveAngelScriptWrapper_INCLUDE_DIRS}
    # AngelScript add-on include directories go here.
)

target_link_libraries(${PROJECT_NAME}
    angelscript
)

target_sources(${PROJECT_NAME} PRIVATE
    "src/AngelScriptEngine.cpp"
    # AngelScript add-on sources go here.
)

add_subdirectory("tests")
