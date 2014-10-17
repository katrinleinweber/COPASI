/*
 * RemoveAllEventRowsCommand.cpp
 *
 *  Created on: 14 Oct 2014
 *      Author: dada
 */

#include "report/CCopasiRootContainer.h"
#include "model/CEvent.h"
#include "model/CModel.h"
#include "UI/CQEventDM.h"

#include "UndoEventData.h"
#include "RemoveAllEventRowsCommand.h"

RemoveAllEventRowsCommand::RemoveAllEventRowsCommand(CQEventDM * pEventDM, const QModelIndex&) {
	mpEventDM = pEventDM;

	assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
	CCopasiDataModel* pDataModel = (*CCopasiRootContainer::getDatamodelList())[0];
	assert(pDataModel != NULL);
	CModel * pModel = pDataModel->getModel();

	assert(pModel != NULL);

	for (int i = 0; i != pEventDM->rowCount()-1; ++i)
	{
		UndoEventData *data = new UndoEventData();
		if (pModel->getEvents()[i]){
			data->setName(pModel->getEvents()[i]->getObjectName());
			data->setPriorityExpression(pModel->getEvents()[i]->getPriorityExpression());
			data->setDelayExpression(pModel->getEvents()[i]->getDelayExpression());
			data->setTriggerExpression(pModel->getEvents()[i]->getTriggerExpression());

			CCopasiVector< CEventAssignment >::const_iterator it = pModel->getEvents()[i]->getAssignments().begin();
			CCopasiVector< CEventAssignment >::const_iterator end = pModel->getEvents()[i]->getAssignments().end();

			for (; it != end; ++it)
			{
				CEventAssignment *eventAssign = new CEventAssignment((*it)->getTargetKey(), pModel->getEvents()[i]->getObjectParent());
				eventAssign->setExpression((*it)->getExpression());
				data->getAssignments()->append(eventAssign);
			}

			mpEventData.append(data);
		}

	}

	this->setText(removeAllEventRowsText());
}

void RemoveAllEventRowsCommand::redo(){
	mpEventDM->removeAllEventRows();
}

void RemoveAllEventRowsCommand::undo(){
	mpEventDM->insertEventRows(mpEventData);
}

QString RemoveAllEventRowsCommand::removeAllEventRowsText() const {
	return QObject::tr(": Removed All Events");
}

RemoveAllEventRowsCommand::~RemoveAllEventRowsCommand() {
	// TODO Auto-generated destructor stub
}

