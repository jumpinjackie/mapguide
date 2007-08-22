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
 * $Id: XQCastAs.cpp,v 1.15 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/ast/XQCastAs.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/exceptions/TypeErrorException.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQCastAs::XQCastAs(ASTNode* expr, SequenceType* exprType, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
  _expr(expr),
  _exprType(exprType)
{
	setType(ASTNode::CAST_AS);
}

Result XQCastAs::createResult(DynamicContext* context, int flags) const
{
  return new CastAsResult(this);
}

static XMLCh szNOTATION[] =  { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_N, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_O, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_I, 
                               XERCES_CPP_NAMESPACE_QUALIFIER chLatin_O, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_N, XERCES_CPP_NAMESPACE_QUALIFIER chNull }; 

ASTNode* XQCastAs::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  _exprType->staticResolution(context);

  const SequenceType::ItemType *itemType = _exprType->getItemType();
  if(itemType != NULL) {
    if((XPath2Utils::equals(itemType->getTypeURI(context, this), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
        XPath2Utils::equals(itemType->getType()->getName(), szNOTATION)) ||
       (XPath2Utils::equals(itemType->getTypeURI(context, this), FunctionConstructor::XMLChXPath2DatatypesURI) &&
        XPath2Utils::equals(itemType->getType()->getName(), AnyAtomicType::fgDT_ANYATOMICTYPE)))
      XQThrow(TypeErrorException,X("XQCastAs::CastAsResult::getSingleResult"),
              X("The target type of a cast expression must be an atomic type that is in the in-scope schema types and is not xs:NOTATION or xdt:anyAtomicType [err:XPST0080]"));
  }

  _expr = new (mm) XQAtomize(_expr, mm);
  _expr->setLocationInfo(this);
  _expr = _expr->staticResolution(context);

  return this;
}

ASTNode *XQCastAs::staticTyping(StaticContext *context)
{
  _src.clear();

  const SequenceType::ItemType *itemType = _exprType->getItemType();
  if(itemType != NULL) {
    bool isPrimitive;
    itemType->getStaticType(_src.getStaticType(), context, isPrimitive, this);
  }

  _expr = _expr->staticTyping(context);
  _src.add(_expr->getStaticResolutionContext());

  if(_expr->isConstant()) {
    return constantFold(context);
  }
  return this;
}

const ASTNode *XQCastAs::getExpression() const {
  return _expr;
}

const SequenceType *XQCastAs::getSequenceType() const {
  return _exprType;
}

void XQCastAs::setExpression(ASTNode *item) {
  _expr = item;
}

XQCastAs::CastAsResult::CastAsResult(const XQCastAs *di)
  : SingleResult(di),
    _di(di)
{
}

Item::Ptr XQCastAs::CastAsResult::getSingleResult(DynamicContext *context) const
{
  // The semantics of the cast expression are as follows:
  //    1. Atomization is performed on the input expression.
	Result toBeCasted(_di->getExpression()->collapseTree(context));

  const Item::Ptr first = toBeCasted->next(context);

	if(first == NULLRCP) {
    //    3. If the result of atomization is an empty sequence:
    //       1. If ? is specified after the target type, the result of the cast expression is an empty sequence.
    //       2. If ? is not specified after the target type, a type error is raised [err:XPTY0004].
    if(_di->getSequenceType()->getOccurrenceIndicator() == SequenceType::EXACTLY_ONE) {
      XQThrow(TypeErrorException,X("XQCastAs::CastAsResult::getSingleResult"),X("The input to a non-optional cast as expression is an empty sequence [err:XPTY0004]"));
    }
    else {
      return 0;
    }
  }

  const Item::Ptr second = toBeCasted->next(context);

  //    2. If the result of atomization is a sequence of more than one atomic value, a type error is raised.[err:XPTY0004]
  if(second != NULLRCP) {
    XQThrow(TypeErrorException,X("XQCastAs::CastAsResult::getSingleResult"),X("The input to a cast as expression is more than one atomic value [err:XPTY0004]"));
  }

  if(_di->getSequenceType()->getItemTestType() != SequenceType::ItemType::TEST_ATOMIC_TYPE)
    XQThrow(TypeErrorException,X("XQCastAs::CastAsResult::getSingleResult"),X("Cannot cast to a non atomic type"));
  //    4. If the result of atomization is a single atomic value, the result of the cast expression depends on the input type and the target type.
  //       The normative definition of these rules is given in [XQuery 1.0 and XPath 2.0 Functions and Operators].
  try {
    return (const Item::Ptr)((const AnyAtomicType::Ptr)first)->castAs(_di->getSequenceType()->getTypeURI(context), _di->getSequenceType()->getConstrainingType()->getName(), context);
  }
  catch(XPath2TypeCastException &e) {
    if(e.getXQueryLine() == 0)
      e.setXQueryPosition(this);
    throw;
  }
}

std::string XQCastAs::CastAsResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<castas/>" << std::endl;

  return oss.str();

}
