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
 * $Id: Except.cpp,v 1.16 2006/11/01 16:37:21 jpcs Exp $
 */
  
#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/operators/Except.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/schema/SequenceType.hpp>

/*static*/ const XMLCh Except::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Except::Except(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr),
    sortAdded_(false)
{
}

ASTNode* Except::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  if(!sortAdded_) {
    sortAdded_ = true;
    // Wrap ourselves in a document order sort
    ASTNode *result = new (mm) XQDocumentOrder(this, mm);
    result->setLocationInfo(this);
    return result->staticResolution(context);
  }

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    SequenceType *seqType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE),
                                                  SequenceType::STAR);
    seqType->setLocationInfo(this);

    *i = new (mm) XQTreatAs(*i, seqType, mm);
    (*i)->setLocationInfo(this);

    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode* Except::staticTyping(StaticContext *context)
{
  _src.getStaticType().flags = StaticType::NODE_TYPE;

  _args[0] = _args[0]->staticTyping(context);
  _src.copy(_args[0]->getStaticResolutionContext());

  _args[1] = _args[1]->staticTyping(context);
  _src.add(_args[1]->getStaticResolutionContext());

  return this;
}

Result Except::createResult(DynamicContext* context, int flags) const
{
  return new ExceptResult(this, flags);
}

Except::ExceptResult::ExceptResult(const Except *op, int flags)
  : ResultImpl(op),
    _op(op),
    _flags(flags),
    _toDo(true),
    _result(0),
    _excpt(0)
{
}

Item::Ptr Except::ExceptResult::next(DynamicContext *context)
{
  if(_toDo) {
    _toDo = false;
    _result = _op->getArgument(0)->collapseTree(context, _flags);
    _excpt = _op->getArgument(1)->collapseTree(context);
  }

  Item::Ptr item = _result->next(context);
  while(item != NULLRCP) {    

    bool found = false;
    Result except_result(_excpt.createResult());
    Item::Ptr except_item;
    while((except_item = except_result->next(context)) != NULLRCP) {
      if(((Node*)item.get())->equals((Node*)except_item.get())) {
        found = true;
        break;
      }
    }

    if(!found) {
      break;
    }

    item = _result->next(context);
  }

  if(item == NULLRCP) {
    _result = 0;
    _excpt = 0;
  }

  return item;
}

std::string Except::ExceptResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<except/>" << std::endl;

  return oss.str();
}
