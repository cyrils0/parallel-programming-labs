cmake_minimum_required(VERSION 4.1)

project(lab2 CXX)

add_executable(lab2 c++/main.cpp)

configure_file(input/matrixA.txt matrixA.txt COPYONLY)
configure_file(input/matrixB.txt matrixB.txt COPYONLY)

target_compile_features(lab2 PUBLIC cxx_std_23)

find_package(OpenMP REQUIRED)

target_link_libraries(lab2 PUBLIC OpenMP::OpenMP_CXX)