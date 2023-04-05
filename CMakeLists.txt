cmake_minimum_required(VERSION 3.14)

project(halide_test)

find_package(Halide REQUIRED)
#find_package(XRT REQUIRED)

add_executable(halide_test
  main.cpp
)

target_compile_features(halide_test
  PUBLIC
  cxx_std_17
)

target_link_libraries(halide_test
  PRIVATE
  Halide::Halide
  #${XRT_CORE_LIBRARIES}
)
