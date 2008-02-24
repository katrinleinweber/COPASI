// Begin CVS Header
//   $Source: /home/cvs/copasi_dev/cvs_admin/addHeader,v $
//   $Revision: 1.8 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2008/01/15 17:42:13 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

/****************************************************************************
 ** Form interface generated from reading ui file 'CQTSSAResultSubWidget.ui'
 **
 ** Created: Fri Jan 25 10:52:35 2008
 **      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#ifndef CQTSSARESULTSUBWIDGET_H
#define CQTSSARESULTSUBWIDGET_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>
#include "copasi.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class CTimeSeriesTable;
class CQArrayAnnotationsWidget;
class QComboBox;
class QPushButton;
class QTabWidget;
class QTextEdit;
class QSlider;
class QLabel;

class CQTSSAResultSubWidget : public QWidget
  {
    Q_OBJECT

  public:
    CQTSSAResultSubWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~CQTSSAResultSubWidget();

    QComboBox* comboBox;
    QPushButton* ButtonSaveData;
    QTabWidget* tabWidget2;
    QWidget* tab;
    QTextEdit* optimizationResultText;
    QWidget* tab_2;
    CTimeSeriesTable* dataTable;
    QWidget* TabPage;
    QTabWidget* mTabWidget;
    QWidget* tab_3;
    CQArrayAnnotationsWidget* pArrayWidget;
    QWidget* tab_4;
    CQArrayAnnotationsWidget* pArrayWidget2;
    QWidget* TabPage_2;
    CQArrayAnnotationsWidget* pArrayWidget3;
    QWidget* TabPage_3;
    CQArrayAnnotationsWidget* pArrayWidget4;
    QSlider* mSlider;
    QLabel* mLabel7;
    QLabel* pLabel1;
    QLabel* mLabel2;
    QLabel* mLabel3;
    QLabel* mLabel4;
    QLabel* mLabel5;
    QLabel* mLabel6;

    void displayOptimizationTab(bool displayOptTab);
    void setStepNumber();
    void discardOldResults();
    void setStepSelectionDisabled(bool set);
    void activateTab(int tab);
    bool clear();

  public slots:
    virtual CTimeSeriesTable * table();
    void changeInterval();
    void hideButtons();

  protected:
    QGridLayout* CQTSSAResultSubWidgetLayout;
    QHBoxLayout* toplayout;
    QSpacerItem* spacer;
    QVBoxLayout* tabLayout;
    QVBoxLayout* tabLayout_2;
    QGridLayout* TabPageLayout;
    QSpacerItem* mSpacer;
    QGridLayout* tabLayout_3;
    QGridLayout* tabLayout_4;
    QGridLayout* TabPageLayout_2;
    QGridLayout* TabPageLayout_3;

  protected slots:
    virtual void languageChange();

    void saveDataToFile();
    void toggleView();
    virtual void init();

  private:
    QPixmap image0;
  };

#endif // CQTSSARESULTSUBWIDGET_H
