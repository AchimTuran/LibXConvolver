About
==============
The goal of LXC (LibXConvolver) is to create a cross platform library for fast convolution (http://en.wikipedia.org/wiki/Convolution).
LXC will be available on Windows and Linux. If it is possible I will port it to Android, iOS and MacOSX.

LibXConvolver will support Native (C implementation), SSE2, SSE3, SSE4, AVX, AVX2, CUDA, OpenCL, ARM_VFP and NEON instructions.

But the first step is to release a convolution addon for Kodi's new Audio DSP system (for more details see http://forum.xbmc.org/showthread.php?tid=186857).


LXC Software Architecture
==============
![Software/Architecture] (Documentation/LXC_SoftwareArchitektur_EN.png)


Dependencies
==============
- fftw
- libsndfile
- libsamplerate

Done
==============
- download and build dependencies with scripts
- include compiler flags to select for which platform the library will be build
- implement convolution algorithm with Native and SSE3 instructions

ToDos
==============
- write API documentation
- implement convolution algorithm with SSE2, SSE4, AVX, AVX2, CUDA, OpenCL, ARM_VFP and NEON instructions
- port library to Linux, MacOSX, iOS and Android

