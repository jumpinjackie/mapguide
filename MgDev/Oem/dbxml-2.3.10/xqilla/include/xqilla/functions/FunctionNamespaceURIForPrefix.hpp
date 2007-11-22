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
 * $Id: FunctionNamespaceURIForPrefix.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*
  namespace-uri-for-prefix function
*/

#ifndef _FUNCTIONGETNAMESPACEURIFORPREFIX_HPP
#define _FUNCTIONGETNAMESPACEURIFORPREFIX_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionNamespaceURIForPrefix : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionNamespaceURIForPrefix(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** Given a uri and a name, it returns an expanded QName **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONGETNAMESPACEURIFORPREFIX_HPP

