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
 * $Id: FunctionLang.hpp,v 1.9 2006/11/01 16:37:13 jpcs Exp $
 */

/*

  lang function

*/

#ifndef _FUNCTIONLANG_HPP
#define _FUNCTIONLANG_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>

/** lang function. */
class XQILLA_API FunctionLang : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionLang(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  /** The lang function returns true or false depending on whether the language of
      the context node as specified by xml:lang attributes is the same as or is a
      sublanguage of the language specified by the argument string. The language of 
      the context node is determined by the value of the xml:lang attribute on the 
      context node, or, if the context node has no xml:lang attribute, by the value 
      of the xml:lang attribute on the nearest ancestor of the context node that has 
      an xml:lang attribute.**/

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONLANG_HPP

