// Begin CVS Header 
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/bindings/swig/CSteadyStateMethod.i,v $ 
//   $Revision: 1.1.24.1 $ 
//   $Name:  $ 
//   $Author: gauges $ 
//   $Date: 2010/05/26 16:15:01 $ 
// End CVS Header 

// Copyright (C) 2010 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., University of Heidelberg, and The University 
// of Manchester. 
// All rights reserved. 

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc. and EML Research, gGmbH. 
// All rights reserved. 

// Copyright © 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

%{

#include "steadystate/CSteadyStateMethod.h"

%}

%newobject CSteadyStateMethod::createSteadyStateMethod(CCopasiMethod::SubType subType = CCopasiMethod::Newton);

%ignore CSteadyStateMethod::process;

%include "steadystate/CSteadyStateMethod.h"

%extend CSteadyStateMatrix
{
    CSteadyStateMethod::ReturnCode process(CState* pState, CMatrix<C_FLOAT64>& jacobianX)
    {
        return $self->process(pState,jacobianX,NULL);
    }
}
