/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiUI/Attic/CScanWidgetRepeat.h,v $
   $Revision: 1.3 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:27:42 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/****************************************************************************
 ** Form interface generated from reading ui file 'CScanWidgetRepeat.ui'
 **
 ** Created: Tue Mar 28 14:23:45 2006
 **      by: The User Interface Compiler ($Id: CScanWidgetRepeat.h,v 1.3 2006/04/27 01:27:42 shoops Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#ifndef CSCANWIDGETREPEAT_H
#define CSCANWIDGETREPEAT_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QLabel;
class QLineEdit;
class CCopasiParameterGroup;
class CScanProblem;

class CScanWidgetRepeat : public QWidget
  {
    Q_OBJECT

  public:
    CScanWidgetRepeat(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~CScanWidgetRepeat();

    QFrame* frame;
    QLabel* labelTitle;
    QLabel* labelNumber;
    QLineEdit* lineEditNumber;

    virtual bool initFromScanItem(CCopasiParameterGroup * pg);
    virtual bool saveToScanItem(CScanProblem * pg) const;

  protected:
    QVBoxLayout* CScanWidgetRepeatLayout;
    QHBoxLayout* frameLayout;
    QSpacerItem* spacer1;

  protected slots:
    virtual void languageChange();

  private:
    void init();
  };

#endif // CSCANWIDGETREPEAT_H
