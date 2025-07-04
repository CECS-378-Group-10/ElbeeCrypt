####################################
# Project Settings
####################################
## Meson Initializer
project(
	'elbeecrypt', # Your project's name (this will be the same name as the output executable)
	'cpp', # Set this as "c" for a C project and "cpp" for a C++ project
	version: '1.0', # Your project's version
	license: 'AFL-3.0', # Your project's license in SPDX format (see https://spdx.org/licenses/), eg: GPL-3.0-or-later
	default_options: ['cpp_std=c++17', 'warning_level=3', 'optimization=2', 'werror=false', 'debug=true'] # Check https://mesonbuild.com/Builtin-options.html for more info on this section
)

##################
## General variables
# Get the compiler that Meson is using
compiler = meson.get_compiler('cpp')

##################

## Project Settings
# General settings
append_version = false # Appends the version info onto the name of the output executable
wingcc_enable_static = true # Statically links GCC/MinGW dependencies with the executable. Increaes build size tenfold, but makes the executable portable

# GCC settings
gcc_pedantic_aggressive = false # Fail the build instead of simply warning (done via the "-pedantic-errors" flag)

####################################


####################################
# Environment Variables
####################################
## General
python_cmd = 'python'
meson_dep_loc = '.mesondep/'

####################################


####################################
# Conan Dependencies
####################################
## Set the dependencies needed by the project (Conan will take care of this section)
dependencies = [
	dependency('libsodium')
]

####################################


####################################
# Compiler Settings
####################################
## File globbing (https://github.com/mesonbuild/meson/tree/master/test%20cases/common/48%20file%20grabber)
# Set the glob extensions for C++ source files
c_source_exts = '"*.c *.cc *.cpp *.cxx *.c++"'
c_header_exts = '"*.h *.hh *.hpp *.hxx *.h++"'

# Set the command prefixes
glob_script_dir = meson_dep_loc + 'glob.py'

# Glob the source directory for the cryptor, decryptor, and common portions of the project
csrc = run_command(python_cmd, glob_script_dir, 'src/elbeecrypt/cryptor', c_source_exts, check: true).stdout().strip()
desrc = run_command(python_cmd, glob_script_dir, 'src/elbeecrypt/decryptor', c_source_exts, check: true).stdout().strip()
common_sources = run_command(python_cmd, glob_script_dir, 'src/elbeecrypt/common', c_source_exts, check: true).stdout().strip()

# Combine the common portions with both the cryptor and decryptor portions 
cryptor_sources = '\n'.join([csrc, common_sources]).split('\n')
decryptor_sources = '\n'.join([desrc, common_sources]).split('\n')

##################
## Compiler/host tweaks

# Set compiler arguments
compiler_args = [] # Empty by default
linker_args = [] # Empty by default

# GCC tweaks
if compiler.get_id() == 'gcc'
	# Fail builds that violate the "pedantic" rule instead of simply warning
	if gcc_pedantic_aggressive
		compiler_args += ['-pedantic-errors']
		linker_args += ['-pedantic-errors']
	endif
endif

# Windows-specific tweaks
if host_machine.system() == 'windows'
	# Enable static compilation for GCC if the user opted for it
	if compiler.get_id() == 'gcc' and wingcc_enable_static
		linker_args += ['-static']
	endif
endif

##################
## Set the executable settings

# Set the name of the executable
executable_name = meson.project_name()
if append_version
	# Add the version info if selected by the user
	executable_name += '_' + meson.project_version()
endif

# Set the executable settings for the cryptor portion
executable(
	executable_name + '-cryptor', # Set the name of the target
	cryptor_sources, # Set the source files
	dependencies: dependencies, # Conan dependencies
	cpp_args: compiler_args,
	link_args: linker_args,
	include_directories: include_directories('include'), # Contains headers for both the project and required libs,
	install: false, # Don't install by default
	win_subsystem: 'windows'
)

# Set the executable settings for the decryptor portion
executable(
	executable_name + '-decryptor', # Set the name of the target
	decryptor_sources, # Set the source files
	dependencies: dependencies, # Conan dependencies
	cpp_args: compiler_args,
	link_args: linker_args,
	include_directories: include_directories('include'), # Contains headers for both the project and required libs,
	install: false, # Don't install by default
	win_subsystem: 'console'
)
####################################