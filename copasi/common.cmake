# Copyright (C) 2017 - 2018 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and University of 
# of Connecticut School of Medicine. 
# All rights reserved. 

# Copyright (C) 2012 - 2016 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 



if(NOT COPASI_COMMON_INCLUDED)
set (COPASI_COMMON_INCLUDED ON)

set(UI_LIBS libCOPASIUI )

include_directories(BEFORE ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_BINARY_DIR}/copasi)

set(SE_LIBS  libCOPASISE-static)
set(SE_DEPENDENCIES  libCOPASISE-static)

set(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${CLAPACK_LIBRARIES})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CLAPACK_LINKER_FLAGS}")

set(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${LIBSBML_LIBRARY})
set(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${RAPTOR_LIBRARY})
set(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${EXPAT_LIBRARY})
set(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${CROSSGUID_LIBRARY})


set(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${LIBSEDML_LIBRARY})
if (LIBNUML_LIBRARY)
  set(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${LIBNUML_LIBRARY})
endif()

set(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${COMBINE_LIBRARY})

if (APPLE)
  FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
  MARK_AS_ADVANCED (COREFOUNDATION_LIBRARY)
  SET(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${COREFOUNDATION_LIBRARY})
endif (APPLE)

if (EXTRA_LIBS)
  SET(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} ${EXTRA_LIBS})
else ()
  if (UNIX AND ${BLA_VENDOR} STREQUAL "Intel (MKL)")
    SET(SE_EXTERNAL_LIBS ${SE_EXTERNAL_LIBS} -lpthread -lm)
  endif()
endif(EXTRA_LIBS)

endif()
