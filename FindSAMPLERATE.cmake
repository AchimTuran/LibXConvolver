# - Try to find fftw3
# Once done this will define
#
# FFTW3_FOUND - system has fftw3
# FFTW3_INCLUDE_DIRS - the fftw3 include directory
# DUMB_LIBRARIES - The fftw3 libraries

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules (SAMPLERATE fftw3f)
  set(SAMPLERATE_INCLUDE_DIRS ${FFTW3_INCLUDEDIR})
endif()

if(NOT SAMPLERATE_FOUND)
  find_path(SAMPLERATE_INCLUDE_DIRS samplerate.h)
  find_library(SAMPLERATE_LIBRARIES NAMES samplerate)
endif()

#include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(SAMPLERATE DEFAULT_MSG SAMPLERATE_INCLUDE_DIRS SAMPLERATE_LIBRARIES)

#mark_as_advanced(SAMPLERATE_INCLUDE_DIRS SAMPLERATE_LIBRARIES)
