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
 * $Id: FTContent.hpp,v 1.4 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FTCONTENT_HPP
#define _FTCONTENT_HPP

#include <xqilla/fulltext/FTOption.hpp>

class XQILLA_API FTContent : public FTOption
{
public:
  enum Type {
    AT_START,
    AT_END,
    ENTIRE_CONTENT
  };

  FTContent(Type type, XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::CONTENT, memMgr), type_(type), arg_(0) {}
  FTContent(FTSelection *arg, Type type, XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::CONTENT, memMgr), type_(type), arg_(arg) {}

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  virtual void setArgument(FTSelection *arg) { arg_ = arg; }
  const FTSelection *getArgument() const { return arg_; }
  Type getType() const { return type_; }

private:
  Type type_;
  FTSelection *arg_;
};

class FTContentAtStartMatches : public AllMatches
{
public:
  FTContentAtStartMatches(const LocationInfo *info, const AllMatches::Ptr &arg)
    : AllMatches(info), arg_(arg) {}
  Match::Ptr next(DynamicContext *context);

private:
  AllMatches::Ptr arg_;
};

class FTContentAtEndMatches : public AllMatches
{
public:
  FTContentAtEndMatches(const LocationInfo *info, const AllMatches::Ptr &arg)
    : AllMatches(info), arg_(arg) {}
  Match::Ptr next(DynamicContext *context);

private:
  AllMatches::Ptr arg_;
};

#endif
