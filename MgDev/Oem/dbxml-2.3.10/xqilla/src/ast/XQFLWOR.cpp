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
 * $Id: XQFLWOR.cpp,v 1.30 2006/11/01 16:37:18 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQFLWOR.cpp: implementation of the XQFLWOR class.
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQFLWOR.hpp>
#include <xqilla/ast/XQDebugHook.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/context/impl/XQDynamicContextImpl.hpp>
#include <xqilla/context/XQDebugCallback.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/context/VarHashEntry.hpp>
#include <xqilla/context/Scope.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/operators/And.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>
#include <xqilla/context/ContextHelpers.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

// needed to sort
#include <algorithm>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/exceptions/ItemException.hpp>
#include <assert.h>

using namespace std;

static XMLCh szForLoopAccumulator[] = { chUnderscore, chUnderscore,
                                        chLatin_R, chLatin_e,
                                        chLatin_t, chLatin_u,
                                        chLatin_r,
                                        chLatin_n, chUnderscore,
                                        chLatin_V, chLatin_a,
                                        chLatin_l, chLatin_u,
                                        chLatin_e, chUnderscore,
                                        chNull };

/////////////////////////////////////////////////////////////////////
// SortableItem

SortableItem::SortableItem()
{
  m_bAscending=1;
  m_bIsMax=0;
  m_bIsMin=0;
  m_context=NULL;
  m_item=NULL;
  m_collation=NULL;
}

SortableItem::SortableItem(const SortableItem& other)
{
  m_bAscending=other.m_bAscending;
  m_bIsMax=other.m_bIsMax;
  m_bIsMin=other.m_bIsMin;
  m_context=other.m_context;
  m_item=other.m_item;
  m_collation=other.m_collation;
}

bool SortableItem::operator==(const SortableItem& other) const
{
  if(m_bIsMax && other.m_bIsMax)
    return true;
  if(m_bIsMin && other.m_bIsMin)
    return true;
  if(!m_bIsMax && !m_bIsMin && !other.m_bIsMax && !other.m_bIsMin)
    return Equals::equals(m_item,other.m_item,m_collation,m_context,m_info);
  return false;
}

bool SortableItem::operator>(const SortableItem& other) const
{
  if(other.m_bIsMax || m_bIsMin)
    return false;
  if(m_bIsMax || other.m_bIsMin)
    return true;
  return GreaterThan::greater_than(m_item,other.m_item,m_collation,m_context,m_info);
}

static bool compareHelper(const XQFLWOR::ResultPair &firstArg, const XQFLWOR::ResultPair &secondArg)
{
  XQSort::SortItems::const_iterator it1,it2;
  // If V1 and V2 are the values of an ordering expression for input items I1 and I2 respectively, then:
  for(it1=firstArg.second.begin(), it2=secondArg.second.begin();it1!=firstArg.second.end() && it2!=secondArg.second.end();++it1,++it2) {
    assert((*it1).m_bAscending==(*it2).m_bAscending);
    // if they are equal, go to the next ordering key to determine ordering
    if((*it1) == (*it2))
      continue;
    else if((*it1).m_bAscending)
      // a) If the ordering expression is ascending, and if V2 gt V1 is true, then I1 precedes I2 in the output sequence.
      return ((*it2) > (*it1));
    else
      // b) If the ordering expression is descending, and if V1 gt V2 is true, then I1 precedes I2 in the output sequence.
      return ((*it1) > (*it2));
  }
  // c) If neither V1 gt V2 nor V2 gt V1 is true, and stable is specified, then the input order of I1 and I2 is preserved in output sequence.
  // d) If neither V1 gt V2 nor V2 gt V1 is true, and stable is not specified, then the order of I1 and I2 in the output sequence is implementation-defined.
  // These are handled with the choice of sort algorithm below
  return false;
}

const long XQSort::SortSpec::ascending=1;
const long XQSort::SortSpec::descending=2;
const long XQSort::SortSpec::empty_greatest=4;
const long XQSort::SortSpec::empty_least=8;

XQSort::SortSpec::SortSpec(ASTNode* expr, sortModifier modifier, const XMLCh* collation)
  : _expr(expr),
    _modifier(modifier),
    _collationURI(collation),
    _collation(0)
{
}

void XQSort::SortSpec::staticResolution(StaticContext *context, StaticResolutionContext &src)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  SequenceType *zero_or_one = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING),
                                                    SequenceType::QUESTION_MARK);
  zero_or_one->setLocationInfo(this);

  _expr = new (mm) XQAtomize(_expr, mm);
  _expr->setLocationInfo(this);
  _expr = new (mm) XQPromoteUntyped(_expr, SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                    SchemaSymbols::fgDT_STRING, mm);
  _expr->setLocationInfo(this);
  _expr = new XQTreatAs(_expr, zero_or_one, mm);
  _expr->setLocationInfo(this);
  _expr = _expr->staticResolution(context);
}

void XQSort::SortSpec::staticTyping(StaticContext *context, StaticResolutionContext &src)
{
  _expr = _expr->staticTyping(context);
  src.add(_expr->getStaticResolutionContext());

  if(_collationURI == NULL)
    _collation = context->getDefaultCollation(this);
  else {
    _collation = context->getCollation(_collationURI, this);
  }
}

SortableItem XQSort::SortSpec::buildKey(DynamicContext* context)
{
  Result atomized = _expr->collapseTree(context);

  SortableItem value;
  value.m_info = this;
  value.m_item = atomized->next(context);

  if(!value.m_item.isNull()) {
    // if it's a NaN, it must be treated as if it were the empty sequence
    const AnyAtomicType *atom = (const AnyAtomicType*)value.m_item.get();
    if(atom->isNumericValue() && XPath2Utils::equals(atom->asString(context), Numeric::NaN_string))
      value.m_item = NULL;
  }

  value.m_bAscending = (_modifier & ascending) && !(_modifier & descending);

  if(value.m_item.isNull()) {
      value.m_bIsMax = (_modifier & empty_greatest) && !(_modifier & empty_least);
      value.m_bIsMin = !(_modifier & empty_greatest) || (_modifier & empty_least);
  }

  value.m_context = context;
  value.m_collation = _collation;

  return value;
}

const ASTNode *XQSort::SortSpec::getExpression() const
{
  return _expr;
}

XQSort::SortSpec::sortModifier XQSort::SortSpec::getModifier() const
{
  return _modifier;
}

const XMLCh *XQSort::SortSpec::getCollation() const
{
  return _collationURI;
}

const Collation *XQSort::SortSpec::getCollationObject() const
{
  return _collation;
}

void XQSort::SortSpec::setExpression(ASTNode *expr)
{
  _expr = expr;
}

/////////////////////////////////////////////////////////////////////
// XQSort

XQSort::XQSort(XQSort::sortType type, VectorOfSortSpec* specList)
  : _type(type),
    _specList(specList)
{

}

void XQSort::staticResolution(StaticContext *context, StaticResolutionContext &src)
{
  for(VectorOfSortSpec::iterator it = _specList->begin(); it != _specList->end(); ++it) {
    (*it)->staticResolution(context, src);
  }
}

void XQSort::staticTyping(StaticContext *context, StaticResolutionContext &src)
{
  for(VectorOfSortSpec::iterator it = _specList->begin(); it != _specList->end(); ++it) {
    (*it)->staticTyping(context, src);
  }
}

XQSort::SortItems XQSort::buildKeys(DynamicContext *context) const
{
  // create a vector to hold the data types of the ordering items, and fill it with UNTYPED_ATOMIC
  vector<AnyAtomicType::AtomicObjectType> orderingItemsTypes;
  VectorOfSortSpec::const_iterator it;
  for(it=_specList->begin(); it!=_specList->end(); ++it)
    orderingItemsTypes.push_back(AnyAtomicType::UNTYPED_ATOMIC);

  XQSort::SortItems keys;
  // evaluate all the keys in the new context
  vector<AnyAtomicType::AtomicObjectType>::iterator typeIt=orderingItemsTypes.begin();
  for(it=_specList->begin(); it!=_specList->end(); ++it,++typeIt)
    {
      SortableItem orderingItem=(*it)->buildKey(context);
      AnyAtomicType::AtomicObjectType keyType=AnyAtomicType::UNTYPED_ATOMIC;
      if(orderingItem.m_item!=NULLRCP)
        keyType=orderingItem.m_item->getPrimitiveTypeIndex();

      if(keyType==AnyAtomicType::ANY_SIMPLE_TYPE)
        {
          orderingItem.m_item=context->getItemFactory()->createString(orderingItem.m_item->asString(context),context);
          keyType=orderingItem.m_item->getPrimitiveTypeIndex();
        }

      // check that the types of the ordering items are the same
      if(*typeIt==AnyAtomicType::UNTYPED_ATOMIC)
        *typeIt=keyType;
      else if(keyType!=AnyAtomicType::UNTYPED_ATOMIC && *typeIt!=keyType)
        XQThrow3(ItemException, X("XQSort::buildKeys"), X("Ordering item is not of the required type"), *it);

      keys.push_back(orderingItem);
    }

  return keys;
}

XQSort::sortType XQSort::getSortType() const
{
  return _type;
}

const XQSort::VectorOfSortSpec *XQSort::getSortSpecs() const
{
  return _specList;
}

/////////////////////////////////////////////////////////////////////
// ProductFactor

XQFLWOR::ProductFactor::ProductFactor(const XQVariableBinding *vb)
  : _vb(vb),
    _values(0),
    _valuesBuffer(0),
    _index(0),
    _refVar(0),
    _posRefVar(0)
{
}

bool XQFLWOR::ProductFactor::initialise(DynamicContext *context)
{
  // Compute the values, if needed
  if(_vb->_valuesResultMustBeRecalculated) {
    _values = _vb->_allValues->collapseTree(context);
  }
  else {
    if(_valuesBuffer.isNull()) {
      _valuesBuffer = _vb->_allValues->collapseTree(context);
    }
    _values = _valuesBuffer.createResult();
  }

  VariableStore* varStore = context->getVariableStore();

  // Make debug callback before declaring the variable
  if(context->getDebugCallback()) context->getDebugCallback()->IsBreakPointHit(context, _vb->getFile(), _vb->getLine());

  if(_vb->_bindingType == XQVariableBinding::letBinding) {
    // Add a new scope, if needed
    if(_vb->_needsNewScope) {
      varStore->addLogicalBlockScope();
    }

    // If it's a let binding populate the variable with all of the values
    varStore->declareVar(_vb->_vURI, _vb->_vName, _values->toSequence(context), context);
  }
  else {
    const Item::Ptr item = _values->next(context);

    if(item == NULLRCP) {
      return false;
    }

    // Add a new scope, if needed
    if(_vb->_needsNewScope) {
      varStore->addLogicalBlockScope();
    }

    // Declare the binding variable, if present
    if(_vb->_variable) {
      varStore->declareVar(_vb->_vURI, _vb->_vName, Sequence(item, context->getMemoryManager()), context);
      _refVar = varStore->getReferenceVar(_vb->_vURI, _vb->_vName, context);
    }

    // Declare the positional variable, if present
    if(_vb->_positionalVariable) {
      _index = 1;
      varStore->declareVar(_vb->_pURI, _vb->_pName,
                           Sequence((const Item::Ptr)context->getItemFactory()->createInteger(_index, context), context->getMemoryManager()), context);
      _posRefVar = varStore->getReferenceVar(_vb->_pURI, _vb->_pName, context);
    }
  }

  return true;
}

bool XQFLWOR::ProductFactor::next(DynamicContext *context)
{
  if(_vb->_bindingType == XQVariableBinding::letBinding) {
    // A let binding never has a next
    if(_vb->_needsNewScope) {
      context->getVariableStore()->removeScope();
    }
    return false;
  }
  else {
    const Item::Ptr item = _values->next(context);

    if(item == NULLRCP) {
      if(_vb->_needsNewScope) {
        context->getVariableStore()->removeScope();
      }
      return false;
    }

    // Make debug callback before declaring the variable
    if(context->getDebugCallback()) context->getDebugCallback()->IsBreakPointHit(context, _vb->getFile(), _vb->getLine());

    // Set the binding variable value
    if(_vb->_variable) {
      _refVar->setValue(Sequence(item, context->getMemoryManager()));
    }

    // Set the positional variable value, if present
    if(_vb->_positionalVariable) {
      ++_index;
      _posRefVar->setValue(Sequence((const Item::Ptr)context->getItemFactory()->createInteger(_index, context), context->getMemoryManager()));
    }

    return true;
  }
}

bool XQFLWOR::ProductFactor::checkWhere(DynamicContext *context)
{
  return _vb->_where == 0 || _vb->_where->collapseTree(context)->getEffectiveBooleanValue(context, _vb->_where);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQFLWOR::XQFLWOR(VectorOfVariableBinding* bindings, ASTNode* where, XQSort* orderBy, ASTNode* returnExpr, XPath2MemoryManager* expr)
  : ASTNodeImpl(expr),
    _return(returnExpr),
    _where(where),
    _sort(orderBy)
{
  _bindings=bindings;
  setType(ASTNode::FLWOR);
}

bool XQFLWOR::nextState(ExecutionBindings &ebs, DynamicContext *context, bool initialisationState) const
{
  ExecutionBindings::reverse_iterator rend = ebs.rend();
  ExecutionBindings::reverse_iterator rbegin = ebs.rbegin();
  ExecutionBindings::reverse_iterator it;

  if(initialisationState) {
    it = rend;
  }
  else {
    it = rbegin;
  }

  // This algorithm works with a REVERSE ITERATOR.
  //
  // When in the initialisation state, we travel backwards (towards
  // the end of the collection), calling initialise() on the ProductFactors.
  //
  // When not in initialisation state, we travel forwards (towards
  // the begining of the collection, calling next() on the ProductFactors.
  //
  // Everything after the iterator (towards the start of the collection)
  // has been initialised. Everything before the iterator (towards the
  // end of the collection) still needs to be initialised. When
  // ProductFactor::next() returns false, the ProductFactor needs to be
  // initialised again, so we enter the initialisation state.
  //
  // When all the next() methods return false, we have finnished looping,
  // and so return false.

  while(true) {
    if(initialisationState) {
      if(it == rbegin) {
        if(checkWhere(context)) {
          return true;
        }
        else {
          initialisationState = false;
        }
      }
      else {
        --it;
        if(!it->initialise(context)) {
          initialisationState = false;
          ++it;
        }
        else if(!it->checkWhere(context)) {
          // Call next on this one, before continuing with the initialisation
          initialisationState = false;
        }
      }
    }
    else {
      if(it == rend) {
        return false;
      }
      else {
        if(it->next(context)) {
          if(it->checkWhere(context)) {
            initialisationState = true;
          }
          else {
            // Call next on this one again
          }
        }
        else {
          ++it;
        }
      }
    }
  }
}

bool XQFLWOR::checkWhere(DynamicContext *context) const
{
  return _where == 0 || _where->collapseTree(context)->getEffectiveBooleanValue(context, _where);
}

VariableStore::Entry *XQFLWOR::getAccumulator(DynamicContext *context) const
{
  VariableStore* varStore = context->getVariableStore();

  if(context->isDebuggingEnabled()) {
    // find a unique name for the variable to be used as accumulator for the result
    // use the line number, plus a counter if more than one "for" is on the same line
    XMLCh szNumBuff[20];
    XMLString::binToText(_bindings->front()->getLine(), szNumBuff, 19, 10);
    const XMLCh *szInitialAccumulator, *szAccumulatorName;
    szInitialAccumulator = szAccumulatorName = XPath2Utils::concatStrings(szForLoopAccumulator, szNumBuff, context->getMemoryManager());
    long index = 1;
    while(varStore->getReferenceVar(szAccumulatorName, context, this) != NULL) {
      static XMLCh szSpaceParent[] = { chSpace, chOpenParen, chNull };
      XMLString::binToText(index++, szNumBuff, 19, 10);
      szAccumulatorName = XPath2Utils::concatStrings(szInitialAccumulator, szSpaceParent, szNumBuff, context->getMemoryManager());
      szAccumulatorName = XPath2Utils::concatStrings(szAccumulatorName, chCloseParen, context->getMemoryManager());
    }

    varStore->declareVar(szAccumulatorName, Sequence(context->getMemoryManager()), context, this);
    return varStore->getReferenceVar(szAccumulatorName, context, this);
  }

  return 0;
}

void XQFLWOR::setAccumulator(VariableStore::Entry *refVar, const PreSortResult &toBeSorted, DynamicContext *context) const
{
  if(context->isDebuggingEnabled()) {
    Sequence curReturnValue(context->getMemoryManager());
    for(PreSortResult::const_iterator orderIt = toBeSorted.begin(); orderIt != toBeSorted.end(); ++orderIt)
      curReturnValue.joinSequence(orderIt->first);
    refVar->setValue(curReturnValue);
  }
}

Result XQFLWOR::createResult(DynamicContext* context, int flags) const
{
  return createResultImpl(_bindings->begin(), _bindings->end(), context, flags);
}

Result XQFLWOR::createResultImpl(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                                 DynamicContext* context, int flags) const
{
  if((_sort) || context->isDebuggingEnabled()) {
    return new SortingFLWORResult(it, end, this, flags, context);
  }
  else {
    return new FLWORResult(it, end, this, flags);
  }
}

ASTNode* XQFLWOR::staticResolution(StaticContext* context)
{
  staticResolutionImpl(context);
  return this;
}

ASTNode *XQFLWOR::staticTyping(StaticContext *context)
{
  _src.clear();

  staticTypingImpl(context);

  if(_bindings->empty()) {
    return _return->staticTyping(context);
  }
  return this;
}

void XQFLWOR::staticResolutionImpl(StaticContext* context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  // Add all the binding variables to the new scope
  VectorOfVariableBinding::iterator end = _bindings->end();
  for(VectorOfVariableBinding::iterator it0 = _bindings->begin(); it0 != end; ++it0) {
    // Work out the uri and localname of the variable binding
    const XMLCh* prefix=XPath2NSUtils::getPrefix((*it0)->_variable, mm);
    if(prefix && *prefix)
      (*it0)->_vURI = context->getUriBoundToPrefix(prefix, this);
    (*it0)->_vName = XPath2NSUtils::getLocalName((*it0)->_variable);

    // call static resolution on the value
    if((*it0)->_seqType!=NULL) {
      if((*it0)->_bindingType == XQVariableBinding::forBinding) {
        // the SequenceType has been specified for each item of the sequence, but we can only apply to the
        // sequence itself, so allow it to match multiple matches
        (*it0)->_seqType->setOccurrence(SequenceType::STAR);
      }
      (*it0)->_allValues = new (mm) XQTreatAs((*it0)->_allValues, (*it0)->_seqType, mm);
      (*it0)->_allValues->setLocationInfo(this);
    }
    (*it0)->_allValues = (*it0)->_allValues->staticResolution(context);

    if((*it0)->_positionalVariable != NULL && *((*it0)->_positionalVariable) != 0) {
      // Work out the uri and localname of the positional variable binding
      const XMLCh* prefix=XPath2NSUtils::getPrefix((*it0)->_positionalVariable, mm);
      if(prefix && *prefix)
        (*it0)->_pURI = context->getUriBoundToPrefix(prefix, this);
      (*it0)->_pName = XPath2NSUtils::getLocalName((*it0)->_positionalVariable);
      (*it0)->_pSrc.getStaticType().flags = StaticType::DECIMAL_TYPE;

      if(XPath2Utils::equals((*it0)->_pURI, (*it0)->_vURI) && 
         XPath2Utils::equals((*it0)->_pName, (*it0)->_vName))
      {
        XMLBuffer errMsg;
        errMsg.set(X("The positional variable with name {"));
        errMsg.append((*it0)->_pURI);
        errMsg.append(X("}"));
        errMsg.append((*it0)->_pName);
        errMsg.append(X(" conflicts with the iteration variable [err:XQST0089]"));
        XQThrow(StaticErrorException,X("XQFLWOR::staticResolutionImpl"), errMsg.getRawBuffer());
      }
    }
    else {
      // Make life easier when we execute
      (*it0)->_positionalVariable = 0;
    }
  }

  // Call staticResolution on the where expression, if there is one
  if(_where) {
    AutoNodeSetOrderingReset orderReset(context);
    _where = _where->staticResolution(context);
  }

  // Call staticResolution on the sort specifications, if there are some
  if(_sort) {
    _sort->staticResolution(context, _src);
  }

  // Call staticResolution on the return expression
  _return = _return->staticResolution(context);
}

void XQFLWOR::staticTypingImpl(StaticContext* context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  VectorOfVariableBinding *newBindings =
    new (getMemoryManager()) VectorOfVariableBinding(XQillaAllocator<XQVariableBinding*>(getMemoryManager()));

  VariableTypeStore* varStore = context->getVariableTypeStore();

  // Add all the binding variables to the new scope
  VectorOfVariableBinding::iterator end = _bindings->end();
  for(VectorOfVariableBinding::iterator it0 = _bindings->begin(); it0 != end; ++it0) {
    varStore->addLogicalBlockScope();

    // call static resolution on the value
    (*it0)->_allValues = (*it0)->_allValues->staticTyping(context);
    (*it0)->_src.getStaticType() = (*it0)->_allValues->getStaticResolutionContext().getStaticType();

    // Declare the variable binding
    if((*it0)->_bindingType == XQVariableBinding::letBinding) {
      (*it0)->_src.setProperties((*it0)->_allValues->getStaticResolutionContext().getProperties());
      varStore->declareVar((*it0)->_vURI, (*it0)->_vName, (*it0)->_src);
    }
    else {
      (*it0)->_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
	      StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE | StaticResolutionContext::ONENODE);
      varStore->declareVar((*it0)->_vURI, (*it0)->_vName, (*it0)->_src);
    }

    if((*it0)->_positionalVariable != NULL) {
      (*it0)->_pSrc.getStaticType().flags = StaticType::DECIMAL_TYPE;

      // Declare the positional variable binding
      varStore->declareVar((*it0)->_pURI, (*it0)->_pName, (*it0)->_pSrc);
    }
  }

  // Call staticResolution on the where expression, if there is one
  if(_where) {
    AutoNodeSetOrderingReset orderReset(context);
    _where = calculateSRCForWhere(_where, context);
    if(_where) _src.add(_where->getStaticResolutionContext());
  }

  // Call staticResolution on the sort specifications, if there are some
  if(_sort) {
    _sort->staticTyping(context, _src);
  }

  // Call staticResolution on the return expression
  _return = _return->staticTyping(context);
  _src.getStaticType() = _return->getStaticResolutionContext().getStaticType();
  _src.setProperties(_return->getStaticResolutionContext().getProperties());
  _src.add(_return->getStaticResolutionContext());

  VectorOfVariableBinding::reverse_iterator rend = _bindings->rend();
  for(VectorOfVariableBinding::reverse_iterator it = _bindings->rbegin(); it != rend; ++it) {
    XQVariableBinding *newVB = new (getMemoryManager()) XQVariableBinding(getMemoryManager(), **it);
    newVB->setLocationInfo(*it);

    // Statically resolve any binding specific where conditions, if we have them
    if(newVB->_where) {
      newVB->_where = newVB->_where->staticTyping(context);
      _src.add(newVB->_where->getStaticResolutionContext());
    }

    // Remove our variable binding and the scope we added
    varStore->removeScope();

    // Remove our binding variable from the StaticResolutionContext data (removing it if it's not used)
    if(!_src.removeVariable(newVB->_vURI, newVB->_vName) && !context->isDebuggingEnabled()) {
      newVB->_variable = 0;
    }

    // Remove our positional variable from the StaticResolutionContext data (removing it if it's not used)
    if(newVB->_positionalVariable && !_src.removeVariable(newVB->_pURI, newVB->_pName) && !context->isDebuggingEnabled()) {
      newVB->_positionalVariable = 0;
    }

    const StaticResolutionContext &valueSrc = newVB->_allValues->getStaticResolutionContext();
    // if the expression makes use of a variable with the same name of the binding, we need a new scope
    if(valueSrc.isVariableUsed(newVB->_vURI, newVB->_vName))
      newVB->_needsNewScope=true;
    _src.add(valueSrc);

    VectorOfVariableBinding::reverse_iterator it2;
    // Check if previous variable bindings have been used by this value,
    for(it2 = it + 1; it2 != rend; ++it2) {
      if((*it2)->_variable && valueSrc.isVariableUsed((*it2)->_vURI, (*it2)->_vName)) {
        newVB->_valuesResultMustBeRecalculated = true;
        break;
      }
      if((*it2)->_positionalVariable && valueSrc.isVariableUsed((*it2)->_pURI, (*it2)->_pName)) {
        newVB->_valuesResultMustBeRecalculated = true;
        break;
      }
    }
    // DbXml optimises collection() and doc(), so they have to be recalculated.
    // Creative expressions must be re-evaluated to get a new node identity.
    if(valueSrc.areDocsOrCollectionsUsed() || valueSrc.isCreative()) {
      newVB->_valuesResultMustBeRecalculated = true;
    }

    // Check to see if this binding has the same name as any before it
    // (4 comparisons, since each binding has two possible variables for it)
    for(it2 = it + 1; newVB->_needsNewScope==false && it2 != rend; ++it2) {
      if(newVB->_variable && (*it2)->_variable &&
         XPath2Utils::equals(newVB->_vName, (*it2)->_vName) && XPath2Utils::equals(newVB->_vURI, (*it2)->_vURI)) {
        newVB->_needsNewScope = true;
        break;
      }
      if(newVB->_variable && (*it2)->_positionalVariable &&
         XPath2Utils::equals(newVB->_vName, (*it2)->_pName) && XPath2Utils::equals(newVB->_vURI, (*it2)->_pURI)) {
        newVB->_needsNewScope = true;
        break;
      }
      if(newVB->_positionalVariable && (*it2)->_variable &&
         XPath2Utils::equals(newVB->_pName, (*it2)->_vName) && XPath2Utils::equals(newVB->_pURI, (*it2)->_vURI)) {
        newVB->_needsNewScope = true;
        break;
      }
      if(newVB->_positionalVariable && (*it2)->_positionalVariable &&
         XPath2Utils::equals(newVB->_pName, (*it2)->_pName) && XPath2Utils::equals(newVB->_pURI, (*it2)->_pURI)) {
        newVB->_needsNewScope = true;
        break;
      }
    }

    // Add the new VB at the front of the new Bindings
    // (If it's a let binding, and it's variable isn't used, don't add it - there's no point)
    if(context->isDebuggingEnabled() || newVB->_bindingType != XQVariableBinding::letBinding || newVB->_variable) {
      newBindings->insert(newBindings->begin(), newVB);
    }

    if(!_src.isUsed()) {
      AutoDelete<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(mm);
      Result result = createResultImpl(newBindings->begin(), newBindings->end(), dContext);

      XQSequence *newReturn = XQSequence::constantFold(result, dContext, getMemoryManager(), *it);
      if(newReturn != 0) {
        // Constant folding succeeded
        _return = newReturn;
        _return->setLocationInfo(*it);
        newBindings->clear();
      }
    }
  }

  // Overwrite our bindings with the new ones
  _bindings = newBindings;
}

ASTNode *XQFLWOR::calculateSRCForWhere(ASTNode *where, StaticContext *context)
{
  if(context->isDebuggingEnabled()) {
    // Don't do anything special if this is a debug build
    where = where->staticTyping(context);
  }
  else {
    if(where->getType()==ASTNode::OPERATOR &&
       XPath2Utils::equals(((XQOperator*)where)->getOperatorName(), And::name)) {

      // If it's an And, treat each argument to the And as a where condition in it's own right
      And* fAnd = (And*)where;
      unsigned int index = 0;
      while(index < fAnd->getNumArgs()) {
        ASTNode* subWhere = calculateSRCForWhere(fAnd->getArgument(index), context);
        if(subWhere) {
          fAnd->setArgument(index, subWhere);
          ++index;
        }
        else {
          fAnd->removeArgument(index);
        }
      }

      if(fAnd->getNumArgs() == 0) {
        // Remove the And if it no longer has any arguments
        where = 0;
      }
    }
    else {
      // Try to send the condition to the outer most condition possible
      where = where->staticTyping(context);
      const StaticResolutionContext &whereSrc = where->getStaticResolutionContext();

      VectorOfVariableBinding::reverse_iterator rend = _bindings->rend();
      VectorOfVariableBinding::reverse_iterator rbegin = _bindings->rbegin();
      VectorOfVariableBinding::reverse_iterator it = rbegin;
      for(; it != rend; ++it) {
        if((*it)->_variable && whereSrc.isVariableUsed((*it)->_vURI, (*it)->_vName)) {
          (*it)->addWhereCondition(where);
          where = 0;
          break;
        }
        if((*it)->_positionalVariable && whereSrc.isVariableUsed((*it)->_pURI, (*it)->_pName)) {
          (*it)->addWhereCondition(where);
          where = 0;
          break;
        }
      }
      if(it == rend && rbegin != rend) {
        // The where condition doesn't use any of the variables from the bindings!
        // Put it on the first binding...
        (*--it)->addWhereCondition(where);
        where = 0;
      }

    }
  }

  return where;
}

const VectorOfVariableBinding *XQFLWOR::getBindings() const
{
  return _bindings;
}

const ASTNode *XQFLWOR::getWhereExpr() const
{
  return _where;
}

const ASTNode *XQFLWOR::getReturnExpr() const
{
  return _return;
}

const XQSort *XQFLWOR::getSort() const
{
  return _sort;
}

void XQFLWOR::setWhereExpr(ASTNode *where)
{
  _where = where;
}

void XQFLWOR::setReturnExpr(ASTNode *ret)
{
  _return = ret;
}

XQFLWOR::SortingFLWORResult::SortingFLWORResult(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                                                const XQFLWOR *flwor, int flags, DynamicContext *context)
  : LazySequenceResult(flwor, context),
    _flwor(flwor),
    _flags(flags)
{
  for(; it != end; ++it) {
    _ebs.push_back(ProductFactor(*it));
  }
}

void XQFLWOR::SortingFLWORResult::getResult(Sequence &toFill, DynamicContext *context) const
{
  // TODO - instead of sorting at the end, pre-calculate the different
  // product factors, and sort them - then use them in order to lazily
  // calculate the return values. - jpcs

  VariableStore* varStore = context->getVariableStore();

  // Initialise and run the execution bindings
  varStore->addLogicalBlockScope();

  VariableStore::Entry *accumulator = _flwor->getAccumulator(context);
  bool sorting = (_flwor->getSort() != 0);

  PreSortResult toBeSorted;
  if(_flwor->nextState(const_cast<ExecutionBindings&>(_ebs), context, true)) {
    do {
      Sequence result(_flwor->getReturnExpr()->collapseTree(context, _flags)->toSequence(context));

      if(sorting) {
        toBeSorted.push_back(ResultPair(result, _flwor->getSort()->buildKeys(context)));
      }
      else {
        toBeSorted.push_back(ResultPair(result, XQSort::SortItems()));
      }

      if(accumulator) _flwor->setAccumulator(accumulator, toBeSorted, context);

    } while(_flwor->nextState(const_cast<ExecutionBindings&>(_ebs), context, false));
  }
  varStore->removeScope();

  if(sorting) {
    // Sort the results
    switch(_flwor->getSort()->getSortType()) {
    case XQSort::stable: {
      stable_sort(toBeSorted.begin(), toBeSorted.end(), compareHelper);
      break;
    }
    case XQSort::unstable: {
      sort(toBeSorted.begin(), toBeSorted.end(), compareHelper);
      break;
    }
    }
  }

  // Concatonate the sorted results
  for(PreSortResult::iterator orderIt = toBeSorted.begin(); orderIt != toBeSorted.end(); ++orderIt)
    toFill.joinSequence(orderIt->first);
}

std::string XQFLWOR::SortingFLWORResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<sortedflwor/>" << std::endl;

  return oss.str();
}

XQFLWOR::FLWORResult::FLWORResult(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                                  const XQFLWOR *flwor, int flags)
  : ResultImpl(flwor),
    _flwor(flwor),
    _flags(flags),
    _toInit(true),
    _scope(0),
    _returnResult(0)
{
  for(; it != end; ++it) {
    _ebs.push_back(ProductFactor(*it));
  }
}

XQFLWOR::FLWORResult::~FLWORResult()
{
  if(_scope) delete _scope;
}

Item::Ptr XQFLWOR::FLWORResult::next(DynamicContext *context)
{
  VariableStore* varStore = context->getVariableStore();

  Scope<Sequence> *oldScope = varStore->getScopeState();

  if(_toInit) {
    _toInit = false;
    varStore->addLogicalBlockScope();
    if(_flwor->nextState(_ebs, context, true)) {
      _returnResult = _flwor->getReturnExpr()->collapseTree(context, _flags);
    }
    else {
      varStore->removeScope();
      _scope = 0;
      return 0;
    }
  }
  else {
    if(_scope == 0) {
      return 0;
    }
    varStore->setScopeState(_scope);
    _scope = 0;
  }

  Item::Ptr result = _returnResult->next(context);

  while(result == NULLRCP) {
    context->testInterrupt();
    if(_flwor->nextState(_ebs, context, false)) {
      _returnResult = _flwor->getReturnExpr()->collapseTree(context, _flags);
      result = _returnResult->next(context);
    }
    else {
      varStore->removeScope();
      _scope = 0;
      _returnResult = 0;
      return 0;
    }
  }

  _scope = varStore->getScopeState();
  varStore->setScopeState(oldScope);

  return result;
}

std::string XQFLWOR::FLWORResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<flwor/>" << std::endl;

  return oss.str();
}
