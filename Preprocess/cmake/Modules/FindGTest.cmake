

# Note: This has only been tested on Ubuntu Linux
find_path(GTEST_INCLUDE_DIR NAMES BamAux.h
          DOC "The BamTools include directory"
)

find_library(GTEST_LIBRARY NAMES bamtools
          DOC "The BamTools library"
)

include("${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake")
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GTest DEFAULT_MSG GTEST_LIBRARY GTEST_INCLUDE_DIR)

if(GTEST_FOUND)
  set( GTEST_LIBRARIES ${GTEST_LIBRARY} )
  set( GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR} )
endif()

mark_as_advanced(GTEST_INCLUDE_DIR GTEST_LIBRARY)
