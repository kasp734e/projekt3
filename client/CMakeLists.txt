cmake_minimum_required(VERSION 3.10)
project(DiskoClient)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

# Find Python dev headers and library (required for Python.h etc.)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Collect all cpp files recursively inside src/
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# llhttp sources (generated, shipped in external/llhttp)
set(LLHTTP_SRC
    ${CMAKE_SOURCE_DIR}/external/llhttp/api.c
    ${CMAKE_SOURCE_DIR}/external/llhttp/http.c
    ${CMAKE_SOURCE_DIR}/external/llhttp/llhttp.c
)

# Fix Windows API version warning - added later for remove warning
if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# fmt library to use header-only mode - added later to avoid fmt issues
add_definitions(-DFMT_HEADER_ONLY)

# Add the AudioFile library
add_subdirectory(include/AudioFile)

# Add pybind11 (header-only) so we can use pybind11::headers
add_subdirectory(include/pybind11)
 
# Add miniaudio (single-file library)
add_subdirectory(include/miniaudio)

# Build executable with project sources + llhttp sources
add_executable(${PROJECT_NAME} ${SRC_FILES} ${LLHTTP_SRC})

# Add local Restinio + dependencies (bundled in external/)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/external/restinio
    ${CMAKE_SOURCE_DIR}/external/asio
    ${CMAKE_SOURCE_DIR}/external/fmt
    ${CMAKE_SOURCE_DIR}/external/json_dto
    ${CMAKE_SOURCE_DIR}/external/llhttp
    ${CMAKE_SOURCE_DIR}/external/expected-lite
    ${CMAKE_SOURCE_DIR}/external/rapidjson
    ${CMAKE_SOURCE_DIR}/include
    
)

# Link the AudioFile library
# Link pybind11 headers-only target
# Link Python (provides include dirs so <Python.h> etc. are found)
# linked dependencies
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Threads::Threads
    CURL::libcurl
    Python3::Python
    pybind11::headers
    AudioFile
    miniaudio
)
