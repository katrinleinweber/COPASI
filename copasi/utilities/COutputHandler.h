/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/utilities/COutputHandler.h,v $
   $Revision: 1.2 $
   $Name:  $
   $Author: ssahle $ 
   $Date: 2004/06/25 15:06:30 $
   End CVS Header */

#if !defined OUTPUT_HANDLER
#define OUTPUT_HANDLER

/**
 *  This is just a wrapper class that is used to call the plotting classes.
 *  We do not want to call gui stuff directly from the trajectory task. 
 */
class CCallbackHandler
  {
  public:

    //for output handler
    virtual bool init();
    virtual bool doOutput();
    virtual bool finish();

    //for progress bar
    virtual bool init(C_INT32 maxSteps);
    virtual bool progress(C_INT32 steps);
    //finish() also works for progress bars
  };

#endif
