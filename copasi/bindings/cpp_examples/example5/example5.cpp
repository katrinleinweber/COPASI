// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/bindings/cpp_examples/example5/example5.cpp,v $
//   $Revision: 1.2 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2009/04/21 15:44:34 $
// End CVS Header
// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

/**
 * This is an example on how to run an optimization task.
 * And how to access the result of an optimization.
 */

#include <iostream>
#include <string>
#include <set>
#include <math.h>
#include <assert.h>

#define COPASI_MAIN

#include "copasi/copasi.h"
#include "copasi/commandline/COptions.h"
#include "copasi/report/CCopasiContainer.h"
#include "copasi/CopasiDataModel/CCopasiDataModel.h"
#include "copasi/model/CModel.h"
#include "copasi/model/CModelValue.h"
#include "copasi/report/CReport.h"
#include "copasi/report/CReportDefinition.h"
#include "copasi/report/CReportDefinitionVector.h"
#include "copasi/trajectory/CTrajectoryTask.h"
#include "copasi/trajectory/CTrajectoryMethod.h"
#include "copasi/trajectory/CTrajectoryProblem.h"
#include "copasi/optimization/COptTask.h"
#include "copasi/optimization/COptMethod.h"
#include "copasi/optimization/COptProblem.h"
#include "copasi/optimization/COptItem.h"
#include "copasi/report/CCopasiRootContainer.h"

int main()
{
  // initialize the backend library
  CCopasiRootContainer::init(0, NULL);
  assert(CCopasiRootContainer::getRoot() != NULL);
  // create a new datamodel
  CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
  assert(CCopasiRootContainer::getDatamodelList()->size() == 1);
  CModel* pModel = pDataModel->getModel();
  assert(pModel != NULL);
  pModel->setVolumeUnit(CModel::fl);
  pModel->setTimeUnit(CModel::s);
  pModel->setQuantityUnit(CModel::fMol);
  CModelValue* pFixedModelValue = pModel->createModelValue("F");
  assert(pFixedModelValue != NULL);
  pFixedModelValue->setStatus(CModelEntity::FIXED);
  pFixedModelValue->setInitialValue(3.0);
  CModelValue* pVariableModelValue = pModel->createModelValue("V");
  assert(pVariableModelValue != NULL);
  pVariableModelValue->setStatus(CModelEntity::ASSIGNMENT);
  // we create a very simple assignment that is easy on the optimization
  // a parabole with the minimum at x=6 should do just fine
  std::string s = pFixedModelValue->getObject(CCopasiObjectName("Reference=Value"))->getCN();
  s = "(<" + s + "> - 6.0)^2";
  pVariableModelValue->setExpression(s);
  // now we compile the model and tell COPASI which values have changed so
  // that COPASI can update the values that depend on those
  std::set<const CCopasiObject*> changedObjects;
  changedObjects.insert(pFixedModelValue->getObject(CCopasiObjectName("Reference=InitialValue")));
  changedObjects.insert(pVariableModelValue->getObject(CCopasiObjectName("Reference=InitialValue")));
  // finally compile the model
  // compile needs to be done before updating all initial values for
  // the model with the refresh sequence
  pModel->compileIfNecessary(NULL);

  // now that we are done building the model, we have to make sure all
  // initial values are updated according to their dependencies
  std::vector<Refresh*> refreshes = pModel->buildInitialRefreshSequence(changedObjects);
  std::vector<Refresh*>::iterator it2 = refreshes.begin(), endit2 = refreshes.end();

  while (it2 != endit2)
    {
      // call each refresh
      (**it2)();
      ++it2;
    }

  // now we set up the optimization

  // we want to do an optimization for the time course
  // so we have to set up the time course task first
  // get the task list
  CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();

  // get the optimization task
  COptTask* pOptTask = dynamic_cast<COptTask*>(TaskList["Optimization"]);
  assert(pOptTask != NULL);
  // we want to use Levenberg-Marquardt as the optimization method
  pOptTask->setMethodType(CCopasiMethod::LevenbergMarquardt);

  // next we need to set subtask type on the problem
  COptProblem* pOptProblem = dynamic_cast<COptProblem*>(pOptTask->getProblem());
  assert(pOptProblem != NULL);
  pOptProblem->setModel(pModel);
  pOptProblem->setSubtaskType(CCopasiTask::timeCourse);

  CTrajectoryTask* pTimeCourseTask = dynamic_cast<CTrajectoryTask*>(TaskList["Time-Course"]);
  assert(pTimeCourseTask != NULL);
  // since for this example it really doesn't matter how long we run the time course
  // we run for 1 second and calculate 10 steps
  // run a deterministic time course
  pTimeCourseTask->setMethodType(CCopasiMethod::deterministic);

  // pass a pointer of the model to the problem
  pTimeCourseTask->getProblem()->setModel(pModel);

  // get the problem for the task to set some parameters
  CTrajectoryProblem* pProblem = dynamic_cast<CTrajectoryProblem*>(pTimeCourseTask->getProblem());
  assert(pProblem != NULL);

  // simulate 10 steps
  pProblem->setStepNumber(10);
  // start at time 0
  pModel->setInitialTime(0.0);
  // simulate a duration of 1 time units
  pProblem->setDuration(1);
  // tell the problem to actually generate time series data
  pProblem->setTimeSeriesRequested(true);
  // we create the objective function
  // we want to minimize the value of the variable model value at the end of
  // the simulation
  // the objective function is normally minimized
  std::string objectiveFunction = pVariableModelValue->getObject(CCopasiObjectName("Reference=Value"))->getCN();
  // we need to put the angled brackets around the common name of the object
  objectiveFunction = "<" + objectiveFunction + ">";
  // now we set the objective function in the problem
  pOptProblem->setObjectiveFunction(objectiveFunction);

  // now we create the optimization items
  // i.e. the model elements that have to be changed during the optimization
  // in order to get to the optimal solution
  COptItem* pOptItem = &pOptProblem->addOptItem(CCopasiObjectName(pFixedModelValue->getObject(CCopasiObjectName("Reference=InitialValue"))->getCN()));
  // we want to change the fixed model value from -100 to +100 with a start
  // value of 50
  pOptItem->setStartValue(50.0);
  pOptItem->setLowerBound(CCopasiObjectName("-100"), pDataModel);
  pOptItem->setUpperBound(CCopasiObjectName("100"), pDataModel);

  // now we set some parameters on the method
  // these parameters are specific to the method type we set above
  // (in this case Levenberg-Marquardt)
  COptMethod* pOptMethod = dynamic_cast<COptMethod*>(pOptTask->getMethod());
  assert(pOptMethod != NULL);

  // now we set some method parameters for the optimization method
  // iteration limit
  CCopasiParameter* pParameter = pOptMethod->getParameter("Iteration Limit");
  assert(pParameter != NULL);
  pParameter->setValue((long int)2000);
  // tolerance
  pParameter = pOptMethod->getParameter("Tolerance");
  assert(pParameter != NULL);
  pParameter->setValue(1.0e-5);

  // create a report with the correct filename and all the species against
  // time.
  CReportDefinitionVector* pReports = pDataModel->getReportDefinitionList();
  // create a new report definition object
  CReportDefinition* pReport = pReports->createReportDefinition("Report", "Output for optimization");
  // set the task type for the report definition to timecourse
  pReport->setTaskType(CCopasiTask::optimization);
  // we don't want a table
  pReport->setIsTable(false);
  // the entries in the output should be seperated by a ", "
  pReport->setSeparator(CCopasiReportSeparator(", "));

  // we need a handle to the header and the body
  // the header will display the ids of the metabolites and "time" for
  // the first column
  // the body will contain the actual timecourse data
  std::vector<CRegisteredObjectName>* pHeader = pReport->getHeaderAddr();
  std::vector<CRegisteredObjectName>* pBody = pReport->getBodyAddr();

  // in the report header we write two strings and a separator
  pHeader->push_back(CRegisteredObjectName(CCopasiStaticString("best value of objective function").getCN()));
  pHeader->push_back(CRegisteredObjectName(pReport->getSeparator().getCN()));
  pHeader->push_back(CRegisteredObjectName(CCopasiStaticString("initial value of F").getCN()));
  // in the report body we write the best value of the objective function and
  // the initial value of the fixed parameter separated by a komma
  pBody->push_back(CRegisteredObjectName(pOptProblem->getObject(CCopasiObjectName("Reference=Best Value"))->getCN()));
  pBody->push_back(CRegisteredObjectName(pReport->getSeparator().getCN()));
  pBody->push_back(CRegisteredObjectName(pFixedModelValue->getObject(CCopasiObjectName("Reference=InitialValue"))->getCN()));

  // set the report for the task
  pOptTask->getReport().setReportDefinition(pReport);
  // set the output filename
  pOptTask->getReport().setTarget("example5.txt");
  // don't append output if the file exists, but overwrite the file
  pOptTask->getReport().setAppend(false);

  bool result = false;

  try
    {
      // initialize the trajectory task
      // we want complete output (HEADER, BODY and FOOTER)
      pOptTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
      // now we run the actual trajectory
      result = pOptTask->process(true);
    }
  catch (...)
    {
      std::cerr << "Error. Running the optimization failed." << std::endl;

      // check if there are additional error messages
      if (CCopasiMessage::size() > 0)
        {
          // print the messages in chronological order
          std::cerr << CCopasiMessage::getAllMessageText(true);
        }

      // clean up the library
      CCopasiRootContainer::destroy();
      exit(1);
    }

  if (!result)
    {
      std::cerr << "Running the optimization failed." << std::endl;

      // clean up the library
      CCopasiRootContainer::destroy();
      exit(1);
    }

  // restore the state of the trajectory
  pOptTask->restore();
  // now we check if the optimization actually got the correct result
  // the best value it should have is 0 and the best parameter value for
  // that result should be 6 for the initial value of the fixed parameter
  double bestValue = pOptProblem->getSolutionValue();
  assert(fabs(bestValue) < 1e-3);
  // we should only have one solution variable since we only have one
  // optimization item
  assert(pOptProblem->getSolutionVariables().size() == 1);
  double solution = pOptProblem->getSolutionVariables()[0];
  assert(fabs((solution - 6.0) / 6.0) < 1e-3);

  // clean up the library
  CCopasiRootContainer::destroy();
}
