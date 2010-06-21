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
 * $Id: FunctionStringToCodepoints.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*

   StringToCodepoints function
*/

#ifndef _FUNCTIONSTRINGTOCODEPOINTS_HPP
#define _FUNCTIONSTRINGTOCODEPOINTS_HPP

#include <xqilla/framework/XQillaExport.hpp>


#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** StringToCodepoints function */
class XQILLA_API FunctionStringToCodepoints : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionStringToCodepoints(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONCONTAINS_HPP

