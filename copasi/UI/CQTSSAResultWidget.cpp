// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
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

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include <QGridLayout>
#include "copasi.h"

#include "CQTSSAResultWidget.h"
#include "CQTSSAResultSubWidget.h"
#include "CopasiDataModel/CDataModel.h"
#include "copasi/core/CRootContainer.h"
#include "copasi/core/CDataVector.h"
#include "tssanalysis/CTSSATask.h"

//#include "report/CKeyFactory.h"
#include "qtUtilities.h"
#include "tssanalysis/CTSSAMethod.h"

#include "tssanalysis/CTSSATask.h"

CTSSATask* pTask;
CTSSAMethod* pTSSILDM;

/*
 *  Constructs a CQTSSAResultWidget which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
CQTSSAResultWidget::CQTSSAResultWidget(QWidget* parent, const char* name, Qt::WindowFlags fl)
  : CopasiWidget(parent, name, fl)
{
  if (!name)
    setObjectName("CQTSSAResultWidget");

  setWindowTitle(trUtf8("CQTSSAResultWidget"));

  mWidgetLayout = new QGridLayout(this);
  mWidgetLayout->setObjectName("Layout");

  mCentralWidget = new CQTSSAResultSubWidget(this);
  mCentralWidget->setObjectName("CQTSSAResultSubWidget");
  mWidgetLayout->addWidget(mCentralWidget, 0, 0);

}

/*
 *  Destroys the object and frees any allocated resources
 */
CQTSSAResultWidget::~CQTSSAResultWidget()
{

}

CQTSSAResultSubWidget *
CQTSSAResultWidget::getSubWidget()
{
  return mCentralWidget;
}

bool CQTSSAResultWidget::enterProtected()
{
  if (!isVisible())
    return true;

  CDataModel* pDataModel = getDataModel();
  assert(pDataModel != NULL);
  pTask =
    dynamic_cast<CTSSATask *>(&pDataModel->getTaskList()->operator[]("Time Scale Separation Analysis"));
  pTSSILDM = dynamic_cast<CTSSAMethod*>(pTask->getMethod());

  if (!pTSSILDM->getCurrentStep())
    {
      mCentralWidget->discardOldResults();
    }

  return true;
}
