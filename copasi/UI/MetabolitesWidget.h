// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/Attic/MetabolitesWidget.h,v $
//   $Revision: 1.35 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2008/12/18 19:57:54 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/****************************************************************************
 **  $ CopasiUI/CompartmentsWidget.h
 **  $ Author  : Mudita Singhal
 **
 ** This is the header file for the Compartments Widget, i.e the First level
 ** of Compartments.
 *****************************************************************************/

#ifndef METABOLITES_WIDGET_H
#define METABOLITES_WIDGET_H

#include "UI/CopasiTableWidget.h"

class CCompartment;

class MetabolitesWidget : public CopasiTableWidget
  {
    Q_OBJECT

  public:
    MetabolitesWidget(QWidget *parent, const char * name = 0, Qt::WFlags f = 0)
        : CopasiTableWidget(parent, false, name, f)
    {init();}

    virtual void setFramework(int framework);

  protected:

    bool mFlagConc;

    /**
     * This initializes the widget
     */
    virtual void init();

    /**
     * shows units for the quantities
     */
    virtual void updateHeaderUnits();

    /**
     * returns a list of objects that should be displayed
     */
    virtual std::vector<const CCopasiObject*> getObjects() const;

    /**
     * fills one table row with the data from one object
     */
    virtual void tableLineFromObject(const CCopasiObject* obj, unsigned C_INT32 row);

    /**
     * reads the contents of one row of the table and writes it to the object
     */
    virtual void tableLineToObject(unsigned C_INT32 row, CCopasiObject* obj);

    /**
     * creates a new object
     */
    virtual CCopasiObject* createNewObject(const std::string & name);

    /**
     * deletes objects. Performs all additional tasks, like asking the user, ...
     */
    virtual void deleteObjects(const std::vector<std::string> & keys);

    /**
     * this is used to fill a row of the table when a new object is added to the table.
     * it fills only the data columns, not the name. It should not fill column exc.
     */
    virtual void defaultTableLineContent(unsigned C_INT32 row, unsigned C_INT32 exc);

    /**
     * the prefix that is used to construct new object names
     */
    virtual QString defaultObjectName() const;

    /**
     * This method provides a hook for derived classes to act on changes in
     * the table.
     * @param unsigned C_INT32 row
     * @param unsigned C_INT32 col
     */
    virtual void valueChanged(unsigned C_INT32 row, unsigned C_INT32 col);

  private:
    /**
     * Called when the initial concentration changed in the table.
     * @param unsigned C_INT32 row
     */
    void initialConcentrationChanged(unsigned C_INT32 row);

    /**
     * Called when the initial number changed in the table.
     * @param unsigned C_INT32 row
     */
    void initialNumberChanged(unsigned C_INT32 row);

    /**
     * Called when the compartment selection changed in the table.
     * @param unsigned C_INT32 row
     */
    void compartmentChanged(unsigned C_INT32 row);

    /**
     * A list containing the possible selections for type
     */
    QStringList mTypes;

    /**
     * A vector mapping the item index to a model valu type
     */
    std::vector< unsigned C_INT32 > mItemToType;
  };

#endif