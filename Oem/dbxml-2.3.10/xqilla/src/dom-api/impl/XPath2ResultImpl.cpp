/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: XPath2ResultImpl.cpp,v 1.7.2.1 2007/01/19 18:52:17 jpcs Exp $
 */

#include "../../config/xqilla_config.h"
#include "XPath2ResultImpl.hpp"
#include "XQillaExpressionImpl.hpp"
#include <string>

#include <xqilla/items/Node.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/exceptions/XQillaException.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/utils/UTF8Str.hpp>

#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>

#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/impl/DOMDocumentImpl.hpp>

using namespace std;
XERCES_CPP_NAMESPACE_USE;

string typeName(XPath2Result::ResultType type) {
    switch(type) {
      case XPath2Result::FIRST_RESULT: return "FIRST_RESULT";
      case XPath2Result::ITERATOR_RESULT: return "ITERATOR_RESULT";
      case XPath2Result::SNAPSHOT_RESULT: return "SNAPSHOT_RESULT";
    }
    return "";
}

string errorMessage(XPath2Result::ResultType requestedType,
                         XPath2Result::ResultType resultType) {
  return typeName(requestedType) + " was requested from a XPath2Result of type " + typeName(resultType);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

XPath2ResultImpl::XPath2ResultImpl(DOMNode* contextNode,
                                   DynamicContext *staticContext,
                                   MemoryManager* memMgr,
                                   XQillaExpressionImpl *exprImpl)
  : _createdWith(memMgr),
    _context(staticContext->createDynamicContext(_createdWith)),
    _currentItem(0),
    _exprToDelete(exprImpl)
{
  //Check for illegal contextNode types
  if(contextNode != 0) {
    //More illegal types here?
    switch (contextNode->getNodeType()) {
    case DOMNode::ENTITY_REFERENCE_NODE:
      throw DOMException(DOMException::NOT_SUPPORTED_ERR, X("Context node of illegal type."));
      break;
    }
    _context->setExternalContextNode(contextNode);
  }
}

XPath2ResultImpl::~XPath2ResultImpl() { 
  delete _context;
  if(_exprToDelete) _exprToDelete->release();
}

const DOMTypeInfo *XPath2ResultImpl::getTypeInfo() const
{
  if(_currentItem.isNull()) return 0;
  return this;
}

bool XPath2ResultImpl::isNode() const {
  return !_currentItem.isNull() && _currentItem->isNode();
}

/// DOMTypeInfo method
const XMLCh* XPath2ResultImpl::getName() const
{
   if(_currentItem.isNull()) return 0;
   return _currentItem->getTypeName();
}

/// DOMTypeInfo method
const XMLCh* XPath2ResultImpl::getNamespace() const
{
   if(_currentItem.isNull()) return 0;
   return _currentItem->getTypeURI();
}

int XPath2ResultImpl::asInt() const
{
  if(_currentItem.isNull()) {
    throw DOMException(DOMException::INVALID_STATE_ERR, XMLString::transcode("There is no current result in the result")); 
  }

  if(_currentItem->isNode()) {
    throw XQillaException(DOMXPathException::TYPE_ERR, X("Cannot convert result to int"));
  }  

  AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr)_currentItem;
  Item::Ptr integer;
  try {
    integer = atom->castAs(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                           SchemaSymbols::fgDT_INTEGER, _context);
  } catch (XPath2TypeCastException &e) {
    throw XQillaException(DOMXPathException::TYPE_ERR, X("Cannot convert result to int"));
  }
  
  return atoi(UTF8(integer->asString(_context)));
}

double XPath2ResultImpl::asDouble() const
{
  if(_currentItem.isNull()) {
    throw DOMException(DOMException::INVALID_STATE_ERR, XMLString::transcode("There is no current result in the result"));
  }

  if(_currentItem->isNode()) {
    throw XQillaException(DOMXPathException::TYPE_ERR, X("Cannot convert result to double"));
  }

  AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr)_currentItem;
  Item::Ptr doubleValue;
  try {
    doubleValue = atom->castAs(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                               SchemaSymbols::fgDT_DOUBLE, _context);
  } catch (XPath2TypeCastException &e) {
    throw XQillaException(DOMXPathException::TYPE_ERR, X("Cannot convert result to double"));
  }

  return atof(UTF8(doubleValue->asString(_context)));
}

const XMLCh* XPath2ResultImpl::asString() const
{
  if(_currentItem.isNull()) {
    throw DOMException(DOMException::INVALID_STATE_ERR, XMLString::transcode("There is no current result in the result"));
  }

  if(_currentItem->isNode()) {
    throw XQillaException(DOMXPathException::TYPE_ERR, X("Cannot convert result to a string"));
  }

  return _currentItem->asString(_context);
}

bool XPath2ResultImpl::asBoolean() const
{
  if(_currentItem.isNull()) {
    throw DOMException(DOMException::INVALID_STATE_ERR, XMLString::transcode("There is no current result in the result"));
  }

  if(_currentItem->isNode()) {
    throw XQillaException(DOMXPathException::TYPE_ERR, X("Cannot convert result to a boolean"));
  }

  AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr)_currentItem;
  Item::Ptr boolean;
  try {
    boolean = atom->castAs(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                           SchemaSymbols::fgDT_BOOLEAN, _context);
  } catch (XPath2TypeCastException &e) {
    throw XQillaException(DOMXPathException::TYPE_ERR, X("Cannot convert result to a boolean"));
  }
  return ((const ATBooleanOrDerived*)boolean.get())->isTrue();
}

const DOMNode* XPath2ResultImpl::asNode() const
{
  if(_currentItem.isNull()) {
    throw DOMException(DOMException::INVALID_STATE_ERR, XMLString::transcode("There is no current result in the result"));
  }

  if(!_currentItem->isNode()) {
    throw XQillaException(DOMXPathException::TYPE_ERR, X("The requested result is not a node"));
  }

  const DOMNode *node = (const DOMNode*)_currentItem->getInterface(Node::gXerces);
  if(node == 0) {
    // Should never happen
    throw XQillaException(DOMXPathException::TYPE_ERR, X("The requested result not a XQilla implementation node"));
  }

  return node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

XPath2FirstResultImpl::XPath2FirstResultImpl(const XQQuery *expression,
                                                   DOMNode* contextNode,
                                                   DynamicContext *staticContext,
                                                   MemoryManager* memMgr,
                                                   XQillaExpressionImpl *exprImpl)
  : XPath2ResultImpl(contextNode, staticContext, memMgr, exprImpl)
{
  try {
    _currentItem = expression->execute(_context)->next(_context);
  }
  catch(const XQException &e) {
    if(XQillaException::getDebug()) {
      e.printDebug( X("Caught exception at Interface") );
    }    
    throw XQillaException(e);
  }
  catch(const XQillaException &) {
    // rethrow it
    throw;
  }
  catch(DOMException &e) {
    throw XQillaException(DOMXPathException::INVALID_EXPRESSION_ERR,
	    X("XQillaExpressionImpl::evaluateToSequence(): DOMException!"));
  }
  catch(XMLException &e) {
    throw XQillaException(DOMXPathException::INVALID_EXPRESSION_ERR, e.getMessage());
  }
  catch (...) {
    throw XQillaException(DOMXPathException::INVALID_EXPRESSION_ERR,
	    X("XQillaExpressionImpl::evaluateToSequence(): Unknown exception caught."));
  }
}

XPath2FirstResultImpl::~XPath2FirstResultImpl() { 
}

void XPath2FirstResultImpl::release()
{
  this->~XPath2FirstResultImpl();
  _createdWith->deallocate(this);
}

XPath2Result::ResultType XPath2FirstResultImpl::getResultType() const {
  return XPath2Result::FIRST_RESULT;
}

bool XPath2FirstResultImpl::getInvalidIteratorState() const {
  return false;
}

unsigned long XPath2FirstResultImpl::getSnapshotLength() const {
  string error = errorMessage(XPath2Result::SNAPSHOT_RESULT, XPath2Result::FIRST_RESULT);
  throw XQillaException(DOMXPathException::TYPE_ERR, X(error.c_str()));
}

bool XPath2FirstResultImpl::iterateNext() {
  string error = errorMessage(XPath2Result::ITERATOR_RESULT, XPath2Result::FIRST_RESULT);
  throw XQillaException(DOMXPathException::TYPE_ERR, X(error.c_str()));
}

bool XPath2FirstResultImpl::snapshotItem(unsigned long index) {
  string error = errorMessage(XPath2Result::SNAPSHOT_RESULT, XPath2Result::FIRST_RESULT);
  throw XQillaException(DOMXPathException::TYPE_ERR, X(error.c_str()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

XPath2SnapshotResultImpl::XPath2SnapshotResultImpl(const XQQuery *expression,
                                                   DOMNode* contextNode,
                                                   DynamicContext *staticContext,
                                                   MemoryManager* memMgr,
                                                   XQillaExpressionImpl *exprImpl)
  : XPath2ResultImpl(contextNode, staticContext, memMgr, exprImpl),
    _sequence(0)
{
  try {
    _sequence = new (_createdWith) Sequence(expression->execute(_context)->toSequence(_context));
  }
  catch(const XQException &e) {
    if(XQillaException::getDebug()) {
      e.printDebug( X("Caught exception at Interface") );
    }    
    throw XQillaException(e);
  }
  catch(const XQillaException &) {
    // rethrow it
    throw;
  }
  catch(DOMException &e) {
    throw XQillaException(DOMXPathException::INVALID_EXPRESSION_ERR,
	    X("XQillaExpressionImpl::evaluateToSequence(): DOMException!"));
  }
  catch(XMLException &e) {
    throw XQillaException(DOMXPathException::INVALID_EXPRESSION_ERR, e.getMessage());
  }
  catch (...) {
    throw XQillaException(DOMXPathException::INVALID_EXPRESSION_ERR,
	    X("XQillaExpressionImpl::evaluateToSequence(): Unknown exception caught."));
  }
}

XPath2SnapshotResultImpl::~XPath2SnapshotResultImpl() { 
  // the destructor will try to access the context, that XPath2ResultImpl is about to delete
  if(_sequence) delete _sequence;
}

void XPath2SnapshotResultImpl::release()
{
  this->~XPath2SnapshotResultImpl();
  _createdWith->deallocate(this);
}

XPath2Result::ResultType XPath2SnapshotResultImpl::getResultType() const {
  return XPath2Result::SNAPSHOT_RESULT;
}

bool XPath2SnapshotResultImpl::getInvalidIteratorState() const {
  return false;
}

unsigned long XPath2SnapshotResultImpl::getSnapshotLength() const {
  return _sequence->getLength();
}

bool XPath2SnapshotResultImpl::iterateNext() {
  string error = errorMessage(XPath2Result::ITERATOR_RESULT, XPath2Result::SNAPSHOT_RESULT);
  throw XQillaException(DOMXPathException::TYPE_ERR, X(error.c_str()));
}

bool XPath2SnapshotResultImpl::snapshotItem(unsigned long index) {
  //Reached end of set, return false
  if(index >= _sequence->getLength()) {
    _currentItem = 0;
    return false;
  }

  // this method is const, need to cast that away
  _currentItem = _sequence->item(index);
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

XPath2IteratorResultImpl::XPath2IteratorResultImpl(const XQQuery *expression,
                                                   DOMNode* contextNode,
                                                   DynamicContext *staticContext,
                                                   MemoryManager* memMgr,
                                                   XQillaExpressionImpl *exprImpl)
  : XPath2ResultImpl(contextNode, staticContext, memMgr, exprImpl),
    _results(0),
    _documentRoot(0),
    _changes(0)
{
  if(contextNode != 0) {
    if(contextNode->getNodeType() == DOMNode::DOCUMENT_NODE) {
      _documentRoot = (DOMDocumentImpl*)contextNode;
    }
    else {
      _documentRoot = (DOMDocumentImpl*)contextNode->getOwnerDocument();
    }
    _changes =_documentRoot->changes();
  }

  try {
    _results = expression->execute(_context);
  }
  catch(const XQException &e) {
    if(XQillaException::getDebug()) {
      e.printDebug( X("Caught exception at Interface") );
    }    
    throw XQillaException(e);
  }
  catch(const XQillaException &) {
    // rethrow it
    throw;
  }
  catch(DOMException &e) {
    throw XQillaException(DOMXPathException::INVALID_EXPRESSION_ERR,
	    X("XQillaExpressionImpl::evaluateToSequence(): DOMException!"));
  }
  catch(XMLException &e) {
    throw XQillaException(DOMXPathException::INVALID_EXPRESSION_ERR, e.getMessage());
  }
  catch (...) {
    throw XQillaException(DOMXPathException::INVALID_EXPRESSION_ERR,
	    X("XQillaExpressionImpl::evaluateToSequence(): Unknown exception caught."));
  }
}

XPath2IteratorResultImpl::~XPath2IteratorResultImpl()
{
  // the destructor will try to access the context, that XPath2ResultImpl is about to delete
  if(!_results.isNull()) _results = 0;
}

void XPath2IteratorResultImpl::release()
{
  this->~XPath2IteratorResultImpl();
  _createdWith->deallocate(this);
}

XPath2Result::ResultType XPath2IteratorResultImpl::getResultType() const
{
  return XPath2Result::ITERATOR_RESULT;
}

bool XPath2IteratorResultImpl::getInvalidIteratorState() const
{
  return _documentRoot != 0 && _documentRoot->changes() != _changes;
}

bool XPath2IteratorResultImpl::iterateNext()
{
  // check for document changes
  if(getInvalidIteratorState()) {
    throw DOMException(DOMException::INVALID_STATE_ERR, XMLString::transcode("Document has changed"));
  }//if

  _currentItem = _results->next(_context);

  if(_currentItem.isNull()) {
    _results = 0;
    return false;
  }
  
  return true;
}

unsigned long XPath2IteratorResultImpl::getSnapshotLength() const
{
  string error = errorMessage(XPath2Result::SNAPSHOT_RESULT, XPath2Result::ITERATOR_RESULT);
  throw XQillaException(DOMXPathException::TYPE_ERR, X(error.c_str()));
}

bool XPath2IteratorResultImpl::snapshotItem(unsigned long index)
{
  string error = errorMessage(XPath2Result::SNAPSHOT_RESULT, XPath2Result::ITERATOR_RESULT);
  throw XQillaException(DOMXPathException::TYPE_ERR, X(error.c_str()));
}
