@echo off
setlocal EnableDelayedExpansion
setlocal EnableExtensions

rem
rem This script builds the project using Conan and Meson (MSVC backend). Check README.md for more information.
rem See: https://stackoverflow.com/a/70505971
rem
rem List files
for /f "delims=" %%i in ('dir /w') do set files=%%i
rem Setup Conan if the directory is nonexistant
if exist conan/conanbuildinfo.txt (
  set /a _0=1
) else (
  set /a _0=0
)
if !_0! NEQ 1 (
  rem Initialize Conan
  conan config init
  rem Generate the necessary configs for Conan via the configuration utility (interactive)
  python .mesondep/conan_init.py
  rem Run Conan
  mkdir conan
  rem conan install . --install-folder^=conan --build^=outdated --profile^=conan-generated -s build_type^=Debug
  conan install . --install-folder^=conan --build^=outdated --profile^=conan-generated
)
rem Build with Conan
conan build . --build-folder conan