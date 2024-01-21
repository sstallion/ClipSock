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
FindGoogleTest
--------------

Find the Google C++ testing framework using :module:`FetchContent`.  To aid in
adding tests, the :module:`GoogleTest` module is also included.

To add support for Google Test, :command:`FetchContent_Declare` should be
called prior to :command:`find_package`:

.. code-block:: cmake

  FetchContent_Declare(
    GoogleTest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG <tag>
    GIT_SHALLOW TRUE
  )

  find_package(GoogleTest)

The following variables are defined by this module:

.. variable:: GoogleTest_FOUND

  True if Google Test was found.

.. note::

  This module uses a different mechanism to locate packages than
  ``OVERRIDE_FIND_PACKAGE``.  As such, this option should not be specified in
  calls to :command:`FetchContent_Declare`.
#]=======================================================================]

include(GoogleTest)

# Disable installation:
set(INSTALL_GTEST OFF)

include(FetchContentHelper)
FetchContentHelper_FindPackage(GoogleTest)

if(GoogleTest_FOUND)
  block()
    # GoogleTest unconditionally sets output directory properties to values
    # that are not desirable when included as a subproject:
    set(targets gtest gtest_main gmock gmock_main)
    set_target_properties(${targets}
                          PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ""
                                     COMPILE_PDB_OUTPUT_DIRECTORY ""
                                     LIBRARY_OUTPUT_DIRECTORY ""
                                     PDB_OUTPUT_DIRECTORY ""
                                     RUNTIME_OUTPUT_DIRECTORY "")

    # Building as a shared library requires that BUILD_SHARED_LIBS be defined
    # and magic compile definitions added to test executables:
    if(BUILD_SHARED_LIBS)
      foreach(target IN LISTS targets)
        target_compile_definitions(${target} INTERFACE
          "$<$<NOT:$<IN_LIST:$<TARGET_PROPERTY:NAME>,${targets}>>:GTEST_LINKED_AS_SHARED_LIBRARY=1>")
      endforeach()
    endif()
  endblock()
endif()
