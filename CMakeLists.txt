cmake_minimum_required(VERSION 3.7.0)
project(tetris)


########### dependencies from cmake #########
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

########### modern C++ flags######################
set (CMAKE_CXX_STANDARD             17)
set (CMAKE_CXX_STANDARD_REQUIRED    ON)
set (CMAKE_CXX_EXTENSIONS           OFF)






######## test ########
########### dependencies ####################

find_package(Catch2 REQUIRED)