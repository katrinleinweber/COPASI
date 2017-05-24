// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2002 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/**
 * CSteadyStateTask class.
 *
 * This class implements a steady state task which is comprised of a
 * of a problem and a method. Additionally calls to the reporting
 * methods are done when initialized.
 *
 * Created for COPASI by Stefan Hoops 2002
 */

#include "copasi.h"

#include "CSteadyStateTask.h"
#include "CSteadyStateProblem.h"
#include "CSteadyStateMethod.h"

#include "math/CMathContainer.h"
#include "model/CModel.h"
#include "model/CMetabNameInterface.h"
#include "report/CKeyFactory.h"
#include "report/CReport.h"

#include <sstream>

#define XXXX_Reporting

CSteadyStateTask::CSteadyStateTask(const CDataContainer * pParent,
                                   const CTaskEnum::Task & type):
  CCopasiTask(pParent, type),
  mSteadyState(),
  mJacobian(),
  mJacobianReduced(),
  mpJacobianAnn(NULL),
  mpJacobianXAnn(NULL),
  mEigenValues("Eigenvalues of Jacobian", this),
  mEigenValuesX("Eigenvalues of reduced system Jacobian", this)
{
  mpProblem = new CSteadyStateProblem(this);

  mpMethod = createMethod(CTaskEnum::Method::Newton);

  initObjects();
}

CSteadyStateTask::CSteadyStateTask(const CSteadyStateTask & src,
                                   const CDataContainer * pParent):
  CCopasiTask(src, pParent),
  mSteadyState(src.mSteadyState),
  mJacobian(src.mJacobian),
  mJacobianReduced(src.mJacobianReduced),
  mpJacobianAnn(NULL),
  mpJacobianXAnn(NULL),
  mEigenValues(src.mEigenValues, this),
  mEigenValuesX(src.mEigenValuesX, this)
{
  mpProblem =
    new CSteadyStateProblem(*(CSteadyStateProblem *) src.mpProblem, this);

  mpMethod = createMethod(src.mpMethod->getSubType());
  *mpMethod = *src.mpMethod;

  initObjects();
}

CSteadyStateTask::~CSteadyStateTask()
{}

void CSteadyStateTask::cleanup()
{}

void CSteadyStateTask::initObjects()
{
  mpJacobianAnn = new CDataArray("Jacobian (complete system)", this,
                                 new CMatrixInterface<CMatrix<C_FLOAT64> >(&mJacobian), true);
  mpJacobianAnn->setMode(CDataArray::OBJECTS);
  mpJacobianAnn->setDescription("");
  mpJacobianAnn->setDimensionDescription(0, "Variables of the system, including dependent species");
  mpJacobianAnn->setDimensionDescription(1, "Variables of the system, including dependent species");

  mpJacobianXAnn = new CDataArray("Jacobian (reduced system)", this,
                                  new CMatrixInterface<CMatrix<C_FLOAT64> >(&mJacobianReduced), true);
  mpJacobianXAnn->setMode(CDataArray::OBJECTS);
  mpJacobianXAnn->setDescription("");
  mpJacobianXAnn->setDimensionDescription(0, "Independent variables of the system");
  mpJacobianXAnn->setDimensionDescription(1, "Independent variables of the system");

  mpEigenvaluesJacobianAnn = new CDataArray("Eigenvalues of Jacobian", this,
      new CMatrixInterface<CMatrix<C_FLOAT64> >(&mEigenvaluesMatrix), true);
  mpEigenvaluesJacobianAnn->setMode(CDataArray::VECTOR);
  mpEigenvaluesJacobianAnn->setDescription("");
  mpEigenvaluesJacobianAnn->setDimensionDescription(0, "n-th value");
  mpEigenvaluesJacobianAnn->setDimensionDescription(1, "Real/Imaginary part");

  mpEigenvaluesJacobianXAnn = new CDataArray("Eigenvalues of reduced system Jacobian", this,
      new CMatrixInterface<CMatrix<C_FLOAT64> >(&mEigenvaluesXMatrix), true);
//  mpEigenvaluesJacobianXAnn->setMode(CArrayAnnotation::VECTOR);
  mpEigenvaluesJacobianXAnn->setMode(CDataArray::OBJECTS);
  mpEigenvaluesJacobianXAnn->setDescription("");
  mpEigenvaluesJacobianXAnn->setDimensionDescription(0, "n-th value");
  mpEigenvaluesJacobianXAnn->setDimensionDescription(1, "Real/Imaginary part");
}

void CSteadyStateTask::print(std::ostream * ostream) const {(*ostream) << (*this);}

void CSteadyStateTask::load(CReadConfig & configBuffer)
{
  configBuffer.getVariable("SteadyState", "bool", &mScheduled,
                           CReadConfig::LOOP);

  ((CSteadyStateProblem *) mpProblem)->load(configBuffer);

  ((CSteadyStateMethod *) mpMethod)->load(configBuffer);
}

const CVectorCore< C_FLOAT64 > & CSteadyStateTask::getState() const
{
  return mSteadyState;
}

const CMatrix< C_FLOAT64 > & CSteadyStateTask::getJacobian() const
{return mJacobian;}
const CMatrix< C_FLOAT64 > & CSteadyStateTask::getJacobianReduced() const
{return mJacobianReduced;}

const CDataArray * CSteadyStateTask::getJacobianAnnotated() const
{
  return mpJacobianAnn;
}

const CDataArray * CSteadyStateTask::getJacobianXAnnotated() const
{
  return mpJacobianXAnn;
}

const CEigen & CSteadyStateTask::getEigenValues() const
{
  return mEigenValues;
}
const CEigen & CSteadyStateTask::getEigenValuesReduced() const
{
  return mEigenValuesX;
}

bool CSteadyStateTask::updateMatrices()
{
  // init Jacobians
  size_t sizeReduced = mpContainer->getState(true).size() - mpContainer->getCountFixedEventTargets() - 1;
  mJacobianReduced.resize(sizeReduced, sizeReduced);
  size_t size = mpContainer->getState(false).size() - mpContainer->getCountFixedEventTargets() - 1;
  mJacobian.resize(size, size);

  // Jacobian Annotations

  mpJacobianAnn->resize();
  mpJacobianXAnn->resize();

  const CMathObject * pObject = mpContainer->getMathObject(mpContainer->getState(false).array() + mpContainer->getCountFixedEventTargets() + 1);
  const CMathObject * pObjectEnd = pObject + sizeReduced;

  size_t i;

  for (i = 0; pObject != pObjectEnd; ++pObject, ++i)
    {
      std::string CN = pObject->getDataObject()->getObjectParent()->getCN();

      mpJacobianXAnn->setAnnotationCN(0 , i, CN);
      mpJacobianXAnn->setAnnotationCN(1 , i, CN);

      mpJacobianAnn->setAnnotationCN(0 , i, CN);
      mpJacobianAnn->setAnnotationCN(1 , i, CN);
    }

  pObjectEnd += size - sizeReduced;

  for (; pObject != pObjectEnd; ++pObject, ++i)
    {
      std::string CN = pObject->getDataObject()->getObjectParent()->getCN();

      mpJacobianAnn->setAnnotationCN(0 , i, CN);
      mpJacobianAnn->setAnnotationCN(1 , i, CN);
    }

  // initial dimension of Eigenvalues of Jacobian
  mEigenvaluesMatrix.resize(size, 2);
  mEigenvaluesXMatrix.resize(sizeReduced, 2);

  //mStatus.set(CCopasiTaskStatus::OutputDataStructuresInitialized);
  return true;
}

bool CSteadyStateTask::initialize(const OutputFlag & of,
                                  COutputHandler * pOutputHandler,
                                  std::ostream * pOstream)
{
  bool success = true;

  assert(mpProblem && mpMethod);

  CSteadyStateProblem* pProblem =
    dynamic_cast<CSteadyStateProblem *>(mpProblem);
  assert(pProblem);

  success &= pProblem->initialize();

  CSteadyStateMethod* pMethod =
    dynamic_cast<CSteadyStateMethod *>(mpMethod);
  assert(pMethod);

  success &= pMethod->initialize(pProblem);

  success &= pMethod->isValidProblem(mpProblem);

  success &= updateMatrices();

  mSteadyState = mpContainer->getState(true);

  success &= CCopasiTask::initialize(of, pOutputHandler, pOstream);

  return success;
}

bool CSteadyStateTask::process(const bool & useInitialValues)
{
  if (useInitialValues)
    {
      mpContainer->applyInitialValues();
    }

  mSteadyState = mpContainer->getState(true);

  // A steady-state makes only sense in an autonomous model,
  // i.e., the time of the steady-state must not be changed
  // during simulation.
  C_FLOAT64 InitialTime = mSteadyState[mpContainer->getCountFixedEventTargets()];

  CSteadyStateMethod* pMethod =
    dynamic_cast<CSteadyStateMethod *>(mpMethod);
  assert(pMethod);

  CSteadyStateProblem* pProblem =
    dynamic_cast<CSteadyStateProblem *>(mpProblem);
  assert(pMethod);

  output(COutputInterface::BEFORE);

  //call the method
  mResult = pMethod->process(mSteadyState,
                             mJacobianReduced,
                             mpCallBack);

  // Reset the time for an autonomous model.
  if (mpContainer->isAutonomous())
    {
      mSteadyState[mpContainer->getCountFixedEventTargets()] = InitialTime;
    }

  if (mResult == CSteadyStateMethod::notFound)
    restore();

  //update Jacobian
  if (mpContainer->isStateValid())
    {
      if (pProblem->isJacobianRequested() ||
          pProblem->isStabilityAnalysisRequested())
        {
          pMethod->doJacobian(mJacobian, mJacobianReduced);
        }

      //mpProblem->getModel()->setState(mpSteadyState);
      //mpProblem->getModel()->updateRates();

      //calculate eigenvalues
      if (pProblem->isStabilityAnalysisRequested())
        {
          mEigenValues.calcEigenValues(mJacobian);
          mEigenValuesX.calcEigenValues(mJacobianReduced);

          mEigenValues.stabilityAnalysis(pMethod->getStabilityResolution());
          mEigenValuesX.stabilityAnalysis(pMethod->getStabilityResolution());
        }
    }

  C_FLOAT64 * pTo;
  size_t i;

  // construct Eigenvalues of Jacobian
  CVector< C_FLOAT64 > vectorEigen_R = mEigenValues.getR();
  CVector< C_FLOAT64 > vectorEigen_I = mEigenValues.getI();

#ifdef DEBUG_UI
  C_INT32 size = vectorEigen_R.size() + vectorEigen_I.size();

  std::cout << "vectorEigen_R.size() = " << vectorEigen_R.size() << " + vectorEigen_I.size() = " << vectorEigen_I.size() << " == " << size << std::endl;
  std::cout << "size = " << mEigenvaluesXMatrix.size() << std::endl;
#endif
  assert(vectorEigen_R.size() == vectorEigen_I.size());

  pTo = mEigenvaluesMatrix.array();

  for (i = 0; i < vectorEigen_R.size(); ++i)
    {
      *pTo = vectorEigen_R[i]; ++pTo;
      *pTo = vectorEigen_I[i]; ++pTo;
    }

#ifdef DEBUG_UI
  std::cout << mEigenvaluesMatrix << std::endl;
#endif

  // construct Eigenvalues of Jacobian of reduced system
  CVector< C_FLOAT64 > vectorEigenX_R = mEigenValuesX.getR();
  CVector< C_FLOAT64 > vectorEigenX_I = mEigenValuesX.getI();

#ifdef DEBUG_UI
  C_INT32 sizeX = vectorEigenX_R.size() + vectorEigenX_I.size();

  std::cout << "vectorEigenX_R.size() = " << vectorEigenX_R.size() << " + vectorEigenX_I.size() = " << vectorEigenX_I.size() << " == " << sizeX << std::endl;
  std::cout << "size = " << mEigenvaluesXMatrix.size() << std::endl;
#endif

  assert(vectorEigenX_R.size() == vectorEigenX_I.size());

  pTo = mEigenvaluesXMatrix.array();

  for (i = 0; i < vectorEigenX_R.size(); ++i)
    {
      *pTo = vectorEigenX_R[i]; ++pTo;
      *pTo = vectorEigenX_I[i]; ++pTo;
    }

#ifdef DEBUG_UI
  std::cout << mEigenvaluesXMatrix << std::endl;
#endif

  mpContainer->updateSimulatedValues(true);
  mpContainer->updateTransientDataValues();
  mpContainer->pushAllTransientValues();

  output(COutputInterface::AFTER);

  return (mResult != CSteadyStateMethod::notFound);
}

bool CSteadyStateTask::restore()
{
  setCallBack(NULL);

  if (mpContainer != NULL)
    {
      if (mUpdateModel &&
          mResult != CSteadyStateMethod::notFound &&
          mpContainer->isStateValid())
        {
          mpContainer->setState(mSteadyState);
          mpContainer->updateSimulatedValues(true);
          mpContainer->setInitialState(mpContainer->getState(false));
        }
      else
        {
          mpContainer->setInitialState(mInitialState);
        }

      mpContainer->updateInitialValues(CCore::Framework::ParticleNumbers);
      mpContainer->pushInitialState();
    }

  return true;
}

// virtual
const CTaskEnum::Method * CSteadyStateTask::getValidMethods() const
{
  static const CTaskEnum::Method ValidMethods[] =
  {
    CTaskEnum::Method::Newton,
    CTaskEnum::Method::UnsetMethod
  };

  return ValidMethods;
}

std::ostream &operator<<(std::ostream &os, const CSteadyStateTask &A)
{
  switch (A.getResult())
    {
      case CSteadyStateMethod::found:
        os << "A steady state with given resolution was found." << std::endl;
        break;

      case CSteadyStateMethod::notFound:
        os << "No steady state with given resolution was found!" << std::endl;
        os << "(below are the last unsuccessful trial values)" << std::endl;
        break;

      case CSteadyStateMethod::foundEquilibrium:
        os << "An equilibrium steady state (zero fluxes) was found." << std::endl;
        break;

      case CSteadyStateMethod::foundNegative:
        os << "An invalid steady state (negative concentrations) was found." << std::endl;
    }

  os << std::endl;

  A.mpContainer->setState(A.mSteadyState);
  A.mpContainer->updateSimulatedValues(false);
  A.mpContainer->updateTransientDataValues();
  A.mpContainer->pushAllTransientValues();

  CModel * pModel = const_cast< CModel * >(&A.mpContainer->getModel());

  // Metabolite Info: Name, Concentration, Concentration Rate, Particle Number, Particle Rate, Transition Time
  const CDataVector<CMetab> & Metabolites = pModel->getMetabolites();
  const CMetab * pMetab;

  size_t i, imax = Metabolites.size();

  os << "Species" << "\t";
  os << "Concentration";

  std::string Units = CUnit::prettyPrint(pModel->getQuantityUnit() + "/(" + pModel->getVolumeUnit() + ")");

  if (Units != "")
    os << " (" << Units << ")";

  os << "\t";

  os << "Concentration Rate";
  Units =  CUnit::prettyPrint(Units + "/(" + pModel->getTimeUnit() + ")");

  if (Units != "")
    os << " (" << Units << ")";

  os << "\t";

  os << "Particle Number" << "\t";

  os << "Particle Number Rate";
  Units = CUnit::prettyPrint("1/(" + pModel->getTimeUnit() + ")");

  if (Units != "")
    os << " (" << Units << ")";

  os << "\t";

  os << "Transition Time";
  Units = CUnit::prettyPrint(pModel->getTimeUnit());

  if (Units != "")
    os << " (" << Units << ")";

  os << std::endl;

  for (i = 0; i < imax; ++i)
    {
      pMetab = &Metabolites[i];
      os << CMetabNameInterface::getDisplayName(pModel, *pMetab, false) << "\t";
      os << pMetab->getConcentration() << "\t";
      os << pMetab->getConcentrationRate() << "\t";
      os << pMetab->getValue() << "\t";
      os << pMetab->getRate() << "\t";
      os << pMetab->getTransitionTime() << std::endl;
    }

  os << std::endl;

  // Reaction Info: Name, Flux, Particle Flux
  const CDataVector<CReaction>& Reactions = pModel->getReactions();
  const CReaction * pReaction;

  imax = Reactions.size();

  os << "Reaction" << "\t";

  os << "Flux";
  Units = CUnit::prettyPrint(pModel->getQuantityUnit() + "/(" + pModel->getTimeUnit() + ")");

  if (Units != "")
    os << " (" << Units << ")";

  os << "\t";

  os << "Particle Flux";
  Units = CUnit::prettyPrint("#/(" + pModel->getTimeUnit() + ")");

  if (Units != "")
    os << " (" << Units << ")";

  os << std::endl;

  for (i = 0; i < imax; ++i)
    {
      pReaction = &Reactions[i];
      os << pReaction->getObjectName() << "\t";
      os << pReaction->getFlux() << "\t";
      os << pReaction->getParticleFlux() << std::endl;
    }

  os << std::endl;

  if (static_cast<CSteadyStateProblem *>(A.mpProblem)->isJacobianRequested())
    {
      os << *A.mpJacobianAnn << std::endl;

      if (static_cast<CSteadyStateProblem *>(A.mpProblem)->isStabilityAnalysisRequested())
        {
          os << "Eigenvalues\treal\timaginary" << std::endl;
          imax = A.mEigenValues.getR().size();

          for (i = 0; i < imax; i++)
            os << "\t" << A.mEigenValues.getR()[i] << "\t" << A.mEigenValues.getI()[i] << std::endl;

          os << std::endl;
        }

      os << *A.mpJacobianXAnn << std::endl;

      if (static_cast<CSteadyStateProblem *>(A.mpProblem)->isStabilityAnalysisRequested())
        {
          os << "Eigenvalues\treal\timaginary" << std::endl;
          imax = A.mEigenValuesX.getR().size();

          for (i = 0; i < imax; i++)
            os << "\t" << A.mEigenValuesX.getR()[i] << "\t" << A.mEigenValuesX.getI()[i] << std::endl;

          os << std::endl;
        }
    }

  if (static_cast<CSteadyStateProblem *>(A.mpProblem)->isStabilityAnalysisRequested())
    {
      os << "Stability Analysis of the Reduced System" << std::endl;
      os << A.mEigenValuesX << std::endl;
    }

  return os;
}
