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
 * $Id: XQTreatAs.cpp,v 1.14 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/context/ContextHelpers.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQTreatAs::XQTreatAs(ASTNode* expr, const SequenceType* exprType, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    _expr(expr),
    _exprType(exprType),
    _doTypeCheck(true),
    _isTreatAs(false)
{
  setType(ASTNode::TREAT_AS);
}

Result XQTreatAs::createResult(DynamicContext* context, int flags) const
{
  Result result = _expr->collapseTree(context, flags);
  if(_exprType->getOccurrenceIndicator() != SequenceType::STAR ||
     _exprType->getItemType() == NULL) {
    result = _exprType->occurrenceMatches(result, this);
  }
  if(_doTypeCheck) {
    result = _exprType->typeMatches(result, this);
  }
  return result;
}

ASTNode* XQTreatAs::staticResolution(StaticContext *context)
{
  _exprType->staticResolution(context);

  if(_exprType->getOccurrenceIndicator() == SequenceType::QUESTION_MARK ||
     _exprType->getOccurrenceIndicator() == SequenceType::EXACTLY_ONE) {
    AutoNodeSetOrderingReset orderReset(context);
    _expr = _expr->staticResolution(context);
  }
  else {
    _expr = _expr->staticResolution(context);
  }

  return this;
}

ASTNode *XQTreatAs::staticTyping(StaticContext *context)
{
  _src.clear();

  _expr = _expr->staticTyping(context);

  // Do as much static time type checking as we can, given the
  // limited static typing that we implement
  const SequenceType::ItemType *itemType = _exprType->getItemType();
  if(itemType != NULL) {
    const StaticType &sType = _expr->getStaticResolutionContext().getStaticType();

    bool isExact;
    itemType->getStaticType(_src.getStaticType(), context, isExact, this);

    if(!sType.containsType(_src.getStaticType().flags) &&
       (_exprType->getOccurrenceIndicator() == SequenceType::EXACTLY_ONE ||
        _exprType->getOccurrenceIndicator() == SequenceType::PLUS)) {
      // It never matches
      XQThrow(XPath2TypeMatchException, X("XQTreatAs::staticResolution"), _isTreatAs ?
              X("The type of the expression doesn't match the sequence type specified in the 'treat as' expression"
                " [err:XPDY0050]") :
              X("ItemType matching failed [err:XPTY0004]"));
    }

    if(isExact && sType.isType(_src.getStaticType().flags)) {
      if(_exprType->getOccurrenceIndicator() == SequenceType::STAR) {
        // It always matches
        return _expr;
      }
      _doTypeCheck = false;
    }

    // Get a better static type by looking at our expression's type too
    _src.getStaticType().typeIntersect(_expr->getStaticResolutionContext().getStaticType());

    if(_src.getStaticType().containsType(StaticType::NODE_TYPE)) {
      // Copy the properties if we return nodes
      _src.setProperties(_expr->getStaticResolutionContext().getProperties());
    }
  }
  else {
    _src.getStaticType().flags = 0;
    _doTypeCheck = false;
  }
  _src.add(_expr->getStaticResolutionContext());

  if(_expr->isConstant()) {
    try
    {
      return constantFold(context);
    }
    catch(XQException& e)
    {
      if(_isTreatAs)
      {
        XMLBuffer buff(1023, context->getMemoryManager());
        buff.set(e.getError());
        int index=XMLString::indexOf(buff.getRawBuffer(),chOpenSquare);
        if(index!=-1)
          XMLString::copyString(buff.getRawBuffer()+index, X("[err:XPDY0050]"));
        else
          buff.append(X("[err:XPDY0050]"));
        XQThrow(XPath2TypeMatchException, X("XQTreatAs::staticResolution"), buff.getRawBuffer());
      }
      else
        throw;
    }
  }
  return this;
}

const ASTNode *XQTreatAs::getExpression() const {
  return _expr;
}

const SequenceType *XQTreatAs::getSequenceType() const {
  return _exprType;
}

void XQTreatAs::setExpression(ASTNode *item) {
  _expr = item;
}
