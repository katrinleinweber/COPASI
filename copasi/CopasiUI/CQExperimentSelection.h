/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiUI/Attic/CQExperimentSelection.h,v $
   $Revision: 1.2 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:27:41 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/****************************************************************************
 ** Form interface generated from reading ui file 'CQExperimentSelection.ui'
 **
 ** Created: Mon Oct 31 11:07:46 2005
 **      by: The User Interface Compiler ($Id: CQExperimentSelection.h,v 1.2 2006/04/27 01:27:41 shoops Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#ifndef CQEXPERIMENTSELECTION_H
#define CQEXPERIMENTSELECTION_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QTable;
class QComboBox;
class CExperimentSet;

class CQExperimentSelection : public QDialog
  {
    Q_OBJECT

  public:
    CQExperimentSelection(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~CQExperimentSelection();

    QPushButton* mpBtnAll;
    QPushButton* mpBtnNone;
    QTable* mpTable;
    QPushButton* mpBtnOK;
    QPushButton* mpBtnCancel;

    virtual void load(QComboBox * pBox, const CExperimentSet * pExperimentSet);

  protected:
    QComboBox * mpBox;

    QVBoxLayout* CQExperimentSelectionLayout;
    QSpacerItem* spacer10;
    QHBoxLayout* mpLayoutTop;
    QHBoxLayout* layout16;
    QHBoxLayout* mpLayoutBottom;

  protected slots:
    virtual void languageChange();

    void slotBtnOK();
    void slotBtnCancel();
    void slotBtnAll();
    void slotBtnNone();

  private:
    void init();
  };

#endif // CQEXPERIMENTSELECTION_H
