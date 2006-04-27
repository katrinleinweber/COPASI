/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQProgressItemText.ui.h,v $
   $Revision: 1.3 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:27:42 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "qtUtilities.h"

bool CQProgressItemText::initFromProcessReportItem(CProcessReportItem * pItem)
{
  mpItem = pItem;
  mParameterValue = mpItem->getValue();

  mItemName->setText(FROM_UTF8(mpItem->getObjectName()));

  this->show();

  return reset();
}

bool CQProgressItemText::process()
{
  (this->*mpSetValue)();
  return true;
}

bool CQProgressItemText::reset()
{
  switch (mpItem->getType())
    {
    case CCopasiParameter::DOUBLE:
    case CCopasiParameter::UDOUBLE:
      mpSetValue = & CQProgressItemText::setValueFromDOUBLE;
      break;

    case CCopasiParameter::INT:
      mpSetValue = & CQProgressItemText::setValueFromINT;
      break;

    case CCopasiParameter::UINT:
      mpSetValue = & CQProgressItemText::setValueFromUINT;
      break;

    case CCopasiParameter::STRING:
      mpSetValue = & CQProgressItemText::setValueFromSTRING;
      break;

    default:
      return false;
      break;
    }

  return process();
}

void CQProgressItemText::setValueFromDOUBLE()
{
  mValue->setText(QString::number(* mParameterValue.pDOUBLE));
}

void CQProgressItemText::setValueFromINT()
{
  mValue->setText(QString::number(* mParameterValue.pINT));
}

void CQProgressItemText::setValueFromUINT()
{
  mValue->setText(QString::number(* mParameterValue.pUINT));
}

void CQProgressItemText::setValueFromSTRING()
{
  mValue->setText(FROM_UTF8(* mParameterValue.pSTRING));
}
