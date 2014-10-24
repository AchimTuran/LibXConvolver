# - Try to find fftw3
# Once done this will define
#
# FFTW3_FOUND - system has fftw3
# FFTW3_INCLUDE_DIRS - the fftw3 include directory
# DUMB_LIBRARIES - The fftw3 libraries

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules (FFTW3 fftw3f)
  set(FFTW3_INCLUDE_DIRS ${FFTW3_INCLUDEDIR})
endif()

if(NOT FFTW3_FOUND)
  find_path(FFTW3_INCLUDE_DIRS fftw3.h)
  find_library(FFTW3_LIBRARIES NAMES fftw3f)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW3 DEFAULT_MSG FFTW3_INCLUDE_DIRS FFTW3_LIBRARIES)

mark_as_advanced(FFTW3_INCLUDE_DIRS FFTW3_LIBRARIES)
