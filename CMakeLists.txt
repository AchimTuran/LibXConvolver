project(LibXConvolver)

cmake_minimum_required(VERSION 2.6)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(FFTW3 REQUIRED)

# For multiarch support on linux (32/64 bit handling)
include(UseMultiArch)

include_directories(${FFTW3_INCLUDE_DIRS})

set(SOURCES LibXConvolverCore/fftHandles/fftHandles.cpp
            LibXConvolverCore/fftHandles/fftwf/fftwfHandle.cpp
            LibXConvolverCore/fftHandles/fftwf/fftwf_fmtc.cpp
            LibXConvolverCore/LXC_Core.cpp
            LibXConvolverCore/LXCHandles/LXC_Native/LXC_Native.cpp
            LibXConvolverCore/LXCHandles/LXC_Native/LXC_NativeBuffer.cpp
            LibXConvolverCore/LXCHandles/LXC_Native/LXC_NativeRingBuffer.cpp)

add_definitions(-DUSE_LXC_NATIVE)

# Main library
add_library(XConvolver ${SOURCES})
target_link_libraries(XConvolver ${FFTW3_LIBRARIES})

enable_testing()

# Test app need the includes
include_directories(${PROJECT_SOURCE_DIR}/LibXConvolverCore/include)

# Build test app - not by default
add_executable(XConvolverCore-tests EXCLUDE_FROM_ALL LibXConvolverCoreTests/LibXConvolverCoreTests.cpp)
target_link_libraries(XConvolverCore-tests XConvolver ${FFTW3_LIBRARIES})

# Add 'check' target to build test app and execute test(s)
add_custom_target(check ${CMAKE_CTEST_COMMAND} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_dependencies(check XConvolverCore-tests)
add_test(NAME XConvolverCore-tests COMMAND XConvolverCore-tests)

# Installation on unix
if(UNIX)
  install(TARGETS XConvolver DESTINATION ${CMAKE_INSTALL_LIBDIR})
  file(GLOB INCLUDES LibXConvolverCore/include/*.h)
  install(FILES ${INCLUDES} DESTINATION include/LXC)
endif()
