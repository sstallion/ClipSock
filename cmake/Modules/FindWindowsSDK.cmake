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
WindowsSDK
----------

Find the Microsoft Windows SDK.  To aid in constructing paths for the host or
target processor architecture, the :module:`Windows` module is also included.

The following variables are defined by this module:

.. variable:: WINDOWS_SDK_DIR

  The normalized path identified by the ``WindowsSdkDir`` environment variable
  or the latest installation root found in the Windows Registry.  If a
  specific version is needed, the :variable:`WINDOWS_SDK_TARGET` variable may
  be defined (eg. ``8.1``, ``10``).

.. variable:: WINDOWS_SDK_BIN_PATH

  The normalized path identified by the ``WindowsSdkBinPath`` environment
  variable or the value of ``${WINDOWS_SDK_DIR}/bin``.

.. variable:: WINDOWS_SDK_VERSION

  The value of the ``WindowsSDKVersion`` environment variable or the latest
  version found in the Windows Registry.

.. variable:: WINDOWS_SDK_LIB_VERSION

  The value of the ``WindowsSDKLibVersion`` environment variable or the value
  of ``${WINDOWS_SDK_VERSION}``.

.. variable:: WINDOWS_SDK_VER_BIN_PATH

  The value of the ``WindowsSdkVerBinPath`` environment variable or the value
  of ``${WINDOWS_SDK_BIN_PATH}/${WINDOWS_SDK_VERSION})``.
#]=======================================================================]

include(Windows)

set(WINDOWS_SDK_REGKEY_INSTALLED_ROOTS
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots")

set(WINDOWS_SDK_DIR $ENV{WindowsSdkDir})
if(NOT WINDOWS_SDK_DIR)
  if(WINDOWS_SDK_TARGET)
    set(WINDOWS_SDK_REGVAL_KITSROOT "KitsRoot${WINDOWS_SDK_TARGET}")
  else()
    block(SCOPE_FOR VARIABLES PROPAGATE WINDOWS_SDK_REGVAL_KITSROOT)
      # Query the Windows Registry for the latest installation root name:
      cmake_host_system_information(RESULT value_names
                                    QUERY WINDOWS_REGISTRY ${WINDOWS_SDK_REGKEY_INSTALLED_ROOTS}
                                                          VALUE_NAMES
                                    VIEW HOST)
      list(FILTER value_names INCLUDE REGEX [[^KitsRoot]])
      list(SORT value_names COMPARE NATURAL)
      list(POP_BACK value_names WINDOWS_SDK_REGVAL_KITSROOT)
    endblock()
  endif()

  # Query the Windows Registry for the installation root directory:
  cmake_host_system_information(RESULT WINDOWS_SDK_DIR
                                QUERY WINDOWS_REGISTRY ${WINDOWS_SDK_REGKEY_INSTALLED_ROOTS}
                                                       VALUE ${WINDOWS_SDK_REGVAL_KITSROOT}
                                VIEW HOST)
endif()
cmake_path(NORMAL_PATH WINDOWS_SDK_DIR)

set(WINDOWS_SDK_BIN_PATH $ENV{WindowsSdkBinPath})
if(WINDOWS_SDK_BIN_PATH)
  cmake_path(NORMAL_PATH WINDOWS_SDK_BIN_PATH)
else()
  set(WINDOWS_SDK_BIN_PATH ${WINDOWS_SDK_DIR}/bin)
endif()

set(WINDOWS_SDK_VERSION $ENV{WindowsSDKVersion})
if(WINDOWS_SDK_VERSION)
  string(REGEX REPLACE [[\\$]] "" WINDOWS_SDK_VERSION ${WINDOWS_SDK_VERSION})
else()
  block(SCOPE_FOR VARIABLES PROPAGATE WINDOWS_SDK_VERSION)
    # Query the Windows Registry for the latest version:
    cmake_host_system_information(RESULT subkeys
                                  QUERY WINDOWS_REGISTRY ${WINDOWS_SDK_REGKEY_INSTALLED_ROOTS}
                                                         SUBKEYS
                                  VIEW HOST)
    list(SORT subkeys COMPARE NATURAL)
    list(POP_BACK subkeys WINDOWS_SDK_VERSION)
  endblock()
endif()

set(WINDOWS_SDK_LIB_VERSION $ENV{WindowsSDKLibVersion})
if(WINDOWS_SDK_LIB_VERSION)
  string(REGEX REPLACE [[\\$]] "" WINDOWS_SDK_LIB_VERSION ${WINDOWS_SDK_LIB_VERSION})
else()
  set(WINDOWS_SDK_LIB_VERSION ${WINDOWS_SDK_VERSION})
endif()

set(WINDOWS_SDK_VER_BIN_PATH $ENV{WindowsSdkVerBinPath})
if(WINDOWS_SDK_VER_BIN_PATH)
  cmake_path(NORMAL_PATH WINDOWS_SDK_VER_BIN_PATH)
else()
  set(WINDOWS_SDK_VER_BIN_PATH ${WINDOWS_SDK_BIN_PATH}/${WINDOWS_SDK_VERSION})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  WindowsSDK REQUIRED_VARS WINDOWS_SDK_DIR
                           WINDOWS_SDK_BIN_PATH
                           WINDOWS_SDK_LIB_VERSION
                           WINDOWS_SDK_VER_BIN_PATH
             VERSION_VAR WINDOWS_SDK_VERSION
             HANDLE_VERSION_RANGE
)
