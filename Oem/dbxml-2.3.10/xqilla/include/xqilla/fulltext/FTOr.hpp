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
 * $Id: FTOr.hpp,v 1.4 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FTOR_HPP
#define _FTOR_HPP

#include <xqilla/fulltext/FTSelection.hpp>

#include <vector>

class XQILLA_API FTOr : public FTSelection
{
public:
  FTOr(XPath2MemoryManager *memMgr);
  FTOr(FTSelection *left, FTSelection *right, XPath2MemoryManager *memMgr);

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  const VectorOfFTSelections &getArguments() const { return args_; }
  void addArg(FTSelection *sel) { args_.push_back(sel); }

private:
  VectorOfFTSelections args_;
};

class FTDisjunctionMatches : public AllMatches
{
public:
  FTDisjunctionMatches(const LocationInfo *info);

  void addMatches(const AllMatches::Ptr &m)
  {
    args_.push_back(m);
  }

  Match::Ptr next(DynamicContext *context);

private:
  bool toDo_;
  std::vector<AllMatches::Ptr> args_;
  std::vector<AllMatches::Ptr>::iterator it_;
};

#endif
