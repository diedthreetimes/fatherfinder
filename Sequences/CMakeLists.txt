option(sequence_build_tests "Build all of Sequences's unit tests." OFF)

add_library (Sequences Sequence.cxx BamSequence.cxx StringSequence.cxx )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(BamTools REQUIRED)
include_directories (${BAMTOOLS_INCLUDE_DIRS})
link_directories (${BAMTOOLS_LIBRARIES})

#find_package(gtest)
#include_directories (${GTEST_INCLUDE_DIRS})
#link_directories (${GTEST_LIBRARIES})

add_gtest(string_sequence_test Sequences Sequences)
add_gtest(bam_sequence_test Sequences Sequences bamtools)

