###############################################################################
#
# Description       : CMake build script for COPASI
# Original author(s): Ralph Gauges <ralph.gauges@bioquant.uni-heidelberg.de>
#                     Frank Bergmann <fbergman@caltech.edu>
#
# This file is part of COPASI.  Please visit http://COPASI.org for more
# information about COPASI, and the latest version of COPASI.
#
# Copyright (C) 2011 - 2010 by Pedro Mendes, Virginia Tech Intellectual
# Properties, Inc., University of Heidelberg, and The University
# of Manchester.
# All rights reserved.
# 
# Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
# Properties, Inc., EML Research, gGmbH, University of Heidelberg,
# and The University of Manchester.
# All rights reserved.
# 
# Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
# Properties, Inc. and EML Research, gGmbH.
# All rights reserved.
#
###############################################################################

# cmake_minimum_required (VERSION 2.6)
# project (model)
# 
# 
# include_directories(
#   "${PROJECT_SOURCE_DIR}/.." 
#   "${PROJECT_SOURCE_DIR}/../.." 
#   "${PROJECT_SOURCE_DIR}"
# )
# 

if (ENABLE_FLEX_BISON)
  BISON_TARGET(CChemEqParser model/CChemEqParser.ypp ${COPASI_SOURCE_DIR}/model/CChemEqParser_yacc.cpp COMPILE_FLAGS ${BISON_BINARY})
  set(COPASI_SOURCES ${COPASI_SOURCES} ${COPASI_SOURCE_DIR}/model/CChemEqParser_yacc.cpp)
  set(COPASI_HEADERS ${COPASI_HEADERS} ${COPASI_SOURCE_DIR}/model/CChemEqParser_yacc.hpp)
  
  get_filename_component(COPASI_FLEX_SOURCE ${COPASI_SOURCE_DIR}/model/CChemEqParser.lpp ABSOLUTE)
  get_filename_component(COPASI_FLEX_TARGET ${COPASI_SOURCE_DIR}/model/CChemEqParser_lex.cpp ABSOLUTE)
  FLEX_TARGET(CChemEqParser ${COPASI_FLEX_SOURCE} ${COPASI_FLEX_TARGET} COMPILE_FLAGS ${FLEX_BINARY})
  set(COPASI_SOURCES ${COPASI_SOURCES} ${COPASI_SOURCE_DIR}/model/CChemEqParser_lex.cpp)
endif (ENABLE_FLEX_BISON)

file(GLOB HEADERS_MODEL model/*.h model/*.hpp)
file(GLOB SOURCES_MODEL model/*.cpp)

list(REMOVE_ITEM HEADERS_MODEL ${CMAKE_CURRENT_SOURCE_DIR}/model/CModelMerging.h)
list(REMOVE_ITEM SOURCES_MODEL ${CMAKE_CURRENT_SOURCE_DIR}/model/CModelMerging.cpp)

# create source group for IDEs
source_group(model FILES ${SOURCES_MODEL} ${HEADERS_MODEL})

# add files to main COPASI sources
set(COPASI_SOURCES ${COPASI_SOURCES} ${SOURCES_MODEL})
set(COPASI_HEADERS ${COPASI_HEADERS} ${HEADERS_MODEL})
