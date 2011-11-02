option(sequence_build_tests "Build all of Sequences's unit tests." ON)

add_library (Sequences Sequence.cxx BamSequence.cxx StringSequence.cxx )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(BamTools REQUIRED)
include_directories (${BAMTOOLS_INCLUDE_DIRS})
link_directories (${BAMTOOLS_LIBRARIES})

#find_package(gtest)
#include_directories (${GTEST_INCLUDE_DIRS})
#link_directories (${GTEST_LIBRARIES})

#TODO: Figure out whats going on with the add_test behaviour
if(seuqence_build_tests OR build_all_tests)
  enable_testing()
    add_executable(test/string_sequence_test
        string_sequence_test.cxx)
    target_link_libraries(test/string_sequence_test
      gtest
      gtest_main
      pthread)
    target_link_libraries(test/string_sequence_test Sequences bamtools)
    add_test(sequence-tests ${EXECUTABLE_OUTPUT_PATH}/test/string_sequence_test)
endif()
