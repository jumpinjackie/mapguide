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
 * $Id: FunctionInScopePrefixes.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*
  in-scope-prefixes function
*/

#ifndef _FUNCTIONGETINSCOPEPREFIXES_HPP
#define _FUNCTIONGETINSCOPEPREFIXES_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionInScopePrefixes : public ConstantFoldingFunction
{
public:
    static const XMLCh name[];
    static const unsigned int minArgs;
    static const unsigned int maxArgs;

    FunctionInScopePrefixes(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
    /** Returns the getinscopenamespaces to the right of parm2 until optional parm3 from string parm1 **/
    Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};


#endif // _FUNCTIONGETINSCOPEPREFIXES_HPP





