/**
 *  CEFMAlgorithm class.
 *  Used to calculate elementary flux modes
 *
 *  Created for Copasi by Stefan Hoops 2002-05-08
 * (C) Stefan Hoops 2002
 */

#include "copasi.h"
#include "CEFMAlgorithm.h"
#include "CFluxMode.h"

CEFMAlgorithm::CEFMAlgorithm()
{
  mCurrentTableau = NULL;
  mNextTableau = NULL;
}
  
CEFMAlgorithm::~CEFMAlgorithm()
{
  pdelete(mCurrentTableau);
  pdelete(mNextTableau);
}


bool CEFMAlgorithm::calculate(const vector < vector < C_FLOAT64 > > & stoi,
                              const unsigned C_INT32 &reversibleNumber,
                              vector < CFluxMode > & fluxModes)
{
  bool Success = TRUE;
  unsigned C_INT32 Step, MaxSteps = stoi[0].size();
  
  /* initialize the current tableu matrix */
  mCurrentTableau = new CTableauMatrix(stoi, reversibleNumber);
  
  /* Do the iteration */
  for (Step = 0; Step < MaxSteps; Step++) calculateNextTableau(Step);

  /* Build the elementary flux modes to be returned */
  buildFluxModes(fluxModes,stoi.size());

  /* Delete the current / final tableu matrix */
  pdelete(mCurrentTableau);
  
  return Success;
}

void CEFMAlgorithm::calculateNextTableau(const unsigned C_INT32 & step)
{
  list < const CTableauLine * >::iterator a;
  list < const CTableauLine * >::iterator b;
  C_FLOAT64 ma, mb;
  
  mNextTableau = new CTableauMatrix();
  
  /* Move all lines with zeros in the step column to the new tableu matrix */
  /* and remove them from the current tableau matrix */
  a = mCurrentTableau->getFirst();
  
  while (a != mCurrentTableau->getEnd())
    if ((*a)->getReaction(step) == 0.0)
      {
        /* We have to make sure that "a" points to the next element in the */
        /* list after the removal of itself */
        if (a == mCurrentTableau->getFirst())
          {
            mNextTableau->addLine(*a);
            mCurrentTableau->removeLine(a);
            a = mCurrentTableau->getFirst();
          }
        else
          {
            /* We have to remember the previous element so that a++ points to */
            /* past the removed one */
            b = a--;
            mNextTableau->addLine(*b);
            mCurrentTableau->removeLine(b);
            a++;
          }
      }
    else
      a++;
  
  /* Now we create all linear combinations of the remaining lines in the */
  /* current tableau */
  a = mCurrentTableau->getFirst();
  while (a != mCurrentTableau->getEnd())
    {
      b = a;
      b++;
      
      while (b != mCurrentTableau->getEnd())
        {
          mb = (*a)->getReaction(step);

          /* We make sure that "mb" is positive */
          if (mb < 0.0)
            {
              mb *= -1;
              ma = (*b)->getReaction(step);
            }
          else
            ma = - (*b)->getReaction(step);
              
          /* The multiplier "ma" for irreversible reactions must be positive */
          if ((*a)->isReversible() || ma > 0.0)
            mNextTableau->addLine(new CTableauLine(ma, **a, mb, **b));
          
          b++;
        }
      a++;
    }
  
  /* Assigne the next tableau to the current tableau and cleanup */
  pdelete(mCurrentTableau);
  mCurrentTableau = mNextTableau;

  mNextTableau = NULL;
}

void CEFMAlgorithm::buildFluxModes(vector < CFluxMode > & fluxModes,
                                   const unsigned C_INT32 reactionNumber)
{
  fluxModes.clear();

  list < const CTableauLine * >::iterator a = mCurrentTableau->getFirst();
  list < const CTableauLine * >::iterator end = mCurrentTableau->getEnd();

  while (a != end)
    {
      fluxModes.push_back(CFluxMode(*a));
      a++;
    }
}

