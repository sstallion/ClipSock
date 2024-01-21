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

cmake_minimum_required(VERSION 3.26)

# Preserve normal variables in third-party projects:
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
                               ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

project(ClipSock
        DESCRIPTION "Network accessible clipboard for Windows"
        HOMEPAGE_URL "https://github.com/sstallion/ClipSock"
        LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

include(Options)
include(Packages)
include(CompileOptions)
include(License)
include(Version)
include(Windows)

find_package(MC REQUIRED)
find_package(GSL REQUIRED)

set(PACKAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Packaging)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

include_directories(${RESOURCE_DIR} ${SOURCE_DIR})

set(INPUT_FILES
    ${RESOURCE_DIR}/${PROJECT_NAME}.exe.manifest.in
    ${RESOURCE_DIR}/${PROJECT_NAME}.rc.in)

source_group("Input Files" FILES ${INPUT_FILES})

set_property(DIRECTORY PROPERTY CMAKE_CONFIGURE_DEPENDS ${INPUT_FILES})

configure_file(${RESOURCE_DIR}/${PROJECT_NAME}.exe.manifest.in
               ${PROJECT_NAME}.exe.manifest)

configure_file(${RESOURCE_DIR}/${PROJECT_NAME}.rc.in
               ${PROJECT_NAME}.rc)

set(MESSAGE_FILES ${RESOURCE_DIR}/messages.mc)

source_group("Message Files" FILES ${MESSAGE_FILES})

add_messages(${PROJECT_NAME}-messages ${MESSAGE_FILES})

set(RESOURCE_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe.manifest
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc
    ${CMAKE_CURRENT_BINARY_DIR}/messages.h
    ${CMAKE_CURRENT_BINARY_DIR}/messages.rc
    ${RESOURCE_DIR}/resource.h)

# Include output directory for generated resources:
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${PROJECT_NAME}-objects OBJECT
            ${SOURCE_DIR}/buffer.h
            ${SOURCE_DIR}/eventlog.cpp
            ${SOURCE_DIR}/eventlog.h
            ${SOURCE_DIR}/notify.cpp
            ${SOURCE_DIR}/notify.h
            ${SOURCE_DIR}/server.cpp
            ${SOURCE_DIR}/server.h
            ${SOURCE_DIR}/settings.cpp
            ${SOURCE_DIR}/settings.h
            ${SOURCE_DIR}/util.h)

target_link_libraries(${PROJECT_NAME}-objects
                      PUBLIC comctl32 iphlpapi version ws2_32)

add_dependencies(${PROJECT_NAME}-objects ${PROJECT_NAME}-messages)

add_executable(${PROJECT_NAME}
               ${INPUT_FILES}
               ${MESSAGE_FILES}
               ${RESOURCE_FILES}
               ${SOURCE_DIR}/winmain.cpp)

set_target_properties(${PROJECT_NAME}
                      PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(${PROJECT_NAME}
                      PRIVATE ${PROJECT_NAME}-objects)

if(BUILD_TESTING)
  enable_testing()

  find_package(GoogleTest REQUIRED)

  add_library(${PROJECT_NAME}-mocks SHARED
              ${TEST_DIR}/mock_global.h
              ${TEST_DIR}/mock_windows.cpp
              ${TEST_DIR}/mock_windows.h
              ${TEST_DIR}/mock_winsock.cpp
              ${TEST_DIR}/mock_winsock.h)

  include(GenerateExportHeader)
  generate_export_header(${PROJECT_NAME}-mocks
                         BASE_NAME MOCK
                         EXPORT_FILE_NAME mock_export.h)

  target_link_libraries(${PROJECT_NAME}-mocks
                        PUBLIC GTest::gmock)

  add_executable(${PROJECT_NAME}-tests
                 ${TEST_DIR}/test_buffer.cpp
                 ${TEST_DIR}/test_server.cpp
                 ${TEST_DIR}/test_support.h
                 ${TEST_DIR}/test_main.cpp)

  target_link_libraries(${PROJECT_NAME}-tests
                        PRIVATE ${PROJECT_NAME}-objects
                                ${PROJECT_NAME}-mocks)

  windows_copy_dlls(${PROJECT_NAME}-tests)

  gtest_discover_tests(${PROJECT_NAME}-tests)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION .)

if(BUILD_PACKAGING)
  # The WiX generator uses INSTALL properties to provide shortcuts, which is
  # poorly documented and has a number of caveats using relative paths. See
  # https://gitlab.kitware.com/cmake/cmake/-/issues/18540.
  set_property(INSTALL $<TARGET_FILE_NAME:${PROJECT_NAME}>
               PROPERTY CPACK_START_MENU_SHORTCUTS ${PROJECT_NAME})

  set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION_WIN32})
  set(CPACK_PACKAGE_VENDOR ${PROJECT_COPYRIGHT_AUTHOR})
  set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION}-${WINDOWS_PROCESSOR})
  set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
  set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_LICENSE_FILE})
  set(CPACK_GENERATOR WIX)

  set(CPACK_WIX_UPGRADE_GUID "62CE5950-C0D7-47ED-B269-51C81515460B")
  set(CPACK_WIX_PRODUCT_ICON ${RESOURCE_DIR}/${PROJECT_NAME}.ico)
  set(CPACK_WIX_UI_BANNER ${PACKAGE_DIR}/WixUIBanner.bmp)
  set(CPACK_WIX_UI_DIALOG ${PACKAGE_DIR}/WixUIDialog.bmp)
  set(CPACK_WIX_PROGRAM_MENU_FOLDER .)
  set(CPACK_WIX_PATCH_FILE ${PACKAGE_DIR}/EventSource.xml
                           ${PACKAGE_DIR}/LaunchApplication.xml)
  set(CPACK_WIX_EXTENSIONS WixUIExtension WixUtilExtension)
  set(CPACK_WIX_PROPERTY_ARPCOMMENTS ${PROJECT_DESCRIPTION})
  set(CPACK_WIX_PROPERTY_APPHELPLINK ${PROJECT_HOMEPAGE_URL})
  set(CPACK_WIX_ARCHITECTURE ${WINDOWS_PROCESSOR})

  include(CPack)
endif()
