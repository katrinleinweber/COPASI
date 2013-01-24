// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2006 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

// adapted by Pedro Mendes, April 2005, from the original Gepasi file
// levmarq.cpp : Restricted step Newton method (Marquardt iteration)

#include <cmath>
#include "copasi.h"

#include "COptMethodLevenbergMarquardt.h"
#include "COptProblem.h"
#include "COptItem.h"
#include "COptTask.h"

#include "parameterFitting/CFitProblem.h"
#include "report/CCopasiObjectReference.h"

#include "clapackwrap.h"
#include "blaswrap.h"

#define LAMBDA_MAX 1e80

COptMethodLevenbergMarquardt::COptMethodLevenbergMarquardt(const CCopasiContainer * pParent):
  COptMethod(CCopasiTask::optimization, CCopasiMethod::LevenbergMarquardt, pParent),
  mIterationLimit(2000),
  mTolerance(1.e-006),
  mModulation(1.e-006),
  mMinModulation(1.e-9),
  mSymmetricDerivatives(false),
  mIteration(0),
  mhIteration(C_INVALID_INDEX),
  mVariableSize(0),
  mCurrent(),
  mBest(),
  mGradient(),
  mStep(),
  mHessian(),
  mHessianLM(),
  mTemp(),
  mBestValue(std::numeric_limits< C_FLOAT64 >::infinity()),
  mEvaluationValue(std::numeric_limits< C_FLOAT64 >::infinity()),
  mContinue(true),
  mHaveResiduals(false),
  mResidualJacobianT()
{
  addParameter("Iteration Limit", CCopasiParameter::UINT, (unsigned C_INT32) 2000);
  addParameter("Tolerance", CCopasiParameter::DOUBLE, (C_FLOAT64) 1.e-006);

#ifdef COPASI_DEBUG
  addParameter("Modulation", CCopasiParameter::DOUBLE, (C_FLOAT64) 1.e-006);
  addParameter("Minimal modulation", CCopasiParameter::DOUBLE, (C_FLOAT64) 1.e-009);
  addParameter("Symmetric derivatives", CCopasiParameter::BOOL, (bool) true);
#endif // COPASI_DEBUG

  initObjects();
}

COptMethodLevenbergMarquardt::COptMethodLevenbergMarquardt(const COptMethodLevenbergMarquardt & src,
    const CCopasiContainer * pParent):
  COptMethod(src, pParent),
  mIterationLimit(src.mIterationLimit),
  mTolerance(src.mTolerance),
  mModulation(src.mModulation),
  mMinModulation(src.mMinModulation),
  mSymmetricDerivatives(src.mSymmetricDerivatives),
  mIteration(0),
  mhIteration(C_INVALID_INDEX),
  mVariableSize(0),
  mCurrent(),
  mBest(),
  mGradient(),
  mStep(),
  mHessian(),
  mHessianLM(),
  mTemp(),
  mBestValue(std::numeric_limits< C_FLOAT64 >::infinity()),
  mEvaluationValue(std::numeric_limits< C_FLOAT64 >::infinity()),
  mContinue(true),
  mHaveResiduals(false),
  mResidualJacobianT()
{initObjects();}

COptMethodLevenbergMarquardt::~COptMethodLevenbergMarquardt()
{cleanup();}

void COptMethodLevenbergMarquardt::initObjects()
{
  addObjectReference("Current Iteration", mIteration, CCopasiObject::ValueInt);

#ifndef COPASI_DEBUG
  removeParameter("Modulation");
  removeParameter("Minimal modulation");
  removeParameter("Symmetric derivatives");
#endif
}

bool COptMethodLevenbergMarquardt::optimise()
{
  if (!initialize())
    {
      if (mpCallBack)
        mpCallBack->finishItem(mhIteration);

      return false;
    }

  C_INT dim, starts, info, nrhs;
  C_INT one = 1;

  size_t i;
  C_FLOAT64 LM_lambda, nu, convp, convx;
  bool calc_hess;
  nrhs = 1;

  dim = (C_INT) mVariableSize;

#ifdef XXXX
  CVector< C_INT > Pivot(dim);
  CVector< C_FLOAT64 > Work(1);
  C_INT LWork = -1;

  //  SUBROUTINE DSYTRF(UPLO, N, A, LDA, IPIV, WORK, LWORK, INFO)
  dsytrf_("L", &dim, mHessianLM.array(), &dim, Pivot.array(),
          Work.array(), &LWork, &info);
  LWork = Work[0];
  Work.resize(LWork);
#endif // XXXX

  // initial point is first guess but we have to make sure that we
  // are within the parameter domain
  for (i = 0; i < mVariableSize; i++)
    {
      const COptItem & OptItem = *(*mpOptItem)[i];

      switch (OptItem.checkConstraint(OptItem.getStartValue()))
        {
          case -1:
            mCurrent[i] = *OptItem.getLowerBoundValue();
            break;

          case 1:
            mCurrent[i] = *OptItem.getUpperBoundValue();
            break;

          case 0:
            mCurrent[i] = OptItem.getStartValue();
            break;
        }

      (*(*mpSetCalculateVariable)[i])(mCurrent[i]);
    }

  // keep the current parameter for later
  mBest = mCurrent;

  evaluate();

  if (!isnan(mEvaluationValue))
    {
      // and store that value
      mBestValue = mEvaluationValue;
      mContinue &= mpOptProblem->setSolution(mBestValue, mBest);

      // We found a new best value lets report it.
      mpParentTask->output(COutputInterface::DURING);
    }

  // Initialize LM_lambda
  LM_lambda = 1.0;
  nu = 2.0;
  calc_hess = true;
  starts = 1;

  for (mIteration = 0; (mIteration < mIterationLimit) && (nu != 0.0) && mContinue; mIteration++)
    {
      // calculate gradient and Hessian
      if (calc_hess) hessian();

      calc_hess = true;

      // mHessianLM will be used for Cholesky decomposition
      mHessianLM = mHessian;

      // add Marquardt lambda to Hessian
      // put -gradient in h
      for (i = 0; i < mVariableSize; i++)
        {
          mHessianLM[i][i] *= 1.0 + LM_lambda; // Improved
          // mHessianLM[i][i] += LM_lambda; // Original
          mStep[i] = - mGradient[i];
        }

      // SUBROUTINE DSYTRF(UPLO, N, A, LDA, IPIV, WORK, LWORK, INFO)
      // dsytrf_("L", &dim, mHessianLM.array(), &dim, Pivot.array(),
      //         Work.array(), &LWork, &info);

      // SUBROUTINE DPOTRF(UPLO, N, A, LDA, INFO)
      // Cholesky factorization
      char UPLO = 'L';
      dpotrf_(&UPLO, &dim, mHessianLM.array(), &dim, &info);

      // if Hessian is positive definite solve Hess * h = -grad
      if (info == 0)
        {
          // SUBROUTINE DPOTRS(UPLO, N, NRHS, A, LDA, B, LDB, INFO)
          dpotrs_(&UPLO, &dim, &one, mHessianLM.array(), &dim, mStep.array(), &dim, &info);

          // SUBROUTINE DSYTRS(UPLO, N, NRHS, A, LDA, IPIV, B, LDB, INFO);
          // dsytrs_("L", &dim, &one, mHessianLM.array(), &dim, Pivot.array(), mStep.array(),
          //         &dim, &info);
        }
      else
        {
          // We are in a concave region. Thus the current step is an over estimation.
          // We reduce it by dividing by lambda
          for (i = 0; i < mVariableSize; i++)
            {
              mStep[i] /= LM_lambda;
            }
        }

//REVIEW:START
// This code is different between Gepasi and COPASI
// Gepasi goes along the direction until it hits the boundary
// COPASI moves in a different direction; this could be a problem

      // Force the parameters to stay within the defined boundaries.
      // Forcing the parameters gives better solution than forcing the steps.
      // It gives same results with Gepasi.
      for (i = 0; i < mVariableSize; i++)
        {
          mCurrent[i] = mBest[i] + mStep[i];

          const COptItem & OptItem = *(*mpOptItem)[i];

          switch (OptItem.checkConstraint(mCurrent[i]))
            {
              case - 1:
                mCurrent[i] = *OptItem.getLowerBoundValue() + std::numeric_limits< C_FLOAT64 >::epsilon();
                break;

              case 1:
                mCurrent[i] = *OptItem.getUpperBoundValue() - std::numeric_limits< C_FLOAT64 >::epsilon();
                break;

              case 0:
                break;
            }
        }

// This is the Gepasi code, which would do the truncation along the search line

      // Assure that the step will stay within the bounds but is
      // in its original direction.
      /* C_FLOAT64 Factor = 1.0;
       while (true)
         {
           convp = 0.0;

           for (i = 0; i < mVariableSize; i++)
             {
               mStep[i] *= Factor;
               mCurrent[i] = mBest[i] + mStep[i];
             }

           Factor = 1.0;

           for (i = 0; i < mVariableSize; i++)
             {
               const COptItem & OptItem = *(*mpOptItem)[i];

               switch (OptItem.checkConstraint(mCurrent[i]))
                 {
                 case - 1:
                   Factor =
                     std::min(Factor, (*OptItem.getLowerBoundValue() - mBest[i]) / mStep[i]);
                   break;

                 case 1:
                   Factor =
                     std::min(Factor, (*OptItem.getUpperBoundValue() - mBest[i]) / mStep[i]);
                   break;

                 case 0:
                   break;
                 }
             }

           if (Factor == 1.0) break;
         }
       */
//REVIEW:END

      // calculate the relative change in each parameter
      for (convp = 0.0, i = 0; i < mVariableSize; i++)
        {
          (*(*mpSetCalculateVariable)[i])(mCurrent[i]);
          convp += fabs((mCurrent[i] - mBest[i]) / mBest[i]);
        }

      // calculate the objective function value
      evaluate();

      // set the convergence check amplitudes
      // convx has the relative change in objective function value
      convx = (mBestValue - mEvaluationValue) / mBestValue;
      // convp has the average relative change in parameter values
      convp /= mVariableSize;

      if (mEvaluationValue < mBestValue)
        {
          // keep this value
          mBestValue = mEvaluationValue;

          // store the new parameter set
          mBest = mCurrent;

          // Inform the problem about the new solution.
          mContinue &= mpOptProblem->setSolution(mBestValue, mBest);

          // We found a new best value lets report it.
          mpParentTask->output(COutputInterface::DURING);

          // decrease LM_lambda
          LM_lambda /= nu;

          if ((convp < mTolerance) && (convx < mTolerance))
            {
              if (starts < 3)
                {
                  // let's restart with lambda=1
                  LM_lambda = 1.0;
                  starts++;
                }
              else
                // signal the end
                nu = 0.0;
            }
        }
      else
        {
          // restore the old parameter values
          mCurrent = mBest;

          for (i = 0; i < mVariableSize; i++)
            (*(*mpSetCalculateVariable)[i])(mCurrent[i]);

          // if lambda too high terminate
          if (LM_lambda > LAMBDA_MAX) nu = 0.0;
          else
            {
              // increase lambda
              LM_lambda *= nu * 2;
              // don't recalculate the Hessian
              calc_hess = false;
              // correct the number of iterations
              mIteration--;
            }
        }

      if (mpCallBack)
        mContinue &= mpCallBack->progressItem(mhIteration);
    }

  if (mpCallBack)
    mpCallBack->finishItem(mhIteration);

  return true;
}

bool COptMethodLevenbergMarquardt::cleanup()
{
  return true;
}

const C_FLOAT64 & COptMethodLevenbergMarquardt::evaluate()
{
  // We do not need to check whether the parametric constraints are fulfilled
  // since the parameters are created within the bounds.

  mContinue &= mpOptProblem->calculate();
  mEvaluationValue = mpOptProblem->getCalculateValue();

  // when we leave either the parameter or functional domain
  // we penalize the objective value by forcing it to be larger
  // than the best value recorded so far.
  if (mEvaluationValue < mBestValue &&
      (!mpOptProblem->checkParametricConstraints() ||
       !mpOptProblem->checkFunctionalConstraints()))
    mEvaluationValue = mBestValue + mBestValue - mEvaluationValue;

  return mEvaluationValue;
}

bool COptMethodLevenbergMarquardt::initialize()
{
  cleanup();

  if (!COptMethod::initialize()) return false;

  mModulation = 0.001;
  mIterationLimit = * getValue("Iteration Limit").pUINT;
  mTolerance = * getValue("Tolerance").pDOUBLE;

#ifdef COPASI_DEBUG
  mModulation = * getValue("Modulation").pDOUBLE;
  mMinModulation = * getValue("Minimal modulation").pDOUBLE;
  mSymmetricDerivatives = *getValue("Symmetric derivatives").pBOOL;
#endif // COPASI_DEBUG

  mIteration = 0;

  if (mpCallBack)
    mhIteration =
      mpCallBack->addItem("Current Iteration",
                          mIteration,
                          & mIterationLimit);

  mVariableSize = mpOptItem->size();

  mCurrent.resize(mVariableSize);
  mBest.resize(mVariableSize);
  mStep.resize(mVariableSize);
  mGradient.resize(mVariableSize);
  mHessian.resize(mVariableSize, mVariableSize);

  mBestValue = std::numeric_limits<C_FLOAT64>::infinity();

  mContinue = true;

  CFitProblem * pFitProblem = dynamic_cast< CFitProblem * >(mpOptProblem);

  if (pFitProblem != NULL)
    // if (false)
    {
      mHaveResiduals = true;
      pFitProblem->setResidualsRequired(true);
      mResidualJacobianT.resize(mVariableSize, pFitProblem->getResiduals().size());
    }
  else
    mHaveResiduals = false;

  return true;
}


C_FLOAT64 one_sided_derivative(const C_FLOAT64 & x, const C_FLOAT64 & modulation, const C_FLOAT64 & min_mod)
{
  if (fabs(modulation*x) > min_mod )
  {
    return x*(1.0+modulation);
  }
  else
  {
    if (x>=0)
      return x+min_mod;
    else
      return x-min_mod;
  }
    
}

void two_sided_derivative(const C_FLOAT64 & x, const C_FLOAT64 & modulation, const C_FLOAT64 & min_mod,
                               C_FLOAT64 & x1, C_FLOAT64 & x2)
{
  if (fabs(2.0*modulation*x) > min_mod )
  {
    x1 = x*(1.0+modulation);
    x2 = x*(1.0-modulation);
  }
  else if (2.0*fabs(x) > min_mod)
  {
    x1 = x + 0.5*min_mod;
    x2 = x - 0.5*min_mod;
  }
  else if (x>=0.0)
  {
    x1 = min_mod;
    x2 = 0.0;
  }
  else
  {
    x1 = 0.0;
    x2 = -min_mod;
  }
}

// evaluate the value of the gradient by forward finite differences
void COptMethodLevenbergMarquardt::gradient()
{
  size_t i;
  
  C_FLOAT64 y;
  C_FLOAT64 x1;
  C_FLOAT64 x2;
  C_FLOAT64 inverse_delta;
  C_FLOAT64 x_orig;
  
  //C_FLOAT64 mod1;
  
  //mod1 = 1.0 + mModulation;
  
  if (!mSymmetricDerivatives)
    y = evaluate();
  
  for (i = 0; i < mVariableSize && mContinue; i++)
  {
    x_orig = mCurrent[i]; //store original parameter value
    
    if (mSymmetricDerivatives)
    {
      two_sided_derivative(mCurrent[i], mModulation, mMinModulation, x1, x2);
    }
    else
    {
      x1=mCurrent[i];
      x2=one_sided_derivative(mCurrent[i], mModulation, mMinModulation);
    }
    inverse_delta = 1.0/(x2-x1);

    if (mSymmetricDerivatives)
    {
      (*(*mpSetCalculateVariable)[i])(x1);
      y=evaluate();
    }

    (*(*mpSetCalculateVariable)[i])(x2);
    mGradient[i] = (evaluate() - y)*inverse_delta;	

    (*(*mpSetCalculateVariable)[i])(x_orig); //restore original parameter value
  }
}


//evaluate the Hessian
void COptMethodLevenbergMarquardt::hessian()
{
  size_t i, j;

  if (mHaveResiduals)
    {
      const CVector< C_FLOAT64 > & Residuals = static_cast<CFitProblem *>(mpOptProblem)->getResiduals();
      size_t ResidualSize = Residuals.size();

      CVector< C_FLOAT64 > Residuals1;
      CVector< C_FLOAT64 > Original_Residuals;
      
      C_FLOAT64 * pJacobianT = mResidualJacobianT.array();
      
      evaluate();  //Are there situations where this has not already been done?
      Original_Residuals = Residuals;
      Residuals1 = Residuals;
      //in any case we need the original residuals. The question is whether we can assume
      //that they are up-to-date when this method is called.
      
      const C_FLOAT64 * pResiduals1;
      const C_FLOAT64 * pEnd = Residuals1.array() + ResidualSize;
      const C_FLOAT64 * pResiduals;

      C_FLOAT64 inverse_delta;
      C_FLOAT64 original_x;
      C_FLOAT64 upper_x;
      C_FLOAT64 lower_x; //upper_x is not necessarily larger than lower_x

      for (i = 0; i < mVariableSize && mContinue; i++)
        {
          original_x=mCurrent[i]; //store original parameter value

          if (mSymmetricDerivatives)
            two_sided_derivative(original_x, mModulation, mMinModulation, upper_x, lower_x);
          else
          {
            upper_x = one_sided_derivative(original_x, mModulation, mMinModulation);
            lower_x = original_x;
          }
          
          inverse_delta = 1.0/(upper_x-lower_x);
          
          if (mSymmetricDerivatives)
          {
            (*(*mpSetCalculateVariable)[i])(lower_x); //change the parameter
            evaluate();
            Residuals1 = Residuals;
          }
          
          (*(*mpSetCalculateVariable)[i])(upper_x); //change the parameter
          evaluate();
          
          // evaluate another column of the Jacobian
          pResiduals1 = Residuals1.array();
          pResiduals = Residuals.array();

          for (; pResiduals1 != pEnd; pResiduals1++, pResiduals++, pJacobianT++)
            *pJacobianT = (*pResiduals - *pResiduals1) * inverse_delta;

          (*(*mpSetCalculateVariable)[i])(original_x); //restore parameter value
        }

      // calculate the gradient
      C_FLOAT64 * pGradient = mGradient.array();
      pJacobianT = mResidualJacobianT.array();

      pEnd = Original_Residuals.array()+ResidualSize;
      for (i = 0; i < mVariableSize; i++, pGradient++)
        {
          *pGradient = 0.0;

          for (pResiduals = Original_Residuals.array(); 
               pResiduals != pEnd;
               pResiduals++, pJacobianT++)
            *pGradient += *pJacobianT * *pResiduals;

          // This is formally correct but cancels out with factor 2 below
          // *pGradient *= 2.0;
        }

      // calculate the Hessian
      C_FLOAT64 * pHessian;
      C_FLOAT64 * pJacobian;

      for (i = 0; i < mVariableSize; i++)
        {
          pHessian = mHessian[i];

          for (j = 0; j <= i; j++, pHessian++)
            {
              *pHessian = 0.0;
              pJacobianT = mResidualJacobianT[i];
              pEnd = pJacobianT + ResidualSize;
              pJacobian = mResidualJacobianT[j];

              for (; pJacobianT != pEnd; pJacobianT++, pJacobian++)
                *pHessian += *pJacobianT * *pJacobian;

              // This is formally correct but cancels out with factor 2 above
              // *pHessian *= 2.0;
            }
        }
    }
  else //without using the residuals
    {
      C_FLOAT64 inverse_delta;
      C_FLOAT64 original_x;
      C_FLOAT64 upper_x;
      
      // calculate the gradient
      gradient();

      // and store it
      mTemp = mGradient;

      // calculate rows of the Hessian
      for (i = 0; i < mVariableSize; i++)
        {
          
          original_x=mCurrent[i]; //store original parameter value
          upper_x = one_sided_derivative(original_x, mModulation, mMinModulation);
          
          inverse_delta = 1.0/(upper_x-original_x);
          (*(*mpSetCalculateVariable)[i])(mCurrent[i]); //change the parameter
          gradient();

          for (j = 0; j <= i; j++)
            mHessian[i][j] = (mGradient[j] - mTemp[j]) * inverse_delta;

          // restore the original parameter value
          mCurrent[i] = original_x;
          (*(*mpSetCalculateVariable)[i])(original_x);
        }

      // restore the gradient
      mGradient = mTemp;
    }

  // make the matrix symmetric
  // not really necessary
  for (i = 0; i < mVariableSize; i++)
    for (j = i + 1; j < mVariableSize; j++)
      mHessian[i][j] = mHessian[j][i];
}
