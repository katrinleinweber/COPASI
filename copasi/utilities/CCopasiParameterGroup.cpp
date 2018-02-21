// Copyright (C) 2017 - 2018 by Pedro Mendes, Virginia Tech Intellectual
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

// Copyright (C) 2003 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/**
 *  CCopasiParameterGroup class.
 *  This class is used to describe parameters. This class is intended
 *  to be used with integration or optimization methods or reactions.
 *
 *  Created for COPASI by Stefan Hoops 2002
 */

#include <sstream>

#include "copasi/copasi.h"

#include "copasi/utilities/CCopasiParameterGroup.h"
#include "copasi/utilities/CCopasiMessage.h"
#include "copasi/utilities/utility.h"

#include "copasi/undo/CData.h"
#include "copasi/undo/CUndoData.h"

CCopasiParameterGroup::CCopasiParameterGroup():
  CCopasiParameter("NoName", CCopasiParameter::Type::GROUP),
  mpElementTemplates(NULL)
{}

CCopasiParameterGroup::CCopasiParameterGroup(const CCopasiParameterGroup & src,
    const CDataContainer * pParent):
  CCopasiParameter(src, pParent),
  mpElementTemplates(src.mpElementTemplates != NULL ? new CCopasiParameterGroup(*src.mpElementTemplates, this) : NULL)
{
  operator=(src);
}

CCopasiParameterGroup::CCopasiParameterGroup(const std::string & name,
    const CDataContainer * pParent,
    const std::string & objectType):
  CCopasiParameter(name, CCopasiParameter::Type::GROUP, NULL, pParent, objectType),
  mpElementTemplates(NULL)
{}

CCopasiParameterGroup::~CCopasiParameterGroup()
{}

// virtual
CData CCopasiParameterGroup::toData() const
{
  CData Data = CCopasiParameter::toData();

  elements::const_iterator it = static_cast< elements * >(mpValue)->begin();
  elements::const_iterator end = static_cast< elements * >(mpValue)->end();
  std::vector< CData > Value;

  for (; it != end; ++it)
    {
      Value.push_back((*it)->toData());
    }

  Data.addProperty(CData::PARAMETER_VALUE, Value);

  return Data;
}

// virtual
bool CCopasiParameterGroup::applyData(const CData & data, CUndoData::ChangeSet & changes)
{
  bool success = CCopasiParameter::applyData(data, changes);

  // This only inserts new parameters modification of existing parameters or their deletion
  // is handled by accessing them directly.
  if (data.isSetProperty(CData::PARAMETER_VALUE))
    {
      const std::vector< CData > & Value = data.getProperty(CData::PARAMETER_VALUE).toDataVector();

      std::vector< CData >::const_iterator it = Value.begin();
      std::vector< CData >::const_iterator end = Value.end();

      for (; it != end; ++it)
        {
          CDataObject * pObject;
          size_t Index = it->getProperty(CData::OBJECT_INDEX).toSizeT();

          if (Index < size())
            {
              pObject = getParameter(Index);
            }

          if (pObject == NULL)
            {
              pObject = const_cast< CDataObject * >(DataObject(getObject(it->getProperty(CData::OBJECT_TYPE).toString() + "=" + it->getProperty(CData::OBJECT_NAME).toString())));
            }

          if (pObject == NULL)
            {
              pObject = dynamic_cast< CCopasiParameter * >(insert(*it));
            }

          if (pObject != NULL)
            {
              success &= pObject->applyData(*it, changes);
            }
          else
            {
              success = false;
            }
        }
    }

  return success;
}

// virtual
void CCopasiParameterGroup::createUndoData(CUndoData & undoData,
    const CUndoData::Type & type,
    const CData & oldData,
    const CCore::Framework & framework) const
{
  CCopasiParameter::createUndoData(undoData, type, oldData, framework);

  if (type != CUndoData::Type::CHANGE)
    {
      return;
    }

  std::cout << undoData << std::endl;

  // We need to remove all old parameter, modify existing parameters, and finally insert new.
  const std::vector< CData > & OldValueData = oldData.getProperty(CData::PARAMETER_VALUE).toDataVector();

  std::vector< CData >::const_iterator itOldData = OldValueData.begin();
  std::vector< CData >::const_iterator endOldData = OldValueData.end();

  std::multimap< std::string, const CData * > OldValueMap;

  for (; itOldData != endOldData; ++itOldData)
    {
      OldValueMap.insert(std::make_pair(itOldData->getProperty(CData::OBJECT_NAME).toString(), &*itOldData));
    }

  std::multimap< std::string, const CData * >::const_iterator itOLD = OldValueMap.begin();
  std::multimap< std::string, const CData * >::const_iterator endOLD = OldValueMap.end();

  const_name_iterator itNEW(getObjects().begin());
  const_name_iterator endNEW(getObjects().end());

  std::map< size_t, const CData * > ToBeRemoved;
  std::map< size_t, const CCopasiParameter * > ToBeAdded;

  std::vector< std::pair< const CData *, const CCopasiParameter * > > ChangedParameter;

  while (itOLD != endOLD && itNEW != endNEW)
    {
      const std::string & NameNEW = itNEW->getObjectName();
      const std::string & NameOLD = itOLD->first;

      // The OLD parameter is missing in the NEW thus we need to remove it
      if (NameOLD < NameNEW)
        {
          ToBeRemoved.insert(std::make_pair(itOLD->second->getProperty(CData::OBJECT_INDEX).toSizeT(), itOLD->second));
          ++itOLD;
          continue;
        }

      // The NEW parameter is missing in the OLD thus we need to insert it
      if (NameOLD > NameNEW)
        {
          ToBeAdded.insert(std::make_pair(getIndex(*itNEW), *itNEW));
          ++itNEW;
          continue;
        }

      // The names are equal it suffices to use the assignment operator of the parameter
      // We need to post process the PARAMETER_INDEX of the recorded changes
      ChangedParameter.push_back(std::make_pair(itOLD->second, *itNEW));

      ++itNEW;
      ++itOLD;
    }

  // All remaining NEW parameters need to be added
  while (itNEW != endNEW)
    {
      ToBeAdded.insert(std::make_pair(getIndex(*itNEW), *itNEW));

      ++itNEW;
    }

  // All remaining OLD parameter need to be removed
  while (itOLD != endOLD)
    {
      // We only assign parameters
      ToBeRemoved.insert(std::make_pair(itOLD->second->getProperty(CData::OBJECT_INDEX).toSizeT(), itOLD->second));

      ++itOLD;
    }

  // We remove the parameters
  std::map< size_t, const CData * >::const_reverse_iterator itToBeRemoved = ToBeRemoved.rbegin();
  std::map< size_t, const CData * >::const_reverse_iterator endToBeRemoved = ToBeRemoved.rend();

  for (; itToBeRemoved != endToBeRemoved; ++itToBeRemoved)
    {
      undoData.addPreProcessData(CUndoData(CUndoData::Type::REMOVE, *itToBeRemoved->second));
    }

  // We add the missing parameters
  CCopasiParameter * pParameter;
  std::map< size_t, const CCopasiParameter * >::const_iterator itToBeAdded = ToBeAdded.begin();
  std::map< size_t, const CCopasiParameter * >::const_iterator endToBeAdded = ToBeAdded.end();

  for (; itToBeAdded != endToBeAdded; ++itToBeAdded)
    {
      undoData.addPostProcessData(CUndoData(CUndoData::Type::INSERT, itToBeAdded->second));
    }

  // Correct the index of the changed data. Each old index needs to be reduced by the number of deleted items before
  // and each new index needs to be reduced by the number of of deleted items before.

  if (!ChangedParameter.empty())
    {
      std::vector< std::pair< const CData *, const CCopasiParameter * > >::const_iterator itChanged = ChangedParameter.begin();
      std::vector< std::pair< const CData *, const CCopasiParameter * > >::const_iterator endChanged = ChangedParameter.end();

      std::vector< CData > OldValues(ChangedParameter.size());
      std::vector< CData > NewValues(ChangedParameter.size());

      for (; itChanged != endChanged; ++itChanged)
        {
          size_t CurrentIndex = itChanged->first->getProperty(CData::OBJECT_INDEX).toSizeT();
          size_t CorrectedIndex = CurrentIndex;

          std::map< size_t, const CData * >::const_iterator itToBeRemoved = ToBeRemoved.begin();
          std::map< size_t, const CData * >::const_iterator endToBeRemoved = ToBeRemoved.end();

          for (; itToBeRemoved != endToBeRemoved && CurrentIndex < itToBeRemoved->first; ++itToBeRemoved)
            {
              CorrectedIndex--;
            }

          OldValues[CorrectedIndex] = *itChanged->first;
          OldValues[CorrectedIndex].addProperty(CData::OBJECT_INDEX, CorrectedIndex);

          CurrentIndex = getIndex(itChanged->second);
          CorrectedIndex = CurrentIndex;

          for (itToBeAdded = ToBeAdded.begin(); itToBeAdded != endToBeAdded && CurrentIndex < itToBeAdded->first; ++itToBeAdded)
            {
              CorrectedIndex--;
            }

          NewValues[CorrectedIndex] = itChanged->second->toData();
          NewValues[CorrectedIndex].addProperty(CData::OBJECT_INDEX, CorrectedIndex);
        }

      undoData.addProperty(CData::PARAMETER_VALUE, OldValues, NewValues);
    }

  return;
}

// virtual
CUndoObjectInterface * CCopasiParameterGroup::insert(const CData & data)
{
  CCopasiParameter * pNew = CCopasiParameter::fromData(data, this);

  elements * pElements = static_cast< elements * >(mpValue);
  size_t Index = data.getProperty(CData::OBJECT_INDEX).toSizeT();
  pElements->insert(pElements->begin() + std::min(Index, static_cast< elements * >(mpValue)->size()), pNew);

  CCopasiParameter::add(pNew, true);

  return pNew;
}

// virtual
const CObjectInterface * CCopasiParameterGroup::getObject(const CCommonName & cn) const
{
  const CObjectInterface * pObjectInterface = CDataContainer::getObject(cn);

  if (pObjectInterface != NULL)
    {
      return pObjectInterface;
    }

  std::string UniqueName = cn.getObjectName();

  std::string::size_type pos = UniqueName.find_last_of('[');
  std::string Name = UniqueName.substr(0, pos);
  size_t Index = strToUnsignedInt(UniqueName.substr(pos + 1).c_str());
  size_t counter = C_INVALID_INDEX;

  index_iterator it = beginIndex();
  index_iterator end = endIndex();

  for (; it != end; ++it)
    {
      if ((*it)->getObjectName() == Name)
        {
          counter++;

          if (counter == Index)
            {
              return (*it)->getObject(cn.getRemainder());
            }
        }
    }

  return NULL;
}

bool CCopasiParameterGroup::elevateChildren() {return true;}

CCopasiParameterGroup & CCopasiParameterGroup::operator = (const CCopasiParameterGroup & rhs)
{
  // Check for assignment to itself
  if (this == &rhs) return *this;

  if (getObjectName() != rhs.getObjectName())
    setObjectName(rhs.getObjectName());

  mUserInterfaceFlag = rhs.mUserInterfaceFlag;
  mValidity = rhs.mValidity;

  const_name_iterator itRHS(rhs.getObjects().begin());
  const_name_iterator endRHS(rhs.getObjects().end());

  name_iterator itLHS(getObjects().begin());
  name_iterator endLHS(getObjects().end());

  std::vector< CCopasiParameter * > ToBeRemoved;
  std::map< size_t,  CCopasiParameter * > ToBeAdded;

  CCopasiParameter * pLHS;
  CCopasiParameter * pRHS;

  while (itRHS != endRHS && itLHS != endLHS)
    {
      // We only assign parameters
      if ((pRHS = dynamic_cast< CCopasiParameter * >(*itRHS)) == NULL)
        {
          ++itRHS;
          continue;
        }

      // We only assign parameters
      if ((pLHS = dynamic_cast< CCopasiParameter * >(*itLHS)) == NULL)
        {
          ++itLHS;
          continue;
        }

      const std::string & NameLHS = pLHS->getObjectName();
      const std::string & NameRHS = pRHS->getObjectName();

      // The LHS parameter is missing on the RHS thus we need to remove it
      if (NameLHS < NameRHS)
        {
          ToBeRemoved.push_back(pLHS);
          ++itLHS;
          continue;
        }

      // The RHS parameter is missing on the LHS thus we need to add it
      if (NameLHS > NameRHS)
        {
          ToBeAdded.insert(std::make_pair(rhs.getIndex(pRHS), pRHS));
          ++itRHS;
          continue;
        }

      // The names are equal it suffices to use the assignment operator of the parameter
      *pLHS = *pRHS;
      ++itLHS;
      ++itRHS;
    }

  // All remaining parameters of the LHS need to be removed
  while (itLHS != endLHS)
    {
      // We only assign parameters
      if ((pLHS = dynamic_cast< CCopasiParameter * >(*itLHS)) != NULL)
        ToBeRemoved.push_back(pLHS);

      ++itLHS;
    }

  // All remaining parameter of the RHS need to be added
  while (itRHS != endRHS)
    {
      // We only assign parameters
      if ((pRHS = dynamic_cast< CCopasiParameter * >(*itRHS)) != NULL)
        ToBeAdded.insert(std::make_pair(rhs.getIndex(pRHS), pRHS));

      ++itRHS;
    }

  // We remove the parameters
  std::vector< CCopasiParameter * >::const_iterator itToBeRemoved = ToBeRemoved.begin();
  std::vector< CCopasiParameter * >::const_iterator endToBeRemoved = ToBeRemoved.end();

  for (; itToBeRemoved != endToBeRemoved; ++itToBeRemoved)
    this->removeParameter(*itToBeRemoved);

  // We add the missing parameters
  CCopasiParameter * pParameter;
  std::map< size_t,  CCopasiParameter * >::const_iterator itToBeAdded = ToBeAdded.begin();
  std::map< size_t,  CCopasiParameter * >::const_iterator endToBeAdded = ToBeAdded.end();

  for (; itToBeAdded != endToBeAdded; ++itToBeAdded)
    {
      if (itToBeAdded->second->getType() == CCopasiParameter::Type::GROUP)
        pParameter = new CCopasiParameterGroup(* static_cast< CCopasiParameterGroup * >(itToBeAdded->second), NO_PARENT);
      else
        pParameter = new CCopasiParameter(*itToBeAdded->second, NO_PARENT);

      CCopasiParameter::add(pParameter, true);

      // Add the parameter to the proper location
      elements * pElements = static_cast< elements * >(mpValue);
      pElements->insert(pElements->begin() + std::min(itToBeAdded->first, pElements->size()), pParameter);
    }

  return *this;
}

void CCopasiParameterGroup::print(std::ostream * ostream) const
{*ostream << *this;}

std::ostream &operator<<(std::ostream &os, const CCopasiParameterGroup & o)
{
  os << "<<< Parameter Group: " << o.getObjectName() << std::endl;

  CCopasiParameterGroup::elements::const_iterator it = o.beginIndex();
  CCopasiParameterGroup::elements::const_iterator end = o.endIndex();

  for (; it != end; ++it)
    {
      (*it)->print(&os);
      os << std::endl;
    }

  os << ">>> Parameter Group: " << o.getObjectName() << std::endl;
  return os;
}

bool operator==(const CCopasiParameterGroup & lhs,
                const CCopasiParameterGroup & rhs)
{
  if (lhs.getObjectName() != rhs.getObjectName()) return false;

  if (lhs.size() != rhs.size()) return false;

  CCopasiParameterGroup::elements::const_iterator itLhs = lhs.beginIndex();
  CCopasiParameterGroup::elements::const_iterator endLhs = lhs.endIndex();
  CCopasiParameterGroup::elements::const_iterator itRhs = rhs.beginIndex();

  for (; itLhs != endLhs; ++itLhs, ++itRhs)
    if (!(**itLhs == **itRhs)) return false;

  return true;
}

bool CCopasiParameterGroup::addParameter(const CCopasiParameter & parameter)
{
  if (parameter.getType() == CCopasiParameter::Type::GROUP)
    {
      CCopasiParameterGroup * pGroup =
        new CCopasiParameterGroup(*dynamic_cast<const CCopasiParameterGroup *>(&parameter), NO_PARENT);
      addParameter(pGroup);
    }
  else
    {
      CCopasiParameter * pParameter = new CCopasiParameter(parameter, NO_PARENT);
      addParameter(pParameter);
    }

  return true;
}

void CCopasiParameterGroup::addParameter(CCopasiParameter * pParameter)
{
  if (pParameter == NULL) return;

  pParameter->setUserInterfaceFlag(mUserInterfaceFlag & pParameter->getUserInterfaceFlag());

  CDataContainer::add(pParameter, true);
  static_cast< elements * >(mpValue)->push_back(pParameter);
}

CCopasiParameterGroup & CCopasiParameterGroup::getElementTemplates()
{
  if (mpElementTemplates == NULL)
    {
      mpElementTemplates = new CCopasiParameterGroup("Element Templates", this);
    }

  return *mpElementTemplates;
}

const CCopasiParameterGroup & CCopasiParameterGroup::getElementTemplates() const
{
  assert(mpElementTemplates != NULL);

  return *mpElementTemplates;
}

bool CCopasiParameterGroup::haveTemplate() const
{
  return (mpElementTemplates != NULL && mpElementTemplates->size() > 0);
}

// virtual
void CCopasiParameterGroup::setUserInterfaceFlag(const CCopasiParameter::UserInterfaceFlag & flag)
{
  CCopasiParameter::setUserInterfaceFlag(flag);

  elements::iterator it = beginIndex();
  elements::iterator end = endIndex();

  for (; it != end; ++it)
    {
      (*it)->setUserInterfaceFlag(mUserInterfaceFlag);
    }
}

CCopasiParameterGroup::index_iterator CCopasiParameterGroup::beginIndex() const
{return static_cast< elements * >(mpValue)->begin();}

CCopasiParameterGroup::index_iterator CCopasiParameterGroup::endIndex() const
{return static_cast< elements * >(mpValue)->end();}

CCopasiParameterGroup::const_name_iterator CCopasiParameterGroup::beginName() const
{
  return const_name_iterator(mObjects.begin());
}

CCopasiParameterGroup::const_name_iterator CCopasiParameterGroup::endName() const
{
  return const_name_iterator(mObjects.end());
}

bool CCopasiParameterGroup::addParameter(const std::string & name,
    const CCopasiParameter::Type type)
{
  CCopasiParameter * pParameter;

  if (type == CCopasiParameter::Type::GROUP)
    pParameter = new CCopasiParameterGroup(name);
  else
    pParameter = new CCopasiParameter(name, type);

  addParameter(pParameter);

  return true;
}

bool CCopasiParameterGroup::addGroup(const std::string & name)
{
  addParameter(new CCopasiParameterGroup(name));
  return true;
}

CCopasiParameterGroup * CCopasiParameterGroup::assertGroup(const std::string & name)
{
  CCopasiParameterGroup * pGrp = getGroup(name);

  if (pGrp) return pGrp;

  removeParameter(name);

  addGroup(name);
  return getGroup(name);
}

bool CCopasiParameterGroup::removeParameter(const std::string & name)
{
  return removeParameter(getParameter(name));
}

bool CCopasiParameterGroup::removeParameter(const size_t & index)
{
  if (index < size())
    {
      return removeParameter(static_cast< elements * >(mpValue)->at(index));
    }

  return false;
}

bool CCopasiParameterGroup::removeParameter(CCopasiParameter * pParameter)
{
  if (pParameter != NULL &&
      pParameter->getObjectParent() == this)
    {
      delete pParameter;
      return true;
    }

  return false;
}

// virtual
bool CCopasiParameterGroup::remove(CDataObject * pObject)
{
  bool success = CCopasiParameter::remove(pObject);

  if (success)
    {
      // elements contains CCopasiParameter *, we therefore must compare it to the same type.
      CCopasiParameter * pParameter = static_cast< CCopasiParameter * >(pObject);

      index_iterator it = static_cast< elements * >(mpValue)->begin();
      index_iterator end = static_cast< elements * >(mpValue)->end();

      for (; it != end; ++it)
        if (*it == pParameter)
          {
            static_cast< elements * >(mpValue)->erase(it, it + 1);
            break;
          }
    }

  return success;
}

CCopasiParameter * CCopasiParameterGroup::getParameter(std::string name)
{
  sanitizeObjectName(name);
  objectMap::range range = getObjects().equal_range(name);

  if (range.first == range.second) return NULL;

  return dynamic_cast<CCopasiParameter *>(const_cast< CDataObject * >(*range.first));
}

const CCopasiParameter * CCopasiParameterGroup::getParameter(std::string name) const
{
  sanitizeObjectName(name);
  objectMap::range range = getObjects().equal_range(name);

  if (range.first == range.second) return NULL;

  return dynamic_cast<CCopasiParameter *>(*range.first);
}

CCopasiParameter * CCopasiParameterGroup::getParameter(const size_t & index)
{
  if (index < size())
    return *(static_cast< elements * >(mpValue)->begin() + index);

  return NULL;
}

const CCopasiParameter * CCopasiParameterGroup::getParameter(const size_t & index) const
{
  if (index < size())
    return *(static_cast< elements * >(mpValue)->begin() + index);

  return NULL;
}

CCopasiParameterGroup * CCopasiParameterGroup::getGroup(const std::string & name)
{return dynamic_cast<CCopasiParameterGroup *>(getParameter(name));}

const CCopasiParameterGroup * CCopasiParameterGroup::getGroup(const std::string & name) const
{return dynamic_cast<const CCopasiParameterGroup *>(getParameter(name));}

CCopasiParameterGroup * CCopasiParameterGroup::getGroup(const size_t & index)
{return dynamic_cast<CCopasiParameterGroup *>(getParameter(index));}

const CCopasiParameterGroup * CCopasiParameterGroup::getGroup(const size_t & index) const
{return dynamic_cast<const CCopasiParameterGroup *>(getParameter(index));}

CCopasiParameter::Type CCopasiParameterGroup::getType(const std::string & name) const
{
  CCopasiParameter * pParameter =
    const_cast< CCopasiParameterGroup * >(this)->getParameter(name);

  if (pParameter) return pParameter->getType();

  return CCopasiParameter::Type::INVALID;
}

CCopasiParameter::Type CCopasiParameterGroup::getType(const size_t & index) const
{
  CCopasiParameter * pParameter =
    const_cast< CCopasiParameterGroup * >(this)->getParameter(index);

  if (pParameter) return pParameter->getType();

  return CCopasiParameter::Type::INVALID;
}

std::string CCopasiParameterGroup::getKey(const std::string & name) const
{
  CCopasiParameter * pParameter =
    const_cast< CCopasiParameterGroup * >(this)->getParameter(name);

  if (pParameter) return pParameter->getKey();

  return "Not Found";
}

std::string CCopasiParameterGroup::getKey(const size_t & index) const
{
  CCopasiParameter * pParameter =
    const_cast< CCopasiParameterGroup * >(this)->getParameter(index);

  if (pParameter) return pParameter->getKey();

  return "Not Found";
}

const std::string & CCopasiParameterGroup::getName(const size_t & index) const
{
  static std::string Invalid("Invalid Index");

  CCopasiParameter * pParameter =
    const_cast< CCopasiParameterGroup * >(this)->getParameter(index);

  if (pParameter) return pParameter->getObjectName();

  return Invalid;
}

bool CCopasiParameterGroup::swap(const size_t & iFrom,
                                 const size_t & iTo)
{
  index_iterator from = beginIndex() + iFrom;
  index_iterator to = beginIndex() + iTo;

  return swap(from, to);
}

bool CCopasiParameterGroup::swap(index_iterator & from,
                                 index_iterator & to)
{
  if (from < beginIndex() || endIndex() <= from ||
      to < beginIndex() || endIndex() <= to)
    return false;

  CCopasiParameter *tmp = *from;
  *from = *to;
  *to = tmp;

  return true;
}

size_t CCopasiParameterGroup::size(const CCopasiParameter::UserInterfaceFlag & flag) const
{
  if (flag == CCopasiParameter::UserInterfaceFlag::All)
    {
      return static_cast< elements * >(mpValue)->size();
    }

  elements::const_iterator it = beginIndex();
  elements::const_iterator end = endIndex();

  size_t size = 0;

  for (; it != end; ++it)
    if (flag & (*it)->getUserInterfaceFlag())
      ++size;

  return size;
}

void CCopasiParameterGroup::clear()
{
  if (mpValue != NULL)
    {
      elements Elements = *static_cast< elements * >(mpValue);
      static_cast< elements * >(mpValue)->clear();

      index_iterator it = Elements.begin();
      index_iterator end = Elements.end();

      for (; it != end; ++it)
        {
          pdelete(*it);
        }
    }
}

size_t CCopasiParameterGroup::getIndex(const std::string & name) const
{
  index_iterator it = static_cast< elements * >(mpValue)->begin();
  index_iterator end = static_cast< elements * >(mpValue)->end();

  for (size_t i = 0; it != end; ++it, ++i)
    if (name == (*it)->getObjectName())
      return i;

  return C_INVALID_INDEX;
}

// virtual
size_t CCopasiParameterGroup::getIndex(const CDataObject * pObject) const
{
  if (dynamic_cast< const CCopasiParameter * >(pObject))
    {
      index_iterator it = static_cast< elements * >(mpValue)->begin();
      index_iterator end = static_cast< elements * >(mpValue)->end();
      size_t i = 0;

      for (; it != end; ++it, ++i)
        if (pObject == *it)
          return i;
    }

  return CCopasiParameter::getIndex(pObject);
}

// virtual
void CCopasiParameterGroup::updateIndex(const size_t & index, const CUndoObjectInterface * pUndoObject)
{
  const CDataObject * pObject = dynamic_cast< const CDataObject * >(pUndoObject);
  size_t Index = getIndex(pObject);

  // We only update the index of container objects.
  if (Index == C_INVALID_INDEX ||
      Index == index)
    {
      return;
    }

  elements * pElements = static_cast< elements * >(mpValue);
  pElements->erase(pElements->begin() + Index);
  pElements->insert(pElements->begin() + std::min(index, pElements->size()), static_cast< CCopasiParameter * >(const_cast< CDataObject * >(pObject)));
}

std::string CCopasiParameterGroup::getUniqueParameterName(const CCopasiParameter * pParameter) const
{
  size_t counter = C_INVALID_INDEX;
  size_t Index = C_INVALID_INDEX;

  std::string Name = pParameter->getObjectName();

  index_iterator it = static_cast< elements * >(mpValue)->begin();
  index_iterator end = static_cast< elements * >(mpValue)->end();

  for (; it != end; ++it)
    {
      if ((*it)->getObjectName() == Name)
        {
          counter++;

          if (*it == pParameter)
            {
              Index = counter;
            }
        }
    }

  if (counter == 0 || Index == C_INVALID_INDEX)
    {
      return Name;
    }

  std::stringstream UniqueName;
  UniqueName << Name << "[" << Index << "]";

  return UniqueName.str();
}
