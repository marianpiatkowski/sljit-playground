find_path(
  SLJIT_INCLUDE_DIR
  NAMES "sljitLir.h"
  PATHS "${SLJIT_ROOT}/sljit_src"
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  "SLJIT"
  DEFAULT_MSG
  SLJIT_INCLUDE_DIR
  )

if(SLJIT_FOUND)
  set(SLJIT_INCLUDE_DIRS "${SLJIT_INCLUDE_DIR}")
  mark_as_advanced(SLJIT_INCLUDE_DIR SLJIT_ROOT)
endif(SLJIT_FOUND)
