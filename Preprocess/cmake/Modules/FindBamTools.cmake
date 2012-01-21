# - Try to find the BamTools sequence reader library
# Once done this will define
#
#  BAMTOOLS_ROOT_DIR - Set this variable to the root installation of BamTools
#
# Read-Only variables:
#  BAMTOOLS_FOUND - system has the BamTools library
#  BAMTOOLS_INCLUDE_DIR - the BamTools include directory
#  BAMTOOLS_LIBRARIES - The libraries needed to use BamTOols
#=============================================================================
# Authored by Sky Faber <skyfaber@gmail.com>
#
# Distributed under the WTFBPPL (http://tomlea.co.uk/WTFBPPL.txt) without warranty


# Note: This has only been tested on Ubuntu Linux
find_path(BAMTOOLS_INCLUDE_DIR NAMES BamAux.h
          DOC "The BamTools include directory"
)

find_library(BAMTOOLS_LIBRARY NAMES bamtools
          DOC "The BamTools library"
)

include("${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake")
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BamTools DEFAULT_MSG BAMTOOLS_LIBRARY BAMTOOLS_INCLUDE_DIR)

if(BAMTOOLS_FOUND)
  set( BAMTOOLS_LIBRARIES ${BAMTOOLS_LIBRARY} )
  set( BAMTOOLS_INCLUDE_DIRS ${BAMTOOLS_INCLUDE_DIR} )
endif()

mark_as_advanced(BAM_TOOLS_INCLUDE_DIR BAMTOOLS_LIBRARY)
