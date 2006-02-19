 /*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/XPath2Result.hpp>
#include <string>

#include <pathan/Node.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/PathanException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/dataItem/Result.hpp>

#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>

#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/impl/DOMDocumentImpl.hpp>


XPath2Result::XPath2Result(const ResultType resultType,
                           const DataItem *expression,
                           DynamicContext *dynamicContext,
                           XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : _createdWith(memMgr),
    _memMgr(memMgr),
    _resultSequence(0),
    _resultType(resultType),
    _contextOwned(false),
    _context(dynamicContext),
    _curIndex(0),
    _changes(-1),
    _documentRoot(0)
{
  if(_resultType == XPath2Result::ITERATOR_RESULT) {
    beforeStart = true;
  }
  evaluate(expression);
}

XPath2Result::XPath2Result(const ResultType resultType,
                           const DataItem *expression,
                           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                           DynamicContext *staticContext,
                           XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : _createdWith(memMgr),
    _memMgr(memMgr),
    _resultSequence(0),
    _resultType(resultType),
    _contextOwned(true),
    _curIndex(0),
    _changes(-1),
    _documentRoot(contextNode->getOwnerDocument())
{
  // retrieve the change count only if we are an iterator
  if(_resultType == XPath2Result::ITERATOR_RESULT && _documentRoot != 0) {
    _changes = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl*)_documentRoot)->changes();
  }

  if(_resultType == XPath2Result::ITERATOR_RESULT) {
    beforeStart = true;
  }

  _context = staticContext->createDynamicContext(&_memMgr);

	//Check for illegal contextNode types
	if(contextNode != 0) {
    //More illegal types here?
    switch (contextNode->getNodeType()) {
    case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ENTITY_REFERENCE_NODE:
      throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NOT_SUPPORTED_ERR, X("Context node of illegal type."));
      break;
    }
    _context->setExternalContextNode(contextNode);
  }
  evaluate(expression);
}

XPath2Result::~XPath2Result() { 
  // the destructor will try to access the context, that we are going to delete now
  if(_resultSequence) delete _resultSequence;
  if(_contextOwned) _context->release();
}

void XPath2Result::release()
{
  this->~XPath2Result();
  _createdWith->deallocate(this);
}

void XPath2Result::evaluate(const DataItem *expression)
{
  int flags = 0;
  if(_resultType == FIRST_RESULT) {
    flags |= DataItem::UNORDERED | DataItem::RETURN_ONE;
  }

  try {
    _resultSequence = new Sequence(expression->collapseTree(_context, flags));
  }
  catch(const DSLException &e) {
    if(PathanException::getDebug()) {
      e.printDebug( X("Caught exception at Interface") );
    }    
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, e.getError());
  }
  catch(const PathanException &) {
    //rethrow it
    throw;
  }
  catch(XERCES_CPP_NAMESPACE_QUALIFIER DOMException &e) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("PathanExpressionImpl::evaluateToSequence(): DOMException!"));
  }
  catch(XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, e.getMessage());
  }
  catch (...) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("PathanExpressionImpl::evaluateToSequence(): Unknown exception caught."));
  }
}

const XPath2Result::ResultType XPath2Result::getResultType() const {
  return _resultType;
}

bool XPath2Result::isNode() const {
  if(_resultSequence->isEmpty() || (_curIndex >=_resultSequence->getLength()) ) {
    return false;
  }

  return _resultSequence->item(_curIndex)->isNode();
}

const XMLCh* XPath2Result::getTypeURI() const
{
   if(_resultSequence->isEmpty() || (_curIndex >=_resultSequence->getLength()) ) {
     return 0;
   }
   const Item::Ptr item = _resultSequence->item(_curIndex);
   if(item->isNode()) {
     const Node::Ptr node = (const Node::Ptr )item;

     ATQNameOrDerived::Ptr name = node->dmTypeName(_context);
     if(name.isNull())
       return 0;
     else
       return ((const ATQNameOrDerived*)name.get())->getURI();
   } else {
     const AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )item;
     return atom->getTypeURI();
   }

   return 0;
}

const XMLCh* XPath2Result::getTypeName() const
{
   if(_resultSequence->isEmpty() || (_curIndex >=_resultSequence->getLength()) ) {
     return 0;
   }
   const Item::Ptr item = _resultSequence->item(_curIndex);
   if(item->isNode()) {
     const Node::Ptr node = (const Node::Ptr )item;

     ATQNameOrDerived::Ptr name = node->dmTypeName(_context);
     if(name.isNull())
       return 0;
     else
       return ((const ATQNameOrDerived*)name.get())->getName();
   } else {
     const AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )item;
     return atom->getTypeName();
   }

   return 0;
}

int XPath2Result::asInt() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) {
  if(_resultSequence->isEmpty() || (_curIndex >=_resultSequence->getLength()) ) {
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::INVALID_STATE_ERR, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("There is no current result in the result")); 
  }

  const Item::Ptr item = _resultSequence->item(_curIndex);
  if(item->isNode()) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("Cannot convert result to int"));
  }  

  AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )item;
  ATDecimalOrDerived::Ptr integer;
  try {
    integer = (const ATDecimalOrDerived::Ptr )atom->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, _context);
  } catch (XPath2TypeCastException &e) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("Cannot convert result to int"));
  }
  
  return atoi(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(integer->asString(_context)));
}

double XPath2Result::asDouble() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) {
  if(_resultSequence->isEmpty() || (_curIndex >=_resultSequence->getLength()) ) {
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::INVALID_STATE_ERR, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("There is no current result in the result"));
  }

  const Item::Ptr item = _resultSequence->item(_curIndex);
  if(item->isNode()) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("Cannot convert result to double"));
  }

  AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )item;
  ATDoubleOrDerived::Ptr doubleValue;
  try {
    doubleValue = (const ATDoubleOrDerived::Ptr )atom->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, _context);
  } catch (XPath2TypeCastException &e) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("Cannot convert result to double"));
  }

  return atof(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(doubleValue->asString(_context)));

}

const XMLCh* XPath2Result::asString() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) {
  if(_resultSequence->isEmpty() || (_curIndex >=_resultSequence->getLength()) ) {
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::INVALID_STATE_ERR, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("There is no current result in the result"));
  }

  const Item::Ptr item = _resultSequence->item(_curIndex);
  if(item->isNode()) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("Cannot convert result to a string"));
  }

  return item->asString(_context);
}

bool XPath2Result::asBoolean() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) {
  if(_resultSequence->isEmpty() || (_curIndex >=_resultSequence->getLength()) ) {
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::INVALID_STATE_ERR, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("There is no current result in the result"));
  }

  const Item::Ptr item = _resultSequence->item(_curIndex);
  if(item->isNode()) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("Cannot convert result to a boolean"));
  }

  AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )item;
  ATBooleanOrDerived::Ptr boolean;
  try {
    boolean = (const ATBooleanOrDerived::Ptr )atom->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN, _context);
  } catch (XPath2TypeCastException &e) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("Cannot convert result to a boolean"));
  }
  return ((const ATBooleanOrDerived*)boolean)->isTrue();
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* XPath2Result::asNode() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) {
  if(_resultSequence->isEmpty() || (_curIndex >=_resultSequence->getLength()) ) {
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::INVALID_STATE_ERR, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("There is no current result in the result"));
  }

  const Item::Ptr item = _resultSequence->item(_curIndex);
  if(!item->isNode()) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("The requested result is not a node"));
  }
  const NodeImpl *nodeImpl = (const NodeImpl*)item->getInterface(Item::gPathan);
  if(nodeImpl == 0) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, X("The requested result not a Pathan implementation node"));
  }

  return nodeImpl->getDOMNode();
 
}

bool XPath2Result::getInvalidIteratorState() const {
  if(_resultType != XPath2Result::ITERATOR_RESULT) {
    return false;
  }

  if(_resultSequence->isEmpty() || (_curIndex >=_resultSequence->getLength()) ) {
    return false;
  }
  
  return this->hasDocumentChanged(_resultSequence->item(_curIndex));
}

unsigned long XPath2Result::getSnapshotLength() const throw(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException) {
  if(_resultType != XPath2Result::SNAPSHOT_RESULT) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, errorMessage(XPath2Result::SNAPSHOT_RESULT, _resultType));
  }
  return _resultSequence->getLength();
}

bool XPath2Result::iterateNext() const throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) {
  if(_resultType != XPath2Result::ITERATOR_RESULT) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, errorMessage(XPath2Result::ITERATOR_RESULT, _resultType));
  }

  // this method is const, need to cast that away
  if(beforeStart) {
    ((XPath2Result*)this)->_curIndex = 0;
    ((XPath2Result*)this)->beforeStart = false;
  } else {
    ((XPath2Result*)this)->_curIndex++;
  }

  //Reached end of set, return false
  if(_curIndex >= _resultSequence->getLength()) {
     return false;
  }
  
  // check for document changes
  if(this->hasDocumentChanged(_resultSequence->item(_curIndex))) {
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::INVALID_STATE_ERR, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("Document has changed"));
  }//if
  
  return true;
}

bool XPath2Result::snapshotItem(unsigned long index) const throw(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException) {
  if(_resultType != XPath2Result::SNAPSHOT_RESULT) {
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::TYPE_ERR, errorMessage(XPath2Result::SNAPSHOT_RESULT, _resultType));
  }

  //Reached end of set, return false
  if(_curIndex >= _resultSequence->getLength()) {
     return false;
  }

  // this method is const, need to cast that away
  ((XPath2Result*)this)->_curIndex = index;
  return true;
}

bool XPath2Result::hasDocumentChanged(const Item::Ptr &item) const {
  if(item->isNode()) {
    if(_documentRoot != 0 && ((XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl*)_documentRoot)->changes() != _changes) {
         return true;
      } else {
         return false;
      }//if
  } else {
    return false;
  }
}

const XMLCh* XPath2Result::errorMessage(XPath2Result::ResultType requestedType,
                                 XPath2Result::ResultType resultType) const {
  std::string message =  typeName(requestedType) + " was requested from a XPath2Result of type " + typeName(resultType);
  return X(message.c_str());
}

const std::string XPath2Result::typeName(XPath2Result::ResultType type) const {
    switch(type) {
      case XPath2Result::FIRST_RESULT: return "FIRST_RESULT";
      case XPath2Result::ITERATOR_RESULT: return "ITERATOR_RESULT";
      case XPath2Result::SNAPSHOT_RESULT: return "SNAPSHOT_RESULT";
    }
    return "";
}
