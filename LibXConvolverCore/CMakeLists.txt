enable_language(CXX)

#find_package(FFTW3 REQUIRED)

# add_definitions(-DUSE_LXC_NATIVE -DUSE_LXC_SSE3 -DUSE_LXC_LOGGING_FILE)

# if(WIN32)
  # add_definitions(-DTARGET_WINDOWS)
# endif()

# Test app need the includes
include_directories(${FFTW3_INCLUDE_DIRS})

# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
add_library (XConvolverCore STATIC
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

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
#target_include_directories (XConvolverCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})