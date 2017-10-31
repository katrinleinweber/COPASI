// Copyright (C) 2017 - 2018 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2006 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "CQModelValue.h"

#include "CQExpressionWidget.h"

#include "UI/CQMessageBox.h"

#include "copasi.h"

#include "qtUtilities.h"
#include "CQValidatorUnit.h"
#include "copasiui3window.h"
#include "CQDependencyWidget.h"
#include "CQScrolledDependenciesWidget.h"

#include "copasi/model/CModel.h"
#include "copasi/model/CModelValue.h"
#include "copasi/function/CExpression.h"
#include "copasi/report/CKeyFactory.h"
#include "copasi/core/CRootContainer.h"
#include "copasi/CopasiDataModel/CDataModel.h"

//UNDO framework classes
#include "copasi/model/CReactionInterface.h"
//#include "undoFramework/GlobalQuantityTypeChangeCommand.h"
#include "copasi/undoFramework/UndoEventData.h"
#include "copasi/undoFramework/UndoEventAssignmentData.h"

/*
 *  Constructs a CQModelValue which is a child of 'parent', with the
 *  name 'name'.'
 */
CQModelValue::CQModelValue(QWidget* parent, const char* name)
  : CopasiWidget(parent, name),
    mObjectCNToCopy(""),
    mpModelValue(NULL),
    mpDependencies(NULL)
{
  setupUi(this);

  init();

  CopasiUI3Window *  pWindow = dynamic_cast<CopasiUI3Window * >(parent->parent());
  setUndoStack(pWindow->getUndoStack());
  mpLblType->setMinimumWidth(90);

  mpDependencies = new CQScrolledDependenciesWidget(this);
  mpDependencies->setVisibleDependencies(REACTION | EVENT | SPECIES | PARAMETERS | COMPARTMENT);
  mpDependencies->setLabelWidth(mpLblValue->width() - 6);
  gridLayout->addWidget(mpDependencies, gridLayout->rowCount(), 0, 1, -1);
}

/*
 *  Destroys the object and frees any allocated resources
 */
CQModelValue::~CQModelValue()
{
  destroy();
  // no need to delete child widgets, Qt does it all for us
}

/// Slot to create a new quantity; activated whenever the New button is clicked
void CQModelValue::slotBtnNew()
{
  leave();

  std::string name = "quantity";
  int i = 1;

  assert(mpDataModel != NULL);

  while (!(mpModelValue = mpDataModel->getModel()->createModelValue(name)))
    {
      i++;
      name = "quantity_";
      name += TO_UTF8(QString::number(i));
    }

  slotNotifyChanges(mpDataModel->recordData(CUndoData(CUndoData::Type::INSERT, mpModelValue)));

  mpListView->switchToOtherWidget(C_INVALID_INDEX, mpModelValue->getCN());
}

void CQModelValue::slotBtnCopy()
{
  mObjectCNToCopy = mObjectCN;
}

void CQModelValue::slotBtnDelete()
{
  if (mpModelValue == NULL) return;

  QMessageBox::StandardButton choice =
    CQMessageBox::confirmDelete(this, "quantity",
                                FROM_UTF8(mpModelValue->getObjectName()),
                                mpModelValue);

  if (choice == QMessageBox::Ok)
    {
      CUndoData UndoData;
      mpModelValue->createUndoData(UndoData, CUndoData::Type::REMOVE);

      slotNotifyChanges(mpDataModel->applyData(UndoData));
    }
}

/*!
    If the simulation type is changed then COPASI will automatically adjust its appearance,
    especially correlating to the Expression Widget and its buttons.
 */
void CQModelValue::slotTypeChanged(const QString & type)
{
  switch (CModelEntity::StatusName.toEnum(TO_UTF8(type), CModelEntity::Status::FIXED))
    {
      case CModelEntity::Status::FIXED:
        // hide label, widget, and all buttons
        mpLblExpression->hide();
        mpExpressionEMW->hide();

        // enable the option of use Initial Expression
        mpBoxUseInitialExpression->setEnabled(true);
        slotInitialTypeChanged(mpBoxUseInitialExpression->isChecked());

        mpBoxAddNoise->hide();
        slotAddNoiseChanged(false);

        break;

      case CModelEntity::Status::ASSIGNMENT:
        // show label, widget, and correct buttons
        mpLblExpression->show();   // show the label
        mpExpressionEMW->show();  // show the widget

        // disable the option of use Initial Expression
        mpBoxUseInitialExpression->setEnabled(false);
        slotInitialTypeChanged(false);

        // update the expression widget
        mpExpressionEMW->updateWidget();

        mpBoxAddNoise->hide();
        slotAddNoiseChanged(false);

        break;

      case CModelEntity::Status::ODE:
        // show label, widget, and correct buttons
        mpLblExpression->show();   // show the label
        mpExpressionEMW->show();  // show the widget

        // enable the option of use Initial Expression
        mpBoxUseInitialExpression->setEnabled(true);
        slotInitialTypeChanged(mpBoxUseInitialExpression->isChecked());

        // update the expression widget
        mpExpressionEMW->updateWidget();

#ifdef WITH_SDE_SUPPORT
        mpBoxAddNoise->show();
        slotAddNoiseChanged(mpBoxAddNoise->isChecked());
#else
        mpBoxAddNoise->hide();
        slotAddNoiseChanged(false);
#endif

        break;

      default:
        break;
    }
}

void CQModelValue::slotAddNoiseChanged(bool hasNoise)
{
  if (hasNoise)
    {
      if (mpModelValue != NULL &&
          mpModelValue->getNoiseExpression().empty())
        {
          mpNoiseExpressionWidget->mpExpressionWidget->setExpression(mpModelValue->getDefaultNoiseExpression());
        }

      mpLblNoiseExpression->show();
      mpNoiseExpressionWidget->show();
      mpNoiseExpressionWidget->updateWidget();
    }
  else
    {
      mpLblNoiseExpression->hide();
      mpNoiseExpressionWidget->hide();
    }
}

void CQModelValue::init()
{
  mpComboBoxType->blockSignals(true);
  mpComboBoxType->insertItem(mpComboBoxType->count(), FROM_UTF8(CModelEntity::StatusName[CModelEntity::Status::FIXED]));
  mpComboBoxType->insertItem(mpComboBoxType->count(), FROM_UTF8(CModelEntity::StatusName[CModelEntity::Status::ASSIGNMENT]));
  mpComboBoxType->insertItem(mpComboBoxType->count(), FROM_UTF8(CModelEntity::StatusName[CModelEntity::Status::ODE]));
  mpComboBoxType->blockSignals(false);

  mpExpressionEMW->mpExpressionWidget->setExpressionType(CQExpressionWidget::TransientExpression);
  mpInitialExpressionEMW->mpExpressionWidget->setExpressionType(CQExpressionWidget::InitialExpression);
  mpNoiseExpressionWidget->mpExpressionWidget->setExpressionType(CQExpressionWidget::TransientExpression);

  mpEditUnits->setValidator(new CQValidatorUnit(mpEditUnits));
}

void CQModelValue::destroy()
{}

bool CQModelValue::updateProtected(ListViews::ObjectType objectType, ListViews::Action action, const CCommonName & cn)
{
  switch (objectType)
    {
      case ListViews::MODEL:

        // For a new model we need to remove references to no longer existing modelvalue
        if (action != ListViews::CHANGE)
          {
            mObjectCN.clear();
            mpObject = NULL;
            mpModelValue = NULL;
          }

        break;

      case ListViews::MODELVALUE:

        // If the currently displayed modelvalue is deleted we need to remove its references.
        if (action == ListViews::DELETE && mObjectCN == cn)
          {
            mObjectCN.clear();
            mpObject = NULL;
            mpModelValue = NULL;
          }

        break;

      case ListViews::STATE:
        break;

      default:
        return true;
        break;
    }

  if (isVisible() && !mIgnoreUpdates)
    load();

  return true;
}

bool CQModelValue::leave()
{
  if (TO_UTF8(mpComboBoxType->currentText()) != CModelEntity::StatusName[CModelEntity::Status::FIXED])
    {
      // -- Expression --
      mpExpressionEMW->updateWidget();
    }

  if (mpBoxUseInitialExpression->isChecked())
    {
      // -- Initial Expression --
      mpInitialExpressionEMW->updateWidget();
    }

  save();

  return true;
}

bool CQModelValue::enterProtected()
{
  mpModelValue = NULL;

  if (mObjectCNToCopy != "")
    {
      CObjectInterface::ContainerList List;
      List.push_back(mpDataModel);

      // This will check the current data model and the root container for the object;
      mpModelValue = dynamic_cast<CModelValue *>(const_cast< CDataObject * >(CObjectInterface::DataObject(CObjectInterface::GetObjectFromCN(List, mObjectCNToCopy))));
      mObjectCNToCopy.clear();
    }
  else
    {
      mpModelValue = dynamic_cast<CModelValue *>(mpObject);
    }

  if (!mpModelValue)
    {
      mpListView->switchToOtherWidget(115, std::string());
      return false;
    }

  load();

  mpModelValue = dynamic_cast<CModelValue *>(mpObject);

  return true;
}

// virtual
bool CQModelValue::event(QEvent * pEvent)
{
  if (pEvent->type() == QEvent::LayoutRequest &&
      mpLblValue->width() - 6 != mpDependencies->getLabelWidth())
    {
      mpDependencies->setLabelWidth(mpLblValue->width() - 6);
    }

  return CopasiWidget::event(pEvent);
}

/*!
    Load any saved values to the screen
 */
void CQModelValue::load()
{
  if (mpModelValue == NULL) return;

  // Unit expression
  mpEditUnits->setText(FROM_UTF8(mpModelValue->getUnitExpression()));

  // Update the unit labels
  slotUnitChanged();

  // Type
  mpComboBoxType->setCurrentIndex(mpComboBoxType->findText(FROM_UTF8(CModelEntity::StatusName[mpModelValue->getStatus()])));

  // Initial Value
  mpEditInitialValue->setText(convertToQString(mpModelValue->getInitialValue()));

  // Current Value
  mpEditCurrentValue->setText(convertToQString(mpModelValue->getValue()));

  // Rate
  mpEditRate->setText(convertToQString(mpModelValue->getRate()));

  // Expression
  mpExpressionEMW->mpExpressionWidget->setExpression(mpModelValue->getExpression());

  // Update Expression Widget
  mpExpressionEMW->updateWidget();

  // Initial Expression
  mpInitialExpressionEMW->mpExpressionWidget->setExpression(mpModelValue->getInitialExpression());

  // Update Initial Expression Widget
  mpInitialExpressionEMW->updateWidget();

  // Noise Expression
  mpNoiseExpressionWidget->mpExpressionWidget->setExpression(mpModelValue->getNoiseExpression());
  mpNoiseExpressionWidget->updateWidget();
  mpBoxAddNoise->setChecked(mpModelValue->hasNoise());

  // Type dependent display of values
  slotTypeChanged(mpComboBoxType->currentText());

  // Use Initial Expression
  if (mpModelValue->getStatus() == CModelEntity::Status::ASSIGNMENT ||
      mpModelValue->getInitialExpression() == "")
    {
      mpBoxUseInitialExpression->setChecked(false);
      //      slotInitialTypeChanged(false);
    }
  else
    {
      mpBoxUseInitialExpression->setChecked(true);
      //      slotInitialTypeChanged(true);
    }

  mpDependencies->loadFrom(mpObject);

  mChanged = false;
}

/*!
    Save any inputs and set them as initial setting values
 */
void CQModelValue::save()
{
  if (mpModelValue == NULL) return;

  mIgnoreUpdates = true;

  CData OldData(mpModelValue->toData());

  // set status
  if (CModelEntity::StatusName[mpModelValue->getStatus()] != TO_UTF8(mpComboBoxType->currentText()))
    {
      mpModelValue->setStatus(CModelEntity::StatusName.toEnum(TO_UTF8(mpComboBoxType->currentText()), CModelEntity::Status::FIXED));
      mChanged = true;
    }

  // set initial value
  if (convertToQString(mpModelValue->getInitialValue()) != mpEditInitialValue->text() &&
      mpModelValue->getStatus() != CModelEntity::Status::ASSIGNMENT)
    {
      mpModelValue->setInitialValue(mpEditInitialValue->text().toDouble());
      mChanged = true;
    }

  // set expression
  if (mpModelValue->getExpression() != mpExpressionEMW->mpExpressionWidget->getExpression())
    {
      mpModelValue->setExpression(mpExpressionEMW->mpExpressionWidget->getExpression());
      mChanged = true;
    }

  // set initial expression
  if (mpModelValue->getStatus() != CModelEntity::Status::ASSIGNMENT)
    {
      if (mpBoxUseInitialExpression->isChecked() &&
          mpModelValue->getInitialExpression() != mpInitialExpressionEMW->mpExpressionWidget->getExpression())
        {
          mpModelValue->setInitialExpression(mpInitialExpressionEMW->mpExpressionWidget->getExpression());
          mChanged = true;
        }
      else if (!mpBoxUseInitialExpression->isChecked() &&
               mpModelValue->getInitialExpression() != "")
        {
          mpModelValue->setInitialExpression("");
          mChanged = true;
        }
    }

  // Add Noise
  if (mpModelValue->hasNoise() != mpBoxAddNoise->isChecked())
    {
      mpModelValue->setHasNoise(mpBoxAddNoise->isChecked());
      mChanged = true;
    }

  // Noise Expression
  if (mpModelValue->getNoiseExpression() != mpNoiseExpressionWidget->mpExpressionWidget->getExpression())
    {
      mpModelValue->setNoiseExpression(mpNoiseExpressionWidget->mpExpressionWidget->getExpression());
      mChanged = true;
    }

  // set unit
  if (mpModelValue->getUnitExpression() != TO_UTF8(mpEditUnits->text()))
    {
      mpModelValue->setUnitExpression(TO_UTF8(mpEditUnits->text()));
      mChanged = true;
    }

  mIgnoreUpdates = false;

  if (mChanged)
    {
      CUndoData UndoData;
      mpModelValue->createUndoData(UndoData, CUndoData::Type::CHANGE, OldData, static_cast< CCore::Framework >(mFramework));

      slotNotifyChanges(mpDataModel->applyData(UndoData));
      load();
    }

  mChanged = false;
}

/*!
    If the initial expression is chosen to be used by checking the mpBoxUseInitialExpression check box being represented by
    the boolean parameter useInitialAssignment (true if checked; false otherwise), COPASI will show the Initial Expression
    Widget with its correct push buttons. Otherwise, the widget and its buttons will be hidden.
 */
void CQModelValue::slotInitialTypeChanged(bool useInitialAssignment)
{
  if (useInitialAssignment)  // use Initial Expression (ie. the mpBoxUseInitialExpression is checked)
    {
      // show label, widget, and the correct buttons
      mpLblInitialExpression->show();  // show the label
      mpInitialExpressionEMW->show(); // show the widget

      // enable the option of use Initial Value
      mpEditInitialValue->setEnabled(false);

      // update the Initial Expression Widget
      mpInitialExpressionEMW->updateWidget();
    }
  else  // mpBoxUseInitialExpression is not checked
    {
      // hide label, widget, and all buttons
      mpLblInitialExpression->hide();
      mpInitialExpressionEMW->hide();

      // enable the option of use Initial Value
      mpEditInitialValue->setEnabled(TO_UTF8(mpComboBoxType->currentText()) != CModelEntity::StatusName[CModelEntity::Status::ASSIGNMENT]);

      // we don't need to update the Initial Expression Widget
    }
}

void CQModelValue::slotUnitChanged()
{
  // Update the labels to reflect the model units
  QString ValueUnits = "";
  QString RateUnits = "";

  if (!mpEditUnits->text().isEmpty())
    {
      ValueUnits = " [" + FROM_UTF8(CUnit::prettyPrint(TO_UTF8(mpEditUnits->text()))) + "]";
      RateUnits = " [" + FROM_UTF8(CUnit::prettyPrint(std::string(TO_UTF8(mpEditUnits->text())) + "/(" + mpDataModel->getModel()->getTimeUnit() + ")")) + "]";
    }

  mpLblInitialValue->setText("Initial Value" + ValueUnits);
  mpLblValue->setText("Value" + ValueUnits);
  mpLblRate->setText("Rate" + RateUnits);
}
