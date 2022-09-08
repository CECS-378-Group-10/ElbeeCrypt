::
:: This script builds the project using Meson (GCC backend). Check README.md for more information.
::

::Setup
@echo off
title Project Builder (GCC)

::Execute Meson
meson build && meson compile -C build
pause