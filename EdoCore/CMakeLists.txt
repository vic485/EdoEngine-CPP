cmake_minimum_required(VERSION 3.14)
project(EdoCore)

set(CMAKE_CXX_STANDARD 14)

add_library(EdoCore SHARED src/library.cpp src/library.h)