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
License
-------

Finds the project license file and extracts copyright information.  When
called from the top-level ``CMakeLists.txt``, the project license file,
copyright, and author are also stored in the :variable:`CMAKE_PROJECT_LICENSE_FILE`,
:variable:`CMAKE_PROJECT_COPYRIGHT`, :variable:`CMAKE_PROJECT_COPYRIGHT_YEAR`,
and :variable:`CMAKE_PROJECT_COPYRIGHT_AUTHOR` variables.

The following variables are defined by this module:

.. variable:: PROJECT_LICENSE_FILE

  Absolute path to the license file for the project.

.. variable:: PROJECT_COPYRIGHT

  Copyright notice for the project.

.. variable:: PROJECT_COPYRIGHT_YEAR

  Copyright year for the project.

.. variable:: PROJECT_COPYRIGHT_AUTHOR

  Copyright author for the project.
#]=======================================================================]

find_file(PROJECT_LICENSE_FILE
          NAMES LICENSE LICENSE.txt
          PATHS ${CMAKE_CURRENT_SOURCE_DIR}
          NO_CACHE
          NO_DEFAULT_PATH)
set(CMAKE_PROJECT_LICENSE_FILE ${PROJECT_LICENSE_FILE}
    CACHE STATIC "Value Computed by License.cmake")

if(PROJECT_LICENSE_FILE)
  block(SCOPE_FOR VARIABLES PROPAGATE PROJECT_COPYRIGHT
                                      PROJECT_COPYRIGHT_YEAR
                                      PROJECT_COPYRIGHT_AUTHOR)
    file(STRINGS ${PROJECT_LICENSE_FILE} lines)
    foreach(line IN LISTS lines)
      if(line MATCHES [[([Cc][Oo][Pp][Yy][Rr][Ii][Gg][Hh][Tt].*)$]])
        string(STRIP ${CMAKE_MATCH_1} PROJECT_COPYRIGHT)
        set(CMAKE_PROJECT_COPYRIGHT ${PROJECT_COPYRIGHT}
            CACHE STATIC "Value Computed by License.cmake")

        if(PROJECT_COPYRIGHT MATCHES [[([12][0-9][0-9][0-9][ ,-]?)+(.*)$]])
          string(STRIP ${CMAKE_MATCH_1} PROJECT_COPYRIGHT_YEAR)
          set(CMAKE_PROJECT_COPYRIGHT_YEAR ${PROJECT_COPYRIGHT_YEAR}
              CACHE STATIC "Value Computed by License.cmake")

          string(STRIP ${CMAKE_MATCH_2} PROJECT_COPYRIGHT_AUTHOR)
          set(CMAKE_PROJECT_COPYRIGHT_AUTHOR ${PROJECT_COPYRIGHT_AUTHOR}
              CACHE STATIC "Value Computed by License.cmake")
        endif()
        break()
      endif()
    endforeach()
  endblock()
endif()
