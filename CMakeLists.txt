cmake_minimum_required(VERSION 3.20)
project(MenuSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(menu_app
    main.cpp
    src/Graph.cpp
    src/HashTable.cpp
)

target_include_directories(menu_app PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

if(MSVC)
    target_compile_options(menu_app PRIVATE /W4)
else()
    target_compile_options(menu_app PRIVATE -Wall -Wextra -pedantic)
endif()
