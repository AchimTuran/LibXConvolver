enable_language(CXX)

#add_definitions(-DUSE_LXC_NATIVE -DUSE_LXC_SSE3 -DUSE_LXC_LOGGING_FILE)
#LibXConvolverUtils needs this include files
include_directories(${SAMPLERATE_INCLUDE_DIRS}
                    ${SNDFILE_INCLUDE_DIRS})

# Add all relevant sources to LibXConvolverUtils
add_library (LibXConvolverUtils   LXC_Exceptions/LXC_CException.cpp
                                  LXC_Exceptions/LXC_CExceptionCout.cpp
                                  LXC_Logging/LXC_LoggingFile.cpp
                                  LXC_Logging/LXC_LoggingPrintf.cpp
                                  LXC_Signal/LXC_Resampler.cpp
                                  LXC_Signal/LXC_WavLoader.cpp
                                  LXC_System/LXC_ModuleTranslator.cpp
                                  LXC_Timer/LXC_CPUTimer.cpp)

# Make sure the compiler can find include files for LibXConvolverUtils
target_include_directories (LibXConvolverUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})