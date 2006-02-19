/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionData.hpp>
#include <pathan/Node.hpp>
#include <pathan/Item.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/DynamicContext.hpp>

const XMLCh FunctionData::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionData::minArgs = 1;
const unsigned int FunctionData::maxArgs = 1;

/*
 * fn:data($arg as item()*) as xdt:anyAtomicType*
 */

FunctionData::FunctionData(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

DataItem* FunctionData::staticResolution(StaticContext *context)
{
  bool allConstant = true;
  for(VectorOfDataItems::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticResolution(context);
    _src.getStaticType() = (*i)->getStaticResolutionContext().getStaticType();
    _src.add((*i)->getStaticResolutionContext());
    if(!(*i)->isConstant()) {
      allConstant = false;
    }
    break; // should only be one
  }

  if(_src.getStaticType().flags & StaticType::NODE_TYPE) {
    _src.getStaticType().flags &= ~StaticType::NODE_TYPE;
    _src.getStaticType().flags |= StaticType::NUMERIC_TYPE | StaticType::OTHER_TYPE;
  }
  else {
    // If the parameter has no nodes, this function does nothing
    _args.front()->addPredicates(getPredicates());
    return _args.front();
  }

  if(allConstant) {
    return constantFold(context);
  }
  else {
    return resolvePredicates(context);
  }
}

Result FunctionData::createResult(DynamicContext* context, int flags) const
{
  return getParamNumber(1, context, flags).atomize(context);
}
















