/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/copasiWidget.h,v $
   $Revision: 1.17.2.1 $
   $Name:  $
   $Author: ssahle $
   $Date: 2006/05/20 01:44:37 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/********************************************************
 Author: Liang Xu
 Version : 1.xx  <first>
 Description:
 Date: 04/03
 Comment : CopasiWidget: Provide base class for all widget: for resizable/minimize
 Contact: Please contact lixu1@vt.edu.
 *********************************************************/

#if !defined COPASI_WIDGET_H
#define COPASI_WIDGET_H
#include <qwidget.h>
#include "listviews.h"

class DataModelGUI;
class QResizeEvent;

class CopasiWidget : public QWidget
  {
  public:
    CopasiWidget(QWidget * parent = 0, const char * name = 0, WFlags f = 0);
    virtual bool update(ListViews::ObjectType objectType, ListViews::Action action, const std::string & key);
    virtual bool leave();
    virtual bool enter(const std::string & key = "");

  protected:
    bool protectedNotify(ListViews::ObjectType objectType, ListViews::Action action, const std::string & key = "");
    bool mIgnoreUpdates;

    static DataModelGUI* dataModel;
    ListViews* pListView;
  };

#endif // !defined(COPASI_WIDGET_H)
