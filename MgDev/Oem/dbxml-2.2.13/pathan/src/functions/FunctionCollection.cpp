/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionCollection.hpp>

#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/Node.hpp>
#include <pathan/ATAnyURIOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <pathan/URIResolver.hpp>
#include <pathan/PathanFactory.hpp>

const XMLCh FunctionCollection::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCollection::minArgs = 0;
const unsigned int FunctionCollection::maxArgs = 1;

/**
 * fn:collection() as node()*
 * fn:collection($arg as xs:string?) as node()*
**/

FunctionCollection::FunctionCollection(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
}

DataItem* FunctionCollection::staticResolution(StaticContext *context)
{
  _src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE);
  _src.getStaticType().flags = StaticType::NODE_TYPE;
  _src.availableCollectionsUsed(true);
  return resolveDataItems(_args, context, false);
}

Sequence FunctionCollection::collapseTreeInternal(DynamicContext* context, int flags) const
{
  //args 0 - URI to resolve
  if(getNumArgs()==0)
      return context->resolveDefaultCollection();

  Sequence arg = getParamNumber(1, context);
  if(arg.isEmpty())
    return Sequence(context->getMemoryManager());
  const XMLCh* currentUri = arg.first()->asString(context);

  try {
    context->getPathanFactory()->createAnyURI(currentUri, context);
  } catch(XPath2ErrorException &e) {
    DSLthrow(FunctionException, X("FunctionCollection::collapseTreeInternal"), X("Invalid URI format"));
  }

  return context->resolveCollection(currentUri);
}
