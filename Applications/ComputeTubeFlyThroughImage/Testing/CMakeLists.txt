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

include( Midas3FunctionAddTest )
set( MIDAS_REST_URL http://midas3.kitware.com/midas/api/rest )
set( MIDAS_KEY_DIR ${TubeTK_SOURCE_DIR}/MIDAS_Keys )

set( TEMP ${TubeTK_BINARY_DIR}/Temporary )

set( PROJ_EXE
  ${TubeTK_LAUNCHER} $<TARGET_FILE:${MODULE_NAME}> )

set( IMAGECOMPARE_EXE
 ${TubeTK_LAUNCHER} $<TARGET_FILE:ImageCompareCommand> )

#  
# Test with 2D Cylinder
#

  # Run module and generate result on test case
  Midas3FunctionAddTest( NAME ${MODULE_NAME}-Test1
  COMMAND ${PROJ_EXE}
    MIDAS{Cylinder_2D.mha.md5}
    MIDAS{Cylinder_2D.tre.md5} 1
    ${TEMP}/${MODULE_NAME}Test1.flyThrough.mha
    ${TEMP}/${MODULE_NAME}Test1.flyThrough.tubeMask.mha )

  # compare outputs with groundtruth		       
  Midas3FunctionAddTest( NAME ${MODULE_NAME}-Test1-Compare-flyThrough
    COMMAND ${IMAGECOMPARE_EXE}
      -t ${TEMP}/${MODULE_NAME}Test1.flyThrough.mha 
      -b MIDAS{${MODULE_NAME}Test1.flyThrough.mha.md5} 
      -i 0.001 )

  set_property( TEST ${MODULE_NAME}-Test1-Compare-flyThrough
		APPEND PROPERTY DEPENDS ${MODULE_NAME}-Test1 )
  
  Midas3FunctionAddTest( NAME ${MODULE_NAME}-Test1-Compare-flyThrough-TubeMask
    COMMAND ${IMAGECOMPARE_EXE}
      -t ${TEMP}/${MODULE_NAME}Test1.flyThrough.tubeMask.mha
      -b MIDAS{${MODULE_NAME}Test1.flyThrough.tubeMask.mha.md5} 
      -i 0.001 )

  set_property( TEST ${MODULE_NAME}-Test1-Compare-flyThrough-TubeMask
		APPEND PROPERTY DEPENDS ${MODULE_NAME}-Test1 )

#  
# Test with 3D Cylinder
#

  # Run module and generate result on test case
  Midas3FunctionAddTest( NAME ${MODULE_NAME}-Test2
    COMMAND ${PROJ_EXE}
      MIDAS{Cylinder_3D.mha.md5}
      MIDAS{Cylinder_3D.tre.md5} 1
      ${TEMP}/${MODULE_NAME}Test2.flyThrough.mha
      ${TEMP}/${MODULE_NAME}Test2.flyThrough.tubeMask.mha)

  # compare outputs with groundtruth		       
  Midas3FunctionAddTest( NAME ${MODULE_NAME}-Test2-Compare-flyThrough
    COMMAND ${IMAGECOMPARE_EXE}
      -t ${TEMP}/${MODULE_NAME}Test2.flyThrough.mha 
      -b MIDAS{${MODULE_NAME}Test2.flyThrough.mha.md5} 
      -i 0.001 )

  set_property( TEST ${MODULE_NAME}-Test2-Compare-flyThrough
		APPEND PROPERTY DEPENDS ${MODULE_NAME}-Test2 )
  
  Midas3FunctionAddTest( NAME ${MODULE_NAME}-Test2-Compare-flyThrough-TubeMask
    COMMAND ${IMAGECOMPARE_EXE}
      -t ${TEMP}/${MODULE_NAME}Test2.flyThrough.tubeMask.mha
      -b MIDAS{${MODULE_NAME}Test2.flyThrough.tubeMask.mha.md5} 
      -i 0.001 )

  set_property( TEST ${MODULE_NAME}-Test2-Compare-flyThrough-TubeMask
		APPEND PROPERTY DEPENDS ${MODULE_NAME}-Test2 )