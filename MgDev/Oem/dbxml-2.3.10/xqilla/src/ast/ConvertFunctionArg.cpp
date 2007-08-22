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
 * $Id: ConvertFunctionArg.cpp,v 1.8 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"

#include <xqilla/ast/ConvertFunctionArg.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQPromoteUntyped::XQPromoteUntyped(ASTNode* expr, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    expr_(expr),
    uri_(uri),
    name_(name)
{
  setType(ASTNode::PROMOTE_UNTYPED);
}

ASTNode* XQPromoteUntyped::staticResolution(StaticContext *context)
{
  expr_ = expr_->staticResolution(context);
  return this;
}

ASTNode* XQPromoteUntyped::staticTyping(StaticContext *context)
{
  _src.clear();

  expr_ = expr_->staticTyping(context);
  _src.getStaticType() = expr_->getStaticResolutionContext().getStaticType();
  _src.add(expr_->getStaticResolutionContext());

  if(_src.getStaticType().containsType(StaticType::UNTYPED_ATOMIC_TYPE)) {
    bool isPrimitive;
    _src.getStaticType().flags &= ~StaticType::UNTYPED_ATOMIC_TYPE;
    _src.getStaticType().flags |= StaticType::getFlagsFor(uri_, name_, context, isPrimitive);
  }
  else {
    return expr_;
  }

  // crioux thinks this should also add: unless the target type is anyAtomicType!
  if(XPath2Utils::equals(name_, AnyAtomicType::fgDT_ANYATOMICTYPE) && 
     XPath2Utils::equals(uri_, FunctionConstructor::XMLChXPath2DatatypesURI)) {
    return expr_;
  }

  if(expr_->isConstant()) {
    return constantFold(context);
  }
  return this;
}

Result XQPromoteUntyped::createResult(DynamicContext* context, int flags) const
{
  return new PromoteUntypedResult(this, expr_->collapseTree(context, flags), uri_, name_);
}

Item::Ptr XQPromoteUntyped::PromoteUntypedResult::next(DynamicContext *context)
{
  Item::Ptr item = parent_->next(context);
  if(item.notNull()) {
    assert(item->isAtomicValue());
    const AnyAtomicType *atomic = (const AnyAtomicType *)item.get();

    // 2. Each item in the atomic sequence that is of type xdt:untypedAtomic is cast to the expected atomic
    //    type. For built-in functions where the expected type is specified as numeric, arguments of type
    //    xdt:untypedAtomic are cast to xs:double.
    if(atomic->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
      try {
        item = atomic->castAs(uri_, name_, context);
      }
      catch(XPath2TypeCastException &e) {
        if(e.getXQueryLine() == 0)
          e.setXQueryPosition(this);
        throw;
      }
    }
  }
  else {
    parent_ = 0;
  }

  return item;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

XQPromoteNumeric::XQPromoteNumeric(ASTNode* expr, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    expr_(expr),
    uri_(uri),
    name_(name)
{
  setType(ASTNode::PROMOTE_NUMERIC);
}

ASTNode* XQPromoteNumeric::staticResolution(StaticContext *context)
{
  expr_ = expr_->staticResolution(context);
  return this;
}

ASTNode* XQPromoteNumeric::staticTyping(StaticContext *context)
{
  _src.clear();

  expr_ = expr_->staticTyping(context);
  _src.getStaticType() = expr_->getStaticResolutionContext().getStaticType();
  _src.add(expr_->getStaticResolutionContext());

  if(_src.getStaticType().containsType(StaticType::NUMERIC_TYPE)) {
    bool isPrimitive;
    unsigned int flags = StaticType::getFlagsFor(uri_, name_, context, isPrimitive);

    if((flags & StaticType::DOUBLE_TYPE) != 0) {
	    _src.getStaticType().flags &= ~(StaticType::DECIMAL_TYPE | StaticType::FLOAT_TYPE);
	    _src.getStaticType().flags |= flags;
    }
    else if((flags & StaticType::FLOAT_TYPE) != 0) {
	    _src.getStaticType().flags &= ~StaticType::DECIMAL_TYPE;
	    _src.getStaticType().flags |= flags;
    }
  }
  else {
    return expr_;
  }

  if(!((XPath2Utils::equals(name_, SchemaSymbols::fgDT_DOUBLE) ||
        XPath2Utils::equals(name_, SchemaSymbols::fgDT_FLOAT) &&
        XPath2Utils::equals(uri_, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)))) {
    return expr_;
  }

  if(expr_->isConstant()) {
    return constantFold(context);
  }
  return this;
}

Result XQPromoteNumeric::createResult(DynamicContext* context, int flags) const
{
  return new PromoteNumericResult(this, expr_->collapseTree(context, flags), uri_, name_);
}

Item::Ptr XQPromoteNumeric::PromoteNumericResult::next(DynamicContext *context)
{
  Item::Ptr item = parent_->next(context);
  if(item.notNull()) {
    assert(item->isAtomicValue());
    const AnyAtomicType *atomic = (const AnyAtomicType *)item.get();

    // 3. For each numeric item in the atomic sequence that can be promoted to the expected atomic type using
    //    the promotion rules in B.1 Type Promotion, the promotion is done.
    if(atomic->isNumericValue()) {
      try {
        const Numeric::Ptr promotedType = ((const Numeric*)atomic)->promoteTypeIfApplicable(uri_, name_, context);
        if(promotedType != NULLRCP) {
          item = promotedType;
        }
      } catch (XPath2TypeCastException &e) {
        XQThrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("Numeric type promotion failed (for promotable type)"));
      } catch (const XMLException& e) {
        XQThrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("Numeric type promotion failed (for promotable type)"));
      }
    }
  }
  else {
    parent_ = 0;
  }

  return item;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

XQPromoteAnyURI::XQPromoteAnyURI(ASTNode* expr, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    expr_(expr),
    uri_(uri),
    name_(name)
{
  setType(ASTNode::PROMOTE_ANY_URI);
}

ASTNode* XQPromoteAnyURI::staticResolution(StaticContext *context)
{
  expr_ = expr_->staticResolution(context);
  return this;
}

ASTNode *XQPromoteAnyURI::staticTyping(StaticContext *context)
{
  _src.clear();

  expr_ = expr_->staticTyping(context);
  _src.getStaticType() = expr_->getStaticResolutionContext().getStaticType();
  _src.add(expr_->getStaticResolutionContext());

  if(_src.getStaticType().flags & StaticType::ANY_URI_TYPE) {
    _src.getStaticType().flags &= ~StaticType::ANY_URI_TYPE;
    _src.getStaticType().flags |= StaticType::STRING_TYPE;
  }
  else {
    return expr_;
  }

  if(!XPath2Utils::equals(name_, SchemaSymbols::fgDT_STRING) ||
     !XPath2Utils::equals(uri_, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
    return expr_;
  }

  if(expr_->isConstant()) {
    return constantFold(context);
  }
  return this;
}

Result XQPromoteAnyURI::createResult(DynamicContext* context, int flags) const
{
  return new PromoteAnyURIResult(this, expr_->collapseTree(context, flags));
}

Item::Ptr XQPromoteAnyURI::PromoteAnyURIResult::next(DynamicContext *context)
{
  Item::Ptr item = parent_->next(context);
  if(item.notNull()) {
    assert(item->isAtomicValue());
    const AnyAtomicType *atomic = (const AnyAtomicType *)item.get();

    // 4. For each item of type xs:anyURI in the atomic sequence that can be promoted to the expected atomic
    //    type using URI promotion as described in B.1 Type Promotion, the promotion is done.
    if(atomic->getPrimitiveTypeIndex() == AnyAtomicType::ANY_URI) {
      try {
        item = atomic->castAs(SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_STRING, context);
      } catch (XPath2TypeCastException &e) {
        XQThrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("AnyURI type promotion failed (for promotable type)"));
      } catch (const XMLException& e) {
        XQThrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("AnyURI type promotion failed (for promotable type)"));
      }
    }
  }
  else {
    parent_ = 0;
  }

  return item;
}
