# Minimum CMake version required to generate
# our build system
cmake_minimum_required(VERSION 3.10)

# Name of our Project
project(sensor)

# add_executable creates an executable with the given name.
add_executable(${CMAKE_PROJECT_NAME} main.cpp i2c.cpp tps43.cpp)

# link libraries
target_link_libraries(${CMAKE_PROJECT_NAME} pthread)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
