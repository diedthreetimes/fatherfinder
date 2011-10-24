cmake_minimum_required (VERSION 2.6)
project (FatherFinder)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(BamTools REQUIRED)
include_directories (${BAMTOOLS_INCLUDE_DIRS})
link_directories (${BAMTOOLS_LIBRARIES})

# set our library and executable destination dirs
set (EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set (LIBRARY_OUTPUT_PATH    "${CMAKE_SOURCE_DIR}/lib")

include_directories (Sequences)
add_subdirectory (Sequences)


add_executable(rflp rflp.cxx)
target_link_libraries (rflp Sequences bamtools)
