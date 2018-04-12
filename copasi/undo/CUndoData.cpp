// Copyright (C) 2017 - 2018 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

#include "copasi.h"

#include "copasi/undo/CUndoData.h"

#include "copasi/core/CDataContainer.h"
#include "copasi/core/CRegisteredCommonName.h"
#include "copasi/core/CDataVector.h"
#include "copasi/CopasiDataModel/CDataModel.h"
#include "copasi/model/CMetab.h"
#include "copasi/model/CModelParameterSet.h"
#include "copasi/utilities/CCopasiParameterGroup.h"

// static
const CEnumAnnotation< std::string, CUndoData::Type > CUndoData::TypeName(
{
  "Insert",
  "Change",
  "Remove"
});

CUndoData::CUndoData():
  mType(CUndoData::Type::CHANGE),
  mOldData(),
  mNewData(),
  mPreProcessData(),
  mPostProcessData(),
  mTime(),
  mAuthorID(C_INVALID_INDEX),
  mChangedProperties()
{
  time(&mTime);
}

CUndoData::CUndoData(const Type & type, const CDataObject * pObject, const size_t & authorId):
  mType(type),
  mOldData(),
  mNewData(),
  mPreProcessData(),
  mPostProcessData(),
  mTime(),
  mAuthorID(authorId),
  mChangedProperties()
{
  assert(pObject != NULL);

  time(&mTime);

  switch (mType)
    {
      case Type::INSERT:
        mNewData = pObject->toData();

        for (CData::const_iterator it = mNewData.begin(), end = mNewData.end(); it != end; ++it)
          {
            mChangedProperties.insert(it->first);
          }

        break;

      case Type::REMOVE:
        mOldData = pObject->toData();

        for (CData::const_iterator it = mOldData.begin(), end = mOldData.end(); it != end; ++it)
          {
            mChangedProperties.insert(it->first);
          }

        break;

      case Type::CHANGE:
        mOldData = pObject->toData();
        mOldData.removeProperty(CData::OBJECT_FLAG);
        mOldData.removeProperty(CData::OBJECT_REFERENCES);

        mNewData = mOldData;
        break;
    }
}

CUndoData::CUndoData(const Type & type, const CData & data, const size_t & authorId):
  mType(CUndoData::Type::CHANGE),
  mOldData(),
  mNewData(),
  mPreProcessData(),
  mPostProcessData(),
  mTime(),
  mAuthorID(C_INVALID_INDEX),
  mChangedProperties()
{
  time(&mTime);

  switch (mType)
    {
      case Type::INSERT:
        mNewData = data;

        for (CData::const_iterator it = mNewData.begin(), end = mNewData.end(); it != end; ++it)
          {
            mChangedProperties.insert(it->first);
          }

        break;

      case Type::REMOVE:
        mOldData = data;

        for (CData::const_iterator it = mOldData.begin(), end = mOldData.end(); it != end; ++it)
          {
            mChangedProperties.insert(it->first);
          }

        break;

      case Type::CHANGE:
        mOldData.addProperty(CData::OBJECT_PARENT_CN, data.getProperty(CData::OBJECT_PARENT_CN));
        mOldData.addProperty(CData::OBJECT_TYPE, data.getProperty(CData::OBJECT_TYPE));
        mOldData.addProperty(CData::OBJECT_NAME, data.getProperty(CData::OBJECT_NAME));
        mOldData.addProperty(CData::OBJECT_INDEX, data.getProperty(CData::OBJECT_INDEX));

        mNewData.addProperty(CData::OBJECT_PARENT_CN, data.getProperty(CData::OBJECT_PARENT_CN));
        mNewData.addProperty(CData::OBJECT_TYPE, data.getProperty(CData::OBJECT_TYPE));
        mNewData.addProperty(CData::OBJECT_NAME, data.getProperty(CData::OBJECT_NAME));
        mNewData.addProperty(CData::OBJECT_INDEX, data.getProperty(CData::OBJECT_INDEX));

        break;
    }
}

CUndoData::CUndoData(const CUndoData & src):
  mType(src.mType),
  mOldData(src.mOldData),
  mNewData(src.mNewData),
  mPreProcessData(src.mPreProcessData),
  mPostProcessData(src.mPostProcessData),
  mTime(src.mTime),
  mAuthorID(src.mAuthorID),
  mChangedProperties(src.mChangedProperties)
{}

CUndoData::~CUndoData()
{}

const CUndoData::Type & CUndoData::getType() const
{
  return mType;
}

bool CUndoData::addProperty(const CData::Property & property, const CDataValue & value)
{
  return addProperty(CData::PropertyName[property], value);
}

bool CUndoData::addProperty(const std::string & name, const CDataValue & value)
{
  bool success = true;

  switch (mType)
    {
      case Type::INSERT:
        success &= mNewData.addProperty(name, value);
        mChangedProperties.insert(name);
        break;

      case Type::REMOVE:
        success &= mOldData.addProperty(name, value);
        mChangedProperties.insert(name);
        break;

      case Type::CHANGE:
        success = false;
        break;
    }

  return success;
}

bool CUndoData::addProperty(const CData::Property & property, const CDataValue & oldValue, const CDataValue & newValue)
{
  return addProperty(CData::PropertyName[property], oldValue, newValue);
}

bool CUndoData::removeProperty(const CData::Property & property)
{
  return removeProperty(CData::PropertyName[property]);
}

bool CUndoData::addProperty(const std::string & name, const CDataValue & oldValue, const CDataValue & newValue)
{
  bool success = false;

  switch (mType)
    {
      case Type::INSERT:
      case Type::REMOVE:
        break;

      case Type::CHANGE:
        if (oldValue != newValue)
          {
            success &= mOldData.addProperty(name, oldValue);
            success &= mNewData.addProperty(name, newValue);
            mChangedProperties.insert(name);
            success = true;
          }
        else
          {
            // These are required to retrieve the object and must not be removed.
            if (name != "Object Name" &&
                name != "Object Parent CN" &&
                name != "Object Type" &&
                name != "Object Hash" &&
                name != "Object Index")
              {
                mOldData.removeProperty(name);
                mNewData.removeProperty(name);
              }

            mChangedProperties.erase(name);
          }

        break;
    }

  return success;
}

bool CUndoData::removeProperty(const std::string & name)
{
  bool success = mOldData.removeProperty(name);
  success |= mNewData.removeProperty(name);
  success &= mChangedProperties.erase(name) > 0;

  return success;
}

bool CUndoData::isSetProperty(const CData::Property & property) const
{
  return isSetProperty(CData::PropertyName[property]);
}

bool CUndoData::isSetProperty(const std::string & name) const
{
  bool isSet = true;

  switch (mType)
    {
      case Type::INSERT:
        isSet &= mNewData.isSetProperty(name);
        break;

      case Type::REMOVE:
        isSet &= mOldData.isSetProperty(name);
        break;

      case Type::CHANGE:
        isSet &= mNewData.isSetProperty(name);
        isSet &= mOldData.isSetProperty(name);
        break;
    }

  return isSet;
}

bool CUndoData::appendData(const CData & data)
{
  if (mType == Type::CHANGE) return false;

  bool success = true;

  CData::const_iterator it = data.begin();
  CData::const_iterator end = data.end();

  for (; it != end; ++it)
    {
      success &= addProperty(it->first, it->second);
    }

  return success;
}

bool CUndoData::appendData(const CData & oldData, const CData & newData)
{
  if (mType != Type::CHANGE) return false;

  bool success = true;

  CData::const_iterator itOld = oldData.begin();
  CData::const_iterator endOld = oldData.end();
  CData::const_iterator itNew = newData.begin();

  for (; itOld != endOld; ++itOld, ++itNew)
    {
      success &= addProperty(itOld->first, itOld->second, itNew->second);
    }

  return success;
}

bool CUndoData::addPreProcessData(const CUndoData & dependentData)
{
  mPreProcessData.push_back(dependentData);

  return true;
}

bool CUndoData::addPreProcessData(std::vector< CUndoData > & dependentData)
{
  mPreProcessData.insert(mPreProcessData.end(), dependentData.begin(), dependentData.end());

  return true;
}

bool CUndoData::addPostProcessData(const CUndoData & dependentData)
{
  mPostProcessData.push_back(dependentData);

  return true;
}

bool CUndoData::addPostProcessData(std::vector< CUndoData > & dependentData)
{
  mPostProcessData.insert(mPostProcessData.end(), dependentData.begin(), dependentData.end());

  return true;
}

const CData & CUndoData::getOldData() const
{
  return mOldData;
}

const CData & CUndoData::getNewData() const
{
  return mNewData;
}

const std::vector< CUndoData > & CUndoData::getPreProcessData() const
{
  return mPreProcessData;
}

std::vector< CUndoData > & CUndoData::getPreProcessData()
{
  return mPreProcessData;
}

const std::vector< CUndoData > & CUndoData::getPostProcessData() const
{
  return mPostProcessData;
}

std::vector< CUndoData > & CUndoData::getPostProcessData()
{
  return mPostProcessData;
}

const std::set< std::string > & CUndoData::getChangedProperties() const
{
  return mChangedProperties;
}

bool CUndoData::isChangedProperty(const CData::Property & property) const
{
  return isChangedProperty(CData::PropertyName[property]);
}

bool CUndoData::isChangedProperty(const std::string & name) const
{
  return mChangedProperties.find(name) != mChangedProperties.end();
}

bool CUndoData::apply(const CDataModel & dataModel, ChangeSet & changes, const bool & execute) const
{
  bool success = true;

  // std::cout << *this << std::endl;

  switch (mType)
    {
      case Type::INSERT:
        success &= insert(dataModel, true, changes, execute);
        break;

      case Type::REMOVE:
        success &= remove(dataModel, true, changes, execute);
        break;

      case Type::CHANGE:
        success &= change(dataModel, true, changes, execute);
        break;
    }

  return success;
}

bool CUndoData::undo(const CDataModel & dataModel, ChangeSet & changes, const bool & execute) const
{
  bool success = true;

  switch (mType)
    {
      case Type::INSERT:
        success &= remove(dataModel, false, changes, execute);
        break;

      case Type::REMOVE:
        success &= insert(dataModel, false, changes, execute);
        break;

      case Type::CHANGE:
        success &= change(dataModel, false, changes, execute);
        break;
    }

  return success;
}

const std::time_t & CUndoData::getTime() const
{
  return mTime;
}

std::tm * CUndoData::getLocalTime() const
{
  return localtime(&mTime);
}

std::tm * CUndoData::getUTCTime() const
{
  return gmtime(&mTime);
}

const size_t CUndoData::getAuthorID() const
{
  return mAuthorID;
}

std::string CUndoData::getObjectDisplayName() const
{
  std::string DisplayName("Unknown");

  switch (mType)
    {
      case Type::INSERT:
        DisplayName = mNewData.getProperty(CData::OBJECT_NAME).toString();
        break;

      case Type::REMOVE:
      case Type::CHANGE:
        DisplayName = mOldData.getProperty(CData::OBJECT_NAME).toString();
        break;
    }

  // Species will always have the name of the parent compartment appended.
  if (getObjectType() == "Metabolite")
    {
      CCommonName CN;

      switch (mType)
        {
          case Type::INSERT:
            CN = mNewData.getProperty(CData::OBJECT_PARENT_CN).toString();
            break;

          case Type::REMOVE:
          case Type::CHANGE:
            CN = mOldData.getProperty(CData::OBJECT_PARENT_CN).toString();
            break;
        }

      CN = CN.substr(CN.find("Vector=Compartments"));

      if (!CN.empty())
        {
          DisplayName += "{" + CN.getElementName(0) + "}";
        }
    }

  return DisplayName;
}

std::string CUndoData::getObjectCN(const bool & apply) const
{
  return CCommonName::fromData(getData(!apply));
}

std::string CUndoData::getObjectType() const
{
  switch (mType)
    {
      case Type::INSERT:
        return mNewData.getProperty(CData::OBJECT_TYPE).toString();
        break;

      case Type::REMOVE:
      case Type::CHANGE:
        return mOldData.getProperty(CData::OBJECT_TYPE).toString();
        break;
    }

  return "Unknown";
}

bool CUndoData::operator < (const CUndoData & rhs) const
{
  if (mType != rhs.mType) return mType < rhs.mType;

  switch (mType)
    {
      case Type::INSERT:
      {
        const std::string & CN = mNewData.getProperty(CData::OBJECT_PARENT_CN).toString();
        const std::string & RhsCN = rhs.mNewData.getProperty(CData::OBJECT_PARENT_CN).toString();

        if (CN != RhsCN) return CN < RhsCN;
      }
      break;

      case Type::REMOVE:
      {
        const std::string & CN = mOldData.getProperty(CData::OBJECT_PARENT_CN).toString();
        const std::string & RhsCN = rhs.mOldData.getProperty(CData::OBJECT_PARENT_CN).toString();

        if (CN != RhsCN) return CN > RhsCN;
      }
      break;

      case Type::CHANGE:
      {
        const std::string & CN = mNewData.getProperty(CData::OBJECT_PARENT_CN).toString();
        const std::string & RhsCN = rhs.mNewData.getProperty(CData::OBJECT_PARENT_CN).toString();

        if (CN != RhsCN) return CN < RhsCN;
      }

      {
        const std::string & CN = mOldData.getProperty(CData::OBJECT_PARENT_CN).toString();
        const std::string & RhsCN = rhs.mOldData.getProperty(CData::OBJECT_PARENT_CN).toString();

        if (CN != RhsCN) return CN > RhsCN;
      }
      break;
    }

  // At the point the data is of the same point and type now we sort by the index
  switch (mType)
    {
      case Type::INSERT:
      {
        size_t Index = mNewData.getProperty(CData::OBJECT_INDEX).toSizeT();
        size_t RhsIndex = rhs.mNewData.getProperty(CData::OBJECT_INDEX).toSizeT();

        if (Index != RhsIndex) return Index < RhsIndex;
      }
      break;

      case Type::REMOVE:
      {
        size_t Index = mOldData.getProperty(CData::OBJECT_INDEX).toSizeT();
        size_t RhsIndex = rhs.mOldData.getProperty(CData::OBJECT_INDEX).toSizeT();

        if (Index != RhsIndex) return Index > RhsIndex;
      }
      break;

      case Type::CHANGE:
      {
        size_t Index = mNewData.getProperty(CData::OBJECT_INDEX).toSizeT();
        size_t RhsIndex = rhs.mNewData.getProperty(CData::OBJECT_INDEX).toSizeT();

        if (Index != RhsIndex) return Index < RhsIndex;
      }

      {
        size_t Index = mOldData.getProperty(CData::OBJECT_INDEX).toSizeT();
        size_t RhsIndex = rhs.mOldData.getProperty(CData::OBJECT_INDEX).toSizeT();

        if (Index != RhsIndex) return Index > RhsIndex;
      }
      break;
    }

  // Default by pointer
  return this < &rhs;
}

bool CUndoData::empty() const
{
  return mChangedProperties.empty();
}

void CUndoData::clear()
{
  mOldData.clear();
  mNewData.clear();
  time(&mTime);
  mPreProcessData.clear();
  mPostProcessData.clear();
  mChangedProperties.clear();
}

// static
bool CUndoData::insert(const CDataModel & dataModel, const bool & apply, ChangeSet & changes, const bool & execute) const
{
  const CData & Data = getData(apply);

  bool success = executePreProcessData(dataModel, apply, changes, execute);

  CDataContainer * pParent = getParent(dataModel, Data);

  if (pParent == NULL)
    {
      executePreProcessData(dataModel, !apply, changes, execute);
      changes.clear();

      return false;
    }

  if (execute)
    {
      CUndoObjectInterface * pObject = pParent->insert(Data);

      if (pObject == NULL)
        {
          success = executePreProcessData(dataModel, !apply, changes, execute);
          changes.clear();

          return false;
        }

      success &= pObject->applyData(Data, changes);
    }

  changes[getObjectCN(apply)] = {CUndoData::Type::INSERT, Data.getProperty(CData::OBJECT_TYPE).toString(), Data.getProperty(CData::OBJECT_NAME).toString()};

  success &= executePostProcessData(dataModel, apply, changes, execute);

  return success;
}

// static
bool CUndoData::remove(const CDataModel & dataModel, const bool & apply, ChangeSet & changes, const bool & execute) const
{
  const CData & Data = getData(apply);

  CUndoObjectInterface * pObject = getObject(dataModel, Data);

  if (pObject == NULL)
    return false;

  bool success = executePreProcessData(dataModel, apply, changes, execute);

  changes[getObjectCN(apply)] = {CUndoData::Type::REMOVE, Data.getProperty(CData::OBJECT_TYPE).toString(), Data.getProperty(CData::OBJECT_NAME).toString()};

  if (execute)
    {
      pObject->destruct();
    }

  success &= executePostProcessData(dataModel, apply, changes, execute);

  return success;
}

// static
bool CUndoData::change(const CDataModel & dataModel, const bool & apply, ChangeSet & changes, const bool & execute) const
{
  const CData & OldData = getData(!apply);
  const CData & NewData = getData(apply);

  CUndoObjectInterface * pObject = getObject(dataModel, execute ? OldData : NewData);

  // We must always have the old object;
  if (pObject == NULL)
    return false;

  bool success = executePreProcessData(dataModel, apply, changes, execute);

  if (execute)
    {
      // A special case is that the ParentCN has changed, i.e., the object has moved
      CDataObject * pDataObject = NULL;

      if (OldData.getProperty(CData::OBJECT_PARENT_CN).toString() != NewData.getProperty(CData::OBJECT_PARENT_CN).toString() &&
          (pDataObject = dynamic_cast< CDataObject * >(pObject)))
        {
          // We need to move
          CDataContainer * pContainer = pDataObject->getObjectParent();

          if (pContainer != NULL)
            pContainer->remove(pDataObject);

          pContainer = getParent(dataModel, NewData);

          if (pContainer != NULL)
            pContainer->add(pDataObject, true);
        }

      success &= pObject->applyData(NewData, changes);
    }

  changes[getObjectCN(apply)] = {CUndoData::Type::CHANGE, NewData.getProperty(CData::OBJECT_TYPE).toString(), NewData.getProperty(CData::OBJECT_NAME).toString()};
  success &= executePostProcessData(dataModel, apply, changes, execute);

  return success;
}

bool CUndoData::executePreProcessData(const CDataModel & dataModel, const bool & apply, ChangeSet & changes, const bool & execute) const
{
  bool success = true;

  if (apply)
    {
      std::vector< CUndoData >::const_iterator it = mPreProcessData.begin();
      std::vector< CUndoData >::const_iterator end = mPreProcessData.end();

      for (; it != end; ++it)
        {
          // If we are here we have dependent data, e.g. particle number changes
          success &= it->apply(dataModel, changes, execute);
        }
    }
  else
    {
      std::vector< CUndoData >::const_reverse_iterator it = mPostProcessData.rbegin();
      std::vector< CUndoData >::const_reverse_iterator end = mPostProcessData.rend();

      for (; it != end; ++it)
        {
          // If we are here we have dependent data, e.g. particle number changes
          success &= it->undo(dataModel, changes, execute);
        }
    }

  return success;
}

bool CUndoData::executePostProcessData(const CDataModel & dataModel, const bool & apply, ChangeSet & changes, const bool & execute) const
{
  bool success = true;

  if (apply)
    {
      std::vector< CUndoData >::const_iterator it = mPostProcessData.begin();
      std::vector< CUndoData >::const_iterator end = mPostProcessData.end();

      for (; it != end; ++it)
        {
          // If we are here we have dependent data, e.g. particle number changes
          success &= it->apply(dataModel, changes, execute);
        }
    }
  else
    {
      std::vector< CUndoData >::const_reverse_iterator it = mPreProcessData.rbegin();
      std::vector< CUndoData >::const_reverse_iterator end = mPreProcessData.rend();

      for (; it != end; ++it)
        {
          // If we are here we have dependent data, e.g. particle number changes
          success &= it->undo(dataModel, changes, execute);
        }
    }

  return success;
}

const CData & CUndoData::getData(const bool & apply) const
{
  switch (mType)
    {
      case Type::CHANGE:
        if (apply)
          return mNewData;
        else
          return mOldData;

        break;

      case Type::INSERT:
        return mNewData;

      case Type::REMOVE:
        return mOldData;
    }

  // This will never be reached. It is there to satisfy the compiler.
  return mNewData;
}

// static
CDataContainer * CUndoData::getParent(const CDataModel & dataModel, const CData & data)
{
  const CDataContainer * pParent = NULL;

  if (!data.empty())
    pParent = dynamic_cast< const CDataContainer * >(dataModel.getObject(data.getProperty(CData::OBJECT_PARENT_CN).toString()));

  return const_cast< CDataContainer * >(pParent);
}

// static
CUndoObjectInterface * CUndoData::getObject(const CDataModel & dataModel, const CData & data)
{
  const CUndoObjectInterface * pObject = NULL;
  CDataContainer * pParent = getParent(dataModel, data);

  if (pParent != NULL)
    {
      size_t Index = 0;
      CCopasiParameterGroup * pGroup = NULL;
      CModelParameterSet * pSet = NULL;

      if ((pGroup = dynamic_cast< CCopasiParameterGroup * >(pParent)) != NULL &&
          (Index = data.getProperty(CData::OBJECT_INDEX).toSizeT()) < pGroup->size())
        {
          pObject = pGroup->getParameter(data.getProperty(CData::OBJECT_INDEX).toSizeT());
        }
      else if ((pSet = dynamic_cast< CModelParameterSet * >(pParent)) != NULL)
        {
          pObject = pSet->getModelParameter(data.getProperty(CData::OBJECT_NAME).toString());
        }
      else if (data.isSetProperty(CData::OBJECT_HASH))
        {
          // We need to search all objects with the same OBJECT_TYPE and OBJECT_NAME for the given hash.
          CDataContainer::objectMap::range Range = pParent->getObjects().equal_range(data.getProperty(CData::OBJECT_NAME).toString());
          const std::string & ObjectType = data.getProperty(CData::OBJECT_TYPE).toString();
          const std::string & ObjectHash = data.getProperty(CData::OBJECT_HASH).toString();

          for (; Range.first != Range.second; ++Range.first)
            if ((*Range.first)->getObjectType() == ObjectType)
              {
                CData Data((*Range.first)->toData());

                if (Data.getProperty(CData::OBJECT_HASH).toString() == ObjectHash)
                  {
                    pObject = (*Range.first);
                    break;
                  }
              }
        }

      if (pObject == NULL)
        {
          pObject = dynamic_cast< const CDataObject * >(pParent->getObject(data.getProperty(CData::OBJECT_TYPE).toString() + "=" + data.getProperty(CData::OBJECT_NAME).toString()));
        }
    }

  return const_cast< CUndoObjectInterface * >(pObject);
}

std::ostream & operator << (std::ostream & os, const CUndoData & o)
{
  os << "Type: " << CUndoData::TypeName[o.mType] << std::endl;
  os << "Old:  " << std::endl << o.mOldData << std::endl;
  os << "New:  " << std::endl << o.mNewData << std::endl;

  return os;
}
