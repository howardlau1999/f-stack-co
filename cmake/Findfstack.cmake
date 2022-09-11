find_path(FSTACK_INCLUDE_DIRS
  NAMES ff_api.h
  HINTS
  ${FSTACK_INCLUDE_DIR}
  ${FSTACK_ROOT_DIR}
  ${FSTACK_ROOT_DIR}/include)

find_library(FSTACK_LIBRARIES
  NAMES fstack
  HINTS
  ${FSTACK_LIB_DIR}
  ${FSTACK_ROOT_DIR}
  ${FSTACK_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(fstack DEFAULT_MSG FSTACK_INCLUDE_DIRS FSTACK_LIBRARIES)
mark_as_advanced(FSTACK_INCLUDE_DIR FSTACK_LIBRARIES)
