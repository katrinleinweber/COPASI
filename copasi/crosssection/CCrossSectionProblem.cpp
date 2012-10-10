// Copyright (C) 2010 - 2012 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include "CCrossSectionProblem.h"

CCrossSectionProblem::CCrossSectionProblem(const CCopasiContainer * pParent):
  CTrajectoryProblem(pParent),
  mpFlagLimitCrossings(NULL),
  mpCrossingsLimit(NULL),
  mpFlagLimitConvergence(NULL),
  mpConvergenceTolerance(NULL),
  mpFlagLimitOutConvergence(NULL),
  mpConvergenceOutTolerance(NULL),
  mpFlagLimitOutCrossings(NULL),
  mpOutCrossingsLimit(NULL),
  mpFlagLimitTime(NULL)
{
  initializeParameter();
  initObjects();
  CONSTRUCTOR_TRACE;
}

CCrossSectionProblem::CCrossSectionProblem(const CCrossSectionProblem & src,
    const CCopasiContainer * pParent):
  CTrajectoryProblem(src, pParent),
  mpFlagLimitCrossings(NULL),
  mpCrossingsLimit(NULL),
  mpFlagLimitConvergence(NULL),
  mpConvergenceTolerance(NULL),
  mpFlagLimitOutConvergence(NULL),
  mpConvergenceOutTolerance(NULL),
  mpFlagLimitOutCrossings(NULL),
  mpOutCrossingsLimit(NULL),
  mpFlagLimitTime(NULL)
{
  initializeParameter();
  initObjects();
  CONSTRUCTOR_TRACE;
}

void CCrossSectionProblem::initializeParameter()
{
  mpFlagLimitCrossings  = assertParameter("LimitCrossings", CCopasiParameter::BOOL, false)->getValue().pBOOL;
  mpCrossingsLimit  = assertParameter("NumCrossingsLimit", CCopasiParameter::UINT, (unsigned C_INT32)0)->getValue().pUINT;
  mpFlagLimitTime  = assertParameter("LimitTime", CCopasiParameter::BOOL, true)->getValue().pBOOL;
  mpFlagLimitOutCrossings  = assertParameter("LimitOutCrossings", CCopasiParameter::BOOL, false)->getValue().pBOOL;
  mpOutCrossingsLimit  = assertParameter("NumOutCrossingsLimit", CCopasiParameter::UINT, (unsigned C_INT32)0)->getValue().pUINT;
  mpFlagLimitConvergence  = assertParameter("LimitUntilConvergence", CCopasiParameter::BOOL, false)->getValue().pBOOL;
  mpConvergenceTolerance  = assertParameter("ConvergenceTolerance", CCopasiParameter::DOUBLE, (C_FLOAT64)1E-6)->getValue().pDOUBLE;
  mpFlagLimitOutConvergence  = assertParameter("DelayOutputUntilConvergence", CCopasiParameter::BOOL, false)->getValue().pBOOL;
  mpConvergenceOutTolerance  = assertParameter("OutputConvergenceTolerance", CCopasiParameter::DOUBLE, (C_FLOAT64)1E-6)->getValue().pDOUBLE;
  mpTriggerExpression =
    assertParameter("TriggerExpression", CCopasiParameter::EXPRESSION, std::string(""))->getValue().pEXPRESSION;
}

void CCrossSectionProblem::initObjects()
{
  //here we should create things like object references to results of the calculation
}

/**
 *  Destructor.
 */
CCrossSectionProblem::~CCrossSectionProblem()
{DESTRUCTOR_TRACE;}

std::ostream &operator<<(std::ostream &os, const CCrossSectionProblem & o)
{
  os << "Cross Section Problem description: Not implemented yet." << std::endl;
  const CCopasiDataModel* pDataModel = o.getObjectDataModel();
  assert(pDataModel != NULL);

  return os;
}

void CCrossSectionProblem::print(std::ostream * ostream) const
{*ostream << *this;}

bool CCrossSectionProblem::getFlagLimitCrossings() const
{return *mpFlagLimitCrossings;}

const unsigned C_INT32 & CCrossSectionProblem::getCrossingsLimit() const
{return *mpCrossingsLimit;}

bool CCrossSectionProblem::getFlagLimitOutCrossings() const
{return *mpFlagLimitOutCrossings;}

const unsigned C_INT32 & CCrossSectionProblem::getOutCrossingsLimit() const
{return *mpOutCrossingsLimit;}

bool CCrossSectionProblem::getFlagLimitTime() const
{return *mpFlagLimitTime;}

const C_FLOAT64 & CCrossSectionProblem::getTimeLimit() const
{return *mpDuration;}

void CCrossSectionProblem::setFlagLimitCrossings(bool flagLimitCrossing)
{*mpFlagLimitCrossings = flagLimitCrossing;}

void CCrossSectionProblem::setCrossingsLimit(const unsigned C_INT32 &crossingLimit)
{*mpCrossingsLimit = crossingLimit;}

void CCrossSectionProblem::setFlagLimitOutCrossings(bool flagLimitCrossing)
{*mpFlagLimitOutCrossings = flagLimitCrossing;}

void CCrossSectionProblem::setOutCrossingsLimit(const unsigned C_INT32 &crossingLimit)
{*mpOutCrossingsLimit = crossingLimit;}

void CCrossSectionProblem::setFlagLimitTime(bool flagLimitTime)
{*mpFlagLimitTime = flagLimitTime;}

void CCrossSectionProblem::setTimeLimit(const C_FLOAT64 &timeLimit)
{*mpDuration = timeLimit;}

bool CCrossSectionProblem::getFlagLimitConvergence() const
{
  return *mpFlagLimitConvergence;
}

const C_FLOAT64 &CCrossSectionProblem::getConvergenceTolerance() const
{
  return *mpConvergenceTolerance;
}

void CCrossSectionProblem::setFlagLimitConvergence(bool flagLimitConvergence)
{
  *mpFlagLimitConvergence = flagLimitConvergence;
}

void CCrossSectionProblem::setConvergenceTolerance(const C_FLOAT64 &convergenceTolerance)
{
  *mpConvergenceTolerance = convergenceTolerance;
}

bool CCrossSectionProblem::getFlagLimitOutConvergence() const
{
  return *mpFlagLimitOutConvergence;
}

const C_FLOAT64 &CCrossSectionProblem::getConvergenceOutTolerance() const
{
  return *mpConvergenceOutTolerance;
}

void CCrossSectionProblem::setFlagLimitOutConvergence(bool flagLimitConvergence)
{
  *mpFlagLimitOutConvergence = flagLimitConvergence;
}

void CCrossSectionProblem::setConvergenceOutTolerance(const C_FLOAT64 &convergenceTolerance)
{
  *mpConvergenceOutTolerance = convergenceTolerance;
}
