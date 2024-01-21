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
FetchContentHelper
------------------

Overview
^^^^^^^^

This module provides support for working with the :module:`FetchContent`
module.

Commands
^^^^^^^^

The following commands are defined by this module:

.. command:: FetchContentHelper_FindPackage

  .. code-block:: cmake

    FetchContentHelper_FindPackage(<contentName>)

  The ``FetchContentHelper_FindPackage()`` macro provides an alternative to
  the ``OVERRIDE_FIND_PACKAGE`` method of finding :module:`FetchContent`
  packages.  This macro simplifies calling :command:`FetchContent_Populate`
  from find modules and permits customization without affecting the caller.
#]=======================================================================]

include(FetchContent)

# This macro should be replaced by calls to FetchContent_MakeAvailable() once
# support for EXCLUDE_FROM_ALL is released and made generally available. See
# https://gitlab.kitware.com/cmake/cmake/-/merge_requests/8822.
macro(FetchContentHelper_FindPackage contentName)
  string(TOLOWER ${contentName} _contentNameLower)
  FetchContent_GetProperties(${contentName})
  if(NOT ${_contentNameLower}_POPULATED)
    block(SCOPE_FOR POLICIES)
      cmake_policy(SET CMP0077 NEW)
      FetchContent_Populate(${contentName})
      if(EXISTS ${${_contentNameLower}_SOURCE_DIR}/CMakeLists.txt)
        add_subdirectory(${${_contentNameLower}_SOURCE_DIR}
                        ${${_contentNameLower}_BINARY_DIR}
                        EXCLUDE_FROM_ALL)
      endif()
    endblock()
  endif()

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(
    ${contentName} REQUIRED_VARS ${_contentNameLower}_SOURCE_DIR
                                 ${_contentNameLower}_BINARY_DIR
  )
  unset(_contentNameLower)
endmacro()
