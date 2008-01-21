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
 * $Id: ASTNode.hpp,v 1.14 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _ASTNODE_HPP
#define _ASTNODE_HPP

#include <vector>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/ast/LocationInfo.hpp>

class DynamicContext;
class StaticContext;
class Result;
class StaticResolutionContext;

class XQILLA_API ASTNode : public LocationInfo
{
public:
  ///enum for data types
  typedef enum {
    LITERAL,
    SEQUENCE,
    FUNCTION,
    NAVIGATION,
    VARIABLE,
    STEP,
    IF,
    INSTANCE_OF,
    CASTABLE_AS,
    CAST_AS,
    TREAT_AS,
    OPERATOR,
    CONTEXT_ITEM,
    PARENTHESIZED,
    DEBUG_HOOK,
    DOM_CONSTRUCTOR,
    FLWOR,
    FLWOR_QUANTIFIED,
    TYPESWITCH,
    VALIDATE,
    FUNCTION_CALL,
    USER_FUNCTION,
    ORDERING_CHANGE,
    XPATH1_CONVERT,
    PROMOTE_UNTYPED,
    PROMOTE_NUMERIC,
    PROMOTE_ANY_URI,
    DOCUMENT_ORDER,
    PREDICATE,
    ATOMIZE,
    FTCONTAINS
  } whichType;

  virtual ~ASTNode() {}

  /// The flags parameter is currently unused
  virtual Result collapseTree(DynamicContext *context, int flags=0) const = 0;

  /** Returns true if this ASTNode has no predicates, and is an instance of
      XQSequence or XQLiteral */
  virtual bool isConstant() const = 0;
  /** Returns true if this ASTNode has no predicates, and is an instance of
      XQSequence or XQLiteral. If the literal value of this ASTNode
      is a single DateOrTimeType, then !hasTimezone() on it must return true,
      otherwise this method will return false. */
  virtual bool isDateOrTimeAndHasNoTimezone(StaticContext* context) const = 0;
  virtual bool isSingleNumericConstant(StaticContext *context) const = 0;

  virtual whichType getType(void) const = 0;

  virtual ASTNode *staticResolution(StaticContext *context) = 0;
  virtual ASTNode *staticTyping(StaticContext *context) = 0;
  /// Returns the StaticResolutionContext for this ASTNode
  virtual const StaticResolutionContext &getStaticResolutionContext() const = 0;
};

typedef std::vector<ASTNode*,XQillaAllocator<ASTNode*> > VectorOfASTNodes;

#endif

