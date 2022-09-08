::
:: This script builds the project using Meson (MSVC backend). Check README.md for more information.
::

::Setup
@echo off
title Project Builder (MSVC)
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

::Execute Meson
meson build && meson compile -C build
pause