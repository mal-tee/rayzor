cmake_minimum_required(VERSION 3.16)
project(Rayzor)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
# configure paths accordingly
set(SFML_DIR "C:/sfml/cmake-build-release/")
set(TGUI_DIR "C:/sfml/tgui/cmake-build-release/")
include_directories("C:/sfml/tgui/include")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # configure paths accordingly
    set(SFML_DIR "C:/sfml/cmake-build-debug/")
    set(TGUI_DIR "C:/sfml/tgui/cmake-build-debug/")
endif()
# Detect and add SFML
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics network audio)
find_package(TGUI 0.8 REQUIRED)

add_executable(Rayzor main.h main.cpp Line.cpp enemy.cpp helper.cpp Level.cpp)

#https://stackoverflow.com/a/50882216
if(MSVC)
    target_compile_options(Rayzor PRIVATE /W3)
else()
    target_compile_options(Rayzor PRIVATE -Wall -Wextra)
endif()

target_link_libraries(Rayzor tgui sfml-graphics sfml-audio )