# Locate the Google C++ Testing Framework.
#
# Defines the following variables:
#
#   GTEST_FOUND - Found the Google Testing framework
#   GTEST_INCLUDE_DIRS - Include directories
#
# Also defines the library variables below as normal
# variables.  These contain debug/optimized keywords when
# a debugging library is found.
#
#   GTEST_BOTH_LIBRARIES - Both libgtest & libgtest-main
#   GTEST_LIBRARIES - libgtest
#   GTEST_MAIN_LIBRARIES - libgtest-main
#
# Accepts the following variables as input:
#
#   GTEST_ROOT - (as a CMake or environment variable)
#                The root directory of the gtest install prefix
#
#   GTEST_MSVC_SEARCH - If compiling with MSVC, this variable can be set to
#                       "MD" or "MT" to enable searching a GTest build tree
#                       (defaults: "MD")
#
#-----------------------
# Example Usage:
#
#    enable_testing()
#    find_package(GTest REQUIRED)
#    include_directories(${GTEST_INCLUDE_DIRS})
#
#    add_executable(foo foo.cc)
#    target_link_libraries(foo ${GTEST_BOTH_LIBRARIES})
#
#    add_test(AllTestsInFoo foo)
#
#-----------------------
#
# If you would like each Google test to show up in CTest as
# a test you may use the following macro.
# NOTE: It will slow down your tests by running an executable
# for each test and test fixture.  You will also have to rerun
# CMake after adding or removing tests or test fixtures.
#
# GTEST_ADD_TESTS(executable extra_args ARGN)
#    executable = The path to the test executable
#    extra_args = Pass a list of extra arguments to be passed to
#                 executable enclosed in quotes (or "" for none)
#    ARGN =       A list of source files to search for tests & test
#                 fixtures.
#
#  Example:
#     set(FooTestArgs --foo 1 --bar 2)
#     add_executable(FooTest FooUnitTest.cc)
#     GTEST_ADD_TESTS(FooTest "${FooTestArgs}" FooUnitTest.cc)

#=============================================================================
# Copyright 2009 Kitware, Inc.
# Copyright 2009 Philip Lowman <philip@yhbt.com>
# Copyright 2009 Daniel Blezek <blezek@gmail.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)
#
# Thanks to Daniel Blezek <blezek@gmail.com> for the GTEST_ADD_TESTS code

function(GTEST_ADD_TESTS executable extra_args)
    if(NOT ARGN)
        message(FATAL_ERROR "Missing ARGN: Read the documentation for GTEST_ADD_TESTS")
    endif()
    foreach(source ${ARGN})
        file(READ "${source}" contents)
        string(REGEX MATCHALL "TEST_?F?\\(([A-Za-z_0-9 ,]+)\\)" found_tests ${contents})
        foreach(hit ${found_tests})
            string(REGEX REPLACE ".*\\( *([A-Za-z_0-9]+), *([A-Za-z_0-9]+) *\\).*" "\\1.\\2" test_name ${hit})
            add_test(NAME ${test_name} COMMAND ${executable} --gtest_filter=${test_name} ${extra_args})
            list(APPEND _test_names ${test_name})
        endforeach()
    endforeach()    
    set(GTEST_ADD_TEST_NAMES ${_test_names} PARENT_SCOPE)
endfunction()

function(_gtest_append_debugs _endvar _library)
    if(${_library} AND ${_library}_DEBUG)
        set(_output optimized ${${_library}} debug ${${_library}_DEBUG})
    else()
        set(_output ${${_library}})
    endif()
    set(${_endvar} ${_output} PARENT_SCOPE)
endfunction()

FIND_PATH(GTEST_INCLUDE_DIR gtest/gtest.h
	PATH
	    $ENV{GTEST_ROOT}
	    ${GTEST_ROOT}
    HINTS
        $ENV{GTEST_ROOT}/include
        ${GTEST_ROOT}/include
)
MARK_AS_ADVANCED(GTEST_INCLUDE_DIR)

FIND_LIBRARY(GTEST_LIBRARY_DEBUG
    NAMES gtest gtest.lib
    PATH
        $ENV{GTEST_ROOT}
        ${GTEST_ROOT}
    HINTS
	    $ENV{GTEST_ROOT}/lib
	    ${GTEST_ROOT}/lib
        $ENV{GTEST_ROOT}/Debug
        ${GTEST_ROOT}/Debug
)

FIND_LIBRARY(GTEST_LIBRARY
    NAMES gtest gtest.lib
    PATH
        $ENV{GTEST_ROOT}
        ${GTEST_ROOT}
    HINTS
		$ENV{GTEST_ROOT}/lib
	    ${GTEST_ROOT}/lib
        $ENV{GTEST_ROOT}/Release
        ${GTEST_ROOT}/Release
)

FIND_LIBRARY(GTEST_MAIN_LIBRARY_DEBUG
    NAMES gtest_main gtest_main.lib
    PATH
        $ENV{GTEST_ROOT}
        ${GTEST_ROOT}
    HINTS
	    $ENV{GTEST_ROOT}/lib
	    ${GTEST_ROOT}/lib
        $ENV{GTEST_ROOT}/Debug
        ${GTEST_ROOT}/Debug
)

FIND_LIBRARY(GTEST_MAIN_LIBRARY
    NAMES gtest_main gtest_main.lib
    PATH
        $ENV{GTEST_ROOT}
        ${GTEST_ROOT}
    HINTS
		$ENV{GTEST_ROOT}/lib
	    ${GTEST_ROOT}/lib
        $ENV{GTEST_ROOT}/Release
        ${GTEST_ROOT}/Release
)
        
IF(GTEST_INCLUDE_DIR AND GTEST_LIBRARY_DEBUG AND GTEST_LIBRARY AND GTEST_MAIN_LIBRARY_DEBUG AND GTEST_MAIN_LIBRARY)
    SET(GTest_FOUND TRUE)
ENDIF()

IF(GTest_FOUND)
    IF (NOT GTest_FIND_QUIETLY)
        MESSAGE(STATUS "Found GTest")
    ENDIF()
    
    set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})
    _gtest_append_debugs(GTEST_LIBRARIES      GTEST_LIBRARY)
    _gtest_append_debugs(GTEST_MAIN_LIBRARIES GTEST_MAIN_LIBRARY)
    set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
ELSE()
    IF (GTest_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find GTest")
    ENDIF()
ENDIF()
