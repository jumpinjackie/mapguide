/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _STATICRESOLUTIONCONTEXT_HPP
#define _STATICRESOLUTIONCONTEXT_HPP

#include <string>

#include <pathan/Pathan.hpp>

#include <xercesc/util/RefHash2KeysTableOf.hpp>
#include <xercesc/util/StringPool.hpp>

class XPath2MemoryManager;

typedef XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOf<int> VariableAccessSet;
typedef XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOfEnumerator<int> VariableAccessSetEnumerator;

/**
 * Class that represents the static type of a sub-expression
 */
class PATHAN_EXPORT StaticType {
public:
  /**
   * Flags that determine what item types are returned from a sub-expression
   */
  enum StaticTypeFlags {
    NODE_TYPE    = 0x01, ///< Results can contain nodes
    NUMERIC_TYPE = 0x02, ///< Results can contain numeric items
    OTHER_TYPE   = 0x04  ///< Results can contain other items
  };

  void typeUnion(const StaticType &st);
  bool isNodesOnly() const;

  unsigned int flags;
};

/**
 * Records access to various parts of the context during static resolution.
 */
class PATHAN_EXPORT StaticResolutionContext
{
public:
  StaticResolutionContext(XPath2MemoryManager* memMgr);
  StaticResolutionContext(const StaticResolutionContext &o, XPath2MemoryManager* memMgr);

  void copy(const StaticResolutionContext &o);

  /// Clears all the information in this StaticResolutionContext
  void clear();

  /** Overrides all the other flags, and never allows this sub-expression
      to be constant folded. */
  void forceNoFolding(bool value);
  bool isNoFoldingForced() const;

  void contextItemUsed(bool value);
  void contextPositionUsed(bool value);
  void contextSizeUsed(bool value);
  bool isContextItemUsed() const;
  bool isContextPositionUsed() const;
  bool isContextSizeUsed() const;
  /** Returns true if any of the context item flags have been used */
  bool areContextFlagsUsed() const;

  void currentTimeUsed(bool value);
  void implicitTimezoneUsed(bool value);
  void availableDocumentsUsed(bool value);
  void availableCollectionsUsed(bool value);
  bool areDocsOrCollectionsUsed() const;
  void staticBaseURIUsed(bool value);

  void variableUsed(const XMLCh *namespaceURI, const XMLCh *name);
  bool removeVariable(const XMLCh *namespaceURI, const XMLCh *name);
  bool isVariableUsed(const XMLCh *namespaceURI, const XMLCh *name) const;

  /** Sets the members of this StaticResolutionContext from the given StaticResolutionContext */
  void add(const StaticResolutionContext &o);
  void addExceptContextFlags(const StaticResolutionContext &o);

  /** Returns true if flags are set, or variables have been used */
  bool isUsed() const;
  bool isUsedExceptContextFlags() const;

  /**
   * Properties that allow optimisation regarding sorting or not.
   * The values are OR'd as flags, so they must be distinct bits
   */
  enum Properties {
    DOCORDER = 0x01, ///< Results are returned in document order
    PEER     = 0x02, ///< Results do not appear in the descendants of other results
    SUBTREE  = 0x04, ///< Results are members of the set of descendants of the context node
    GROUPED  = 0x08, ///< Results are grouped by the document they come from
    SAMEDOC  = 0x10, ///< Results are from the same document as the context node
    ONENODE  = 0x20  ///< Only ever returns one node
  };

  unsigned int getProperties() const;
  void setProperties(unsigned int props);

  const StaticType &getStaticType() const;
  StaticType &getStaticType();

  std::string toString() const;

private:
  StaticResolutionContext(const StaticResolutionContext &o);
  StaticResolutionContext &operator=(const StaticResolutionContext &o);

  bool _contextItem;
  bool _contextPosition;
  bool _contextSize;
  bool _currentTime;
  bool _implicitTimezone;
  bool _availableDocuments;
  bool _availableCollections;
  bool _staticBaseURI;
  bool _forceNoFolding;

  unsigned int _properties;
  StaticType _staticType;

  VariableAccessSet _dynamicVariables;
  XERCES_CPP_NAMESPACE_QUALIFIER XMLStringPool _uriPool;
  XPath2MemoryManager* _memMgr;
};

#endif
