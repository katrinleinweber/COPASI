// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/utilities/CCopasiParameter.cpp,v $
//   $Revision: 1.33 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2009/02/19 19:54:02 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/**
 *  CCopasiParameter class.
 *  This class is used to describe method paramters. This class is intended
 *  to be used with integration or optimization methods.
 *
 *  Created for Copasi by Stefan Hoops 2002
 */

#include <string>

#include "copasi.h"
#include "CCopasiParameter.h"
#include "CCopasiParameterGroup.h"
#include "CCopasiMessage.h"
#include "report/CKeyFactory.h"
#include "report/CCopasiObjectReference.h"
#include "copasi/report/CCopasiRootContainer.h"

const std::string CCopasiParameter::TypeName[] =
  {
    "float",
    "unsigned float",
    "integer",
    "unsigned integer",
    "bool",
    "group",
    "string",
    "common name",
    "key",
    "file",
    ""
  };

const char* CCopasiParameter::XMLType[] =
  {
    "float",
    "unsignedFloat",
    "integer",
    "unsignedInteger",
    "bool",
    "group",
    "string",
    "cn",
    "key",
    "file",
    NULL
  };

CCopasiParameter::CCopasiParameter():
    CCopasiContainer("NoName", NULL, "Parameter"),
    mKey(CCopasiRootContainer::getKeyFactory()->add("Parameter", this)),
    mType(INVALID),
    mSize(0),
    mValue()
{mValue.pVOID = NULL;}

CCopasiParameter::CCopasiParameter(const CCopasiParameter & src,
                                   const CCopasiContainer * pParent):
    CCopasiContainer(src, pParent),
    mKey(CCopasiRootContainer::getKeyFactory()->add(src.getObjectType(), this)),
    mType(src.mType),
    mSize(0),
    mValue(createValue(src.mValue))
{}

CCopasiParameter::CCopasiParameter(const std::string & name,
                                   const CCopasiParameter::Type & type,
                                   const void * pValue,
                                   const CCopasiContainer * pParent,
                                   const std::string & objectType):
    CCopasiContainer(name, pParent, objectType,
                     CCopasiObject::Container |
                     ((type == DOUBLE || type == UDOUBLE) ? CCopasiObject::ValueDbl :
                      ((type == INT || type == UINT) ? CCopasiObject::ValueInt :
                       ((type == STRING || type == CN || type == KEY || type == FILE) ? CCopasiObject::ValueString :
                        (type == BOOL) ? CCopasiObject::ValueBool : 0)))),
    mKey(CCopasiRootContainer::getKeyFactory()->add(objectType, this)),
    mType(type),
    mSize(0),
    mValue()
{
  CCopasiParameter::Value value;
  value.pVOID = const_cast<void *>(pValue);
  mValue = createValue(value);
}

CCopasiParameter::~CCopasiParameter()
{
  CCopasiRootContainer::getKeyFactory()->remove(mKey);
  deleteValue();
}

CCopasiParameter & CCopasiParameter::operator = (const CCopasiParameter & rhs)
{
  assert (mType == rhs.mType);

  if (getObjectName() != rhs.getObjectName())
    setObjectName(rhs.getObjectName());

  switch (mType)
    {
    case CCopasiParameter::DOUBLE:
      case CCopasiParameter::UDOUBLE:
      *mValue.pDOUBLE = *rhs.mValue.pDOUBLE;
      break;

    case CCopasiParameter::INT:
      *mValue.pINT = *rhs.mValue.pINT;
      break;

    case CCopasiParameter::UINT:
      *mValue.pUINT = *rhs.mValue.pUINT;
      break;

    case CCopasiParameter::BOOL:
      *mValue.pBOOL = *rhs.mValue.pBOOL;
      break;

    case CCopasiParameter::STRING:
    case CCopasiParameter::KEY:
    case CCopasiParameter::FILE:
      *mValue.pSTRING = *rhs.mValue.pSTRING;
      break;

    case CCopasiParameter::CN:
      *mValue.pCN = *rhs.mValue.pCN;
      break;

    case CCopasiParameter::GROUP:
      *static_cast<CCopasiParameterGroup *>(this) = *static_cast<const CCopasiParameterGroup *>(&rhs);
      break;

    case CCopasiParameter::INVALID:
      break;
    }

  return *this;
}
const std::string & CCopasiParameter::getKey() const {return mKey;}

bool CCopasiParameter::setValue(const std::vector< CCopasiParameter * > & /* value */)
{
  fatalError();
  return false;
}

const CCopasiParameter::Value & CCopasiParameter::getValue() const {return mValue;}

CCopasiParameter::Value & CCopasiParameter::getValue() {return mValue;}

const CCopasiParameter::Type & CCopasiParameter::getType() const
  {return mType;}

bool CCopasiParameter::isValidValue(const C_FLOAT64 & value) const
  {
    if ((mType != CCopasiParameter::DOUBLE &&
         mType != CCopasiParameter::UDOUBLE) ||
        (mType == CCopasiParameter::UDOUBLE &&
         value < 0.0)) return false;

    return true;
  }

bool CCopasiParameter::isValidValue(const C_INT32 & C_UNUSED(value)) const
  {
    if (mType != CCopasiParameter::INT) return false;
    return true;
  }

bool CCopasiParameter::isValidValue(const unsigned C_INT32 & C_UNUSED(value)) const
  {
    if (mType != CCopasiParameter::UINT) return false;
    return true;
  }

bool CCopasiParameter::isValidValue(const bool & C_UNUSED(value)) const
  {
    if (mType != CCopasiParameter::BOOL) return false;
    return true;
  }

bool CCopasiParameter::isValidValue(const std::string & value) const
  {
    if (mType == CCopasiParameter::KEY)
      return CKeyFactory::isValidKey(value);

    if (mType != CCopasiParameter::STRING &&
        mType != CCopasiParameter::FILE) return false;
    return true;
  }

bool CCopasiParameter::isValidValue(const CCopasiObjectName & C_UNUSED(value)) const
  {
    if (mType != CCopasiParameter::CN) return false;
    return true;
  }

bool CCopasiParameter::isValidValue(const CCopasiParameterGroup::parameterGroup & C_UNUSED(value)) const
  {
    if (mType != CCopasiParameter::GROUP) return false;
    return true;
  }

void CCopasiParameter::print(std::ostream * ostream) const
{*ostream << *this;}

std::ostream &operator<<(std::ostream &os, const CCopasiParameter & o)
{
  os << "    " << o.getObjectName() << ": ";

  switch (o.mType)
    {
    case CCopasiParameter::DOUBLE:
    case CCopasiParameter::UDOUBLE:
      os << * o.mValue.pDOUBLE;
      break;

    case CCopasiParameter::INT:
      os << * o.mValue.pINT;
      break;

    case CCopasiParameter::UINT:
      os << * o.mValue.pUINT;
      break;

    case CCopasiParameter::BOOL:
      os << * o.mValue.pBOOL;
      break;

    case CCopasiParameter::STRING:
    case CCopasiParameter::KEY:
    case CCopasiParameter::FILE:
      os << * o.mValue.pSTRING;
      break;

    case CCopasiParameter::CN:
      os << * o.mValue.pCN;
      break;

    case CCopasiParameter::GROUP:
    case CCopasiParameter::INVALID:
      break;
    }

  return os;
}

bool operator==(const CCopasiParameter & lhs, const CCopasiParameter & rhs)
{
  if (lhs.getObjectName() != rhs.getObjectName()) return false;
  if (lhs.mType != rhs.mType) return false;

  switch (lhs.mType)
    {
    case CCopasiParameter::DOUBLE:
    case CCopasiParameter::UDOUBLE:
      return *lhs.mValue.pDOUBLE == *rhs.mValue.pDOUBLE;
      break;

    case CCopasiParameter::INT:
      return *lhs.mValue.pINT == *rhs.mValue.pINT;
      break;

    case CCopasiParameter::UINT:
      return *lhs.mValue.pUINT == *rhs.mValue.pUINT;
      break;

    case CCopasiParameter::BOOL:
      return *lhs.mValue.pBOOL == *rhs.mValue.pBOOL;
      break;

    case CCopasiParameter::STRING:
    case CCopasiParameter::KEY:
    case CCopasiParameter::FILE:
      return *lhs.mValue.pSTRING == *rhs.mValue.pSTRING;
      break;

    case CCopasiParameter::CN:
      return *lhs.mValue.pCN == *rhs.mValue.pCN;
      break;

    case CCopasiParameter::GROUP:
      return *static_cast<const CCopasiParameterGroup *>(&lhs) ==
      *static_cast<const CCopasiParameterGroup *>(&rhs);
      break;

    case CCopasiParameter::INVALID:
    default:
      return !memcmp(lhs.mValue.pVOID, rhs.mValue.pVOID, lhs.mSize);
      break;
    }
}

void * CCopasiParameter::getValuePointer() const
{return const_cast<void *>(mValue.pVOID);}

CCopasiParameter::Value CCopasiParameter::createValue(const Value & value)
{
  switch (mType)
    {
    case CCopasiParameter::DOUBLE:
    case CCopasiParameter::UDOUBLE:
      mValue.pDOUBLE = new C_FLOAT64;
      if (value.pDOUBLE) * mValue.pDOUBLE = * value.pDOUBLE;
      mSize = sizeof(C_FLOAT64);
      addObjectReference("Value", * mValue.pDOUBLE, CCopasiObject::ValueDbl);
      break;

    case CCopasiParameter::INT:
      mValue.pINT = new C_INT32;
      if (value.pINT) * mValue.pINT = * value.pINT;
      mSize = sizeof(C_INT32);
      addObjectReference("Value", * mValue.pINT, CCopasiObject::ValueInt);
      break;

    case CCopasiParameter::UINT:
      mValue.pUINT = new unsigned C_INT32;
      if (value.pUINT) * mValue.pUINT = * value.pUINT;
      mSize = sizeof(unsigned C_INT32);
      addObjectReference("Value", * mValue.pUINT, CCopasiObject::ValueInt);
      break;

    case CCopasiParameter::BOOL:
      mValue.pBOOL = new bool;
      if (value.pBOOL) * mValue.pBOOL = * value.pBOOL;
      mSize = sizeof(bool);
      addObjectReference("Value", * mValue.pBOOL, CCopasiObject::ValueBool);
      break;

    case CCopasiParameter::STRING:
    case CCopasiParameter::KEY:
    case CCopasiParameter::FILE:
      if (value.pSTRING)
        mValue.pSTRING = new std::string(* value.pSTRING);
      else
        mValue.pSTRING = new std::string;
      mSize = sizeof(std::string);
      addObjectReference("Value", * mValue.pSTRING, CCopasiObject::ValueString);
      break;

    case CCopasiParameter::CN:
      if (value.pCN)
        mValue.pCN = new CRegisteredObjectName(* value.pCN);
      else
        mValue.pCN = new CRegisteredObjectName;
      mSize = sizeof(CRegisteredObjectName);
      addObjectReference("Value", * mValue.pCN, CCopasiObject::ValueString);
      break;

    case CCopasiParameter::GROUP:
    case CCopasiParameter::INVALID:
      mValue.pVOID = NULL;
      mSize = 0;
      break;
    }

  return mValue;
}

void CCopasiParameter::deleteValue()
{
  if (!mValue.pVOID) return;

  switch (mType)
    {
    case CCopasiParameter::DOUBLE:
    case CCopasiParameter::UDOUBLE:
      delete mValue.pDOUBLE;
      break;

    case CCopasiParameter::INT:
      delete mValue.pINT;
      break;

    case CCopasiParameter::UINT:
      delete mValue.pUINT;
      break;

    case CCopasiParameter::BOOL:
      delete mValue.pBOOL;
      break;

    case CCopasiParameter::STRING:
    case CCopasiParameter::KEY:
    case CCopasiParameter::FILE:
      delete mValue.pSTRING;
      break;

    case CCopasiParameter::CN:
      delete mValue.pCN;
      break;

    default:
      fatalError();
      break;
    }

  mValue.pVOID = NULL;
  return;
}

std::string CCopasiParameter::getObjectDisplayName(bool regular, bool richtext) const
  {
    // if one of the ancestors is a reaction and the parameter is not a group
    // it is (hopefully) a kinetic parameter

    CCopasiObject* tmp = /*dynamic_cast<CModel*>*/(this->getObjectAncestor("Reaction"));
    if (tmp)
      if (getType() != GROUP)
        {
          return tmp->getObjectDisplayName() + "." + getObjectName();
        }

    return CCopasiObject::getObjectDisplayName(regular, richtext);
  }
