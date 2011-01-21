// Begin CVS Header 
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/bindings/swig/CScanProblem.i,v $ 
//   $Revision: 1.4.2.1 $ 
//   $Name:  $ 
//   $Author: gauges $ 
//   $Date: 2011/01/21 13:21:20 $ 
// End CVS Header 

// Copyright (C) 2010 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., University of Heidelberg, and The University 
// of Manchester. 
// All rights reserved. 

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
// and The University of Manchester. 
// All rights reserved. 

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc. and EML Research, gGmbH. 
// All rights reserved. 

// Copyright © 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

%{

#include "scan/CScanProblem.h"

%}

%newobject CScanProblem::createScanItem(CScanProblem::Type type, unsigned C_INT32 steps = 5, const CCopasiObject* obj = NULL);

%ignore CScanProblem::getScanItemType(size_t index);
%ignore CScanProblem::getScanItem(size_t index) const;
%ignore CScanProblem::load;

%include "scan/CScanProblem.h"



