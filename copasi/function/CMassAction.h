/**
 * CMassAction
 * 
 * Created for Copasi by Stefan Hoops
 * (C) Stefan Hoops 2001
 */

#ifndef COPASI_CMassAction
#define COPASI_CMassAction

#include "CFunction.h"

class CMassAction : public CFunction
  {
    // Attributes
  private:

    // Operations
  public:
    /**
     *  Default constructor
     */
    CMassAction();

    /**
     *  Copy constructor
     */
    CMassAction(const CFunction & src);

    /**
     *  Specified constructor
     */
    CMassAction(const TriLogic & reversible);

    /**
     *  Destructor
     */
    ~CMassAction();

    /**
     *  Retrieves the position of a parameter in the function call
     *  @param "const string &" name
     *  @return "const unsigned C_INT32" index
     */
    virtual unsigned C_INT32 getParameterPosition(const std::string & name);

    /**
     *  Returns a string containing the explicit function in SBML format
     *  @param "vector < void * >" callParameters
    *  @param "const string &" r a suffix for parameter names (usually reaction number)
     */
    std::string getSBMLString(const CCallParameters & callParameterNames, const std::string &r);

    /**
     *  Calculates the value of the function
     *  @param "vector < void * >" callParameters
     *  @return "C_FLOAT64" result
     */
    virtual C_FLOAT64
    calcValue(const CCallParameters & callParameters) const;
  };

#endif // COPASI_CMassAction
