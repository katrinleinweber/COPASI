// Copyright (C) 2015 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

/**
 * CAnalyticsTask class.
 *
 * This class implements a trajectory task which is comprised of a
 * of a problem and a method. Additionally calls to the reporting
 * methods are done when initialized.
 *
 * Created for COPASI by Sven Sahle 2012
 */

#ifndef COPASI_CAnalyticsTask
#define COPASI_CAnalyticsTask

#include "copasi/model/CEvent.h"
#include "copasi/trajectory/CTrajectoryTask.h"
#include "copasi/utilities/CCopasiTask.h"
#include "copasi/utilities/CReadConfig.h"
#include "copasi/trajectory/CTimeSeries.h"
#include "copasi/utilities/CVector.h"
#include "utilities/CAnnotatedMatrix.h"
#include "utilities/CMatrix.h"
#include "analytics/CStatistics.h"

class CAnalyticsProblem;
class CState;
class CMathEvent;
class CCallbackInterface;

class CAnalyticsTask : public CTrajectoryTask
{
private:
  /**
   * Default constructor
   */
  CAnalyticsTask();

public:
  /**
   * Specific constructor
   * @param const CCopasiContainer * pParent
   * @param const CTaskEnum::Task & type (default: analytics)
   */
  CAnalyticsTask(const CCopasiContainer * pParent,
                 const CTaskEnum::Task & type = CTaskEnum::analytics);

  /**
   * Copy constructor
   * @param const CAnalyticsTask & src
   * @param const CCopasiContainer * pParent (default: NULL)
   */
  CAnalyticsTask(const CAnalyticsTask & src,
                 const CCopasiContainer * pParent = NULL);

  /**
   * Destructor
   */
  ~CAnalyticsTask();

  /**
   * Initialize the task. If an ostream is given this ostream is used
   * instead of the target specified in the report. This allows nested
   * tasks to share the same output device.
   * @param const OutputFlag & of
   * @param COutputHandler * pOutputHandler
   * @param std::ostream * pOstream (default: NULL)
   * @return bool success
   */
  virtual bool initialize(const OutputFlag & of,
                          COutputHandler * pOutputHandler,
                          std::ostream * pOstream);

  /**
   * Process the task with or without initializing to the initial state.
   * @param const bool & useInitialValues
   * @return bool success
   */
  virtual bool process(const bool & useInitialValues);

  void createEvent();
  void removeEvent();

  /**
   * Perform necessary cleanup procedures
   */
  virtual bool restore();

  /**
   * Retrieve the list of valid methods
   * @return const CTaskEnum::Method * pValidMethods
   */
  virtual const CTaskEnum::Method * getValidMethods() const;

private:
  /**
   * initialize the object references
   */
  void initObjects();

  /**
   * Set or unset the event callback
   * @param const bool & set
   */
  void setEventCallback(const bool & set);

  /**
   * This is the member function that is called by the static call back function
   * It checks if an event describes the cut plane and does all
   * the necessary analysis and output in this case
   */
  void eventCallBack(void * pData, void * pCaller);

  /**
   * should be called by all code paths that finish the task.
   * -finishes progress reporting
   * -finishes output
   * -resets call back function
   */
  void finish();

  C_FLOAT64 relativeDifferenceOfStates(const CVectorCore< C_FLOAT64 > & s1,
                                       const CVectorCore< C_FLOAT64 > & s2);

  /**
   * Compute the max or min of the values contained in the first parameter vector
   */
  void computeSelectedStatistics(std::vector< C_FLOAT64 >, std::vector< C_FLOAT64 >);

private:

  // Attributes
  /**
   * Index of the selected object for the task
   */
  size_t mIndex;

  /**
   * A pointer to the trajectory Problem
   */
  CAnalyticsProblem * mpAnalyticsProblem;

  /**
   * time at which the simulation starts.
   */
  C_FLOAT64 mStartTime;

  size_t mNumCrossings;

  size_t mOutputStartNumCrossings;

  size_t mMaxNumCrossings;

  /**
   * handle for progress reporting
   */
  size_t mhProgress;

  /**
   * this holds the max value for the progress reporting
   */
  C_FLOAT64 mProgressMax;

  /**
   * this holds the current value for the progress reporting
   */
  C_FLOAT64 mProgressValue;

  /**
   * this holds the current value for the progress reporting
   */
  C_FLOAT64 mProgressFactor;

  /**
   * Pointer to the event representing the cut plane
   */
  CMathEvent * mpEvent;

  /**
   * A pointer to the callback register with the event.
   */
  CCallbackInterface *mpEventCallback;

  /**
   * describes the internal state of the calculation
   */
  enum STATE
  {
    TRANSIENT = 0, //before the condition for starting output is met
    MAIN,          //the main part of the run, while output is generated
    FINISH         //when the conditions for finishing are met
  };

  STATE mState;

  std::vector< CVector< C_FLOAT64 > > mStatesRing;
  //std::vector<C_FLOAT64> mvTimesRing;
  //--- ETTORE start ---
  std::vector< C_FLOAT64 > mValues;
  std::vector< C_FLOAT64 > mTimes;
  std::vector< CVector< C_FLOAT64 > > mValuesSeries;
  //--- ETTORE end -----

  //the number of states already pushed to the ring buffer
  size_t mStatesRingCounter;

  C_FLOAT64 mPreviousCrossingTime;
  C_FLOAT64 mPeriod;
  C_FLOAT64 mAveragePeriod;
  C_FLOAT64 mLastPeriod;
  C_INT mPeriodicity;
  C_FLOAT64 mLastFreq;
  C_FLOAT64 mFreq;
  C_FLOAT64 mAverageFreq;

  //--- ETTORE start ---
  const CCopasiObject *mpSelectedObject;
  CCopasiObject * mpObjectSelected;
  CCopasiObject * mpObjectRate;

  /**
   *  This holds the result
   */
  //CMatrix < C_FLOAT64 > mStatVal;
  //CMatrix < C_FLOAT64 > mStatTime;

  C_FLOAT64 mStatVal;
  C_FLOAT64 mStatTime;

  CStatistics * pStats;
  CStatistics * pStatsTime;

  //CArrayAnnotation * mpStatValAnn;
  //CArrayAnnotation * mpStatTimeAnn;
  //--- ETTORE end -----
};
#endif // COPASI_CAnalyticsTask
