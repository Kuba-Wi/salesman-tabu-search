cmake_minimum_required(VERSION 3.10)

project(salesman_tabu_search)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPILE_FLAGS -Wall -pedantic)
add_executable(${PROJECT_NAME} main.cpp pathFinder.cpp geneticFinder.cpp)
target_link_libraries(${PROJECT_NAME} pthread)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_FLAGS})
