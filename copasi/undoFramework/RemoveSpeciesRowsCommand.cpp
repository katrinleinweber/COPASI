// Copyright (C) 2015 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

/*
 * RemoveSpecieRowsCommand.cpp
 *
 *  Created on: 28 Aug 2014
 *      Author: dada
 */

#include "report/CCopasiRootContainer.h"
#include "model/CMetab.h"
#include "model/CReactionInterface.h"
#include "model/CModel.h"
#include "CQSpecieDM.h"
#include "function/CFunctionDB.h"

#include "UndoSpeciesData.h"
#include "UndoReactionData.h"
#include "UndoEventData.h"
#include "UndoGlobalQuantityData.h"
#include "RemoveSpeciesRowsCommand.h"

RemoveSpecieRowsCommand::RemoveSpecieRowsCommand(
  QModelIndexList rows, CQSpecieDM * pSpecieDM, const QModelIndex&)
  : CCopasiUndoCommand("Species", SPECIES_REMOVE)
  , mpSpecieDM(pSpecieDM)
  , mRows(rows)
  , mpSpeciesData()
  , mFirstTime(true)
{
  GET_MODEL_OR_RETURN(pModel);

  QModelIndexList::const_iterator i;

  for (i = rows.begin(); i != rows.end(); ++i)
    {
      UndoSpeciesData *data = new UndoSpeciesData();

      if (!pSpecieDM->isDefaultRow(*i) && pModel->getMetabolites()[(*i).row()])
        {
          //  mpReactionData = new  QList <UndoReactionData*>();
          //  mpGlobalQuantityData = new  QList <UndoGlobalQuantityData*>();
          //  mpEventData = new  QList <UndoEventData*>();
          data->setName(pModel->getMetabolites()[(*i).row()]->getObjectName());
          data->setIConc(pModel->getMetabolites()[(*i).row()]->getInitialConcentration());
          data->setCompartment(pModel->getMetabolites()[(*i).row()]->getCompartment()->getObjectName());
          data->setStatus(pModel->getMetabolites()[(*i).row()]->getStatus());

          if (pModel->getMetabolites()[(*i).row()]->getStatus() != CModelEntity::ASSIGNMENT)
            {
              data->setIConc(pModel->getMetabolites()[(*i).row()]->getInitialConcentration());
            }

          if (pModel->getMetabolites()[(*i).row()]->getStatus() ==  CModelEntity::ASSIGNMENT || pModel->getMetabolites()[(*i).row()]->getStatus() == CModelEntity::ODE)
            {
              data->setExpression(pModel->getMetabolites()[(*i).row()]->getExpression());
            }

          // set initial expression
          if (pModel->getMetabolites()[(*i).row()]->getStatus() != CModelEntity::ASSIGNMENT)
            {
              data->setInitialExpression(pModel->getMetabolites()[(*i).row()]->getInitialExpression());
            }

          setDependentObjects(pModel->getMetabolites()[(*i).row()]->getDeletedObjects());
          data->setReactionDependencyObjects(getReactionData());
          data->setGlobalQuantityDependencyObjects(getGlobalQuantityData());
          data->setEventDependencyObjects(getEventData());

          mpSpeciesData.append(data);
        }
    }

  this->setText(removeSpecieRowsText());
}

void RemoveSpecieRowsCommand::redo()
{
  if (mFirstTime)
    {
      mpSpecieDM->removeSpecieRows(mRows, QModelIndex());
      mFirstTime = false;
    }
  else
    {
      mpSpecieDM->deleteSpecieRows(mpSpeciesData);
    }

  setUndoState(true);
  setAction("Delete set");
}

void RemoveSpecieRowsCommand::undo()
{
  mpSpecieDM->insertSpecieRows(mpSpeciesData);
  setUndoState(false);
  setAction("Undelete set");
}

QString RemoveSpecieRowsCommand::removeSpecieRowsText() const
{
  return QObject::tr(": Removed Species");
}

RemoveSpecieRowsCommand::~RemoveSpecieRowsCommand()
{
  // freeing the memory allocated above
  foreach(UndoSpeciesData * data, mpSpeciesData)
  {
    pdelete(data);
  }
  mpSpeciesData.clear();
}
