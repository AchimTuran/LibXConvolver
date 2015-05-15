enable_language(CXX)

find_package(FFTW3 REQUIRED)
find_package(KISS_FFT REQUIRED)

add_definitions(-DUSE_LXC_NATIVE -DUSE_LXC_SSE3 -DUSE_LXC_LOGGING_FILE)

if(WIN32)
  add_definitions(-DTARGET_WINDOWS)
endif()

# include needed libraries
include_directories(${FFTW3_INCLUDE_DIRS}
                    ${KISS_FFT_INCLUDE_DIRS})

# Add all relevant sources to LibXConvolverCore
add_library (LibXConvolverCore STATIC
                               LXC_Core.cpp
                               LXC_fftHandles/LXC_fftHandles.cpp
                               LXC_fftHandles/LXC_fftwf/LXC_fftwf_fmtc.cpp
                               LXC_fftHandles/LXC_fftwf/LXC_fftwfHandle.cpp
                               LXC_Handles/LXC_Native/LXC_Native.cpp
                               LXC_Handles/LXC_Native/LXC_NativeBuffer.cpp
                               LXC_Handles/LXC_Native/LXC_NativeRingBuffer.cpp
                               LXC_Handles/LXC_SSE3/LXC_SSE3.cpp
                               LXC_Handles/LXC_SSE3/LXC_SSE3Buffer.cpp
                               LXC_Handles/LXC_SSE3/LXC_SSE3RingBuffer.cpp)

# Make sure the compiler can find include files for LibXConvolverCore
target_include_directories (LibXConvolverCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})