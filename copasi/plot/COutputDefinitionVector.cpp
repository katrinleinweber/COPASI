/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/plot/COutputDefinitionVector.cpp,v $
   $Revision: 1.2 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:30:41 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "COutputDefinitionVector.h"
#include "report/CKeyFactory.h"

COutputDefinitionVector::COutputDefinitionVector(const std::string & name,
    const CCopasiContainer * pParent):
    CCopasiVectorN< CPlotSpecification >(name, pParent),
    mKey(GlobalKeys.add("COutputDefinitionVector", this))
{}

COutputDefinitionVector::~COutputDefinitionVector()
{
  cleanup();
}

void COutputDefinitionVector::cleanup()
{
  GlobalKeys.remove(mKey);
}

const std::string& COutputDefinitionVector::getKey()
{
  return mKey;
}

CPlotSpecification* COutputDefinitionVector::createPlotSpec(const std::string & name,
    CPlotItem::Type type)
{
  unsigned C_INT32 i;
  for (i = 0; i < size(); i++)
    if ((*this)[i]->getObjectName() == name)
      return NULL; // duplicate name

  CPlotSpecification* pNewPlotSpec = new CPlotSpecification(name, this, type);
  pNewPlotSpec->setObjectName(name);

  add(pNewPlotSpec);
  return pNewPlotSpec;
}

bool COutputDefinitionVector::removePlotSpec(const std::string & key)
{
  CPlotSpecification* pPl =
    dynamic_cast<CPlotSpecification*>(GlobalKeys.get(key));
  unsigned C_INT32 index = this->CCopasiVector<CPlotSpecification>::getIndex(pPl);
  if (index == C_INVALID_INDEX)
    return false;

  this->CCopasiVector<CPlotSpecification>::remove(index);

  return true;
}
