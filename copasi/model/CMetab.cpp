/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/model/CMetab.cpp,v $
   $Revision: 1.98 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:29:21 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include <iostream>
#include <string>
#include <vector>
#include <limits>

#include "copasi.h"

#include "CopasiDataModel/CCopasiDataModel.h"
#include "utilities/utility.h"
#include "report/CCopasiObjectReference.h"
#include "report/CKeyFactory.h"
#include "CCompartment.h"
#include "CModel.h"
#include "CMetab.h"
#include "CMetabNameInterface.h"

//static
C_FLOAT64 CMetab::convertToNumber(const C_FLOAT64 & concentration,
                                  const CCompartment & compartment,
                                  const CModel & model)
{return concentration * compartment.getVolume() * model.getQuantity2NumberFactor();}

//static
C_FLOAT64 CMetab::convertToConcentration(const C_FLOAT64 & number,
    const CCompartment & compartment,
    const CModel & model)
{return number / compartment.getVolume() * model.getNumber2QuantityFactor();}

CMetab::CMetab(const std::string & name,
               const CCopasiContainer * pParent):
    CModelEntity(name, pParent, "Metabolite",
                 CCopasiObject::NonUniqueName),
    mConc(std::numeric_limits<C_FLOAT64>::quiet_NaN()),
    mIConc(0.0),
    mConcRate(0.0),
    mTT(0.0),
    mpCompartment(NULL),
    mMoieties()
{
  mKey = GlobalKeys.add("Metabolite", this);
  initObjects();

  setStatus(REACTIONS);

  if (getObjectParent())
    {
      initModel();
      initCompartment(NULL);

      setInitialConcentration(1.0);
      setConcentration(1.0);
    }

  CONSTRUCTOR_TRACE;
}

CMetab::CMetab(const CMetab & src,
               const CCopasiContainer * pParent):
    CModelEntity(src, pParent),
    mConc(src.mConc),
    mIConc(src.mIConc),
    mConcRate(src.mConcRate),
    mTT(src.mTT),
    mpCompartment(NULL),
    mMoieties()
{
  mKey = GlobalKeys.add("Metabolite", this);

  initObjects();

  initModel();
  initCompartment(src.mpCompartment);
  CONSTRUCTOR_TRACE;
}

CMetab &CMetab::operator=(const CMetabOld &RHS)
{
  setObjectName(RHS.getObjectName());

  setStatus(RHS.mStatus);

  setInitialConcentration(RHS.mIConc);
  refreshInitialConcentration();

  mRate = 0.0;
  mConcRate = 0.0;
  mTT = 0.0;

  return *this;  // Assignment operator returns left side.
}

CMetab::~CMetab()
{
  GlobalKeys.remove(mKey);
  DESTRUCTOR_TRACE;
}

void CMetab::cleanup() {}

void CMetab::initModel()
{
  //  mpModel = dynamic_cast< CModel * >(getObjectAncestor("Model"));
  //  if (!mpModel && CCopasiDataModel::Global) mpModel = CCopasiDataModel::Global->getModel();
}

void CMetab::initCompartment(const CCompartment * pCompartment)
{
  mpCompartment = (const CCompartment *) getObjectAncestor("Compartment");
  if (!mpCompartment) mpCompartment = pCompartment;
  //  if (!mpCompartment) mpCompartment = mpParentCompartment;
}

const C_FLOAT64 & CMetab::getConcentration() const {return mConc;}

const C_FLOAT64 & CMetab::getInitialConcentration() const {return mIConc;}

const CCompartment * CMetab::getCompartment() const {return mpCompartment;}

const CModel * CMetab::getModel() const {return mpModel;}

void CMetab::setTransitionTime(const C_FLOAT64 & transitionTime)
{mTT = transitionTime;}

const C_FLOAT64 & CMetab::getTransitionTime() const {return mTT;}

bool CMetab::setObjectParent(const CCopasiContainer * pParent)
{
  CModelEntity::setObjectParent(pParent);

  CCopasiObject * pObject =
    const_cast< CCopasiObject * >(getObject(CCopasiObjectName("Reference=Concentration")));

  if (mpModel)
    pObject->setRefresh(mpModel, &CModel::applyAssignments);
  else
    pObject->clearRefresh();

  pObject =
    const_cast< CCopasiObject * >(getObject(CCopasiObjectName("Reference=TransitionTime")));

  if (mpModel)
    pObject->setRefresh(mpModel, &CModel::setTransitionTimes);
  else
    pObject->clearRefresh();

  initCompartment(NULL);
  initModel();

  return true;
}

// ***** set quantities ********

void CMetab::setConcentration(const C_FLOAT64 concentration)
{
  if (isFixed()) return;

  mConc = concentration;
  *mpValueData = concentration * mpCompartment->getVolume()
                 * mpModel->getQuantity2NumberFactor();

#ifdef COPASI_DEBUG
  //if (mStatus == METAB_FIXED)
  //std::cout << "warning: set the transient concentration on a fixed metab" << std::endl;
#endif
}

void CMetab::setInitialConcentration(const C_FLOAT64 & initialConcentration)
{
  mIConc = initialConcentration;
  *mpIValue = initialConcentration * mpCompartment->getVolume()
              * mpModel->getQuantity2NumberFactor();

  // This is obsolete.
  // if (isFixed())
  //   setConcentration(initialConcentration);

  std::set< CMoiety * >::iterator it = mMoieties.begin();
  std::set< CMoiety * >::iterator end = mMoieties.end();

  for (; it != end; ++it)
    (*it)->setInitialValue();

  return;
}

void CMetab::setValue(const C_FLOAT64 & value)
{
  if (isFixed()) return;

  mConc = value / mpCompartment->getVolume()
          * mpModel->getNumber2QuantityFactor();
  *mpValueAccess = value;

#ifdef COPASI_DEBUG
  //  if (mStatus == METAB_FIXED)
  //    std::cout << "warning: set the transient particle number on a fixed metab" << std::endl;
#endif
}

void CMetab::setInitialValue(const C_FLOAT64 & initialValue)
{
  if (*mpIValue == initialValue) return;

  if (initialValue)
    mIConc = initialValue / mpCompartment->getVolume() * mpModel->getNumber2QuantityFactor();

  *mpIValue = initialValue;

  // This is obsolete.
  // if (isFixed())
  //   setNumber(initialValue);

  std::set< CMoiety * >::iterator it = mMoieties.begin();
  std::set< CMoiety * >::iterator end = mMoieties.end();

  for (; it != end; ++it)
    (*it)->setInitialValue();

  return;
}

void CMetab::refreshInitialConcentration()
{
  mIConc = *mpIValue / mpCompartment->getVolume() * mpModel->getNumber2QuantityFactor();

  if (isFixed()) mConc = mIConc;
}

void CMetab::refreshConcentration()
{
  mConc = *mpValueAccess / mpCompartment->getVolume() * mpModel->getNumber2QuantityFactor();
}

void CMetab::setStatus(const CModelEntity::Status & status)
{
  CModelEntity::setStatus(status);

  if (mpModel && mpCompartment) refreshConcentration();
}

void CMetab::initObjects()
{
  CCopasiObject * pObject;

  pObject = addObjectReference("Concentration", mConc, CCopasiObject::ValueDbl);

  if (mpModel)
    pObject->setRefresh(mpModel, &CModel::applyAssignments);

  pObject = addObjectReference("InitialConcentration", mIConc, CCopasiObject::ValueDbl);
  pObject->setUpdateMethod(this, &CMetab::setInitialConcentration);
  // pObject->setRefresh(this, &CMetab::refreshInitialConcentration);

  mpValueReference->setObjectName("ParticleNumber");
  mpIValueReference->setObjectName("InitialParticleNumber");
  mpIValueReference->setUpdateMethod(this, &CMetab::setInitialValue);
  mpRateReference->setObjectName("ParticleNumberRate");

  pObject = addObjectReference("Rate", mConcRate, CCopasiObject::ValueDbl);
  std::set< const CCopasiObject * > Dependencies;
  Dependencies.insert(mpRateReference);
  pObject->setDirectDependencies(Dependencies);
  pObject->setRefresh(this, &CMetab::refreshRate);

  pObject = addObjectReference("TransitionTime", mTT, CCopasiObject::ValueDbl);
  pObject->setDirectDependencies(Dependencies);
  if (mpModel)
    pObject->setRefresh(mpModel, &CModel::setTransitionTimes);
}

C_FLOAT64 CMetab::getConcentrationRate() const
  {
    return mRate / getCompartment()->getVolume()
    * mpModel->getNumber2QuantityFactor();
  }

void CMetab::refreshRate()
{
  mConcRate = mRate / getCompartment()->getVolume() * mpModel->getNumber2QuantityFactor();
}

void * CMetab::getValuePointer() const
  {
    //std::cout << "CMetab::getValuePointer();" << std::endl;
    return const_cast<C_FLOAT64 *>(&mConc);
  }

std::ostream & operator<<(std::ostream &os, const CMetab & d)
{
  os << "    ++++CMetab: " << d.getObjectName() << std::endl;
  os << "        mConc " << d.mConc << " mIConc " << d.mIConc << std::endl;
  os << "        mValue (particle number) " << *d.mpValueAccess << " mIValue " << *d.mpIValue << std::endl;
  os << "        mRate " << d.mRate << " mTT " << d.mTT << " mStatus " << d.getStatus() << std::endl;

  if (d.mpCompartment)
    os << "        mpCompartment == " << d.mpCompartment << std::endl;
  else
    os << "        mpCompartment == 0 " << std::endl;

  if (d.mpModel)
    os << "        mpModel == " << d.mpModel << std::endl;
  else
    os << "        mpModel == 0 " << std::endl;

  os << "    ----CMetab " << std::endl;

  return os;
}

C_INT32 CMetab::load(CReadConfig &configbuffer)
{
  C_INT32 Fail = 0;

  std::string tmp;
  Fail = configbuffer.getVariable("Metabolite", "string",
                                  (void *) & tmp,
                                  CReadConfig::SEARCH);

  if (Fail)
    return Fail;
  setObjectName(tmp);

  Fail = configbuffer.getVariable("InitialConcentration", "C_FLOAT64",
                                  (void *) & mIConc);

  setInitialConcentration(mIConc);
  setConcentration(mIConc);

  Status GepasiStatus;
  Fail = configbuffer.getVariable("Type", "C_INT16",
                                  (void *) & GepasiStatus);

  if (Fail)
    return Fail;

  setStatus(GepasiStatus);

  // sanity check
  if ((GepasiStatus < 0) || (GepasiStatus > 7))
    {
      CCopasiMessage(CCopasiMessage::WARNING,
                     "The file specifies a non-existing type "
                     "for '%s'.\nReset to internal metabolite.",
                     getObjectName().c_str());
      setStatus(REACTIONS);
    }

  // sanity check
  if ((GepasiStatus != METAB_MOIETY) && (mIConc < 0.0))
    {
      CCopasiMessage(CCopasiMessage::WARNING,
                     "The file specifies a negative concentration "
                     "for '%s'.\nReset to default.",
                     getObjectName().c_str());
      mIConc = 1.0;
    }

  return Fail;
}

std::string CMetab::getObjectDisplayName(bool regular, bool richtext) const
  {
    CModel* tmp = dynamic_cast<CModel*>(this->getObjectAncestor("Model"));
    if (tmp)
      {
        return CMetabNameInterface::getDisplayName(tmp, *this);
      }

    return CCopasiObject::getObjectDisplayName(regular, richtext);
  }

void CMetab::addMoiety(CMoiety * pMoiety)
{mMoieties.insert(pMoiety);}

void CMetab::clearMoieties()
{mMoieties.clear();}

//******************* CMetabOld ***************************************************

CMetabOld::CMetabOld(const std::string & name,
                     const CCopasiContainer * pParent):
    CCopasiContainer(name, pParent, "Old Metabolite"),
    mIConc(1.0),
    mStatus(CModelEntity::REACTIONS),
    mCompartment()
{CONSTRUCTOR_TRACE;}

CMetabOld::CMetabOld(const CMetabOld & src,
                     const CCopasiContainer * pParent):
    CCopasiContainer(src, pParent),
    mIConc(src.mIConc),
    mStatus(src.mStatus),
    mCompartment(src.mCompartment)
{CONSTRUCTOR_TRACE;}

CMetabOld::~CMetabOld() {DESTRUCTOR_TRACE;}

void CMetabOld::cleanup(){}

C_INT32 CMetabOld::load(CReadConfig &configbuffer)
{
  C_INT32 Fail = 0;
  std::string tmp;
  Fail = configbuffer.getVariable("Metabolite", "string",
                                  (void *) & tmp,
                                  CReadConfig::SEARCH);

  if (Fail)
    return Fail;
  setObjectName(tmp);

  Fail = configbuffer.getVariable("Concentration", "C_FLOAT64",
                                  (void *) & mIConc);

  if (Fail)
    return Fail;

  Fail = configbuffer.getVariable("Compartment", "C_INT32",
                                  (void *) & mCompartment);

  if (Fail)
    return Fail;

  C_INT32 Status;

  Fail = configbuffer.getVariable("Type", "C_INT32",
                                  (void *) & Status);

  mStatus = (CMetab::Status) Status;

  // sanity check
  if ((mStatus < 0) || (mStatus > 7))
    {
      CCopasiMessage(CCopasiMessage::WARNING,
                     "The file specifies a non-existing type "
                     "for '%s'.\nReset to internal metabolite.",
                     getObjectName().c_str());
      mStatus = CModelEntity::REACTIONS;
    }

  // sanity check
  if ((mStatus != METAB_MOIETY) && (mIConc < 0.0))
    {
      CCopasiMessage(CCopasiMessage::WARNING,
                     "The file specifies a negative concentration "
                     "for '%s'.\nReset to default.",
                     getObjectName().c_str());
      mIConc = 1.0;
    }

  return Fail;
}

C_INT32 CMetabOld::getIndex() const {return mCompartment;}
