# ElbeeCrypt - A PoC file-encrypting ransomware for CECS-378

## What Is It?

ElbeeCrypt is a file-encrypting ransomware that utilizes LibSodium, a lightweight cryptography library, to encrypt a user's files using symmetric-key cryptography. This sample is a proof of concept, and does NOT seek to be malicious in any way. A decryptor is available for this ransomware and is built in conjunction with the encryptor. By using the code contained in this repository, we assume **NO RESPONSIBILITY** for any damage to your files or system as a whole.

## Why?

Ransomware is a growing threat to the internet. Large companies and organizations providing essential services such as energy, food, etc are especially at risk. This project was created to better understand the threat from the perspective of an attacker. By completing this project, we now have a deeper understanding of how ransomware works, what it does under the hood, and how defenses can be put up to combat threats such as this in the future.

## Building Instructions

### Overview

This program uses [Meson](https://mesonbuild.com/) (with a Ninja backend) to build. See the below requirements section for further details regarding the toolchain necessary to build this project.

### Requirements

All of the following requirements can be met via your package manager (eg: apt, dnf, pacman, etc) on Linux, Brew or MacPorts on Mac, and Chocolatey on Windows. Additional scripts needed to build the project are included in the `.mesondep` folder. The following programs are needed to compile this project:

- Python (at least version 3.5)
  - Available as `python3` on most Linux distributions
  - Available on Windows via [Python's website](https://www.python.org/downloads/) or the Windows Store
  - Having the Pip package manager installed is highly recommended, and allows builder dependencies to be acquired very easily irregardless of the host
- Meson (at least version 0.55.0)
  - Available as a system package (`meson`) or on PyPi via Pip (`python -m pip install meson`)
- Conan
  - Available as a system package (`conan`) or on PyPi via Pip (`python -m pip install conan`)
- Ninja
  - Available as a system package (`ninja-build` for Debian and Fedora derivatives or `ninja` for everywhere else) or on PyPi via Pip (`python -m pip install ninja`)
    - See Ninja's [Pre built Ninja packages](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages) section on the Ninja wiki for more details
- CMake
  - Available as a system package (`cmake`)
- pkg-config
  - Available as a system package via Chocolatey on Windows (`choco install pkgconfiglite`)
- A compiler toolchain that supports the C++17 standard
  - GCC/Clang (any supported platform) or MSVC on Windows (improves portability and reduces binary size, accessed via the Visual Studio Developer Command Prompt)

Compilation has been tested and is confirmed to work on the following platforms:

- Windows (x86 and x86-64)
  
  - Tested with both MSVC and GCC, though GCC binaries are considerably bigger in size for portability reasons

\* Other operating systems and platforms are UNTESTED, but should still work, given the aforementioned requirements are met.

### Steps (Automatic)

1. Simply run either `conan-build.bat`. The script will prompt you for settings customization, but leaving it at its default (simply press enter when prompted) should be okay. This script will then fetch the appropriate dependencies and build the project automatically.

### Steps (Manual); DEPRECATED: This section is no longer applicable, as the project requires Conan dependencies to run

1. Run the command `meson build` in the root project build directory to generate the output folder. It will be under the name `build`. This directory name can be changed per Meson's syntax, but using the name `build` is standard convention. Additional steps and supporting scripts assume this to be the case, so it is recommended to leave the name as the default

2. Run the command `meson compile -C build` in the root project directory to build the executable via Meson's backend (Ninja by default)

3. Enter the build folder. The resulting executables will be available under the names `elbeecrypt-cryptor` and `elbeecrypt-decryptor`, with appended `.exe` extensions if the host is Windows

\* Optionally, the above steps can be run as a single command for ease of use:

`meson build && meson compile -C build`

\* The resulting executable is portable and is able to run on any device that matches the OS and architecture. However, the target must have the glibc (for GCC) or Visual C++ Redistributable (for MSVC) libs on path and available for execution to be successful on systems other than those that compiled the executable.

\* When building project repeatedly during testing, stick to the same compiler backend you used to initially generate the build directory (eg: if you used the MSVC backend, stick with MSVC for subsequent builds). The build folder can be regenerated simply by deleting it.

## Contributing

Feel free to contribute ideas or fixes as either an issue or a pull request. However, please follow the guidelines outlined in [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) to ensure your input is successfully received. Please keep any and all discussion civil and related to the project. Those not abiding by these guidelines will be prohibited from contributing further. Political discussion of any kind **WILL NOT BE TOLERATED**.
