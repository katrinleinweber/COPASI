// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/trajectory/Attic/CLsodarMethod.cpp,v $
//   $Revision: 1.13 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2009/04/24 19:28:09 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "copasi.h"

#include "CLsodarMethod.h"
#include "CTrajectoryProblem.h"

#include "CopasiDataModel/CCopasiDataModel.h"
#include "report/CCopasiRootContainer.h"
#include "model/CModel.h"
#include "model/CState.h"

CLsodarMethod::CLsodarMethod(const CCopasiContainer * pParent):
    CTrajectoryMethod(CCopasiMethod::LSODAR, pParent),
    mpState(NULL),
    mY(NULL)
{
  assert((void *) &mData == (void *) &mData.dim);

  mData.pMethod = this;
  initializeParameter();
}

CLsodarMethod::CLsodarMethod(const CLsodarMethod & src,
                             const CCopasiContainer * pParent):
    CTrajectoryMethod(src, pParent),
    mpState(NULL),
    mY(NULL)
{
  assert((void *) &mData == (void *) &mData.dim);

  mData.pMethod = this;
  initializeParameter();
}

CLsodarMethod::~CLsodarMethod()
{
  pdelete(mpState);
}

void CLsodarMethod::initializeParameter()
{
  CCopasiParameter *pParm;

  assertParameter("Integrate Reduced Model", CCopasiParameter::BOOL, (bool) false);
  assertParameter("Relative Tolerance", CCopasiParameter::UDOUBLE, (C_FLOAT64) 1.0e-6);
  assertParameter("Absolute Tolerance", CCopasiParameter::UDOUBLE, (C_FLOAT64) 1.0e-12);
  assertParameter("Adams Max Order", CCopasiParameter::UINT, (unsigned C_INT32) 12);
  assertParameter("BDF Max Order", CCopasiParameter::UINT, (unsigned C_INT32) 5);
  assertParameter("Max Internal Steps", CCopasiParameter::UINT, (unsigned C_INT32) 10000);

  // Check whether we have a method with the old parameter names
  if ((pParm = getParameter("LSODA.RelativeTolerance")) != NULL)
    {
      setValue("Relative Tolerance", *pParm->getValue().pUDOUBLE);
      removeParameter("LSODA.RelativeTolerance");

      if ((pParm = getParameter("LSODA.AbsoluteTolerance")) != NULL)
        {
          setValue("Absolute Tolerance", *pParm->getValue().pUDOUBLE);
          removeParameter("LSODA.AbsoluteTolerance");
        }

      if ((pParm = getParameter("LSODA.AdamsMaxOrder")) != NULL)
        {
          setValue("Adams Max Order", *pParm->getValue().pUINT);
          removeParameter("LSODA.AdamsMaxOrder");
        }

      if ((pParm = getParameter("LSODA.BDFMaxOrder")) != NULL)
        {
          setValue("BDF Max Order", *pParm->getValue().pUINT);
          removeParameter("LSODA.BDFMaxOrder");
        }

      if ((pParm = getParameter("LSODA.MaxStepsInternal")) != NULL)
        {
          setValue("Max Internal Steps", *pParm->getValue().pUINT);
          removeParameter("LSODA.MaxStepsInternal");
        }
    }

  // Check whether we have a method with "Use Default Absolute Tolerance"
  if ((pParm = getParameter("Use Default Absolute Tolerance")) != NULL)
    {
      C_FLOAT64 NewValue;

      if (*pParm->getValue().pBOOL)
        {
          // The default
          NewValue = 1.e-12;
        }
      else
        {
          C_FLOAT64 OldValue = *getValue("Absolute Tolerance").pUDOUBLE;

          CCopasiDataModel* pDataModel = getObjectDataModel();
          assert(pDataModel != NULL);
          CModel * pModel = pDataModel->getModel();

          if (pModel == NULL)
            // The default
            NewValue = 1.e-12;
          else
            {
              const CCopasiVectorNS< CCompartment > & Compartment = pModel->getCompartments();
              unsigned C_INT32 i, imax;
              C_FLOAT64 Volume = DBL_MAX;

              for (i = 0, imax = Compartment.size(); i < imax; i++)
                if (Compartment[i]->getValue() < Volume)
                  Volume = Compartment[i]->getValue();

              if (Volume == DBL_MAX)
                // The default
                NewValue = 1.e-12;
              else
                // Invert the scaling as best as we can
                NewValue = OldValue / (Volume * pModel->getQuantity2NumberFactor());
            }
        }

      setValue("Absolute Tolerance", NewValue);
      removeParameter("Use Default Absolute Tolerance");
    }
}

bool CLsodarMethod::elevateChildren()
{
  initializeParameter();
  return true;
}

void CLsodarMethod::step(const double & deltaT)
{
  if (!mData.dim) //just do nothing if there are no variables
    {
      mTime = mTime + deltaT;
      mpState->setTime(mTime);
      *mpCurrentState = *mpState;

      return;
    }

  C_FLOAT64 EndTime = mTime + deltaT;
  C_INT ITOL = 2; // mRtol scalar, mAtol vector
  C_INT one = 1;
  C_INT DSize = mDWork.size();
  C_INT ISize = mIWork.size();

  mLSODAR(&EvalF, //  1. evaluate F
          &mData.dim, //  2. number of variables
          mY, //  3. the array of current concentrations
          &mTime, //  4. the current time
          &EndTime, //  5. the final time
          &ITOL, //  6. error control
          &mRtol, //  7. relative tolerance array
          mAtol.array(), //  8. absolute tolerance array
          &mState, //  9. output by overshoot & interpolation
          &mLsodarStatus, // 10. the state control variable
          &one, // 11. further options (one)
          mDWork.array(), // 12. the double work array
          &DSize, // 13. the double work array size
          mIWork.array(), // 14. the int work array
          &ISize, // 15. the int work array size
          NULL, // 16. evaluate J (not given)
          &mJType, // 17. type of j evaluation 2 internal full matrix
          &EvalR, // 18. evaluate constraint functions
          &mNumRoots, // 19. number of constraint functions g(i)
          mRoots.array()); // 20. integer array of length NG for output of root information

  if (mLsodarStatus == -1) mLsodarStatus = 2;

  if ((mLsodarStatus != 1) && (mLsodarStatus != 2) && (mLsodarStatus != -1))
    {
      CCopasiMessage(CCopasiMessage::EXCEPTION, MCTrajectoryMethod + 6, mErrorMsg.str().c_str());
    }

  mpState->setTime(mTime);
  *mpCurrentState = *mpState;

  return;
}

void CLsodarMethod::start(const CState * initialState)
{
  /* Retrieve the model to calculate */
  mpModel = mpProblem->getModel();

  /* Reset lsoda */
  mLsodarStatus = 1;
  mState = 1;
  mJType = 2;
  mErrorMsg.str("");
  mLSODAR.setOstream(mErrorMsg);

  /* Release previous state and make the initialState the current */
  pdelete(mpState);
  mpState = new CState(*initialState);
  mY = mpState->beginIndependent();
  mTime = mpState->getTime();

  mReducedModel = * getValue("Integrate Reduced Model").pBOOL;

  if (mReducedModel)
    mData.dim = mpState->getNumIndependent();
  else
    mData.dim = mpState->getNumIndependent() + mpModel->getNumDependentReactionMetabs();

  mYdot.resize(mData.dim);

  mNumRoots = 0;
  mRoots.resize(mNumRoots);

  /* Configure lsodar */
  mRtol = * getValue("Relative Tolerance").pUDOUBLE;

  C_FLOAT64 * pTolerance = getValue("Absolute Tolerance").pUDOUBLE;
  mAtol = mpModel->initializeAtolVector(*pTolerance, mReducedModel);

  mDWork.resize(22 + mData.dim * std::max<C_INT>(16, mData.dim + 9) + 3 * mNumRoots);
  mDWork[4] = mDWork[5] = mDWork[6] = mDWork[7] = mDWork[8] = mDWork[9] = 0.0;
  mIWork.resize(20 + mData.dim);
  mIWork[4] = mIWork[6] = mIWork[9] = 0;

  mIWork[5] = * getValue("Max Internal Steps").pUINT;
  mIWork[7] = * getValue("Adams Max Order").pUINT;
  mIWork[8] = * getValue("BDF Max Order").pUINT;

  return;
}

void CLsodarMethod::EvalF(const C_INT * n, const C_FLOAT64 * t, const C_FLOAT64 * y, C_FLOAT64 * ydot)
{static_cast<Data *>((void *) n)->pMethod->evalF(t, y, ydot);}

void CLsodarMethod::evalF(const C_FLOAT64 * t, const C_FLOAT64 * y, C_FLOAT64 * ydot)
{
  assert(y == mY);

  mpState->setTime(*t);

  mpModel->setState(*mpState);
  mpModel->updateSimulatedValues(mReducedModel);

  if (mReducedModel)
    mpModel->calculateDerivativesX(ydot);
  else
    mpModel->calculateDerivatives(ydot);

  return;
}

void CLsodarMethod::EvalR(const C_INT * n, const C_FLOAT64 * t, const C_FLOAT64 * y,
                          const C_INT * nr, const double * r)
{static_cast<Data *>((void *) n)->pMethod->evalR(t, y, nr, r);}

void CLsodarMethod::evalR(const C_FLOAT64 * /* t */, const C_FLOAT64 * /* y */,
                          const C_INT * /* nr */, const double * /* r */)
{};
