/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/optimization/Attic/COptFunction.h,v $
   $Revision: 1.17 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:29:52 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/********************************************************
Author: Liang Xu
Version : 1.xx  <first>
Description:
Date: 09/20
Comment : COptFunction for optimization function
Contact: Please contact lixu1@vt.edu.
 *********************************************************/

#ifndef COPASI_COptExpression
#define COPASI_COptExpression

#include "function/CKinFunction.h"
#include "utilities/CReadConfig.h"
#include "utilities/CCopasiVector.h"
#include "report/CCopasiObject.h"
#include "report/CCopasiContainer.h"

class COptMethod;
class COptProblem;

class COptFunction: public CCopasiContainer
  {
  public:         //function
    std::vector<CCopasiObject*> mParaList;
    std::vector<std::string> mMinList;
    std::vector<std::string> mMaxList;
    std::vector<std::string> mMinOperList;
    std::vector<std::string> mMaxOperList;
    std::vector<CKinFunction*> mMinFunctionList;
    std::vector<CKinFunction*> mMaxFunctionList;

    // check if a parameter already existing inside the mParaList
    int Index(const std::string & name);

    // add a new item inside
    int addItem(CCopasiObject* pObject);

    //remove an item with the existing name
    bool removeItem(const std::string & name);

    //get key function of a COptFunction
    inline std::string getKey()
    {return mKey;}

  public:         // constructor and deconstuctor
    /**
     * Default constructor
     * @param const std::string & name (default: "NoName")
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    COptFunction(const std::string & name = "NoName",
                 const CCopasiContainer * pParent = NULL);

    /**
     * Copy constructor
     * @param "const COptFunction &" src
     * @param "CReadConfig *" configBuffer (Default = NULL)
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    COptFunction(const COptFunction & src,
                 CReadConfig * configBuffer = NULL,
                 const CCopasiContainer * pParent = NULL);

    ~COptFunction();

  private:
    void cleanup();
    std::string mKey;
  };
#endif
