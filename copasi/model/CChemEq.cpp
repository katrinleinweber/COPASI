// CChemEqElement
//
// A class describing an element of a chemical equation
// (C) Stefan Hoops 2001
//

#define  COPASI_TRACE_CONSTRUCTION

#include "copasi.h"
#include "CChemEq.h"
#include "utilities/utilities.h"

CChemEq::CChemEq()
{
  CONSTRUCTOR_TRACE;
  mChemicalEquation.erase();
  mChemicalEquationConverted.erase();
}

CChemEq::CChemEq(const CChemEq & src)
{
  CONSTRUCTOR_TRACE;
  mChemicalEquation = src.mChemicalEquation;
  mChemicalEquationConverted = src.mChemicalEquationConverted;
  mSubstrates = src.mSubstrates;
  mProducts = src.mProducts;
  mBalances = src.mBalances;
}
CChemEq::~CChemEq(){cleanup(); DESTRUCTOR_TRACE;}

void CChemEq::cleanup()
{
  mSubstrates.cleanup();
  mProducts.cleanup();
  mBalances.cleanup();
}

void CChemEq::compile(CCopasiVectorN < CCompartment > & compartments)
{
  compileChemEqElements(mSubstrates, compartments);
  compileChemEqElements(mProducts, compartments);
  compileChemEqElements(mBalances, compartments);
}

bool CChemEq::setChemicalEquation(const std::string & chemicalEquation)
{
  std::string Substrates, Products;
  bool reversible;

  cleanup();
  mChemicalEquation = chemicalEquation;

  reversible = splitChemEq(Substrates, Products);

  setChemEqElements(mSubstrates, Substrates);

  setChemEqElements(mProducts, Products);

  setChemEqElements(mBalances, Substrates, CChemEq::SUBSTRATE);
  setChemEqElements(mBalances, Products);

  writeChemicalEquation();
  writeChemicalEquationConverted();
  return reversible;
}

const std::string & CChemEq::getChemicalEquation() const
{
  return mChemicalEquation;
}

const std::string & CChemEq::getChemicalEquationConverted() const
{
  return mChemicalEquationConverted;
}

const CCopasiVector < CChemEqElement > & CChemEq::getSubstrates()
{
  return mSubstrates;
}

const CCopasiVector < CChemEqElement > & CChemEq::getProducts()
{
  return mProducts;
}

const CCopasiVector < CChemEqElement > & CChemEq::getBalances()
{
  return mBalances;
}

CChemEqElement CChemEq::extractElement(const std::string & input,
                                       std::string::size_type & pos) const
{
  CChemEqElement Element;
  std::string Value;

  std::string::size_type Start = input.find_first_not_of(" ", pos);
  std::string::size_type End = input.find(" + ", Start);
  std::string::size_type Multiplier = input.find("*", Start);
  std::string::size_type NameStart;
  std::string::size_type NameEnd;

  if (Multiplier == std::string::npos || Multiplier > End)
  {
    NameStart = Start;
    Element.setMultiplicity(1.0);
    }
  else
    {
      NameStart = input.find_first_not_of(" ", Multiplier + 1);
      Value = input.substr(Start, Multiplier - Start);
      Element.setMultiplicity(atof(Value.c_str()));
    }

  NameEnd = input.find_first_of(" ", NameStart);

  if (NameStart != std::string::npos)
  Element.setMetaboliteName(input.substr(NameStart, NameEnd - NameStart));
  else
    Element.setMetaboliteName("");

    pos = (End == std::string::npos) ? End : End + 3;

    return Element;
  }

  void CChemEq::addElement(CCopasiVector < CChemEqElement > & structure,
                           const CChemEqElement & element,
                           CChemEq::MetaboliteRole role)
    {
      unsigned C_INT32 i;

      std::string Name = element.getMetaboliteName();

      if (Name == "")
        return; // don�t add empty element

      for (i = 0; i < structure.size(); i++)
        if (Name == structure[i]->getMetaboliteName())
          break;

      if (i >= structure.size())
        {
          CChemEqElement * Element = new CChemEqElement(element);

          if (role == CChemEq::SUBSTRATE)
            Element->setMultiplicity(- Element->getMultiplicity());

          structure.add(Element);
        }
      else if (role == CChemEq::SUBSTRATE)
        structure[i]->addToMultiplicity(- element.getMultiplicity());
      else
        structure[i]->addToMultiplicity(element.getMultiplicity());
    }

void CChemEq::setChemEqElements(CCopasiVector < CChemEqElement >
                                & elements,
                                const std::string & reaction,
                                CChemEq::MetaboliteRole role)
{
  std::string::size_type pos = 0;

  while (pos != std::string::npos)
    addElement(elements, extractElement(reaction, pos), role);
}

#ifdef XXXX
void CChemEq::cleanupChemEqElements(std::vector < CChemEqElement * > & elements)
{
  for (unsigned C_INT32 i = 0; i < elements.size(); i++)
    free(elements[i]);

  elements.clear();
}

#endif // XXXX

bool CChemEq::splitChemEq(std::string & left, std::string & right) const
{
  std::string::size_type equal = std::string::npos;
  bool reversibility;
  std::string Separator[] = {"->", "=", ""};
  unsigned C_INT32 i = 0;

  while (*Separator != "" && equal == std::string::npos)
  equal = mChemicalEquation.find(Separator[i++]);

  if (equal == std::string::npos)
    fatalError();

    if (mChemicalEquation.substr(equal, 1) == "=")
      reversibility = true;
      else
        reversibility = false;
        right = mChemicalEquation.substr(equal + (Separator[--i].length()));

        left = mChemicalEquation.substr(0, equal);

        return reversibility;
      }

      void CChemEq::compileChemEqElements(CCopasiVector < CChemEqElement > & elements,
                                          CCopasiVectorN < CCompartment > & compartments)
        {
          unsigned C_INT32 i, imax = elements.size();

          for (i = 0; i < imax; i++)
            elements[i]->compile(compartments);
        }

void CChemEq::writeChemicalEquation()
{
  std::string::size_type equal = std::string::npos;
  std::string Separator[] = {"->", "=>", "=", ""};
  unsigned C_INT32 i = 0, j;

  while (Separator[i] != "" && equal == std::string::npos)
    equal = mChemicalEquation.find(Separator[i++]);

  if (equal == std::string::npos)
    fatalError();

  mChemicalEquation.erase();

  for (j = 0; j < mSubstrates.size(); j++)
    {
      if (j)
        mChemicalEquation += " + ";

      mChemicalEquation += mSubstrates[j]->writeElement();
    }

  mChemicalEquation += " " + Separator[--i] + " ";

  for (j = 0; j < mProducts.size(); j++)
    {
      if (j)
        mChemicalEquation += " + ";

      mChemicalEquation += mProducts[j]->writeElement();
    }
}

void CChemEq::writeChemicalEquationConverted()
{
  std::string::size_type equal = std::string::npos;
  std::string Separator[] = {"->", "=>", "=", ""};
  unsigned C_INT32 i = 0, j, k, kmax;

  while (Separator[i] != "" && equal == std::string::npos)
    equal = mChemicalEquation.find(Separator[i++]);

  if (equal == std::string::npos)
    fatalError();

  mChemicalEquationConverted.erase();

  for (j = 0; j < mSubstrates.size(); j++)
    {
      if (j)
        mChemicalEquationConverted += " + ";

      kmax = (unsigned C_INT32) mSubstrates[j]->getMultiplicity();

      for (k = 0; k < kmax; k++)
        {
          if (k)
            mChemicalEquationConverted += " + ";

          mChemicalEquationConverted += mSubstrates[j]->getMetaboliteName();
        }
    }

  mChemicalEquationConverted += " " + Separator[--i] + " ";

  for (j = 0; j < mProducts.size(); j++)
    {
      if (j)
        mChemicalEquationConverted += " + ";

      kmax = (unsigned C_INT32) mProducts[j]->getMultiplicity();

      for (k = 0; k < kmax; k++)
        {
          if (k)
            mChemicalEquationConverted += " + ";

          mChemicalEquationConverted += mProducts[j]->getMetaboliteName();
        }
    }
}

bool CChemEq::initialized()
{
  return !mChemicalEquation.empty();
}
