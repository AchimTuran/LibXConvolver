# - Try to find fftw3
# Once done this will define
#
# FFTW3_FOUND - system has fftw3
# FFTW3_INCLUDE_DIRS - the fftw3 include directory
# DUMB_LIBRARIES - The fftw3 libraries

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules (SNDFILE fftw3f)
  set(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDEDIR})
endif()

if(NOT SNDFILE_FOUND)
  find_path(SNDFILE_INCLUDE_DIRS sndfile.hh)
  find_library(SNDFILE_LIBRARIES NAMES sndfile)
endif()

#include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(SNDFILE DEFAULT_MSG SNDFILE_INCLUDE_DIRS SNDFILE_LIBRARIES)

#mark_as_advanced(SNDFILE_INCLUDE_DIRS FFTW3_LIBRARIES)
