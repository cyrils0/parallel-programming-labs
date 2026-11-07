cmake_minimum_required(VERSION 3.25)

project(lab4 LANGUAGES CXX CUDA)

add_executable(lab4 c++/main.cu)

set_target_properties(lab4 PROPERTIES
    CUDA_ARCHITECTURES 89
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)