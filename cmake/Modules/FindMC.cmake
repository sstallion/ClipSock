# Copyright 2024 Steven Stallion
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

#[=======================================================================[.rst:
FindMC
------

Find the Microsoft Win32 Message Compiler, which compiles instrumentation
manifests and message text files.

Commands
^^^^^^^^

The following commands are defined by this module:

.. command:: add_messages

  .. code-block:: cmake

    add_messages(<name> <message>...
                 [OUTPUT_DIRECTORY <directory>])

  The :command:`add_messages()` command adds a custom target ``<name>``, which
  compiles the specified ``<message>``, optionally placing generated header
  and resource files under the ``<directory>`` specified by the
  ``OUTPUT_DIRECTORY`` option.  Relative paths are treated with respect to the
  value of the :variable:`CMAKE_CURRENT_SOURCE_DIR` and
  :variable:`CMAKE_CURRENT_BINARY_DIR` variables for the message and output
  directory, respectively. By default, generated files are placed under
  the :variable:`CMAKE_CURRENT_BINARY_DIR` directory.

Variables
^^^^^^^^^

The following variables are defined by this module:

.. variable:: MC_FOUND

  True if the Message Compiler was found.

.. variable:: MC_EXECUTABLE

  The path to the Message Compiler executable.
#]=======================================================================]

function(add_messages name message)
  set(options)
  set(oneValueArgs OUTPUT_DIRECTORY)
  set(multiValueArgs)
  cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(arg_OUTPUT_DIRECTORY)
    cmake_path(ABSOLUTE_PATH arg_OUTPUT_DIRECTORY BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  else()
    set(arg_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  endif()

  foreach(input IN LISTS message arg_UNPARSED_ARGUMENTS)
    cmake_path(ABSOLUTE_PATH input BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    cmake_path(GET input STEM stem)

    set(header ${arg_OUTPUT_DIRECTORY}/${stem}.h)
    set(resource ${arg_OUTPUT_DIRECTORY}/${stem}.rc)
    add_custom_command(OUTPUT ${header} ${resource}
                       COMMAND ${MC_EXECUTABLE} ${MC_FLAGS} ${input}
                       DEPENDS ${input}
                       WORKING_DIRECTORY ${arg_OUTPUT_DIRECTORY}
                       COMMENT "" # MC is verbose by default
                       VERBATIM)
    list(APPEND depends ${header} ${resource})
  endforeach()

  add_custom_target(${name} DEPENDS ${depends})
endfunction()

find_package(WindowsSDK REQUIRED)
find_program(MC_EXECUTABLE mc.exe PATHS ${WINDOWS_SDK_VER_BIN_PATH}/${WINDOWS_HOST_PROCESSOR})
mark_as_advanced(MC_EXECUTABLE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MC REQUIRED_VARS MC_EXECUTABLE)
