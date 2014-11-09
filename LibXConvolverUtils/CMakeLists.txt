enable_language(CXX)

#find_package(FFTW3 REQUIRED)

#add_definitions(-DUSE_LXC_NATIVE -DUSE_LXC_SSE3 -DUSE_LXC_LOGGING_FILE)
# Test app need the includes
include_directories(${SAMPLERATE_INCLUDE_DIRS}
					${SNDFILE_INCLUDE_DIRS})

# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
add_library (XConvolverUtils 	LXC_Exceptions/LXC_CException.cpp
								LXC_Exceptions/LXC_CExceptionCout.cpp
								LXC_Logging/LXC_LoggingFile.cpp
								LXC_Logging/LXC_LoggingPrintf.cpp
								LXC_Signal/LXC_Resampler.cpp
								LXC_Signal/LXC_WavLoader.cpp
								LXC_System/LXC_OptimizationTranslator.cpp
								LXC_Timer/LXC_CPUTimer.cpp)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
#target_include_directories (XConvolverUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})