# Begin CVS Header 
#   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/bindings/cpp_examples/cpp_examples.pro,v $ 
#   $Revision: 1.1.2.4 $ 
#   $Name:  $ 
#   $Author: shoops $ 
#   $Date: 2009/03/30 15:56:54 $ 
# End CVS Header 

# Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
# and The University of Manchester. 
# All rights reserved. 

TEMPLATE = subdirs

include(../../common.pri)

QMAKE = $(QTDIR)/bin/qmake

SUBDIRS += example1
SUBDIRS += example2
SUBDIRS += example3
SUBDIRS += example4
SUBDIRS += example5
#SUBDIRS += example6

