cmake_minimum_required(VERSION 4.1)

project(lab1 CXX)

add_executable(lab1 main.cpp)

configure_file(matrixA.txt matrixA.txt COPYONLY)
configure_file(matrixB.txt matrixB.txt COPYONLY)

target_compile_features(lab1 PUBLIC cxx_std_23)