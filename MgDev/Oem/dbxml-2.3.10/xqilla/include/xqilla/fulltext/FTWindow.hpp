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
 * $Id: FTWindow.hpp,v 1.4 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FTWINDOW_HPP
#define _FTWINDOW_HPP

#include <xqilla/fulltext/FTOption.hpp>

#include <set>

class ASTNode;

class XQILLA_API FTWindow : public FTOption
{
public:
  FTWindow(ASTNode *expr, FTUnit unit, XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::WINDOW, memMgr), expr_(expr), unit_(unit), arg_(0) {}
  FTWindow(FTSelection *arg, ASTNode *expr, FTUnit unit, XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::WINDOW, memMgr), expr_(expr), unit_(unit), arg_(arg) {}

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  virtual void setArgument(FTSelection *arg) { arg_ = arg; }
  const FTSelection *getArgument() const { return arg_; }
  const ASTNode *getExpr() const { return expr_; }
  FTUnit getUnit() const { return unit_; }

private:
  ASTNode *expr_;
  FTUnit unit_;
  FTSelection *arg_;
};

class XQILLA_API FTWindowLiteral : public FTOption
{
public:
  FTWindowLiteral(FTSelection *arg, unsigned int distance, FTUnit unit,
                  XPath2MemoryManager *memMgr)
    : FTOption(WINDOW_LITERAL, memMgr), arg_(arg), distance_(distance),
      unit_(unit) {}

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  virtual void setArgument(FTSelection *arg) { arg_ = arg; }
  const FTSelection *getArgument() const { return arg_; }
  unsigned int getDistance() const { return distance_; }
  FTUnit getUnit() const { return unit_; }

private:
  FTSelection *arg_;
  unsigned int distance_;
  FTUnit unit_;
};

class FTWindowMatches : public AllMatches
{
public:
  FTWindowMatches(const LocationInfo *info, unsigned int distance, FTOption::FTUnit unit, const AllMatches::Ptr &arg)
    : AllMatches(info), distance_(distance), unit_(unit), arg_(arg) {}
  Match::Ptr next(DynamicContext *context);

private:
  unsigned int distance_;
  FTOption::FTUnit unit_;
  AllMatches::Ptr arg_;
  Match::Ptr match_;
  std::set<unsigned int> excludeValues_;
  std::set<unsigned int>::iterator excludeIt_;
};

#endif
