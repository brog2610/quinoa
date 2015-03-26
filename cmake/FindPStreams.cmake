# Find the Pstreams library
#
#  PSTREAMS_FOUND - System has pstreams
#  PSTREAMS_INCLUDES - The pstreams include directory (pstreams is header-only)
#
#  Set PSTREAMS_ROOT before calling find_package to a path to add an additional
#  search path, e.g.,
#
#  Usage:
#
#  set(PSTREAMS_ROOT "/path/to/custom/pstreams") # prefer over system
#  find_package(Pstreams)

if(PSTREAMS_INCLUDES)
  # Already in cache, be silent
  set (PSTREAMS_FIND_QUIETLY TRUE)
endif()

FIND_PATH(PSTREAMS_INCLUDES NAMES pstreams/pstream.h
          HINTS ${PSTREAMS_ROOT}/include)

# Handle the QUIETLY and REQUIRED arguments and set PSTREAMS_FOUND to TRUE if
# all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Pstreams DEFAULT_MSG PSTREAMS_INCLUDES)

MARK_AS_ADVANCED(PSTREAMS_INCLUDES)