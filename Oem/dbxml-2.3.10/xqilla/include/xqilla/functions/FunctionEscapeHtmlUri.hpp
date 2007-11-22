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
 * $Id: FunctionEscapeHtmlUri.hpp,v 1.4 2006/11/01 16:37:13 jpcs Exp $
 */

/*
  fn:escape-html-uri($uri as xs:string?) as xs:string
*/

#ifndef _FUNCTIONESCAPEHTMLURI_HPP
#define _FUNCTIONESCAPEHTMLURI_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionEscapeHtmlUri : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionEscapeHtmlUri(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONESCAPEHTMLURI_HPP



