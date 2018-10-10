cmake_minimum_required(VERSION 3.7)
project(tncg15-mc-raytracer)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Scene.h Utility.h Camera.h RayTrace.h)
add_executable(tncg15-mc-raytracer ${SOURCE_FILES})