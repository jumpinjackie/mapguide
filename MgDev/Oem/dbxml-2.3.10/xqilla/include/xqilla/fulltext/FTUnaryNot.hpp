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
 * $Id: FTUnaryNot.hpp,v 1.4 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FTUNARYNOT_HPP
#define _FTUNARYNOT_HPP

#include <xqilla/fulltext/FTSelection.hpp>

class XQILLA_API FTUnaryNot : public FTSelection
{
public:
  FTUnaryNot(FTSelection *arg, XPath2MemoryManager *memMgr);

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  const FTSelection *getArgument() const { return arg_; }

private:
  FTSelection *arg_;
};

class FTUnaryNotMatches : public AllMatches
{
public:
  FTUnaryNotMatches(const LocationInfo *info, const AllMatches::Ptr &arg)
    : AllMatches(info), toDo_(true), arg_(arg) {}
  Match::Ptr next(DynamicContext *context);

private:
  bool toDo_;
  AllMatches::Ptr arg_;
};

#endif
