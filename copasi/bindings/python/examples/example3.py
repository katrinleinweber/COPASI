# -*- coding: utf-8 -*-
# Begin CVS Header 
#   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/bindings/python/examples/example3.py,v $ 
#   $Revision: 1.4.2.3 $ 
#   $Name:  $ 
#   $Author: gauges $ 
#   $Date: 2009/03/06 16:06:47 $ 
# End CVS Header 
# Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
# and The University of Manchester. 
# All rights reserved. 

# 
# This is an example on how to import an sbml file
# create a report for a time course simulation 
# and run a time course simulation
# 

from COPASI import *
import sys

def main(args):
  # the only argument to the main routine should be the name of an SBML file
  if len(args) == 1:
      filename = args[0]
      try:
          # load the model 
          CCopasiDataModel.GLOBAL.importSBML(filename)
      except:
          print >> sys.stderr,  "Error while importing the model from file named \"" + filename + "\"." 
          return 1
      model = CCopasiDataModel.GLOBAL.getModel()
      assert model != None
      # create a report with the correct filename and all the species against
      # time.
      reports = CCopasiDataModel.GLOBAL.getReportDefinitionList()
      # create a report definition object
      report = reports.createReportDefinition("Report", "Output for timecourse")
      # set the task type for the report definition to timecourse
      report.setTaskType(CCopasiTask.timeCourse)
      # we don't want a table
      report.setIsTable(False)
      # the entries in the output should be seperated by a ", "
      report.setSeparator(CCopasiReportSeparator(", "))

      # we need a handle to the header and the body
      # the header will display the ids of the metabolites and "time" for
      # the first column
      # the body will contain the actual timecourse data
      header = report.getHeaderAddr()
      body = report.getBodyAddr()
      
      body.push_back(CRegisteredObjectName(CCopasiObjectName(CCopasiDataModel.GLOBAL.getModel().getCN().getString() + ",Reference=Time").getString()))
      body.push_back(CRegisteredObjectName(report.getSeparator().getCN().getString()))
      header.push_back(CRegisteredObjectName(CCopasiStaticString("time").getCN().getString()))
      header.push_back(CRegisteredObjectName(report.getSeparator().getCN().getString()))

      iMax = model.getMetabolites().size()
      for i in range(0,iMax):
          metab = model.getMetabolite(i)
          assert metab != None
          # we don't want output for FIXED metabolites right now
          if (metab.getStatus() != CModelEntity.FIXED):
              # we want the concentration oin the output
              # alternatively, we could use "Reference=Amount" to get the
              # particle number
              body.push_back(CRegisteredObjectName(metab.getObject(CCopasiObjectName("Reference=Concentration")).getCN().getString()))
              # add the corresponding id to the header
              header.push_back(CRegisteredObjectName(CCopasiStaticString(metab.getSBMLId()).getCN().getString()))
              # after each entry, we need a seperator
              if(i!=iMax-1):
                body.push_back(CRegisteredObjectName(report.getSeparator().getCN().getString()))
                header.push_back(CRegisteredObjectName(report.getSeparator().getCN().getString()))


      # get the trajectory task object
      trajectoryTask = None #CCopasiDataModel.GLOBAL.getTask("Time-Course")
      # if there isn't one
      if (trajectoryTask == None):
          # create a one
          trajectoryTask = CTrajectoryTask()
          # add the time course task to the task list
          # this method makes sure the object is now owned by the list
          # and that SWIG does not delete it
          CCopasiDataModel.GLOBAL.getTaskList().addAndOwn(trajectoryTask)


      # run a deterministic time course
      trajectoryTask.setMethodType(CCopasiMethod.deterministic)

      # pass a pointer of the model to the problem
      trajectoryTask.getProblem().setModel(CCopasiDataModel.GLOBAL.getModel())

      # actiavate the task so that it will be run when the model is saved
      # and passed to CopasiSE
      trajectoryTask.setScheduled(True)

      # set the report for the task
      trajectoryTask.getReport().setReportDefinition(report)
      # set the output filename
      trajectoryTask.getReport().setTarget("example3.txt")
      # don't append output if the file exists, but overwrite the file
      trajectoryTask.getReport().setAppend(False)

      # get the problem for the task to set some parameters
      problem = trajectoryTask.getProblem()

      # simulate 100 steps
      problem.setStepNumber(100)
      # start at time 0
      CCopasiDataModel.GLOBAL.getModel().setInitialTime(0.0)
      # simulate a duration of 10 time units
      problem.setDuration(10)
      # tell the problem to actually generate time series data
      problem.setTimeSeriesRequested(True)

      # set some parameters for the LSODA method through the method
      method = trajectoryTask.getMethod()

      parameter = method.getParameter("Absolute Tolerance")
      assert parameter != None
      assert parameter.getType() == CCopasiParameter.UDOUBLE
      parameter.setValue(1.0e-12)

      result=True
      try:
          # now we run the actual trajectory
          result=trajectoryTask.process(True)
      except:
          print >> sys.stderr,  "Error. Running the time course simulation failed." 
          # check if there are additional error messages
          if CCopasiMessage.size() > 0:
              # print the messages in chronological order
              print >> sys.stderr, CCopasiMessage.getAllMessageText(True)
          return 1
      if result == False:
          print >> sys.stderr,  "Error. Running the time course simulation failed." 
          # check if there are additional error messages
          if CCopasiMessage.size() > 0:
              # print the messages in chronological order
              print >> sys.stderr, CCopasiMessage.getAllMessageText(True)
          return 1

      # look at the timeseries
      timeSeries = trajectoryTask.getTimeSeries()
      # we simulated 100 steps, including the initial state, this should be
      # 101 step in the timeseries
      assert timeSeries.getRecordedSteps() == 101
      print  "The time series consists of " , timeSeries.getRecordedSteps() , "." 
      print  "Each step contains " , timeSeries.getNumVariables() , " variables." 
      print  "The final state is: " 
      iMax = timeSeries.getNumVariables()
      lastIndex = timeSeries.getRecordedSteps() - 1
      for i in range(0,iMax):
          # here we get the particle number (at least for the species)
          # the unit of the other variables may not be particle numbers
          # the concentration data can be acquired with getConcentrationData
          print timeSeries.getTitle(i) + ": " , timeSeries.getData(lastIndex, i) 

  else:
      print >> sys.stderr, "Usage: example3 SBMLFILE" 
      return 1;

if(__name__ == '__main__'):
   main(sys.argv[1:]) 



