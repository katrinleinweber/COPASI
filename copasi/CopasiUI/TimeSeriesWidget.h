/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiUI/Attic/TimeSeriesWidget.h,v $
   $Revision: 1.3 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:27:46 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef TSWIDGET_H
#define TSWIDGET_H

#include "copasiWidget.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
//class QPushButton;

class TimeSeriesSubWidget;

class TimeSeriesWidget : public CopasiWidget
  {
    Q_OBJECT

  public:
    TimeSeriesWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~TimeSeriesWidget();

    virtual bool update(ListViews::ObjectType objectType, ListViews::Action action, const std::string & key);
    virtual bool leave();
    virtual bool enter(const std::string & key = "");

  protected slots:
    //virtual void slotBtnCancelClicked();
    //virtual void slotBtnOKClicked();

  protected:
    bool loadFromBackend();
    bool saveToBackend();
    std::string objKey;

    QGridLayout* mWidgetLayout;

    TimeSeriesSubWidget* mCentralWidget;
  };

#endif
