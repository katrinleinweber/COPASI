#include "CReactionInterface.h"

#include "model/CReaction.h"
#include "model/CModel.h"
#include "model/CChemEqElement.h"
#include "utilities/CGlobals.h"
#include "utilities/CMethodParameter.h"
#include "function/CFunctionDB.h"

CReactionInterface::CReactionInterface(){emptyString = "";}
CReactionInterface::~CReactionInterface(){}

std::vector<std::string> CReactionInterface::getListOfMetabs(std::string role) const
  {
    const CCopasiVector<CChemEqElement> * el;
    if (role == "SUBSTRATE") el = &(mChemEq.getSubstrates());
    else if (role == "PRODUCT") el = &(mChemEq.getProducts());
    else if (role == "MODIFIER") el = &(mChemEq.getModifiers());
    else fatalError();

    std::vector<std::string> ret;

    C_INT32 i, imax = el->size();
    for (i = 0; i < imax; ++i)
      ret.push_back((*el)[i]->getMetaboliteName());

    return ret;
  }

std::vector< std::string > CReactionInterface::getListOfPossibleFunctions() const
  {
    TriLogic reversible;
    if (isReversible() == false)
      reversible = TriFalse;
    else
      reversible = TriTrue;

    const CCopasiVector < CFunction > & Functions =
      Copasi->pFunctionDB->suitableFunctions(mChemEq.getSubstrates().size(),
                                             mChemEq.getProducts().size(),
                                             reversible);

    std::vector<std::string> ret;
    ret.clear();
    C_INT32 i, imax = Functions.size();
    for (i = 0; i < imax; ++i)
    {ret.push_back(Functions[i]->getName());}

    return ret;
  }

void CReactionInterface::initFromReaction(const std::string & rn, const CModel & model)
{
  mReactionReferenceName = rn;

  const CReaction *rea;
  rea = model.getReactions()[rn];

  mReactionName = rea->getName();

  mChemEq = rea->getChemEq();

  mpFunction = &(rea->getFunction());
  mParameters = mpFunction->getParameters();

  mNameMap = rea->getParameterMappingName();

  C_INT32 i, imax = size();
  mValues.resize(imax);
  for (i = 0; i < imax; ++i)
    if (getUsage(i) == "PARAMETER") mValues[i] = rea->getParameterValue(getParameterName(i));

  mValid = true; //assume a reaction is valid before editing
}

void CReactionInterface::writeBackToReaction(CModel & model) const
  {
    if (!mValid) return; // do nothing

    if (!(mParameters == mpFunction->getParameters())) return; // do nothing

    CReaction *rea;
    rea = model.getReactions()[mReactionReferenceName];

    rea->setName(mReactionName); //TODO: what else needs to be done here?

    rea->setChemEq(mChemEq.getChemicalEquation());

    // TODO. check if function has changed since it was set in the R.I.
    rea->setFunction(mpFunction->getName());

    C_INT32 j, jmax;
    C_INT32 i, imax = size();

    for (i = 0; i < imax; ++i)
      {
        if (getUsage(i) == "PARAMETER")
        {rea->setParameterValue(getParameterName(i), mValues[i]);}
        else
          {
            if (isVector(i))
              {
                rea->clearParameterMapping(i);
                jmax = mNameMap[i].size();
                for (j = 0; j < jmax; ++j)
                  rea->addParameterMapping(i, mNameMap[i][j]);
              }
            else
              rea->setParameterMapping(i, mNameMap[i][0]);
          }
      }

    rea->compile(model.getCompartments());
  }

void CReactionInterface::setFunction(const std::string & fn, bool force)
{
  //do nothing if the function is the same as before
  if ((getFunctionName() == fn) && (!force)) return;

if (fn == "") {clearFunction(); return;}

  // save the old parameter names
  CFunctionParameters oldParameters = mParameters;

  //get the function
  mpFunction = Copasi->pFunctionDB->findLoadFunction(fn);
  if (!mpFunction) fatalError();
  mParameters = mpFunction->getParameters();

  //initialize mValues[]
  //try to keep old values if the name is the same
  std::vector<C_FLOAT64> oldValues = mValues;
  C_INT32 j, jmax = oldValues.size();
  C_INT32 i, imax = size();
  mValues.resize(imax);
  for (i = 0; i < imax; ++i)
    {
      if (getUsage(i) == "PARAMETER")
        {
          for (j = 0; j < jmax; ++j)
            if (oldParameters[j]->getName() == getParameterName(i)) break;

          if (j == jmax) mValues[i] = 0.1;
          else mValues[i] = oldValues[j];
        }
    }

  //initialize mNameMap[]
  mNameMap.clear();
  mNameMap.resize(imax);
  for (i = 0; i < imax; ++i)
    {
      if (!isVector(i)) mNameMap[i].resize(1);
    }

  //guess initial connections between metabs and function parameters
  mValid = true;
  connectFromScratch("SUBSTRATE", true);
  connectFromScratch("PRODUCT", true);
  connectFromScratch("MODIFIER", false); // we can not be pedantic about modifiers
  // because modifiers are not taken into acount
  // when looking for suitable functions
}

void CReactionInterface::clearFunction()
{
  mpFunction = NULL;
  mParameters.cleanup();
  mValid = false;

  mValues.clear();
  mNameMap.clear();
}

void CReactionInterface::setChemEqString(const std::string & eq)
{
  mChemEq.setChemicalEquation(eq);

  //get list of possible functions and check if the current function is in it.
  std::vector<std::string> fl = getListOfPossibleFunctions();

  C_INT32 i, imax = fl.size();
  for (i = 0; i < imax; ++i)
    if (fl[i] == getFunctionName()) break;

  //if (i<imax) return; // brute force
  if (i == imax) i = 0;

  //change function  TODO : heuristics
  if (imax == 0) setFunction("", true);
  else setFunction(fl[i], true); // brute force
}

void CReactionInterface::setReversibility(bool rev)
{
  //set the new reversibility
  mChemEq.setReversibility(rev);

  //get list of possible functions and check if the current function is in it.
  std::vector<std::string> fl = getListOfPossibleFunctions();

  C_INT32 i, imax = fl.size();
  for (i = 0; i < imax; ++i)
    if (fl[i] == getFunctionName()) break;

  //if (i<imax) return; // brute force

  //change function  TODO : heuristics
  if (imax == 0) setFunction("", true);
  else setFunction(fl[0], true);  //brute force
}

void CReactionInterface::connectFromScratch(std::string role, bool pedantic)
{
  unsigned C_INT32 i, imax = mParameters.getNumberOfParametersByUsage(role);
  if (!imax) return;

  // get the list of chem eq elements
  const CCopasiVector<CChemEqElement> * el;
  if (role == "SUBSTRATE") el = &(mChemEq.getSubstrates());
  else if (role == "PRODUCT") el = &(mChemEq.getProducts());
  else if (role == "MODIFIER") el = &(mChemEq.getModifiers());
  else fatalError();

  // get the first parameter with the respective role
  CFunctionParameter::DataType Type;
  unsigned C_INT32 pos = 0;
  Type = mParameters.getParameterByUsage(role, pos).getType();

  if (Type == CFunctionParameter::VFLOAT64)
    {
      --pos;
      mNameMap[pos].clear();
      imax = el->size();
      for (i = 0; i < imax; ++i) mNameMap[pos].push_back((*el)[i]->getMetaboliteName());
    }
  else if (Type == CFunctionParameter::FLOAT64)
    {
      if ((imax != el->size()) && pedantic) fatalError();

      if (el->size() > 0)
        mNameMap[pos - 1][0] = (*el)[0]->getMetaboliteName();
    else {mNameMap[pos - 1][0] = "unknown"; mValid = false;}

      for (i = 1; i < imax; ++i)
        {
          Type = mParameters.getParameterByUsage(role, pos).getType();
          if (Type != CFunctionParameter::FLOAT64) fatalError();

          if (el->size() > (pos - 1))
            mNameMap[pos - 1][0] = (*el)[i]->getMetaboliteName();
        else {mNameMap[pos - 1][0] = "unknown"; mValid = false;}
        }
    }
  else fatalError();
}

bool CReactionInterface::isLocked(C_INT32 index) const
{return isLocked(getUsage(index));}

bool CReactionInterface::isLocked(std::string usage) const
  {
    // get number of metabs in chemEq
    unsigned C_INT32 listSize;
    if (usage == "PARAMETER") return false;
    else if (usage == "SUBSTRATE") listSize = mChemEq.getSubstrates().size();
    else if (usage == "PRODUCT") listSize = mChemEq.getProducts().size();
    else if (usage == "MODIFIER") listSize = mChemEq.getModifiers().size();

    // get number of parameters
    unsigned C_INT32 paramSize = mParameters.getNumberOfParametersByUsage(usage);

    // get index of first parameter
    unsigned C_INT32 pos = 0;
    mParameters.getParameterByUsage(usage, pos); --pos;

    // decide
    if (isVector(pos))
      {
        if (paramSize != 1) fatalError();
        if (listSize == 0) return true; else return false; //really?
      }
    else
      {
        // we cannot be pedantic about modifiers.
        if ((listSize < paramSize) && (usage != "MODIFIER")) fatalError();
        if (listSize <= 1) return true; else return false;
      }
  }

void CReactionInterface::setMetab(C_INT32 index, std::string mn)
{
  std::string usage = getUsage(index);
  C_INT32 listSize;
  if (usage == "PARAMETER") return;
  else if (usage == "SUBSTRATE") listSize = mChemEq.getSubstrates().size();
  else if (usage == "PRODUCT") listSize = mChemEq.getProducts().size();
  else if (usage == "MODIFIER") listSize = mChemEq.getModifiers().size();

  if (isVector(index)) mNameMap[index].push_back(mn);
  else
    {
      mNameMap[index][0] = mn;

      // if we have two parameters of this usage change the other one.
      if ((listSize == 2) && (mParameters.getNumberOfParametersByUsage(usage) == 2))
        {
          // get index of other parameter
          unsigned C_INT32 pos = 0;
          mParameters.getParameterByUsage(usage, pos);
          if ((pos - 1) == index) mParameters.getParameterByUsage(usage, pos);
          --pos;

          // get name if other metab
          std::vector<std::string> ml = getListOfMetabs(usage);
          std::string otherMetab;
          if (ml[0] == mn) otherMetab = ml[1]; else otherMetab = ml[0];

          // set other parameter
          mNameMap[pos][0] = otherMetab;
        }
    }
}
