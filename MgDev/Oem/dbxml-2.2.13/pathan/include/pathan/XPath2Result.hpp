/*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __XPATH2RESULT_HPP
#define __XPATH2RESULT_HPP

#include "Pathan.hpp"

#include <xercesc/util/XMLString.hpp>

#include "Sequence.hpp"
#include "dataItem/DataItem.hpp"
#include "internal/ProxyMemoryManager.hpp"

#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMXPathException.hpp>

#include <string>

class DynamicContext;

class PATHAN_EXPORT XPath2Result
{
public:

  enum ResultType {
    FIRST_RESULT    = 100,
    ITERATOR_RESULT = 101,
    SNAPSHOT_RESULT = 102
  };

  XPath2Result(const ResultType resultType,
               const DataItem *expression,
               DynamicContext *dynamicContext,
               XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);

  XPath2Result(const ResultType resultType,
               const DataItem *expression,
               XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
               DynamicContext *staticContext,
               XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);

  ~XPath2Result();

  const ResultType getResultType() const;

  bool isNode() const;

  int asInt() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);
  double asDouble() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);
  const XMLCh* asString() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);
  bool asBoolean() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* asNode() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);

  bool getInvalidIteratorState() const;
  bool iterateNext() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);

  unsigned long getSnapshotLength() const throw(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException);
  bool snapshotItem(unsigned long index) const throw(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException);

  /** @name Non-standard Extension */
  //@{
  /**
   * Called to indicate that this object (and its associated children) is no longer in use
   * and that the implementation may relinquish any resources associated with it and
   * its associated children.
   *
   * Access to a released object will lead to unexpected result.
   */
  void release();
  //@}

  //@{
  /**
   * Called to retrieve the namespace URI of the type of the current result item
   */
  const XMLCh* getTypeURI() const;
  //@}

  //@{
  /**
   * Called to retrieve the name of the type of the current result item
   */
  const XMLCh* getTypeName() const;
  //@}

protected:
  void evaluate(const DataItem *expression);
  bool hasDocumentChanged(const Item::Ptr &item) const;

  const XMLCh* errorMessage(ResultType requestedType,
                            ResultType resultType) const;
  const std::string typeName(ResultType type) const;

  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* _createdWith;
  ProxyMemoryManager _memMgr;

  Sequence *_resultSequence;
  ResultType _resultType;

  bool _contextOwned;
  DynamicContext* _context;

  unsigned int _curIndex;

  int _changes;
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* _documentRoot;

  bool beforeStart;  // used for iterator

}; //XPath2Result

#endif //__XPATH2RESULT_HPP
