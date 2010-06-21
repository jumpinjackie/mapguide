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
 * $Id: FTOrder.cpp,v 1.4 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTOrder.hpp>
#include <xqilla/context/DynamicContext.hpp>

FTSelection *FTOrder::staticResolution(StaticContext *context)
{
  arg_ = arg_->staticResolution(context);
  return this;
}

FTSelection *FTOrder::staticTyping(StaticContext *context)
{
  src_.clear();

  arg_ = arg_->staticTyping(context);
  src_.add(arg_->getStaticResolutionContext());

  return this;
}

FTSelection *FTOrder::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *newarg = arg_->optimize(ftcontext, execute);
  if(newarg == 0) return 0;

  if(newarg->getType() == WORD) {
    return newarg;
  }

  newarg = new (mm) FTOrder(newarg, mm);
  newarg->setLocationInfo(this);
  return newarg;
}

AllMatches::Ptr FTOrder::execute(FTContext *ftcontext) const
{
  return new FTOrderMatches(this, arg_->execute(ftcontext));
}

Match::Ptr FTOrderMatches::next(DynamicContext *context)
{
  if(arg_.isNull()) return 0;

  Match::Ptr match(0);
  while(match.isNull()) {
    match = arg_->next(context);
    if(match.isNull()) {
      arg_ = 0;
      return 0;
    }

    StringMatches::const_iterator begin = match->getStringIncludes().begin();
    StringMatches::const_iterator end = match->getStringIncludes().end();
    StringMatches::const_iterator i, j;
    for(i = begin; i != end && match.notNull(); ++i) {
      for(j = i, ++j; j != end; ++j) {
        if((i->queryPos > j->queryPos && i->tokenInfo->getPosition() < j->tokenInfo->getPosition()) ||
           (i->queryPos < j->queryPos && i->tokenInfo->getPosition() > j->tokenInfo->getPosition())) {
          match = 0;
          break;
        }
      }
    }
  }

  Match::Ptr result = new Match();
  result->addStringIncludes(match->getStringIncludes());

  StringMatches::const_iterator e_end = match->getStringExcludes().end();
  StringMatches::const_iterator e = match->getStringExcludes().begin();
  StringMatches::const_iterator i_begin = match->getStringIncludes().begin();
  StringMatches::const_iterator i_end = match->getStringIncludes().end();
  StringMatches::const_iterator i;
  for(; e != e_end; ++e) {
    for(i = i_begin; i != i_end; ++i) {
      if((i->queryPos <= e->queryPos && i->tokenInfo->getPosition() <= e->tokenInfo->getPosition()) ||
         (i->queryPos >= e->queryPos && i->tokenInfo->getPosition() >= e->tokenInfo->getPosition())) {
        result->addStringExclude(*e);
      }
    }
  }

  return result;
}
