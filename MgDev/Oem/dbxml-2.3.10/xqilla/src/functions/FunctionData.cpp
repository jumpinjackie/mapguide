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
 * $Id: FunctionData.cpp,v 1.12 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionData.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/XQAtomize.hpp>

const XMLCh FunctionData::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionData::minArgs = 1;
const unsigned int FunctionData::maxArgs = 1;

/*
 * fn:data($arg as item()*) as xdt:anyAtomicType*
 */

FunctionData::FunctionData(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

ASTNode* FunctionData::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  ASTNode *result = new (mm) XQAtomize(_args.front(), mm);
  result->setLocationInfo(this);

  return result->staticResolution(context);
}

ASTNode *FunctionData::staticTyping(StaticContext *context)
{
  // Should never happen
  return this;
}

Result FunctionData::createResult(DynamicContext* context, int flags) const
{
  // Should never happen
  assert(0);
  return 0;
}
















