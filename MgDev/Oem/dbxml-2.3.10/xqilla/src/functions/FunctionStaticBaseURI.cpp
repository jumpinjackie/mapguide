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
 * $Id: FunctionStaticBaseURI.cpp,v 1.12 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionStaticBaseURI.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/AnyAtomicTypeConstructor.hpp>
#include <xqilla/ast/XQSequence.hpp>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

const XMLCh FunctionStaticBaseURI::name[] = {
  chLatin_s, chLatin_t, chLatin_a, 
  chLatin_t, chLatin_i, chLatin_c, 
  chDash,    chLatin_b, chLatin_a, 
  chLatin_s, chLatin_e, chDash, 
  chLatin_u, chLatin_r, chLatin_i, 
  chNull 
};
const unsigned int FunctionStaticBaseURI::minArgs = 0;
const unsigned int FunctionStaticBaseURI::maxArgs = 0;

/**
 * fn:static-base-uri() as xs:anyURI?
**/

FunctionStaticBaseURI::FunctionStaticBaseURI(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

ASTNode* FunctionStaticBaseURI::staticResolution(StaticContext *context) {
  XPath2MemoryManager* mm=context->getMemoryManager();

  ItemConstructor *item = new (mm) AnyAtomicTypeConstructor(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                            SchemaSymbols::fgDT_ANYURI,
                                                            context->getBaseURI(),
                                                            AnyAtomicType::ANY_URI);
  ASTNode* newBlock = new (mm) XQSequence(item, mm);
  newBlock->setLocationInfo(this);

  return newBlock->staticResolution(context);
}

ASTNode *FunctionStaticBaseURI::staticTyping(StaticContext *context)
{
  // Should never happen
  return this;
}

Sequence FunctionStaticBaseURI::collapseTreeInternal(DynamicContext* context, int flags) const
{
  // Always constant folded
  return Sequence(context->getMemoryManager());
}
