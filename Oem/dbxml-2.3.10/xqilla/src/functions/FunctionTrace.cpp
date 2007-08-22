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
 * $Id: FunctionTrace.cpp,v 1.10 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionTrace.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

const XMLCh FunctionTrace::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionTrace::minArgs = 2;
const unsigned int FunctionTrace::maxArgs = 2;

/*
 * fn:trace($value as item()*, $label as xs:string) as item()*
 */

FunctionTrace::FunctionTrace(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "item()*, string", args, memMgr)
{
}

ASTNode* FunctionTrace::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionTrace::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.forceNoFolding(true);
  ASTNode *result = calculateSRCForArguments(context);
  if(result == this) {
    _src.getStaticType() = _args.front()->getStaticResolutionContext().getStaticType();
  }
  return result;
}

Sequence FunctionTrace::collapseTreeInternal(DynamicContext* context, int flags) const
{	
  Sequence value = getParamNumber(1, context)->toSequence(context);
  Sequence label = getParamNumber(2, context)->toSequence(context);
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer valueStr(1023, context->getMemoryManager());
  unsigned int len=value.getLength();
  if(len>0)
  {
      if(len==1)
          valueStr.set(value.first()->asString(context));
      else
      {
          XMLCh szOpenParen[]={ XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
          XMLCh szCloseParen[]={ XERCES_CPP_NAMESPACE_QUALIFIER chCloseParen, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
          XMLCh szComma[]={ XERCES_CPP_NAMESPACE_QUALIFIER chComma, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
          valueStr.set(szOpenParen);
          Sequence::iterator end = value.end();
          for(Sequence::iterator i = value.begin(); i != end;) {
            valueStr.append((*i)->asString(context));
            if(++i != end)
              valueStr.append(szComma);
          }
          valueStr.append(szCloseParen);
      }
  }
  context->trace(label.first()->asString(context),valueStr.getRawBuffer());
  return value;
}

