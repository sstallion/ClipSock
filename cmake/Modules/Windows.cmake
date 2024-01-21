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
Windows
-------

Provides additional support for developing on Windows platforms.  This module
attempts to bridge the gap between older and newer platform nomenclature where
possible, which is useful when working with Visual Studio or Windows SDK paths
that use newer naming conventions.

Commands
^^^^^^^^

The following commands are defined by this module:

.. command:: windows_copy_dlls

  .. code-block:: cmake

    windows_copy_dlls(<target>)

    The :command:`windows_copy_dlls()` command adds a custom command to the
    specified ``<target>`` that copies the list of DLLs the target depends on
    at runtime to its output directory.  This command is intended for targets
    that depend on DLLs built by subprojects that are not installed.  See
    :genex:`TARGET_RUNTIME_DLLS` for details.

Variables
^^^^^^^^^

The following variables are defined by this module:

.. variable:: WINDOWS_HOST_PROCESSOR

  The name of the host processor architecture using newer naming conventions
  (eg. ``x64`` instead of ``AMD64``).

.. variable:: WINDOWS_PROCESSOR

  The name of the target processor architecture using newer naming conventions
  (eg. ``x64`` instead of ``AMD64``).  Unlike :variable:`CMAKE_SYSTEM_PROCESSOR`,
  cross-compiling is taken into account (eg. ``x86`` instead of ``AMD64``).
#]=======================================================================]

macro(windows_copy_dlls target)
  add_custom_command(TARGET ${target} POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy -t "$<TARGET_FILE_DIR:${target}>"
                                                         "$<TARGET_RUNTIME_DLLS:${target}>"
                     COMMAND_EXPAND_LISTS)
endmacro()

if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "AMD64")
  set(WINDOWS_HOST_PROCESSOR "x64")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES [[x86|ARM(64)?]])
  string(TOLOWER ${CMAKE_HOST_SYSTEM_PROCESSOR} WINDOWS_HOST_PROCESSOR)
else()
  message(FATAL_ERROR "Unsupported host processor architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES [[AMD64|x86]])
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WINDOWS_PROCESSOR "x64")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(WINDOWS_PROCESSOR "x86")
  endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES [[ARM(64)?]])
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WINDOWS_PROCESSOR "arm64")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(WINDOWS_PROCESSOR "arm")
  endif()
else()
  message(FATAL_ERROR "Unsupported processor architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()
