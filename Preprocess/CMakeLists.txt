cmake_minimum_required (VERSION 2.6)
project (FatherFinder)

option(build_all_tests "Build all unit tests." ON)

# set our library and executable destination dirs
set (EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set (LIBRARY_OUTPUT_PATH    "${CMAKE_SOURCE_DIR}/lib")

macro (add_gtest NAME GROUP )  
  if(${GROUP}_build_tests OR build_all_tests)
    enable_testing()
    add_executable(test/${GROUP}/${NAME}
      ${NAME}.cxx)
    target_link_libraries(test/${GROUP}/${NAME}
      gtest
      gtest_main
      pthread
      ${ARGN})
    add_test(${GROUP}-tests ${EXECUTABLE_OUTPUT_PATH}/test/${NAME})
  endif()
endmacro (add_gtest) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(BamTools REQUIRED)
include_directories (${BAMTOOLS_INCLUDE_DIRS})
link_directories (${BAMTOOLS_LIBRARIES})

include_directories (Sequences)
add_subdirectory (Sequences)

include_directories (encryption)
add_subdirectory (encryption)

add_executable(rflp rflp.cxx)
add_executable(saveAsText saveAsTxt.cxx)
add_executable(encryptGenome encryptGenome.cxx) 
add_executable(encryptBench encryptBench.cxx) 
add_executable(shi-sm shi-sm.cxx) # This exe does not belong here but we piggy back anyways

# if we need to link bamtools here than we may need to do a find package first
target_link_libraries(shi-sm gmp gmpxx encryption crypto)
target_link_libraries(encryptGenome gmpxx encryption crypto gmp)
target_link_libraries(encryptBench gmpxx encryption crypto gmp)
target_link_libraries(saveAsText Sequences bamtools)
target_link_libraries(rflp Sequences bamtools) 
