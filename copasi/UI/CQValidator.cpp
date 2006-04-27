/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQValidator.cpp,v $
   $Revision: 1.6 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:27:42 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include <qlineedit.h>

#include <float.h>

#include "CQValidator.h"

CQValidator::CQValidator(QLineEdit * parent, const char * name):
    QValidator(parent, name),
    mpLineEdit(parent),
    mLastAccepted("")
{
  int h, s, v;

  mSavedColor = mpLineEdit->paletteBackgroundColor();
  mSavedColor.getHsv(&h, &s, &v);

  if (s < 20) s = 20;

  mAcceptableColor.setHsv(240, s, v);
  mErrorColor.setHsv(0, s, v);
}

QValidator::State CQValidator::validate(QString & input, int & /* pos */) const
  {
    if (input == mLastAccepted)
      mpLineEdit->setPaletteBackgroundColor(mSavedColor);
    else
      setColor(Acceptable);

    return Acceptable;
  }

QValidator::State CQValidator::revalidate()
{
  int pos = mLastAccepted.length();

  return validate(mLastAccepted, pos);
}

void CQValidator::saved() const
  {
    const_cast<CQValidator *>(this)->mLastAccepted = mpLineEdit->text();
    mpLineEdit->setPaletteBackgroundColor(mSavedColor);
  }

void CQValidator::force(const QString & input) const
  {
    const_cast<CQValidator *>(this)->mLastAccepted = input;
    setColor(Acceptable);
  }

QValidator::State CQValidator::setColor(const QValidator::State & state) const
  {
    if (state == Invalid)
      mpLineEdit->setPaletteBackgroundColor(mErrorColor);
    else
      mpLineEdit->setPaletteBackgroundColor(mAcceptableColor);

    return state;
  }

CQValidatorNotEmpty::CQValidatorNotEmpty(QLineEdit * parent, const char * name):
    CQValidator(parent, name)
{}

QValidator::State CQValidatorNotEmpty::validate(QString & input, int & pos) const
  {
    if (input != "")
      return CQValidator::validate(input, pos);

    setColor(Invalid);
    return Intermediate;
  }

CQValidatorBound::CQValidatorBound(QLineEdit * parent, const char * name):
    CQValidator(parent, name),
    mpDoubleValidator(new QDoubleValidator(-DBL_MAX, DBL_MAX, DBL_DIG, this)),
    mValidBound("\t") // It is not possible to insert a tab.
{}

QValidator::State CQValidatorBound::validate (QString & input, int & pos) const
  {
    if (input == mValidBound ||
        mpDoubleValidator->validate(input, pos) == Acceptable)
      {
        force(input);
        return Acceptable;
      }

    setColor(Invalid);
    return Intermediate;
  }

void CQValidatorBound::force(const QString & input) const
  {
    const_cast<CQValidatorBound *>(this)->mValidBound = input;

    CQValidator::force(input);
  }
