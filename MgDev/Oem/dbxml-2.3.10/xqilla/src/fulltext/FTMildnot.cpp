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
 * $Id: FTMildnot.cpp,v 1.4 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTMildnot.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>

FTMildnot::FTMildnot(FTSelection *left, FTSelection *right, XPath2MemoryManager *memMgr)
    : FTSelection(FTSelection::MILD_NOT, memMgr),
      left_(left),
      right_(right)
{
}

FTSelection *FTMildnot::staticResolution(StaticContext *context)
{
  left_ = left_->staticResolution(context);
  right_ = right_->staticResolution(context);
  return this;
}

FTSelection *FTMildnot::staticTyping(StaticContext *context)
{
  src_.clear();

  left_ = left_->staticTyping(context);
  src_.add(left_->getStaticResolutionContext());

  right_ = right_->staticTyping(context);
  src_.add(right_->getStaticResolutionContext());

  return this;
}

FTSelection *FTMildnot::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *newleft = left_->optimize(ftcontext, execute);
  if(newleft == 0) return 0;

  FTSelection *newright = right_->optimize(ftcontext, execute);
  if(newright == 0) return newleft;

  FTSelection *result = new (mm) FTMildnot(newleft, newright, mm);
  result->setLocationInfo(this);
  return result;
}

AllMatches::Ptr FTMildnot::execute(FTContext *ftcontext) const
{
  AllMatches::Ptr leftMatches = left_->execute(ftcontext);
  AllMatches::Ptr rightMatches = right_->execute(ftcontext);
  return new FTMildnotMatches(this, leftMatches, rightMatches);
}

Match::Ptr FTMildnotMatches::next(DynamicContext *context)
{
  if(left_.isNull()) return 0;

  if(right_.notNull()) {
    Match::Ptr match(0);
    while((match = right_->next(context)).notNull()) {
      if(!match->getStringExcludes().empty())
        XQThrow(XPath2ErrorException, X("FTMildnotMatches::next"),
                X("Invalid expression on the right-hand side of a not-in"));

      StringMatches::const_iterator end = match->getStringIncludes().end();
      for(StringMatches::const_iterator i = match->getStringIncludes().begin();
          i != end; ++i) {
        badTokens_.insert(i->tokenInfo->getPosition());
      }
    }
    right_ = 0;
  }

  Match::Ptr match(0);
  while(match.isNull()) {
    match = left_->next(context);
    if(match.isNull()) {
      left_ = 0;
      return 0;
    }

    StringMatches::const_iterator end = match->getStringIncludes().end();
    for(StringMatches::const_iterator i = match->getStringIncludes().begin();
        i != end; ++i) {
      if(badTokens_.find(i->tokenInfo->getPosition()) != badTokens_.end()) {
        match = 0;
        break;
      }
    }
  }

  return match;
}
