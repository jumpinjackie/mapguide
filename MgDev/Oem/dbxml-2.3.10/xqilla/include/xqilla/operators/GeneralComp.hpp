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
 * $Id: GeneralComp.hpp,v 1.12 2006/11/01 16:37:14 jpcs Exp $
 */

/*
   General Comparison operator function
*/

#ifndef _GENERALCOMP_HPP
#define _GENERALCOMP_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/runtime/SingleResult.hpp>

/** GeneralComp operator function*/
class XQILLA_API GeneralComp : public XQOperator
{
public:
  typedef enum {
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN,
    GREATER_THAN_EQUAL
  } ComparisonOperation;

  static const XMLCh name[];

  GeneralComp(ComparisonOperation operation, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

  ComparisonOperation getOperation() const;
  bool getXPath1CompatibilityMode() const { return xpath1compat_; }

  static bool compare(GeneralComp::ComparisonOperation operation,
                      AnyAtomicType::Ptr first, AnyAtomicType::Ptr second,
                      Collation* collation, DynamicContext *context,
                      bool xpath1compat, const LocationInfo *info);

protected:
  class GeneralCompResult : public SingleResult
  {
  public:
    GeneralCompResult(const GeneralComp *op);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const GeneralComp *_op;
  };

  ComparisonOperation _operation;
  bool xpath1compat_;
};

#endif // _GENERALCOMP_HPP



