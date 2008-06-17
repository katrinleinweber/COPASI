// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQExpressionWidget.cpp,v $
//   $Revision: 1.25 $
//   $Name:  $
//   $Author: pwilly $
//   $Date: 2008/06/17 10:03:12 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include <iostream>

#include "CQExpressionWidget.h"
#include "CQMessageBox.h"
#include "CCopasiSelectionDialog.h"
#include "qtUtilities.h"

#include "copasi.h"

#include "CopasiDataModel/CCopasiDataModel.h"
#include "function/CExpression.h"

CQExpressionHighlighter::CQExpressionHighlighter(CQExpressionWidget* ew)
    : QSyntaxHighlighter(ew)
{}

int CQExpressionHighlighter::highlightParagraph (const QString & text, int /* endStateOfLastPara */)
{
  int pos = 0;
  int oldpos = -1;
  int delta;

  while (true)
    {
      pos = text.find("<", pos);
      if (pos == -1)
        delta = 0;
      else
        delta = pos - oldpos - 1;
      setFormat(oldpos + 1, delta, QColor(0, 0, 0));
      if (pos == -1) break;
      oldpos = pos;

      pos = text.find(">", pos);
      while (pos > 0 && text[pos - 1] == '\\')
        pos = text.find(">", pos + 1);

      if (pos == -1)
        delta = 0;
      else
        delta = pos - oldpos + 1;

      setFormat(oldpos, delta, QColor(100, 0, 200));
      if (pos == -1) break;
      oldpos = pos;
    }
  return 0;
}

//***********************************************************************

CQValidatorExpression::CQValidatorExpression(QTextEdit * parent, const char * name, bool isBoolean):
    CQValidator< QTextEdit >(parent, name),
    mExpression()
{
  mExpression.setBoolean(isBoolean);
}

/**
  *  This function ensures that any characters on Expression Widget are validated
  *  to go to further processes.
  */
QValidator::State CQValidatorExpression::validate(QString & input, int & pos) const
  {
    //    std::cout << "CQVE::validate(__) " << (const char *) input.utf8() << std::endl;

    if (const_cast< CExpression * >(&mExpression)->setInfix((const char *) input.utf8()) &&
        const_cast< CExpression * >(&mExpression)->compile())
      {
        QString Input = mpLineEdit->text();
        //     std::cout << mpLineEdit->text() << std::endl;
        return CQValidator< QTextEdit >::validate(Input, pos);
      }

    setColor(Invalid);
    return Intermediate;
  }

/**
  * Function to get CExpression object
  */
CExpression *CQValidatorExpression::getExpression()
{
  //  return const_cast< CExpression * >(&mExpression);
  return &mExpression;
}

//***********************************************************************

CQExpressionWidget::CQExpressionWidget(QWidget * parent, const char * name, bool isBoolean)
    : QTextEdit(parent, name),
    mOldPar(0),
    mOldPos(0),
    mExpressionType(CCopasiSimpleSelectionTree::TRANSIENT_EXPRESSION),
    mpCurrentObject(NULL),
    mNewName("")
{
  setTextFormat(PlainText);
  setTabChangesFocus(true);

  new CQExpressionHighlighter(this);

  int h, s, v;

  mSavedColor = paletteBackgroundColor();
  mSavedColor.getHsv(&h, &s, &v);

  if (s < 20) s = 20;
  mChangedColor.setHsv(240, s, v);

  mpValidator = new CQValidatorExpression(this, "", isBoolean);
  mpValidator->revalidate();

  connect(this, SIGNAL(cursorPositionChanged(int, int)),
          this, SLOT(slotCursorPositionChanged(int, int)));
  connect(this, SIGNAL(selectionChanged()),
          this, SLOT(slotSelectionChanged()));
  connect(this, SIGNAL(textChanged()),
          this, SLOT(slotTextChanged()));
}

void CQExpressionWidget::keyPressEvent (QKeyEvent * e)
{
  //filter "<" and ">"
  if (e->text() == "<")
    return;
  if (e->text() == ">")
    return;

  QTextEdit::keyPressEvent(e);
}

void CQExpressionWidget::slotSelectionChanged()
{
  int par1, par2, pos1, pos2;
  getSelection(&par1, &pos1, &par2, &pos2);

  if (par1 == -1) //no selection, do nothing
    {
      getSelection(&mOldPar1, &mOldPos1, &mOldPar2, &mOldPos2);
      return;
    }

  //debug output
  //   std::cout << "sc:  " << par1 << ", "
  //                        << pos1 << ", "
  //                        << par2 << ", "
  //                        << pos2 << ", " << std::endl;

  //make sure a selection contains an object completely or not at all
  //TODO
  bool iio1 = isInObject(par1, pos1);
  bool iio2 = isInObject(par2, pos2);

  //   std::cout << iio1 << " " << iio2 << std::endl;

  //if both borders are outside do nothing.

  //if at least one is inside clear selection
  if (iio1 || iio2)
    removeSelection();

  //TODO: right now the any invalid selection is just cleared.
  //in some cases it would be nicer for the user if it would be
  //extended instead

  getSelection(&mOldPar1, &mOldPos1, &mOldPar2, &mOldPos2);
}

/**
  *  This slot checks any characters that are newly typed on Expression Widget.
  */
void CQExpressionWidget::slotTextChanged()
{
  //  std::cout << "CQEW::slotTextChanged()" << std::endl;
  int pos = 0;
  QString Expression = FROM_UTF8(getExpression());
  emit valid(mpValidator->validate(Expression, pos) == QValidator::Acceptable);
}

void CQExpressionWidget::slotCursorPositionChanged(int para, int pos)
{
  //debug output
  //std::cout << "cpc: " << para << ", " << pos << " . " << isInObject() << std::endl;

  //check if we are inside an object
  if (isInObject(para, pos))
    {
      int newpos;
      //first decide in which direction we want to leave the object
      if (compareCursorPositions(mOldPar, mOldPos, para, pos))
        {
          //move right
          newpos = text(para).find(">", pos);
          if (newpos != -1)
            setCursorPosition(para, newpos + 1);
        }
      else
        {
          //move left
          newpos = text(para).findRev("<", pos);
          if (newpos != -1)
            setCursorPosition(para, newpos);
        }
    }

  getCursorPosition(&mOldPar, &mOldPos);
}

bool CQExpressionWidget::isInObject()
{
  int para, pos;
  getCursorPosition(&para, &pos);
  return isInObject(para, pos);

  /*  //the following code assumes the presence of the syntax highlighter
    if (color() == QColor(0,0,0)) return false;

    if (pos==0) return false;

    QString t = text(para);
    if (t[pos-1] == '>') return false;

    return true;*/
}

bool CQExpressionWidget::isInObject(int par, int pos)
{
  if (pos == 0) return false;

  bool result = false;

  QString tmp = text(par);
  //std::cout << "iio? " << par << " " << pos << std::endl;

  //first look to the left
  int lo, lc;
  lo = tmp.findRev('<', pos - 1);
  lc = tmp.findRev('>', pos - 1);
  while (lc > 0 && tmp[lc - 1] == '\\')
    lc = tmp.findRev('>', lc - 1);
  //std::cout << "left:"  << lo << " " << lc  << std::endl;

  if ((lo == -1) && (lc == -1))
    result = false;
  else if (lc == -1)
    result = true;
  else if (lo == -1)
    {
      //std::cout << "inconsistent expression!" << std::endl;
      result = false;
    }
  else if (lo < lc)
    result = false;
  else // lo > lc
    result = true;

  //TODO: we could implement a consistency check by trying to find the same
  //information from looking to the right.

  return result;
}

bool CQExpressionWidget::compareCursorPositions(int parold, int posold, int par, int pos)
{
  if (par > parold) return true;
  if (par < parold) return false;

  //we are in the same paragraph
  if (pos > posold) return true;
  return false;
}

void CQExpressionWidget::doKeyboardAction(QTextEdit::KeyboardAction action)
{
  int para, pos;
  getCursorPosition(&para, &pos);

  //handle backspace and delete. All other actions are ignored
  switch (action)
    {
    case QTextEdit::ActionBackspace:
      if (pos == 0) return;
      if (text(para)[pos - 1] == '>')
        {
          //std::cout << "Backspace into object." << std::endl;
          QString tmp = text(para);
          int left = tmp.findRev('<', pos);
          setSelection(para, left, para, pos);
          removeSelectedText();
          //std::cout << pos << " " << left << std::endl;
        }
      else
        QTextEdit::doKeyboardAction(action);
      break;

    case QTextEdit::ActionDelete:
      if ((unsigned int) pos == text().length()) return;
      if (text(para)[pos] == '<')
        {
          //std::cout << "Delete into object." << std::endl;
          QString tmp = text(para);
          int right = tmp.find('>', pos);
          setSelection(para, pos, para, right + 1);
          removeSelectedText();
          //std::cout << pos << " " << right << std::endl;
        }
      else
        QTextEdit::doKeyboardAction(action);
      break;

    default:
      QTextEdit::doKeyboardAction(action);
      break;
    }
}

void CQExpressionWidget::setExpression(const std::string & expression)
{
  // Reset the parse list.
  mParseList.clear();

  std::string Expression = expression;
  std::string out_str = "";

  unsigned C_INT32 i = 0;

  while (i < Expression.length())
    {
      if (Expression[i] == '<')
        {
          i++;
          std::string objectName = "";

          while (Expression[i] != '>' && i < Expression.length())
            {
              if (Expression[i] == '\\')
                objectName += Expression[i++];

              objectName += Expression[i];
              i++;
            }

          CCopasiObjectName temp_CN(objectName);
          CCopasiObject * temp_object = const_cast<CCopasiObject *>(RootContainer.getObject(temp_CN));
          if (temp_object != NULL)
            {
              std::string DisplayName = temp_object->getObjectDisplayName();
              mParseList[DisplayName] = temp_object;

              // We need to escape >
              std::string::size_type pos = DisplayName.find_first_of("\\>");
              while (pos != std::string::npos)
                {
                  DisplayName.insert(pos, "\\");
                  pos += 2;
                  pos = DisplayName.find_first_of("\\>", pos);
                }

              out_str += "<" + DisplayName + ">";
            }
          continue;
        }

      else if (Expression[i] == '>')
        {
          //do nothing
        }

      else
        {
          out_str += Expression[i];
        }

      i++;
    }

  setText(FROM_UTF8(out_str));

  mpValidator->saved();

  return;
}

std::string CQExpressionWidget::getExpression() const
  {
    std::string DisplayName = "";
    std::string InfixCN = "";

    std::string InfixDispayName = (const char *)text().utf8();
    std::map< std::string, const CCopasiObject *>::const_iterator it;

    unsigned int i;
    for (i = 0; i < InfixDispayName.length(); i++)
      {
        InfixCN += InfixDispayName[i];
        DisplayName = "";

        if (InfixDispayName[i] == '<')
          {
            i++;
            while (i < InfixDispayName.length() && InfixDispayName[i] != '>')
              {
                if (InfixDispayName[i] == '\\') // '\' is an escape character.
                  i++;

                DisplayName += InfixDispayName[i++];
              }

            it = mParseList.find(DisplayName);

            if (it != mParseList.end())
              InfixCN += it->second->getCN() + ">";
            else
              InfixCN = InfixCN.substr(0, InfixCN.length() - 1);
          }
      }

    return InfixCN;
  }
/*
CExpression *CQExpressionWidget::getExpression()
{
//  return const_cast< CExpression * >(&mExpression);
  return &(mpValidator->mExpression);
}*/

void CQExpressionWidget::setExpressionType(const CCopasiSimpleSelectionTree::SelectionFlag & expressionType)
{
  mExpressionType = expressionType;
}

void CQExpressionWidget::slotSelectObject()
{
  const CCopasiObject * pObject =
    CCopasiSelectionDialog::getObjectSingle(this, mExpressionType);

  if (pObject)
    {
      // Check whether the object is valid
      if (!CCopasiSimpleSelectionTree::filter(mExpressionType, pObject))
        {
          CQMessageBox::critical(this, "Invalid Selection",
                                 "The use of the selected object is not allowed in this type of expression.");
          return;
        }

      std::string Insert = pObject->getObjectDisplayName();
      mParseList[Insert] = pObject;

      // We need to escape >
      std::string::size_type pos = Insert.find_first_of("\\>");
      while (pos != std::string::npos)
        {
          Insert.insert(pos, "\\");
          pos += 2;
          pos = Insert.find_first_of("\\>", pos);
        }

      insert(FROM_UTF8("<" + Insert + ">"));
    }
}
