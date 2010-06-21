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
 * $Id: Union.cpp,v 1.15 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/operators/Union.hpp>

#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/schema/SequenceType.hpp>

/*static*/ const XMLCh Union::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Union::Union(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr),
    sortAdded_(false)
{
}

ASTNode* Union::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  if(!sortAdded_) {
    sortAdded_ = true;
    // Wrap ourselves in a document order sort
    ASTNode *result = new (mm) XQDocumentOrder(this, mm);
    return result->staticResolution(context);
  }

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    SequenceType *seqType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE),
                                                  SequenceType::STAR);
    seqType->setLocationInfo(this);

    *i = new (mm) XQTreatAs(*i, seqType, mm);
    (*i)->setLocationInfo(*i);

    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode* Union::staticTyping(StaticContext *context)
{
  _src.clear();

  _args[0] = _args[0]->staticTyping(context);
  _src.add(_args[0]->getStaticResolutionContext());
  _src.getStaticType() = _args[0]->getStaticResolutionContext().getStaticType();

  _args[1] = _args[1]->staticTyping(context);
  _src.add(_args[1]->getStaticResolutionContext());
  _src.getStaticType().typeUnion(_args[1]->getStaticResolutionContext().getStaticType());

  return this;
}

Result Union::createResult(DynamicContext* context, int flags) const
{
  return new UnionResult(this, flags);
}

Union::UnionResult::UnionResult(const Union *op, int flags)
  : ResultImpl(op),
    _op(op),
    _flags(flags),
    _index(0),
    _result(0)
{
}

Item::Ptr Union::UnionResult::next(DynamicContext *context)
{
  Item::Ptr item = 0;
  while(true) {
    item = _result->next(context);

    if(item == NULLRCP) {
      if(_index > 1) {
        _index = 0;
        return 0;
      }
      else {
        _result = _op->getArgument(_index++)->collapseTree(context, _flags);
      }
    }
    else {
      return item;
    }
  }

  return 0;
}

std::string Union::UnionResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<union/>" << std::endl;

  return oss.str();
}
