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
 * $Id: XQCastableAs.cpp,v 1.15 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/ast/XQCastableAs.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/exceptions/TypeErrorException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQCastableAs::XQCastableAs(ASTNode* expr, SequenceType* exprType, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
  _expr(expr),
  _exprType(exprType)
{
	setType(ASTNode::CASTABLE_AS);
}

Result XQCastableAs::createResult(DynamicContext* context, int flags) const
{
  return new CastableAsResult(this);
}

static XMLCh szNOTATION[] =  { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_N, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_O, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_I, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chLatin_O, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_N, XERCES_CPP_NAMESPACE_QUALIFIER chNull }; 

ASTNode* XQCastableAs::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  _exprType->staticResolution(context);

  const SequenceType::ItemType* itemType = _exprType->getItemType();
  if((XPath2Utils::equals(itemType->getTypeURI(context, this), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
      XPath2Utils::equals(itemType->getType()->getName(), szNOTATION)) ||
     (XPath2Utils::equals(itemType->getTypeURI(context, this), FunctionConstructor::XMLChXPath2DatatypesURI) &&
		  XPath2Utils::equals(itemType->getType()->getName(), AnyAtomicType::fgDT_ANYATOMICTYPE)))
    XQThrow(TypeErrorException,X("XQCastableAs::staticResolution"),
            X("The target type of a castable expression must be an atomic type that is in the in-scope schema types and is not xs:NOTATION or xdt:anyAtomicType [err:XPST0080]"));

  _expr = new (mm) XQAtomize(_expr, mm);
  _expr->setLocationInfo(this);

  {
    AutoNodeSetOrderingReset orderReset(context);
    _expr = _expr->staticResolution(context);
  }

  return this;
}

ASTNode *XQCastableAs::staticTyping(StaticContext *context)
{
  _src.clear();

  _expr = _expr->staticTyping(context);

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;
  _src.add(_expr->getStaticResolutionContext());
  if(_expr->isConstant()) {
    return constantFold(context);
  }
  return this;
}

const ASTNode *XQCastableAs::getExpression() const {
  return _expr;
}

const SequenceType *XQCastableAs::getSequenceType() const {
  return _exprType;
}

void XQCastableAs::setExpression(ASTNode *item) {
  _expr = item;
}

XQCastableAs::CastableAsResult::CastableAsResult(const XQCastableAs *di)
  : SingleResult(di),
    _di(di)
{
}

Item::Ptr XQCastableAs::CastableAsResult::getSingleResult(DynamicContext *context) const
{
  // The semantics of the cast expression are as follows:
  //    1. Atomization is performed on the input expression.
  Result toBeCasted(_di->getExpression()->collapseTree(context));

  const Item::Ptr first = toBeCasted->next(context);

  bool result = false;
	if(first == NULLRCP) {
    //    3. If the result of atomization is an empty sequence:
    //       1. If ? is specified after the target type, the result of the cast expression is an empty sequence.
    //       2. If ? is not specified after the target type, a type error is raised.[err:XP0004][err:XP0006]
    result = _di->getSequenceType()->getOccurrenceIndicator() != SequenceType::EXACTLY_ONE;
  }
  else {
    const Item::Ptr second = toBeCasted->next(context);

    //    2. If the result of atomization is a sequence of more than one atomic value, a type error is raised.[err:XP0004][err:XP0006]
    if(second != NULLRCP) {
      result = false;
    }
    else {
      if(_di->getSequenceType()->getItemTestType() != SequenceType::ItemType::TEST_ATOMIC_TYPE)
        XQThrow(TypeErrorException,X("XQCastableAs::collapseTreeInternal"),X("Cannot cast to a non atomic type"));
      //    4. If the result of atomization is a single atomic value, the result of the cast expression depends on the input type and the target type.
      //       The normative definition of these rules is given in [XQuery 1.0 and XPath 2.0 Functions and Operators].
      result = ((const AnyAtomicType::Ptr)first)->castable(_di->getSequenceType()->getTypeURI(context), _di->getSequenceType()->getConstrainingType()->getName(), context);
    }
  }
  return (const Item::Ptr)context->getItemFactory()->createBoolean(result, context);
}

std::string XQCastableAs::CastableAsResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<castableas/>" << std::endl;

  return oss.str();

}
