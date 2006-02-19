/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  in-scope-prefixes function
*/

#ifndef _FUNCTIONGETINSCOPEPREFIXES_HPP
#define _FUNCTIONGETINSCOPEPREFIXES_HPP

#include <pathan/Pathan.hpp>
#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionInScopePrefixes : public ConstantFoldingFunction
{
public:
    static const XMLCh name[];
    static const unsigned int minArgs;
    static const unsigned int maxArgs;

    FunctionInScopePrefixes(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
    /** Returns the getinscopenamespaces to the right of parm2 until optional parm3 from string parm1 **/
    Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};


#endif // _FUNCTIONGETINSCOPEPREFIXES_HPP





