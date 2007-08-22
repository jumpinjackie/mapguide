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
 * $Id: XPath1Compat.cpp,v 1.5 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"

#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/functions/FunctionNumber.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XPath1CompatConvertFunctionArg::XPath1CompatConvertFunctionArg(ASTNode* expr, const SequenceType *seqType, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    expr_(expr),
    seqType_(seqType)
{
  setType(ASTNode::XPATH1_CONVERT);
}

ASTNode* XPath1CompatConvertFunctionArg::staticResolution(StaticContext *context)
{
  seqType_->staticResolution(context);
  expr_ = expr_->staticResolution(context);
  return this;
}

ASTNode *XPath1CompatConvertFunctionArg::staticTyping(StaticContext *context)
{
  _src.clear();

  expr_ = expr_->staticTyping(context);
  _src.copy(expr_->getStaticResolutionContext());

  if(expr_->isConstant()) {
    return constantFold(context);
  }
  return this;
}

Result XPath1CompatConvertFunctionArg::createResult(DynamicContext* context, int flags) const
{
  return new XPath1CompatConvertFunctionArgResult(this, expr_->collapseTree(context, flags), seqType_);
}

XPath1CompatConvertFunctionArg::XPath1CompatConvertFunctionArgResult::
XPath1CompatConvertFunctionArgResult(const XPath1CompatConvertFunctionArg *di, const Result &parent, const SequenceType *seqType)
  : ResultImpl(di),
    seqType_(seqType),
    parent_(parent),
    oneDone_(false)
{
}

Item::Ptr XPath1CompatConvertFunctionArg::XPath1CompatConvertFunctionArgResult::next(DynamicContext *context)
{
  // If XPath 1.0 compatibility mode is true and an argument is not of the expected type, then the following
  // conversions are applied sequentially to the argument value V:

  // 1. If the expected type calls for a single item or optional single item (examples: xs:string,
  //    xs:string?, xdt:untypedAtomic, xdt:untypedAtomic?, node(), node()?, item(), item()?), then the
  //    value V is effectively replaced by V[1].
  if(oneDone_) {
    return 0;
  }

  Item::Ptr item = parent_->next(context);

  if(seqType_->getItemTestType() == SequenceType::ItemType::TEST_ATOMIC_TYPE) {
    const XMLCh* typeURI = seqType_->getTypeURI(context);
    const XMLCh* typeName = seqType_->getConstrainingType()->getName();

    // 2. If the expected type is xs:string or xs:string?, then the value V is effectively replaced by
    //    fn:string(V).
    if(context->isTypeOrDerivedFromType(typeURI, typeName,
                                        SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                        SchemaSymbols::fgDT_STRING)) {
      item = FunctionString::string(item, context);
    }

    // 3. If the expected type is a (possibly optional) numeric type, then the value V is effectively
    //    replaced by fn:number(V).
    if(context->isTypeOrDerivedFromType(typeURI, typeName,
                                        SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                        SchemaSymbols::fgDT_DOUBLE)) {
      assert(item->isAtomicValue()); // Since atomization should have happened
      item = FunctionNumber::number((const AnyAtomicType::Ptr )item, context);
    }
  }

  oneDone_ = true;
  return item;
}

std::string XPath1CompatConvertFunctionArg::XPath1CompatConvertFunctionArgResult::asString(DynamicContext *context, int indent) const
{
  return "xpath1convertfunctionarg";
}
