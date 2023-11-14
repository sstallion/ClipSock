# Copyright 2023 Steven Stallion
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

Find the Google C++ testing framework using :module:`FetchContent`.  If found,
the :module:`GoogleTest` module will be included.

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

# Disable installation:
set(INSTALL_GTEST OFF)

include(FetchContentHelper)
FetchContentHelper_FindPackage(GoogleTest)

if(GoogleTest_FOUND)
  include(GoogleTest)
endif()
