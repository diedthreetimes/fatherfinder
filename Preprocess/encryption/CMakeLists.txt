option(encryption_build_tests "Build all of encryption's unit tests." OFF)

add_library (encryption elgamal.cxx)

find_package(GTest)
include_directories (${GTEST_INCLUDE_DIRS})
link_directories (${GTEST_LIBRARIES})

add_gtest(elgamal_test encryption encryption gmp gmpxx)
