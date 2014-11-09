project(LibXConvolver)

cmake_minimum_required(VERSION 2.6)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(FFTW3 REQUIRED)
if(NOT FFTW3_FOUND)
  add_library(fftw3f STATIC IMPORTED)
  set_property(TARGET fftw3f PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/Libs/fftw3_dll32/libfftw3f-3.lib)
  add_dependencies(fftw3f project_fftw3f)
  set(FFTW3_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/Libs/fftw3_dll32)
  set(FFTW3_LIBRARIES ${PROJECT_SOURCE_DIR}/Libs/fftw3_dll32/libfftw3f-3.lib)
  set(FFTW3_FOUND TRUE)
endif()

find_package(SAMPLERATE REQUIRED)
if(NOT SAMPLERATE_FOUND)
  add_library(SAMPLERATE STATIC IMPORTED)
  set_property(TARGET SAMPLERATE PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/Libs/samplerate/samplerate.lib)
  add_dependencies(SAMPLERATE project_SAMPLERATE)
  set(SAMPLERATE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/Libs/samplerate)
  set(SAMPLERATE_LIBRARIES ${PROJECT_SOURCE_DIR}/Libs/samplerate/samplerate.lib)
  set(SAMPLERATE_FOUND TRUE)
endif()

find_package(SNDFILE REQUIRED)
if(NOT SNDFILE_FOUND)
  add_library(SNDFILE STATIC IMPORTED)
  set_property(TARGET SNDFILE PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/Libs/sndfile_32/libsndfile-1.lib)
  add_dependencies(SNDFILE project_SNDFILE)
  set(SNDFILE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/Libs/sndfile_32)
  set(SNDFILE_LIBRARIES ${PROJECT_SOURCE_DIR}/Libs/sndfile_32/libsndfile-1.lib)
  set(SNDFILE_FOUND TRUE)
endif()

include_directories(${install_dir}/Libs/include)

# For multiarch support on linux (32/64 bit handling)
include(UseMultiArch)

include_directories(${FFTW3_INCLUDE_DIRS})

add_definitions(-DUSE_LXC_NATIVE -DUSE_LXC_SSE3 -DUSE_LXC_LOGGING_FILE)

if(WIN32)
  add_definitions(-DTARGET_WINDOWS)
endif()

add_subdirectory(LibXConvolverCore)
add_subdirectory(LibXConvolverUtils)
#add_subdirectory(LibXConvolverCoreTests)

# Main library
#add_library(XConvolver ${SOURCES})
#target_link_libraries(XConvolver ${FFTW3_LIBRARIES})

enable_testing()

# Test app need the includes
include_directories(${XConvolverCore}
					${SAMPLERATE_INCLUDE_DIRS}
					${FFTW3_INCLUDE_DIRS}
					${SNDFILE_INCLUDE_DIRS})


#${PROJECT_SOURCE_DIR}/LibXConvolverCore/include)

# Build test app - not by default
add_executable(XConvolverCore-tests EXCLUDE_FROM_ALL LibXConvolverCoreTests/LibXConvolverCoreTests.cpp)
target_link_libraries(XConvolverCore-tests 	XConvolverCore 
											XConvolverUtils 
											${FFTW3_LIBRARIES}
											${SNDFILE_LIBRARIES}
											${SAMPLERATE_LIBRARIES})

# Add 'check' target to build test app and execute test(s)
#add_custom_target(check ${CMAKE_CTEST_COMMAND} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
#add_dependencies(check XConvolverCore-tests)
#add_test(NAME XConvolverCore-tests COMMAND XConvolverCore-tests)
add_test(XConvolverCore-Test XConvolverCore-tests)

# Installation on unix
#if(UNIX)
#  install(TARGETS XConvolver DESTINATION ${CMAKE_INSTALL_LIBDIR})
#  file(GLOB INCLUDES LibXConvolverCore/include/*.h)
#  install(FILES ${INCLUDES} DESTINATION include/LXC)
#endif()
