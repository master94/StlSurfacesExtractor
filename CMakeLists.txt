project(StlSurfacesExtractor)
cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11)

file(GLOB SRC_FILES "*.cpp" "*.h")
add_executable(${PROJECT_NAME} ${SRC_FILES})
