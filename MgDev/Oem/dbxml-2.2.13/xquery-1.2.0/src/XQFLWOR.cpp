//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQFLWOR.cpp,v 1.31 2005/11/30 17:08:55 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQFLWOR.cpp: implementation of the XQFLWOR class.
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include "xquery/XQEngine.hpp"
#include "xquery/dataItem/XQFLWOR.hpp"
#include "xquery/dataItem/XQDebugHook.hpp"
#include "xquery/context/XQDynamicContextImpl.hpp"
#include "xquery/XQDebugCallback.hpp"
#include <pathan/operators/And.hpp>
#include <pathan/VariableStore.hpp>
#include <pathan/simpleVariables/VariableTypeStore.hpp>
#include <pathan/simpleVariables/VarHashEntry.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/parser/QName.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/PathanFactory.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

// needed to sort
#include <algorithm>
#include <pathan/operators/GreaterThan.hpp>
#include <pathan/operators/Equals.hpp>
#include <pathan/exceptions/ItemException.hpp>
#include <assert.h>

using namespace std;

static XMLCh szForLoopAccumulator[] = { XERCES_CPP_NAMESPACE_QUALIFIER chUnderscore, XERCES_CPP_NAMESPACE_QUALIFIER chUnderscore,
                                        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_R, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
                                        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u,
                                        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
                                        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chUnderscore,
                                        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_V, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
                                        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u,
                                        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chUnderscore,
                                        XERCES_CPP_NAMESPACE_QUALIFIER chNull };

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
    return Equals::equals(m_item,other.m_item,m_collation,m_context);
  return false;
}

bool SortableItem::operator>(const SortableItem& other) const
{
  if(other.m_bIsMax || m_bIsMin)
    return false;
  if(m_bIsMax || other.m_bIsMin)
    return true;
  return GreaterThan::greater_than(m_item,other.m_item,m_collation,m_context);
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

XQSort::SortSpec::SortSpec(DataItem* expr, sortModifier modifier, const XMLCh* collation)
  : _expr(expr),
    _modifier(modifier),
    _collation(collation)
{
}

void XQSort::SortSpec::staticResolution(StaticContext *context, StaticResolutionContext &src)
{
  _expr = _expr->staticResolution(context);
  src.add(_expr->getStaticResolutionContext());
}

SortableItem XQSort::SortSpec::buildKey(DynamicContext* context)
{
  Sequence atomized=_expr->collapseTree(context).atomize(context);
  SortableItem value;
  if(atomized.isEmpty())
    value.m_item=NULL;
  else if(atomized.getLength()>1)
    DSLthrow(ItemException, X("XQSort::SortSpec::buildKey"), X("The specified key returns multiple values"));
  else
  {
    AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )atomized.first();
    assert(atom->isAtomicValue());
    // need to manually convert xdt:untypedAtomic to xs:string
    if(atom->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC)
      atom = atom->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
    value.m_item=atom;
  }
  value.m_bAscending=(_modifier & ascending) && !(_modifier & descending);
  if(value.m_item==NULLRCP)
  {
      value.m_bIsMax=(_modifier & empty_greatest) && !(_modifier & empty_least);
      value.m_bIsMin=!(_modifier & empty_greatest) || (_modifier & empty_least);
  }
  value.m_context=context;
  if(_collation==NULL)
    value.m_collation=context->getDefaultCollation();
  else
  {
    Collation* collation=context->getCollation(_collation);
    if(collation==NULL)
      DSLthrow(ItemException,X("XQSort::SortSpec::buildKey"),X("Collation object is not available"));
    value.m_collation=collation;
  }
  return value;
}

const DataItem *XQSort::SortSpec::getExpression() const
{
  return _expr;
}

XQSort::SortSpec::sortModifier XQSort::SortSpec::getModifier() const
{
  return _modifier;
}

const XMLCh *XQSort::SortSpec::getCollation() const
{
  return _collation;
}

void XQSort::SortSpec::setExpression(DataItem *expr)
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
          orderingItem.m_item=context->getPathanFactory()->createString(orderingItem.m_item->asString(context),context);
          keyType=orderingItem.m_item->getPrimitiveTypeIndex();
        }

      // check that the types of the ordering items are the same
      if(*typeIt==AnyAtomicType::UNTYPED_ATOMIC)
        *typeIt=keyType;
      else if(keyType!=AnyAtomicType::UNTYPED_ATOMIC && *typeIt!=keyType)
        DSLthrow(ItemException, X("XQSort::buildKeys"), X("Ordering item is not of the required type"));

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

XQFLWOR::ProductFactor::ProductFactor(const XQVariableBinding *vb, DynamicContext *context)
  : _vb(vb),
    _values(0),
    _valuesBuffer(0),
    _index(0),
    _refVar(0),
    _posRefVar(0)
{
}

bool XQFLWOR::ProductFactor::initialise(DynamicContext *ctx)
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);

  // Compute the values, if needed
  if(_vb->_valuesResultMustBeRecalculated) {
    _values = _vb->_allValues->collapseTree(context);
  }
  else {
    if(_valuesBuffer.isNull()) {
      _valuesBuffer = _vb->_allValues->collapseTree(context);
    }
    _values = _valuesBuffer.createResult(context);
  }

  VariableStore* varStore = context->getVariableStore();

  // Make debug callback before declaring the variable
  if(context->getDebugCallback()) context->getDebugCallback()->IsBreakPointHit(context, _vb->_file, _vb->_line);

  if(_vb->_bindingType == XQVariableBinding::letBinding) {
    if(_vb->_seqType!=NULL)
      _values=_values.matches(_vb->_seqType, context);

    // Add a new scope, if needed
    if(_vb->_needsNewScope) {
      varStore->addLogicalBlockScope();
    }

    // If it's a let binding populate the variable with all of the values
    varStore->declareVar(_vb->_vURI, _vb->_vName, _values.toSequence(context), context);
  }
  else {
    // the SequenceType has been specified for each item of the sequence, but we can only apply to the
    // sequence itself, so allow it to match multiple matches
    if(_vb->_seqType!=NULL)
    {
      SequenceType::OccurrenceIndicator occurr=_vb->_seqType->getOccurrenceIndicator();
      if(occurr==SequenceType::EXACTLY_ONE)
        _vb->_seqType->setOccurrence(SequenceType::PLUS);
      else if(occurr==SequenceType::QUESTION_MARK)
        _vb->_seqType->setOccurrence(SequenceType::STAR);
      _values=_values.matches(_vb->_seqType, context);
    }

    const Item::Ptr item = _values.next(context);

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
                           Sequence((const Item::Ptr)context->getPathanFactory()->createInteger(_index, context), context->getMemoryManager()), context);
      _posRefVar = varStore->getReferenceVar(_vb->_pURI, _vb->_pName, context);
    }
  }

  return true;
}

bool XQFLWOR::ProductFactor::next(DynamicContext *ctx)
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);

  if(_vb->_bindingType == XQVariableBinding::letBinding) {
    // A let binding never has a next
    if(_vb->_needsNewScope) {
      context->getVariableStore()->removeScope();
    }
    return false;
  }
  else {
    const Item::Ptr item = _values.next(context);

    if(item == NULLRCP) {
      if(_vb->_needsNewScope) {
        context->getVariableStore()->removeScope();
      }
      return false;
    }

    // Make debug callback before declaring the variable
    if(context->getDebugCallback()) context->getDebugCallback()->IsBreakPointHit(context, _vb->_file, _vb->_line);

    // Set the binding variable value
    if(_vb->_variable) {
      _refVar->setValue(Sequence(item, context->getMemoryManager()));
    }

    // Set the positional variable value, if present
    if(_vb->_positionalVariable) {
      ++_index;
      _posRefVar->setValue(Sequence((const Item::Ptr)context->getPathanFactory()->createInteger(_index, context), context->getMemoryManager()));
    }

    return true;
  }
}

bool XQFLWOR::ProductFactor::checkWhere(DynamicContext *context)
{
  return _vb->_where == 0 || _vb->_where->collapseTree(context, DataItem::UNORDERED|DataItem::RETURN_TWO).getEffectiveBooleanValue(context);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQFLWOR::XQFLWOR(VectorOfVariableBinding* bindings, DataItem* where, XQSort* orderBy, DataItem* returnExpr, XPath2MemoryManager* expr)
  : DataItemImpl(expr),
    _return(returnExpr),
    _where(where),
    _sort(orderBy)
{
  _bindings=bindings;
  setType((DataItem::whichType)XQContext::FLWOR);
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
  return _where == 0 || _where->collapseTree(context, DataItem::UNORDERED|DataItem::RETURN_TWO).getEffectiveBooleanValue(context);
}

VariableStore::Entry *XQFLWOR::getAccumulator(DynamicContext *ctx) const
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);
  VariableStore* varStore = context->getVariableStore();

  if(context->isDebuggingEnabled()) {
    // find a unique name for the variable to be used as accumulator for the result
    // use the line number, plus a counter if more than one "for" is on the same line
    XMLCh szNumBuff[20];
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(_bindings->front()->_line, szNumBuff, 19, 10);
    const XMLCh *szInitialAccumulator, *szAccumulatorName;
    szInitialAccumulator = szAccumulatorName = XPath2Utils::concatStrings(szForLoopAccumulator, szNumBuff, context->getMemoryManager());
    long index = 1;
    while(varStore->getReferenceVar(szAccumulatorName, context) != NULL) {
      static XMLCh szSpaceParent[] = { XERCES_CPP_NAMESPACE_QUALIFIER chSpace, XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(index++, szNumBuff, 19, 10);
      szAccumulatorName = XPath2Utils::concatStrings(szInitialAccumulator, szSpaceParent, szNumBuff, context->getMemoryManager());
      szAccumulatorName = XPath2Utils::concatStrings(szAccumulatorName, XERCES_CPP_NAMESPACE_QUALIFIER chCloseParen, context->getMemoryManager());
    }

    varStore->declareVar(szAccumulatorName, Sequence(context->getMemoryManager()), context);
    return varStore->getReferenceVar(szAccumulatorName, context);
  }

  return 0;
}

void XQFLWOR::setAccumulator(VariableStore::Entry *refVar, const PreSortResult &toBeSorted, DynamicContext *ctx) const
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);
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
                                 DynamicContext* ctx, int flags) const
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);
  if((_sort && !(flags & DataItem::UNORDERED)) ||
     context->isDebuggingEnabled()) {
    return new SortingFLWORResult(it, end, this, flags, context);
  }
  else {
    return new FLWORResult(it, end, this, flags, context);
  }
}

DataItem* XQFLWOR::staticResolution(StaticContext* context)
{
  staticResolutionImpl(context);

  if(_src.isUsed()) {
    return resolvePredicates(context);
  }
  else {
    _return->addPredicates(getPredicates());
    return _return->staticResolution(context);
  }
}

void XQFLWOR::staticResolutionImpl(StaticContext* ctx)
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);

  VectorOfVariableBinding *newBindings =
    new (getMemoryManager()) VectorOfVariableBinding(PathanAllocator<XQVariableBinding*>(getMemoryManager()));

  VariableTypeStore* varStore = context->getVariableTypeStore();

  // Add all the binding variables to the new scope
  VectorOfVariableBinding::iterator end = _bindings->end();
  for(VectorOfVariableBinding::iterator it0 = _bindings->begin(); it0 != end; ++it0) {
    varStore->addLogicalBlockScope();

    // Work out the uri and localname of the variable binding
    (*it0)->_vURI = context->getUriBoundToPrefix(XPath2NSUtils::getPrefix((*it0)->_variable, context->getMemoryManager()));
    (*it0)->_vName = XPath2NSUtils::getLocalName((*it0)->_variable);

    // call static resolution on the value
    (*it0)->_allValues = (*it0)->_allValues->staticResolution(context);
    (*it0)->_src.getStaticType() = (*it0)->_allValues->getStaticResolutionContext().getStaticType();

    // Declare the variable binding
    if((*it0)->_bindingType == XQVariableBinding::letBinding) {
      (*it0)->_src.setProperties((*it0)->_allValues->getStaticResolutionContext().getProperties());
      varStore->declareVar((*it0)->_vURI, (*it0)->_vName, (*it0)->_src);
    }
    else {
      (*it0)->_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE | StaticResolutionContext::ONENODE);
      varStore->declareVar((*it0)->_vURI, (*it0)->_vName, (*it0)->_src);
    }

    if((*it0)->_positionalVariable != NULL && *((*it0)->_positionalVariable) != 0) {
      // Work out the uri and localname of the positional variable binding
      (*it0)->_pURI = context->getUriBoundToPrefix(XPath2NSUtils::getPrefix((*it0)->_positionalVariable, context->getMemoryManager()));
      (*it0)->_pName = XPath2NSUtils::getLocalName((*it0)->_positionalVariable);
      (*it0)->_pSrc.getStaticType().flags = StaticType::NUMERIC_TYPE;

      // Declare the positional variable binding
      varStore->declareVar((*it0)->_pURI, (*it0)->_pName, (*it0)->_pSrc);
    }
    else {
      // Make life easier when we execute
      (*it0)->_positionalVariable = 0;
    }
  }

  // Call staticResolution on the where expression, if there is one
  if(_where) {
    _where = staticallyResolveWhere(_where, context);
    if(_where) _src.add(_where->getStaticResolutionContext());
  }

  // Call staticResolution on the sort specifications, if there are some
  if(_sort) {
    _sort->staticResolution(context, _src);
  }

  // Call staticResolution on the return expression
  _return = _return->staticResolution(context);
  _src.getStaticType() = _return->getStaticResolutionContext().getStaticType();
  _src.add(_return->getStaticResolutionContext());

  VectorOfVariableBinding::reverse_iterator rend = _bindings->rend();
  for(VectorOfVariableBinding::reverse_iterator it = _bindings->rbegin(); it != rend; ++it) {
    XQVariableBinding *newVB = new (getMemoryManager()) XQVariableBinding(getMemoryManager(), **it);

    // Statically resolve any binding specific where conditions, if we have them
    if(newVB->_where) {
      newVB->_where = newVB->_where->staticResolution(context);
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
    // DbXml optimises collection() and doc(), so they have to be recalculated
    if(valueSrc.areDocsOrCollectionsUsed()) {
      newVB->_valuesResultMustBeRecalculated = true;
    }

    // Check to see if this binding has the same name as any before it
    // (4 comparisons, since each binding has two possible variables for it)
    for(it2 = it + 1; it2 != rend; ++it2) {
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
      AutoRelease<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(context->getMemoryManager());
      Result result = createResultImpl(newBindings->begin(), newBindings->end(), dContext);
      _return = new (getMemoryManager()) DataItemSequence(result, dContext, getMemoryManager());
      newBindings->clear();
    }
  }

  // Overwrite our bindings with the new ones
  _bindings = newBindings;
}

DataItem *XQFLWOR::staticallyResolveWhere(DataItem *where, StaticContext *ctx)
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);

  if(context->isDebuggingEnabled()) {
    // Don't do anything special if this is a debug build
    where = where->staticResolution(context);
  }
  else {
    if(where->getType()==DataItem::OPERATOR &&
       XPath2Utils::equals(((DataItemOperator*)where)->getOperatorName(), And::name)) {

      // If it's an And, treat each argument to the And as a where condition in it's own right
      And* fAnd = (And*)where;
      unsigned int index = 0;
      while(index < fAnd->getNumArgs()) {
        DataItem* subWhere = staticallyResolveWhere(fAnd->getArgument(index), context);
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
      where = where->staticResolution(context);
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

const DataItem *XQFLWOR::getWhereExpr() const
{
  return _where;
}

const DataItem *XQFLWOR::getReturnExpr() const
{
  return _return;
}

const XQSort *XQFLWOR::getSort() const
{
  return _sort;
}

void XQFLWOR::setWhereExpr(DataItem *where)
{
  _where = where;
}

void XQFLWOR::setReturnExpr(DataItem *ret)
{
  _return = ret;
}

XQFLWOR::SortingFLWORResult::SortingFLWORResult(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                                                const XQFLWOR *flwor, int flags, DynamicContext *context)
  : LazySequenceResult(context),
    _flwor(flwor),
    _flags(flags)
{
  for(; it != end; ++it) {
    _ebs.push_back(ProductFactor(*it, context));
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
  bool sorting = _flwor->getSort() && !(_flags & DataItem::UNORDERED);

  unsigned int resultsSoFar = 0;
  PreSortResult toBeSorted;
  if(_flwor->nextState(const_cast<ExecutionBindings&>(_ebs), context, true)) {
    do {
      Sequence result(_flwor->getReturnExpr()->collapseTree(context, _flags).toSequence(context));
      resultsSoFar += result.getLength();

      if(sorting) {
        toBeSorted.push_back(ResultPair(result, _flwor->getSort()->buildKeys(context)));
      }
      else {
        toBeSorted.push_back(ResultPair(result, XQSort::SortItems()));
      }

      if(accumulator) _flwor->setAccumulator(accumulator, toBeSorted, context);

      if(!sorting) {
        // Try to short the for loop execution
        if((_flags & DataItem::RETURN_ONE) && resultsSoFar)
          break;
        if((_flags & DataItem::RETURN_TWO) && resultsSoFar > 1)
          break;
      }
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
                                  const XQFLWOR *flwor, int flags, DynamicContext *context)
  : ResultImpl(context),
    _flwor(flwor),
    _flags(flags),
    _toInit(true),
    _scope(0),
    _returnResult(0)
{
  for(; it != end; ++it) {
    _ebs.push_back(ProductFactor(*it, context));
  }
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
  }

  Item::Ptr result = _returnResult.next(context);

  while(result == NULLRCP) {
    if(_flwor->nextState(_ebs, context, false)) {
      _returnResult = _flwor->getReturnExpr()->collapseTree(context, _flags);
      result = _returnResult.next(context);
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
