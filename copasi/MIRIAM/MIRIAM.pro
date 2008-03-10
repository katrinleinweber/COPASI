# Begin CVS Header 
#   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/MIRIAM/MIRIAM.pro,v $ 
#   $Revision: 1.7 $ 
#   $Name:  $ 
#   $Author: aekamal $ 
#   $Date: 2008/03/10 15:49:56 $ 
# End CVS Header 

# Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
# and The University of Manchester. 
# All rights reserved. 

# Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc. and EML Research, gGmbH. 
# All rights reserved. 

LIB = MIRIAM

include(../lib.pri)
include(../common.pri)

CONFIG -= qt

DEPENDPATH += .. 
INCLUDEPATH += ..

# Input
HEADERS += CBiologicalDescription.h
HEADERS += CCreator.h
HEADERS += CConstants.h
HEADERS += CModelMIRIAMInfo.h
HEADERS += CModified.h
HEADERS += CReference.h
HEADERS += CRaptorInit.h
HEADERS += CRDFEdge.h
HEADERS += CRDFGraph.h
HEADERS += CRDFLiteral.h
HEADERS += CRDFNode.h
HEADERS += CRDFObject.h
HEADERS += CRDFParser.h
HEADERS += CRDFSubject.h
HEADERS += CRDFWriter.h

SOURCES += CBiologicalDescription.cpp 
SOURCES += CCreator.cpp 
SOURCES += CConstants.cpp 
SOURCES += CModelMIRIAMInfo.cpp
SOURCES += CModified.cpp 
SOURCES += CReference.cpp 
SOURCES += CRaptorInit.cpp
SOURCES += CRDFEdge.cpp
SOURCES += CRDFGraph.cpp
SOURCES += CRDFLiteral.cpp
SOURCES += CRDFNode.cpp
SOURCES += CRDFObject.cpp
SOURCES += CRDFParser.cpp
SOURCES += CRDFSubject.cpp
SOURCES += CRDFWriter.cpp
