@echo off
rem place this script into root boost directory
rem adapted from https://github.com/Studiofreya/boost-build-scripts

rem Directory to boost root
rem set boost_dir=boost_1_66_0

rem Number of cores to use when building boost
set cores=%NUMBER_OF_PROCESSORS%

rem What toolset to use when building boost.

rem Visual Studio 2012 -> set msvcver=msvc-11.0
rem Visual Studio 2013 -> set msvcver=msvc-12.0
rem Visual Studio 2015 -> set msvcver=msvc-14.0
rem Visual Studio 2017 -> set msvcver=msvc-14.1

set msvcver=msvc-14.1

rem Start building boost
echo Building %boost_dir% with %cores% cores using toolset %msvcver%.

rem cd %boost_dir%
call bootstrap.bat

rem Static libraries
echo Building 64-bit static library
b2 -j%cores% toolset=%msvcver% address-model=64 architecture=x86 link=static threading=multi runtime-link=shared stage --stagedir=stage/Win64 > build-boost-Win64.log
echo Building 32-bit static library
b2 -j%cores% toolset=%msvcver% address-model=32 architecture=x86 link=static threading=multi runtime-link=shared stage --stagedir=stage/Win32 > build-boost-Win32.log

rem Build DLLs
rem b2 -j%cores% toolset=%msvcver% address-model=64 architecture=x86 link=shared threading=multi runtime-link=shared --build-type=minimal stage --stagedir=stage/x64
rem b2 -j%cores% toolset=%msvcver% address-model=32 architecture=x86 link=shared threading=multi runtime-link=shared --build-type=minimal stage --stagedir=stage/win32

rem pause
