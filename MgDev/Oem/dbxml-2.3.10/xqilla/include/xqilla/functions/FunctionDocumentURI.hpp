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
 * $Id: FunctionDocumentURI.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*

   document-uri function

*/

#ifndef _FUNCTIONDOCUMENTURI_HPP
#define _FUNCTIONDOCUMENTURI_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class Sequence;

/* document-uri function */
class XQILLA_API FunctionDocumentURI : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDocumentURI(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
	/* returns the document-URI property of the node */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif


