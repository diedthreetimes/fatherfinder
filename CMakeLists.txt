cmake_minimum_required (VERSION 2.6)
project (FatherFinder)

option(build_all_tests "Build all unit tests." ON)

# set our library and executable destination dirs
set (EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set (LIBRARY_OUTPUT_PATH    "${CMAKE_SOURCE_DIR}/lib")

include_directories (Sequences)
add_subdirectory (Sequences)

add_executable(rflp rflp.cxx)
target_link_libraries (rflp Sequences) #Is this needed if the other library depends on it? bamtools)
