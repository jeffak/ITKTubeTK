##############################################################################
#
# Library:   TubeTK
#
# Copyright 2010 Kitware Inc. 28 Corporate Drive,
# Clifton Park, NY, 12065, USA.
#
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################

include_regular_expression( "^.*$" )

find_package( PythonInterp )
if( PYTHON_EXECUTABLE )
  add_test( PrintSelf-BaseCommon ${PYTHON_EXECUTABLE}
    ${TubeTK_SOURCE_DIR}/CMake/PrintSelfCheck.py
    ${TubeTK_SOURCE_DIR}/Base/Common )
endif( PYTHON_EXECUTABLE )

find_package( SlicerExecutionModel REQUIRED )
include( ${SlicerExecutionModel_USE_FILE} )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

set( BASE_COMMON_TESTS
  ${TubeTK_LAUNCHER} $<TARGET_FILE:tubeBaseCommonTests> )

set( BASE_COMMON_HEADER_TEST
  ${TubeTK_LAUNCHER} $<TARGET_FILE:tubeBaseCommonHeaderTest> )

set( tubeBaseCommonTests_SRCS
  tubeBaseCommonPrintTest.cxx
  tubeMacroTest.cxx
  tubeMessageTest.cxx
  tubeObjectTest.cxx )

include_directories(
  ${TubeTK_SOURCE_DIR}/Base/Common
  ${TubeTK_SOURCE_DIR}/Base/Filtering )

add_executable( tubeBaseCommonHeaderTest tubeBaseCommonHeaderTest.cxx )
target_link_libraries( tubeBaseCommonHeaderTest ${ITK_LIBRARIES} )

set( no_install_option )
if( NOT TubeTK_INSTALL_DEVELOPMENT )
  set( no_install_option NO_INSTALL )
endif()

SEMMacroBuildCLI(
  NAME tubeBaseCommonTests
  ADDITIONAL_SRCS
    ${tubeBaseCommonTests_SRCS}
  LOGO_HEADER ${TubeTK_SOURCE_DIR}/Base/CLI/TubeTKLogo.h
  TARGET_LIBRARIES
    ${ITK_LIBRARIES}
    ${JsonCpp_LIBRARIES}
    TubeCLI TubeTKCommon
  EXECUTABLE_ONLY
  ${no_install_option}
  )

add_test( NAME tubeBaseCommonHeaderTest COMMAND ${BASE_COMMON_HEADER_TEST} )

add_test( NAME tubeBaseCommonPrintTest
  COMMAND ${BASE_COMMON_TESTS}
    tubeBaseCommonPrintTest )

add_test( NAME tubeMacroTest
  COMMAND ${BASE_COMMON_TESTS}
    tubeMacroTest )

add_test( NAME tubeMessageTest
  COMMAND ${BASE_COMMON_TESTS}
    tubeMessageTest )

add_test( NAME tubeObjectTest
  COMMAND ${BASE_COMMON_TESTS}
    tubeObjectTest )
