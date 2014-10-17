/*
 * InsertEventRowsCommand.h
 *
 *  Created on: 14 Oct 2014
 *      Author: dada
 */

#ifndef INSERTEVENTROWSCOMMAND_H_
#define INSERTEVENTROWSCOMMAND_H_

#include "CCopasiUndoCommand.h"

class InsertEventRowsCommand: public CCopasiUndoCommand {
public:
	InsertEventRowsCommand(int position, int rows, CQEventDM *pEventDM, const QModelIndex&);
	virtual ~InsertEventRowsCommand();
	void redo();
	void undo();
	QString insertRowsText() const;

private:
	CQEventDM* mpEventDM;
	int mRows, mPosition;
	QModelIndex mIndex;
	UndoEventData* mpEventData;
	int firstTime;
};

#endif /* INSERTEVENTROWSCOMMAND_H_ */
