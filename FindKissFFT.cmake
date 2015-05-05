# - Try to find KissFFT
# Once done this will define
#
# KISSFFT_FOUND - system has KissFFT
# KISSFFT_INCLUDE_DIRS - the KissFFT include directory
# DUMB_LIBRARIES - The KissFFT libraries

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules (KISSFFT KissFFT)
  set(KISSFFT_INCLUDE_DIRS ${KISSFFT_INCLUDEDIR})
endif()

if(NOT KISSFFT_FOUND)
  find_path(KISSFFT_INCLUDE_DIRS kiss_fft.h)
  find_library(KISSFFT_LIBRARIES NAMES KissFFT)
endif()

#include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(KISSFFT DEFAULT_MSG KISSFFT_INCLUDE_DIRS KISSFFT_LIBRARIES)

#mark_as_advanced(KISSFFT_INCLUDE_DIRS KISSFFT_LIBRARIES)
