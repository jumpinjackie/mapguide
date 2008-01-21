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
 * $Id: FunctionQName.cpp,v 1.13 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionQName.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/ATQNameOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/exceptions/FunctionException.hpp>

#include <xercesc/util/XMLChar.hpp>

const XMLCh FunctionQName::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Q, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_N, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionQName::minArgs = 2;
const unsigned int FunctionQName::maxArgs = 2;

/**
 * fn:QName($paramURI as xs:string?, $paramLocal as xs:string) as xs:QName
**/

/** Returns a QName with the URI given in $paramURI and the localname
 * given in $paramLocal **/

FunctionQName::FunctionQName(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, string", args, memMgr)
{
  _src.getStaticType().flags = StaticType::QNAME_TYPE;
}

Sequence FunctionQName::collapseTreeInternal(DynamicContext* context, int flags) const {
  Sequence paramURIseq = getParamNumber(1, context)->toSequence(context);
  Sequence paramLocalseq = getParamNumber(2, context)->toSequence(context);

  const XMLCh* uri = NULL;
  if(!paramURIseq.isEmpty())
    uri=paramURIseq.first()->asString(context);
  const XMLCh* local = paramLocalseq.first()->asString(context);
  if(!XERCES_CPP_NAMESPACE_QUALIFIER XMLChar1_0::isValidQName(local, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(local)))
    XQThrow(FunctionException,X("FunctionQName::collapseTreeInternal"),X("The second argument to fn:QName is not a valid xs:QName [err:FOCA0002]"));

  const XMLCh* prefix = XPath2NSUtils::getPrefix(local, context->getMemoryManager());
  if((uri==NULL || *uri==0) && !(prefix==NULL || *prefix==0))
    XQThrow(FunctionException,X("FunctionQName::collapseTreeInternal"),X("The second argument to fn:QName specifies a prefix, but the specified uri is empty [err:FOCA0002]"));

  local = XPath2NSUtils::getLocalName(local);
  //Construct QName here
  Sequence result(context->getItemFactory()->createQName(uri, prefix, local, context),
                  context->getMemoryManager());

	return result;
}


