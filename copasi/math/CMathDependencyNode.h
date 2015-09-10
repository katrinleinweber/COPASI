// Copyright (C) 2011 - 2015 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#ifndef COPASI_CObjectDependencyNode
#define COPASI_CObjectDependencyNode

#include <set>
#include <vector>

#include "copasi/report/CCopasiObject.h"
#include "copasi/math/CMathEnum.h"

class CMathDependencyNode
{
  // Operations
private:
  CMathDependencyNode(void);

public:
  /**
   * Specific constructor
   * @param const CObjectInterface * pObject
   */
  CMathDependencyNode(const CObjectInterface * pObject);

  /**
   * Copy constructor
   */
  CMathDependencyNode(const CMathDependencyNode & src);

  /**
   * Destructor
   */
  ~CMathDependencyNode(void);

  /**
   * Retrieve a pointer to the object the node is representing
   * @return   const CObjectInterface * pObject
   */
  const CObjectInterface * getObject() const;

  /**
   * Add a prerequisite
   * @param CMathDependencyNode * pNode
   */
  void addPrerequisite(CMathDependencyNode * pNode);

  /**
   * Remove a prerequisite
   * @param CMathDependencyNode * pNode
   */
  void removePrerequisite(CMathDependencyNode * pNode);

  /**
   * Retrieve the prerequisites
   * @return std::vector< CMathDependencyNode * > prerequisites
   */
  std::vector< CMathDependencyNode * > & getPrerequisites();

  /**
   * Add a dependent
   * @param CMathDependencyNode * pNode
   */
  void addDependent(CMathDependencyNode * pNode);

  /**
   * Remove a dependent
   * @param CMathDependencyNode * pNode
   */
  void removeDependent(CMathDependencyNode * pNode);

  /**
   * Retrieve the dependents
   * @return std::vector< CMathDependencyNode * > dependents
   */
  std::vector< CMathDependencyNode * > & getDependents();

  /**
   * Update the state of all dependents (and dependents thereof) to changed,
   * @param const CMath::SimulationContextFlag & context
   * @param const CObjectInterface::ObjectSet & changedObjects
   * @return bool success
   */
  bool updateDependentState(const CMath::SimulationContextFlag & context,
                            const CObjectInterface::ObjectSet & changedObjects);

  /**
   * Update the state of all prerequisites (and prerequisites thereof) to requested.
   * @param const CMath::SimulationContextFlag & context
   * @param const CObjectInterface::ObjectSet & changedObjects
   * @return bool success
   */
  bool updatePrerequisiteState(const CMath::SimulationContextFlag & context,
                               const CObjectInterface::ObjectSet & changedObjects);

  /**
   * Update the state of all prerequisites (and prerequisites thereof) to calculate.
   * @param const CMath::SimulationContextFlag & context
   * @param const CObjectInterface::ObjectSet & changedObjects
   * @return bool success
   */
  bool updateCalculatedState(const CMath::SimulationContextFlag & context,
                             const CObjectInterface::ObjectSet & changedObjects);

  /**
   * Build the sequence of objects which need to be updated to calculate the object value.
   * @param const CMath::SimulationContextFlag & context
   * @param CObjectInterface::UpdateSequence & updateSequence
   * @return bool success
   */
  bool buildUpdateSequence(const CMath::SimulationContextFlag & context,
                           CObjectInterface::UpdateSequence & updateSequence);

  /**
   * Set whether the current node has changed its value
   * @param const bool & changed
   */
  void setChanged(const bool & changed);

  /**
   * Check whether the current nodes value is changed
   * @return const bool & isChanged
   */
  const bool & isChanged() const;

  /**
   * Set whether the current node's value is requested
   * @param const bool & requested
   */
  void setRequested(const bool & requested);

  /**
   * Check whether the current node's value is requested
   * @param const bool & isRequested
   */
  const bool & isRequested() const;

  /**
   * Reset the flags requested and changed
   */
  void reset();

  /**
   * Relocate the objects in the nodes of the graph.
   * @param std::vector< CMath::sRelocate > & relocations
   */
  void relocate(const std::vector< CMath::sRelocate > & relocations);

  void remove();

  void updateEdges(const std::map< CMathDependencyNode *, CMathDependencyNode * > & map);

  // Attributes
private:
  const CObjectInterface * mpObject;
  std::vector< CMathDependencyNode * > mPrerequisites;
  std::vector< CMathDependencyNode * > mDependents;
  bool mChanged;
  bool mRequested;
};

#endif // COPASI_CObjectDependencyNode
