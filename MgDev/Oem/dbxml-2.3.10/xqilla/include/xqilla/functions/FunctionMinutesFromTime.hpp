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
 * $Id: FunctionMinutesFromTime.hpp,v 1.9 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONGETMINUTESFROMTIME_HPP
#define _FUNCTIONGETMINUTESFROMTIME_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>

/**
 * Getter function for minutes component of time.
 *
 * xf:minutes-from-time(time? $srcval) => integer?
 */
class XQILLA_API FunctionMinutesFromTime : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionMinutesFromTime(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  /**
   * Returns the integer representing the minutes in the value of $srcval
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETMINUTESFROMTIME_HPP



