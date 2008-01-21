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
 * $Id: FunctionIdref.hpp,v 1.9 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONIDREF_HPP
#define _FUNCTIONIDREF_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>

#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <vector>

/** 
 * Function idref
 * 
 * fn:idref(string* $srcval) => element*
 *
 */
class XQILLA_API FunctionIdref : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionIdref(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  /** 
   * Returns the sequence of elements nodes having either an IDREF attribute 
   * whose value matches the value of one of the items in the value of $srcval 
   * or an IDREFS attribute whose value contains an IDREF value that matches 
   * the value of one of the items in the value of $srcval. 
   *
   * This function allows reverse navigation from IDs to IDREFs.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONIDREF_HPP

