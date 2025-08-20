cmake_minimum_required(VERSION 3.15)
project(StochasticRayTracer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(raytracer
  src/main.cpp
)

# Faster release flags
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(raytracer PRIVATE -O3 -march=native -mtune=native -Wall -Wextra -Wno-missing-field-initializers)
elseif (MSVC)
  target_compile_options(raytracer PRIVATE /O2 /arch:AVX2 /permissive- /W4)
endif()
