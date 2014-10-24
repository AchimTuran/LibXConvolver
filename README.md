About
==============
The goal of LXC (LibXConvolver) is to create a cross platform library for fast convolution (http://en.wikipedia.org/wiki/Convolution).
LXC will be available on Windows and Linux. If it is possible I will port it to Android, iOS and MacOSX.

LibXConvolver will support SSE2, SSE3, AVX, AVX2, CUDA, OpenCL and NEON instructions.

But the first step is to release a convolution addon for Kodi's new Audio DSP system (for more details see http://forum.xbmc.org/showthread.php?tid=186857).


LXC Software Architecture
==============
![Software/Architecture] (Documentation/LXC_SoftwareArchitektur_EN.png)


Dependencies
==============
- fftw
- libsndfile



ToDos
==============
- write API documentation
- download and build dependencies with scripts
- include compiler flags to select for which platform the library will be build
- write a C++-Class interface to use LXC_Core
- write algorithms for SSE2, AVX, AVX2, CUDA, OpenCL and NEON (SSE3 is almost finished, but not tested enough).
- port library to Linux, MacOSX, iOS and Android

