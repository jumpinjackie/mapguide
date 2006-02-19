/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionTrace.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
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

FunctionTrace::FunctionTrace(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "item()*, string", args, memMgr)
{
}

DataItem* FunctionTrace::staticResolution(StaticContext *context) {
  bool allConstant = true;
  for(VectorOfDataItems::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticResolution(context);
    _src.add((*i)->getStaticResolutionContext());
    if(!(*i)->isConstant()) {
      allConstant = false;
    }
  }

  _src.getStaticType() = _args.front()->getStaticResolutionContext().getStaticType();
  _src.forceNoFolding(true);

  return resolvePredicates(context);
}

Sequence FunctionTrace::collapseTreeInternal(DynamicContext* context, int flags) const
{	
  Sequence value = getParamNumber(1, context).toSequence(context);
  Sequence label = getParamNumber(2, context).toSequence(context);
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

