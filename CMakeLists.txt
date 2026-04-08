cmake_minimum_required(VERSION 3.10)
project(paint VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(server source/server.cpp)
add_executable(client source/client.cpp)

find_package(raylib REQUIRED)
find_package(SFML 2.6 COMPONENTS network REQUIRED)
target_link_libraries(server PRIVATE raylib sfml-network)
target_link_libraries(client PRIVATE raylib sfml-network)
