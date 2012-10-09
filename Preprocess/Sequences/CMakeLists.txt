option(sequence_build_tests "Build all of Sequences's unit tests." OFF)

add_library (Sequences Sequence.cxx BamSequence.cxx StringSequence.cxx Enzyme.cxx InsertionSequence.cxx)

find_package(GTest)
include_directories (${GTEST_INCLUDE_DIRS})
link_directories (${GTEST_LIBRARIES})

add_gtest(string_sequence_test Sequences Sequences)
add_gtest(bam_sequence_test Sequences Sequences bamtools)
add_gtest(insertion_sequence_test Sequences Sequences bamtools)
