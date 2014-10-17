// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQEventDM.h,v $
//   $Revision: 1.3 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2011/03/29 16:17:20 $
// End CVS Header

// Copyright (C) 2011 - 2010 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#ifndef CQEventDM_H
#define CQEventDM_H

#include "model/CModel.h"
#include "CQBaseDataModel.h"

#ifdef COPASI_UNDO
class UndoEventData;
#endif

#define COL_NAME_EVENTS             1
#define COL_TRIGGER_EVENTS          2
#define COL_DELAYED_EVENTS          3
#define COL_DELAY_EXPRESSION_EVENTS 4
#define COL_ASSIGNTARGET_EVENTS     5
#define COL_ASSIGNEXPRESSION_EVENTS 6
#define TOTAL_COLS_EVENTS           7

class CQEventDM : public CQBaseDataModel
{
  Q_OBJECT

#ifdef COPASI_UNDO
  friend class EventDataChangeCommand;
  friend class InsertEventRowsCommand;
#endif


public:
  CQEventDM(QObject *parent = 0);
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole);
  bool removeRows(QModelIndexList rows, const QModelIndex &index = QModelIndex());

  //TODO Undo
#ifdef COPASI_UNDO
  bool eventDataChange(const QModelIndex &index, const QVariant &value, int role);
  void insertNewEventRow(int position, int rows, const QModelIndex&);
  void addEventRow(UndoEventData *pEventData);
  void deleteEventRow(UndoEventData *pEventData);
  bool removeEventRows(QModelIndexList rows, const QModelIndex&);
  bool insertEventRows(QList <UndoEventData *> pGlobalQuatityData);
  void deleteEventRows(QList <UndoEventData *> pEventData);
  bool removeAllEventRows();
  bool clear();

  signals:
  void changeWidget(const size_t & id);
#endif

protected:
  bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
  bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
};

#endif //CQEventDM_H
