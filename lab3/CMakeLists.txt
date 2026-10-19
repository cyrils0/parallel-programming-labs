cmake_minimum_required(VERSION 4.1)

project(lab3 CXX)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(lab3 c++/main.cpp)

configure_file(input/matrixA.txt matrixA.txt COPYONLY)
configure_file(input/matrixB.txt matrixB.txt COPYONLY)

target_compile_features(lab3 PUBLIC cxx_std_23)

target_link_libraries(lab3 PRIVATE MPI::MPI_CXX)
