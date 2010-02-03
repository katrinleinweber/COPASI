// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/trajectory/CTauLeapMethod.cpp,v $
//   $Revision: 1.27 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2010/02/03 21:22:30 $
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

/**
 *   CTauLeapMethod
 *
 *   This class implements the tau-Leap method for the simulation of a
 *   biochemical system over time (see Gillespie (2001): Approximate
 *   accelerated stochastic simulation of chemically reacting systems.
 *   J. Chemical Physics, 115:1716-1733).
 *
 *   File name: CTauLeapMethod.cpp
 *   Author: Juergen Pahle
 *   Email: juergen.pahle@eml-r.villa-bosch.de
 *
 *   Last change: 20, April 2004
 *
 *   (C) European Media Lab 2004.
 */

/* DEFINE ********************************************************************/

#include "mathematics.h" // pow(), floor()

#include "copasi.h"

#include "CTauLeapMethod.h"
#include "CTrajectoryProblem.h"
#include "CHybridMethod.h" // CHybridBalance, todo: beautify this
#include "model/CModel.h"
#include "model/CMetab.h"
#include "model/CReaction.h"
#include "model/CState.h"
#include "model/CChemEq.h"
#include "model/CChemEqElement.h"
#include "model/CCompartment.h"
#include "utilities/CCopasiVector.h"
#include "utilities/CMatrix.h"
#include "utilities/CDependencyGraph.h"
#include "utilities/CIndexedPriorityQueue.h"
#include "randomGenerator/CRandom.h"

/* PUBLIC METHODS ************************************************************/

/**
 *   Creates a TauLeapMethod adequate for the problem.
 *   (only regular TauLeapMethod so far)
 */
CTauLeapMethod *CTauLeapMethod::createTauLeapMethod()
{
  C_INT32 result = 1; // regular TauLeap method as default
  /*  if (pProblem && pProblem->getModel())
      {
      result = checkModel(pProblem->getModel());
      }*/
  CTauLeapMethod * method = NULL;

  switch (result)
    {
        // Error: TauLeap simulation impossible
        /*    case - 3:      // non-integer stoichiometry
        CCopasiMessage(CCopasiMessage::ERROR, MCTrajectoryMethod + 1);
        break;
        case - 2:      // reversible reaction exists
        CCopasiMessage(CCopasiMessage::ERROR, MCTrajectoryMethod + 2);
        break;
        case - 1:      // more than one compartment involved
        CCopasiMessage(CCopasiMessage::ERROR, MCTrajectoryMethod + 3);
        break;*/
        // Everything all right: Hybrid simulation possible
      case 1:
      default:
        method = new CTauLeapMethod();
        break;
    }

  return method;
}

/**
 *   Default constructor.
 */
CTauLeapMethod::CTauLeapMethod(const CCopasiContainer * pParent):
    CTrajectoryMethod(CCopasiMethod::tauLeap, pParent)
{
  mpRandomGenerator = CRandom::createGenerator(CRandom::mt19937);
  initializeParameter();
}

CTauLeapMethod::CTauLeapMethod(const CTauLeapMethod & src,
                               const CCopasiContainer * pParent):
    CTrajectoryMethod(src, pParent)
{
  mpRandomGenerator = CRandom::createGenerator(CRandom::mt19937);
  initializeParameter();
}

/**
 *   Destructor.
 */
CTauLeapMethod::~CTauLeapMethod()
{
  cleanup();
  DESTRUCTOR_TRACE;
}

void CTauLeapMethod::initializeParameter()
{
  CCopasiParameter *pParm;

  assertParameter("Tau", CCopasiParameter::DOUBLE, (C_FLOAT64) TAU);
  assertParameter("Use Random Seed", CCopasiParameter::BOOL, false);
  assertParameter("Random Seed", CCopasiParameter::UINT, (unsigned C_INT32) 1);

  // Check whether we have a method with the old parameter names
  if ((pParm = getParameter("TAULEAP.Tau")) != NULL)
    {
      setValue("Tau", *pParm->getValue().pDOUBLE);
      removeParameter("TAULEAP.Tau");

      if ((pParm = getParameter("TAULEAP.UseRandomSeed")) != NULL)
        {
          setValue("Use Random Seed", *pParm->getValue().pBOOL);
          removeParameter("TAULEAP.UseRandomSeed");
        }

      if ((pParm = getParameter("TAULEAP.RandomSeed")) != NULL)
        {
          setValue("Random Seed", *pParm->getValue().pUINT);
          removeParameter("TAULEAP.RandomSeed");
        }
    }
}

bool CTauLeapMethod::elevateChildren()
{
  initializeParameter();
  return true;
}

CTrajectoryMethod::Status CTauLeapMethod::step(const double & deltaT)
{
  // write the current state to the model
  // mpProblem->getModel()->setState(mpCurrentState); // is that correct?

  // do several steps
  C_FLOAT64 time = mpCurrentState->getTime();
  C_FLOAT64 endTime = time + deltaT;

  C_FLOAT64 ds;

  ds = mTau;

  while ((time + ds) < endTime)
    {
      doSingleStep(ds);
      time += ds;
    }

  if (time < endTime)
    {
      doSingleStep(endTime - time);
      time = endTime;
    }

  *mpCurrentState = mpProblem->getModel()->getState();
  mpCurrentState->setTime(time);

  return NORMAL;
}

void CTauLeapMethod::start(const CState * initialState)
{
  *mpCurrentState = *initialState;

  mpModel = mpProblem->getModel();
  mpModel->setState(*mpCurrentState);

  if (mpModel->getModelType() == CModel::deterministic)
    mDoCorrection = true;
  else
    mDoCorrection = false;

  mHasAssignments = modelHasAssignments(mpModel);

  mFirstMetabIndex = mpModel->getStateTemplate().getIndex(mpModel->getMetabolitesX()[0]);

  // call init of the simulation method, can be overloaded in derived classes
  initMethod();

  return;
}

/* PROTECTED METHODS *********************************************************/

/**
 *  Initializes the solver and sets the model to be used.
 *
 *  @param model A reference to an instance of a CModel
 */
void CTauLeapMethod::initMethod()
{
  unsigned C_INT32 i;

  mpReactions = &mpModel->getReactions();
  mNumReactions = mpReactions->size();
  mpMetabolites = &(const_cast < CCopasiVector < CMetab > & >(mpModel->getMetabolitesX()));
  mAmu.clear();
  mAmu.resize(mpReactions->size());
  mK.clear();
  mK.resize(mpReactions->size());

  mNumNumbers = mpModel->getMetabolitesX().size();
  mNumbers.clear();
  mNumbers.resize(mNumNumbers);

  for (i = 0; i < mNumNumbers; ++i)
    {
      mNumbers[i] = (C_INT64) mpModel->getMetabolitesX()[i]->getValue();
      mpModel->getMetabolitesX()[i]->setValue(mNumbers[i]);
      mpModel->getMetabolitesX()[i]->refreshConcentration();
    }

  //TODO also put fixed variables here

  mpModel->updateSimulatedValues(false); //for assignments

  //  imax = mpCurrentState->getNumFixed();
  //  for (i = 0; i < imax; ++i, Dbl++)
  //    *Dbl = floor(*Dbl);

  /* get configuration data */
  mTau = * getValue("Tau").pDOUBLE;
  mUseRandomSeed = * getValue("Use Random Seed").pBOOL;
  mRandomSeed = * getValue("Random Seed").pUINT;

  if (mUseRandomSeed) mpRandomGenerator->initialize(mRandomSeed);

  /* set up internal data structures */
  setupBalances(); // initialize mBalances (has to be called first!)

  /* poission-distr. random number generation */
  sq = -1.0;
  alxm = -1.0;
  g = -1.0;
  oldm = -1.0;

  return;
}

/**
 *  Cleans up memory, etc.
 */
void CTauLeapMethod::cleanup()
{
  delete mpRandomGenerator;
  mpRandomGenerator = NULL;
  mpModel = NULL;
  return;
}

/**
 *   Test the model if it is proper to perform stochastic simulations on.
 *   Several properties are tested (e.g. integer stoichometry, all reactions
 *   take place in one compartment only, irreversibility...).
 *
 *   @return 0, if everything is ok; <0, if an error occured.
 */
C_INT32 CTauLeapMethod::checkModel(CModel * model)
{
  CCopasiVectorNS <CReaction> * mpReactions = &model->getReactions();
  CMatrix <C_FLOAT64> mStoi = model->getStoi();
  C_INT32 i, multInt, numReactions = mpReactions->size();
  unsigned C_INT32 j;
  C_FLOAT64 multFloat;
  //  C_INT32 metabSize = mpMetabolites->size();

  for (i = 0; i < numReactions; i++) // for every reaction
    {
      // TEST getCompartmentNumber() == 1
      if ((*mpReactions)[i]->getCompartmentNumber() != 1) return - 1;

      // TEST isReversible() == 0
      if ((*mpReactions)[i]->isReversible() != 0) return - 2;

      // TEST integer stoichometry
      // Iterate through each the metabolites
      // juergen: the number of rows of mStoi equals the number of non-fixed metabs!
      //  for (j=0; i<metabSize; j++)
      for (j = 0; j < mStoi.numRows(); j++)
        {
          multFloat = mStoi[j][i];
          multInt = static_cast<C_INT32>(floor(multFloat + 0.5)); // +0.5 to get a rounding out of the static_cast to int!

          if ((multFloat - multInt) > INT_EPSILON) return - 3; // INT_EPSILON in CTauLeapMethod.h
        }
    }

  return 1; // Model is appropriate for hybrid simulation
}

/**
 *  Simulates the system over the next interval of time. The timestep
 *  is given as argument.
 *
 *  @param  ds A C_FLOAT64 specifying the timestep
 */
void CTauLeapMethod::doSingleStep(C_FLOAT64 ds)
{
  unsigned C_INT32 i;
  C_FLOAT64 lambda;

  updatePropensities();

  for (i = 0; i < mNumReactions; i++)
    {
      if ((lambda = mAmu[i] * ds) < 0.0)
        CCopasiMessage(CCopasiMessage::EXCEPTION, MCTrajectoryMethod + 10);

      mK[i] = (C_INT64)mpRandomGenerator->getRandomPoisson(mAmu[i] * ds);
    }

  updateSystem();
  return;
}

/**
 *   Sets up an internal representation of the balances for each reaction.
 *   This is done in order to be able to deal with fixed metabolites and
 *   to avoid a time consuming search for the indices of metabolites in the
 *   model.
 */
void CTauLeapMethod::setupBalances()
{
  unsigned C_INT32 i, j;
  CHybridBalance newElement;

  mNumReactions = mpReactions->size();
  mLocalBalances.clear();
  mLocalBalances.resize(mNumReactions);
  mLocalSubstrates.clear();
  mLocalSubstrates.resize(mNumReactions);

  for (i = 0; i < mNumReactions; i++)
    {
      const CCopasiVector <CChemEqElement> * balances =
        &(*mpReactions)[i]->getChemEq().getBalances();

      for (j = 0; j < balances->size(); j++)
        {
          newElement.mpMetabolite = const_cast < CMetab* >((*balances)[j]->getMetabolite());
          newElement.mIndex = mpModel->getMetabolitesX().getIndex(newElement.mpMetabolite);
          // + 0.5 to get a rounding out of the static_cast to C_INT32!
          newElement.mMultiplicity = static_cast<C_INT32>(floor((*balances)[j]->getMultiplicity() + 0.5));

          if ((newElement.mpMetabolite->getStatus()) != CModelEntity::FIXED)
            mLocalBalances[i].push_back(newElement); // element is copied for the push_back
        }

      balances = &(*mpReactions)[i]->getChemEq().getSubstrates();

      for (j = 0; j < balances->size(); j++)
        {
          newElement.mpMetabolite = const_cast < CMetab* >((*balances)[j]->getMetabolite());
          newElement.mIndex = mpModel->getMetabolitesX().getIndex(newElement.mpMetabolite);
          // + 0.5 to get a rounding out of the static_cast to C_INT32!
          newElement.mMultiplicity = static_cast<C_INT32>(floor((*balances)[j]->getMultiplicity() + 0.5));

          mLocalSubstrates[i].push_back(newElement); // element is copied for the push_back
        }
    }

  return;
}

void CTauLeapMethod::updatePropensities()
{
  //mA0Old = mA0;
  mA0 = 0;

  for (unsigned C_INT32 i = 0; i < mNumReactions; i++)
    {
      calculateAmu(i);
      mA0 += mAmu[i];
    }

  return;
}

C_INT32 CTauLeapMethod::calculateAmu(C_INT32 index)
{
  if (!mDoCorrection)
    {
      mAmu[index] = mpModel->getReactions()[index]->calculateParticleFlux();
      return 0;
    }

  // We need the product of the cmu and hmu for this step.
  // We calculate this in one go, as there are fewer steps to
  // perform and we eliminate some possible rounding errors.
  C_FLOAT64 amu = 1; // initially
  //C_INT32 total_substrates = 0;
  C_INT32 num_ident = 0;
  C_INT64 number = 0;
  C_INT64 lower_bound;
  // substrate_factor - The substrates, raised to their multiplicities,
  // multiplied with one another. If there are, e.g. m substrates of type m,
  // and n of type N, then substrate_factor = M^m * N^n.
  C_FLOAT64 substrate_factor = 1;
  // First, find the reaction associated with this index.
  // Keep a pointer to this.
  // Iterate through each substrate in the reaction
  const std::vector<CHybridBalance> & substrates = mLocalSubstrates[index];

  int flag = 0;

  for (unsigned C_INT32 i = 0; i < substrates.size(); i++)
    {
      num_ident = substrates[i].mMultiplicity;

      if (num_ident > 1)
        {
          flag = 1;
          number = mNumbers[substrates[i].mIndex];
          lower_bound = number - num_ident;
          substrate_factor = substrate_factor * pow((double) number, (int)(num_ident - 1));  //optimization

          number--; //optimization

          while (number > lower_bound)
            {
              amu *= number;
              number--;
            }
        }
    }

  if ((amu == 0) || (substrate_factor == 0))  // at least one substrate particle number is zero
    {
      mAmu[index] = 0;
      return 0;
    }

  // We assume that all substrates are in the same compartment.
  // If there are no substrates, then volume is irrelevant. Otherwise,
  // we can use the volume of the compartment for the first substrate.
  //if (substrates.size() > 0) //check again!!
  /*if (total_substrates > 1) //check again!!
    {
      C_FLOAT64 invvolumefactor =
        pow((double)
            (substrates[0]->getMetabolite().getCompartment()->getVolumeInv()
             * substrates[0]->getMetabolite().getModel()->getNumber2QuantityFactor()),
            (int) total_substrates - 1);
      amu *= invvolumefactor;
      substrate_factor *= invvolumefactor;
    }*/

  // rate_factor is the rate function divided by substrate_factor.
  // It would be more efficient if this was generated directly, since in effect we
  // are multiplying and then dividing by the same thing (substrate_factor)!
  C_FLOAT64 rate_factor = mpModel->getReactions()[index]->calculateParticleFlux();

  if (flag)
    {
      //cout << "Rate factor = " << rate_factor << endl;
      amu *= rate_factor / substrate_factor;
      mAmu[index] = amu;
    }
  else
    {
      mAmu[index] = rate_factor;
    }

  return 0;

  // a more efficient way to calculate mass action kinetics could be included
}

/**
 *   Updates the system according to the probabilistic
 *   number of firings mK[i] of each reaction i
 */
void CTauLeapMethod::updateSystem()
{
  unsigned C_INT32 i;
  std::vector<CHybridBalance>::const_iterator it;
  CMetab* pTmpMetab;

  for (i = 0; i < mNumReactions; i++)
    for (it = mLocalBalances[i].begin(); it != mLocalBalances[i].end(); it++)
      {
        mNumbers[it->mIndex] += mK[i] * (C_INT64)(it->mMultiplicity);
        pTmpMetab = mpModel->getMetabolitesX()[it->mIndex];
        pTmpMetab->setValue(mNumbers[it->mIndex]);
        pTmpMetab->refreshConcentration();
      }

  if (mHasAssignments)
    {
      mpModel->updateSimulatedValues(false);

      for (i = 0; i < mNumNumbers; ++i)
        {
          if (mpModel->getMetabolitesX()[i]->getStatus() == CModelEntity::ASSIGNMENT)
            {
              mNumbers[i] = (C_INT64) mpModel->getMetabolitesX()[i]->getValue();
              mpModel->getMetabolitesX()[i]->setValue(mNumbers[i]);
              mpModel->getMetabolitesX()[i]->refreshConcentration();
            }
        }
    }

  return;
}

//virtual
bool CTauLeapMethod::isValidProblem(const CCopasiProblem * pProblem)
{
  if (!CTrajectoryMethod::isValidProblem(pProblem)) return false;

  const CTrajectoryProblem * pTP = dynamic_cast<const CTrajectoryProblem *>(pProblem);

  if (pTP->getDuration() < 0.0)
    {
      //back integration not possible
      CCopasiMessage(CCopasiMessage::EXCEPTION, MCTrajectoryMethod + 9);
      return false;
    }

  //TODO: rewrite CModel::suitableForStochasticSimulation() to use
  //      CCopasiMessage
  std::string message = pTP->getModel()->suitableForStochasticSimulation();

  if (message != "")
    {
      //model not suitable, message describes the problem
      CCopasiMessage(CCopasiMessage::EXCEPTION, message.c_str());
      return false;
    }

  mTau = * getValue("Tau").pDOUBLE;

  if (mTau <= 0.0)
    {
      // tau-value is not positive
      CCopasiMessage(CCopasiMessage::EXCEPTION, MCTrajectoryMethod + 11, mTau);
      return false;
    }

  return true;
}

//static
bool CTauLeapMethod::modelHasAssignments(const CModel* pModel)
{
  C_INT32 i, imax = pModel->getNumModelValues();

  for (i = 0; i < imax; ++i)
    {
      if (pModel->getModelValues()[i]->getStatus() == CModelEntity::ASSIGNMENT)
        if (pModel->getModelValues()[i]->isUsed())
          {
            //used assignment found
            return true;
          }
    }

  imax = pModel->getNumMetabs();

  for (i = 0; i < imax; ++i)
    {
      if (pModel->getMetabolites()[i]->getStatus() == CModelEntity::ASSIGNMENT)
        if (pModel->getMetabolites()[i]->isUsed())
          {
            //used assignment found
            return true;
          }
    }

  imax = pModel->getCompartments().size();

  for (i = 0; i < imax; ++i)
    {
      if (pModel->getCompartments()[i]->getStatus() == CModelEntity::ASSIGNMENT)
        if (pModel->getCompartments()[i]->isUsed())
          {
            //used assignment found
            return true;
          }
    }

  return false;
}
