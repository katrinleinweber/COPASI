/*
 * SpecieInitialValueLostFocusCommand.cpp
 *
 *  Created on: 19 Sep 2014
 *      Author: dada
 */

#include "qtUtilities.h"

#include "model/CMetab.h"
#include "UI/CQSpeciesDetail.h"
#include "model/CCompartment.h"

#include "UndoSpecieData.h"

#include "SpecieInitialValueLostFocusCommand.h"

SpecieInitialValueLostFocusCommand::SpecieInitialValueLostFocusCommand(CQSpeciesDetail *pSpecieDetail) {
	mpSpecieDetail = pSpecieDetail;
	mFirstTime = true;

	mpSpecieData = new UndoSpecieData();
	std::string sName = mpSpecieDetail->mpMetab->getObjectName();
	mpSpecieData->setName(sName);
	mpSpecieData->setIConc(mpSpecieDetail->mInitialConcentration);
	mpSpecieData->setINumber(mpSpecieDetail->mInitialNumber);

	this->setText(specieInitialValueLostFocusText(sName));
}
void SpecieInitialValueLostFocusCommand::redo(){
	if(mFirstTime){
		mpSpecieDetail->specieInitialValueLostFocus();
		mFirstTime = false;
	}else{
		mpSpecieDetail->specieInitialValueLostFocus(mpSpecieData);
	}
}
void SpecieInitialValueLostFocusCommand::undo(){
	mpSpecieDetail->specieInitialValueLostFocus(mpSpecieData);
}
QString SpecieInitialValueLostFocusCommand::specieInitialValueLostFocusText(std::string &name) const {
	std::string myEntityName (": Species Initial Value Change for "+name);
	char* entityName = (char*)myEntityName.c_str();
	return QObject::tr(entityName);
}

SpecieInitialValueLostFocusCommand::~SpecieInitialValueLostFocusCommand() {
	// TODO Auto-generated destructor stub
}

