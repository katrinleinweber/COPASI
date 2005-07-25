/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/utilities/CCopasiTask.cpp,v $
   $Revision: 1.26 $
   $Name:  $
   $Author: ssahle $ 
   $Date: 2005/07/25 09:48:10 $
   End CVS Header */

/**
 *  CCopasiTask class.
 *  This class is used to describe a task in COPASI. This class is 
 *  intended to be used as the parent class for all tasks whithin COPASI.
 *  
 *  Created for Copasi by Stefan Hoops 2003
 */

#include "copasi.h"

#include "CCopasiTask.h"
#include "CCopasiProblem.h"
#include "CCopasiMethod.h"
#include "report/CReport.h"
#include "report/CKeyFactory.h"
#include "utilities/COutputHandler.h"
#include "model/CModel.h"

const std::string CCopasiTask::TypeName[] =
  {
    "Steady-State",
    "Time-Course",
    "Scan",
    "Elementary Flux Modes",
    "Optimization",
    "Parameter Fitting",
    "Metabolic Control Analysis",
    "Time scale separation",
    ""
  };

const char* CCopasiTask::XMLType[] =
  {
    "steadyState",
    "timeCourse",
    "scan",
    "fluxMode",
    "optimization",
    "parameterFitting",
    "metabolicControlAnalysis",
    "timeScaleSeparation",
    NULL
  };

//static
CCopasiTask::Type CCopasiTask::XMLNameToEnum(const char * xmlTypeName)
{
  unsigned C_INT32 i = 0;
  while (XMLType[i] && strcmp(xmlTypeName, XMLType[i])) i++;

  if (XMLType[i]) return (CCopasiTask::Type) i;
  else return CCopasiTask::unset;
}

CCopasiTask::CCopasiTask(const std::string & name,
                         const CCopasiContainer * pParent,
                         const std::string & type):
    CCopasiContainer(name, pParent, type),
    mType(CCopasiTask::unset),
    mKey(GlobalKeys.add("Task", this)),
    mScheduled(false),
    mpProblem(NULL),
    mpMethod(NULL),
    mReport(),
    mpOutputHandler(NULL),
    mpCallBack(NULL),
    mpSliders(NULL),
    mDoOutput(OUTPUT_COMPLETE)
{}

CCopasiTask::CCopasiTask(const CCopasiTask::Type & taskType,
                         const CCopasiContainer * pParent,
                         const std::string & type):
    CCopasiContainer(CCopasiTask::TypeName[taskType], pParent, type),
    mType(taskType),
    mKey(GlobalKeys.add("Task", this)),
    mScheduled(false),
    mpProblem(NULL),
    mpMethod(NULL),
    mReport(),
    mpOutputHandler(NULL),
    mpCallBack(NULL),
    mpSliders(NULL),
    mDoOutput(OUTPUT_COMPLETE)
{}

CCopasiTask::CCopasiTask(const CCopasiTask & src,
                         const CCopasiContainer * pParent):
    CCopasiContainer(src, pParent),
    mType(src.mType),
    mKey(GlobalKeys.add("Task", this)),
    mScheduled(src.mScheduled),
    mpProblem(NULL),
    mpMethod(NULL),
    mReport(src.mReport),
    mpOutputHandler(NULL),
    mpCallBack(NULL),
    mpSliders(NULL),
    mDoOutput(OUTPUT_COMPLETE)
{}

CCopasiTask::~CCopasiTask()
{
  GlobalKeys.remove(mKey);
  pdelete(mpProblem);
  pdelete(mpMethod);
  pdelete(mpSliders);
}

//const std::string & CCopasiTask::getName() const {return getObjectName();}

bool CCopasiTask::setName(const std::string & name)
{return setObjectName(name);}

CCopasiTask::Type CCopasiTask::getType() const {return mType;}

void CCopasiTask::setType(const CCopasiTask::Type & type) {mType = type;}

const std::string & CCopasiTask::getKey() const {return mKey;}

void CCopasiTask::setScheduled(const bool & scheduled) {mScheduled = scheduled;}

const bool & CCopasiTask::isScheduled() const {return mScheduled;}

bool CCopasiTask::setCallBack(CProcessReport * pCallBack)
{
  mpCallBack = pCallBack;
  return true;
}

bool CCopasiTask::initialize(std::ostream * pOstream)
{
  if (!mpProblem) return false;
  if (!mpProblem->getModel()) return false;

  if (!mpProblem->getModel()->compileIfNecessary()) return false;
  if (!mReport.open(pOstream)) return false;
  if (!mReport.compile()) return false;

  return true;
}

bool CCopasiTask::process(OutputFlag C_UNUSED(of)) {return false;}

bool CCopasiTask::processForScan(bool C_UNUSED(useInitialConditions), bool C_UNUSED(doOutput)) {return false;}

bool CCopasiTask::restore()
{
  //mReport.close();
  setCallBack(NULL);

  return true;
}

CCopasiProblem * CCopasiTask::getProblem() {return mpProblem;}

const CCopasiProblem * CCopasiTask::getProblem() const {return mpProblem;}

bool CCopasiTask::setMethodType(const int & C_UNUSED(type))
{return false;}

CCopasiMethod * CCopasiTask::getMethod() {return mpMethod;}

CReport & CCopasiTask::getReport() {return mReport;}

void CCopasiTask::cleanup() {}

void CCopasiTask::setOutputHandler(CCallbackHandler* pHandler)
{mpOutputHandler = pHandler;}

CCallbackHandler* CCopasiTask::getOutputHandlerAddr()
{return mpOutputHandler;}

/*
void CCopasiTask::setProgressHandler(CProcessReport * pHandler)
{mpCallBack = pHandler;}
 */

CCopasiParameterGroup * CCopasiTask::getSliders()
{return mpSliders;}

// output stuff

bool CCopasiTask::initOutput()
{
  if (mDoOutput == OUTPUT_COMPLETE)
    {
      mReport.printHeader();
      if (mpOutputHandler) mpOutputHandler->init();
    }
  return true;
}

bool CCopasiTask::doOutput()
{
  if (mDoOutput)
    {
      mReport.printBody();
      if (mpOutputHandler) mpOutputHandler->doOutput();
    }
  return true;
}

bool CCopasiTask::finishOutput()
{
  if (mDoOutput == OUTPUT_COMPLETE)
    {
      mReport.printFooter();
      mReport.close();
      if (mpOutputHandler) mpOutputHandler->finish();
    }
  return true;
}
