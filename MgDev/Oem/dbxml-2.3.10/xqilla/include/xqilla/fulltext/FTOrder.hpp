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
 * $Id: FTOrder.hpp,v 1.4 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FTORDER_HPP
#define _FTORDER_HPP

#include <xqilla/fulltext/FTOption.hpp>

class XQILLA_API FTOrder : public FTOption
{
public:
  FTOrder(XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::ORDER, memMgr), arg_(0) {}
  FTOrder(FTSelection *arg, XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::ORDER, memMgr), arg_(arg) {}

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  virtual void setArgument(FTSelection *arg) { arg_ = arg; }
  const FTSelection *getArgument() const { return arg_; }

private:
  FTSelection *arg_;
};

class FTOrderMatches : public AllMatches
{
public:
  FTOrderMatches(const FTOrder *ft, const AllMatches::Ptr &arg)
    : AllMatches(ft), arg_(arg) {}
  Match::Ptr next(DynamicContext *context);

private:
  AllMatches::Ptr arg_;
};

#endif
