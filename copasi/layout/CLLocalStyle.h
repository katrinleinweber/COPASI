// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#ifndef CLLocalStyle_H__
#define CLLocalStyle_H__

#include <set>
#include <string>

#include "copasi/layout/CLStyle.h"

LIBSBML_CPP_NAMESPACE_BEGIN
class LocalStyle;
LIBSBML_CPP_NAMESPACE_END

class CLLocalStyle : public CLStyle
{
protected:
  std::set<std::string> mKeyList;

public:
  /**
   * Static method to create a CDataObject based on the provided data
   * @param const CData & data
   * @return CLLocalStyle * pDataObject
   */
  static CLLocalStyle * fromData(const CData & data);

  /**
   * Retrieve the data describing the object
   * @return CData data
   */
  virtual CData toData() const;

  /**
   * Apply the provided data to the object
   * @param const CData & data
   * @return bool success
   */
  virtual bool applyData(const CData & data, CUndoData::ChangeSet & changes);

  /**
   * Constructor.
   */
  CLLocalStyle(CDataContainer* pParent = NULL);

  /**
   * Copy Constructor.
   */
  CLLocalStyle(const CLLocalStyle& source, CDataContainer* pParent = NULL);

  /**
   * Constructor to generate object from the corresponding SBML object.
   */
  CLLocalStyle(const LocalStyle& source, CDataContainer* pParent = NULL);

  /**
   * Returns the number of keys in the key set.
   */
  size_t getNumKeys() const;

  /**
   * Returns the key list.
   */
  const std::set<std::string>& getKeyList() const;

  /**
   * Sets the key list.
   */
  void setKeyList(const std::set<std::string>& keyList);

  /**
   * Checks if a certain key is in the set or not.
   */
  bool isKeyInSet(const std::string& key) const;

  /**
   * Adds another key to the set.
   */
  void addKey(const std::string& key);

  /**
   * Checks whether a given key is in the role list.
   */
  bool isInKeyList(const std::string& key) const;

  /**
   * Removes an key from the set.
   */
  void removeKey(const std::string& key);

  /**
   * Converts this object to the corresponding SBML object.
   */
  LocalStyle* toSBML(unsigned int level, unsigned int version) const;
};

#endif /* CLLocalStyle_H__ */
