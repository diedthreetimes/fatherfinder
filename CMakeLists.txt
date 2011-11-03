cmake_minimum_required (VERSION 2.6)
project (FatherFinder)

option(build_all_tests "Build all unit tests." ON)

# set our library and executable destination dirs
set (EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set (LIBRARY_OUTPUT_PATH    "${CMAKE_SOURCE_DIR}/lib")

include_directories (Sequences)
add_subdirectory (Sequences)

# add_executable(rflp rflp.cxx)
# target_link_libraries (rflp Sequences) #Is this needed if the other library depends on it? bamtools)

macro (add_gtest NAME GROUP )  
  if(${GROUP}_build_tests OR build_all_tests)
    enable_testing()
    add_executable(test/${NAME}
      ${NAME}.cxx)
    target_link_libraries(test/${NAME}
      gtest
      gtest_main
      pthread
      ${ARGN})
    add_test(${GROUP}-tests ${EXECUTABLE_OUTPUT_PATH}/test/${NAME})
  endif()
endmacro (add_gtest) 


# For now the macro is not working as desired so we declare all tests here
# Because of that we also need all linked libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(BamTools REQUIRED)
include_directories (${BAMTOOLS_INCLUDE_DIRS})
link_directories (${BAMTOOLS_LIBRARIES})

add_gtest(Sequences/string_sequence_test sequence Sequences)
add_gtest(Sequences/bam_sequence_test sequence Sequences bamtools)
