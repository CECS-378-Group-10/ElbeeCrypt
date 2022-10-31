::
:: This script builds the project using Meson (MSVC backend). Check README.md for more information.
::

::Setup
@echo off
title Project Builder (MSVC)

::Get the location of vcvarsall via a Python script
for /F "tokens=*" %%g in ('python .mesondep/msvc_locator.py') do (set VCVARSALL=%%g)

::Announce the location of vcvarsall
echo Found 'vcvarsall.bat' at path '%VCVARSALL%'

::Initialize the env with vcvarsall.bat
call "%VCVARSALL%" x64

::Execute Meson
meson build && meson compile -C build