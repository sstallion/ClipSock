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
FindGSL
-------

Find the Microsoft Guidelines Support Library using :module:`FetchContent`. If
found, the GSL headers will be made available to all targets.

To add support for the Guidelines Support Library, :command:`FetchContent_Declare`
should be called prior to :command:`find_package`:

.. code-block:: cmake

    FetchContent_Declare(
      GSL
      GIT_REPOSITORY https://github.com/microsoft/GSL.git
      GIT_TAG <tag>
      GIT_SHALLOW TRUE
    )

  find_package(GSL)

The following variables are defined by this module:

.. variable:: GSL_FOUND

True if the Guidelines Support Library was found.

.. note::

  This module uses a different mechanism to locate packages than
  ``OVERRIDE_FIND_PACKAGE``.  As such, this option should not be specified in
  calls to :command:`FetchContent_Declare`.
#]=======================================================================]

include(FetchContentHelper)
FetchContentHelper_FindPackage(GSL)

if(GSL_FOUND)
  link_libraries(Microsoft.GSL::GSL)
endif()
