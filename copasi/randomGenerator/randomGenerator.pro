# Begin CVS Header 
#   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/randomGenerator/randomGenerator.pro,v $ 
#   $Revision: 1.10 $ 
#   $Name:  $ 
#   $Author: shoops $ 
#   $Date: 2010/03/16 18:57:04 $ 
# End CVS Header 

# Copyright (C) 2010 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 

# Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
# and The University of Manchester. 
# All rights reserved. 

######################################################################
# $Revision: 1.10 $ $Author: shoops $ $Date: 2010/03/16 18:57:04 $
######################################################################

LIB = randomGenerator

# Input
HEADERS += Cmt19937.h \
           Cr250.h \
           CRandom.h

SOURCES += Cmt19937.cpp \
           Cr250.cpp \
           CRandom.cpp

include(../lib.pri)
include(../common.pri)

#The following line was inserted by qt3to4
QT +=  qt3support 
