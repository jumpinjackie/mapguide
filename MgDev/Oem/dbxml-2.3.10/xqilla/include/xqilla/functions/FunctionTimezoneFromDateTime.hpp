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
 * $Id: FunctionTimezoneFromDateTime.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONGETTIMEZONEFROMDATETIME_HPP
#define _FUNCTIONGETTIMEZONEFROMDATETIME_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/**
 * Getter function for the timezone component of dateTime
 *
 *  timezone-from-dateTime(dateTime $srcval) => xdt:dayTimeDuration?
 */
class XQILLA_API FunctionTimezoneFromDateTime : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionTimezoneFromDateTime(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /**
   * Returns a string representing the timezone component in the value of
   * $srcval.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETTIMEZONEFROMDATETIME_HPP



