/*
 * Copyright (c) 2001-2008
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2008
 *     Oracle. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Id$
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTContent.hpp>
#include <xqilla/fulltext/FTDistance.hpp>
#include <xqilla/context/DynamicContext.hpp>

FTSelection *FTContent::staticResolution(StaticContext *context)
{
  arg_ = arg_->staticResolution(context);
  return this;
}

FTSelection *FTContent::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  arg_ = arg_->staticTyping(context, styper);
  src_.add(arg_->getStaticAnalysis());

  return this;
}

FTSelection *FTContent::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *newarg = arg_->optimize(ftcontext, execute);
  if(newarg == 0) return 0;

  if(type_ == ENTIRE_CONTENT) {
    newarg = new (mm) FTDistanceLiteral(newarg, FTRange::EXACTLY, 0, 0, FTOption::WORDS, mm);
    newarg->setLocationInfo(this);
    newarg = new (mm) FTContent(newarg, AT_START, mm);
    newarg->setLocationInfo(this);
    newarg = new (mm) FTContent(newarg, AT_END, mm);
    newarg->setLocationInfo(this);
  }
  else {
	  newarg = new (mm) FTContent(newarg, type_, mm);
	  newarg->setLocationInfo(this);
  }

  return newarg;
}

AllMatches::Ptr FTContent::execute(FTContext *ftcontext) const
{
  switch(type_) {
  case AT_START:
    return new FTContentAtStartMatches(this, arg_->execute(ftcontext));
  case AT_END:
    return new FTContentAtEndMatches(this, arg_->execute(ftcontext));
  default:
    // ENTIRE_CONTENT is handled by optimize()
    assert(0);
    break;
  }
  return 0;
}

Match::Ptr FTContentAtStartMatches::next(DynamicContext *context)
{
  if(arg_.isNull()) return 0;

  StringMatches::const_iterator smallest;
  Match::Ptr match(0);
  while(match.isNull()) {
    match = arg_->next(context);
    if(match.isNull()) {
      arg_ = 0;
      return 0;
    }

    StringMatches::const_iterator i = match->getStringIncludes().begin();
    StringMatches::const_iterator end = match->getStringIncludes().end();
    if(i != end) {
      smallest = i;
      for(++i; i != end; ++i) {
        if(i->tokenInfo->getPosition() < smallest->tokenInfo->getPosition()) {
          smallest = i;
        }
      }
    }
    else {
      match = 0;
    }
  }

  Match::Ptr result = new Match();
  StringMatches::const_iterator i = match->getStringIncludes().begin();
  StringMatches::const_iterator end = match->getStringIncludes().end();
  for(; i != end; ++i) {
    if(i == smallest) {
      StringMatch sm(*i);
      sm.startToken = true;
      result->addStringInclude(sm);
    }
    else {
      result->addStringInclude(*i);
    }
  }
  result->addStringExcludes(match->getStringExcludes());

  return result;
}

Match::Ptr FTContentAtEndMatches::next(DynamicContext *context)
{
  if(arg_.isNull()) return 0;

  StringMatches::const_iterator largest;
  Match::Ptr match(0);
  while(match.isNull()) {
    match = arg_->next(context);
    if(match.isNull()) {
      arg_ = 0;
      return 0;
    }

    StringMatches::const_iterator i = match->getStringIncludes().begin();
    StringMatches::const_iterator end = match->getStringIncludes().end();
    if(i != end) {
      largest = i;
      for(++i; i != end; ++i) {
        if(i->tokenInfo->getPosition() > largest->tokenInfo->getPosition()) {
          largest = i;
        }
      }
    }
    else {
      match = 0;
    }
  }

  Match::Ptr result = new Match();
  StringMatches::const_iterator i = match->getStringIncludes().begin();
  StringMatches::const_iterator end = match->getStringIncludes().end();
  for(; i != end; ++i) {
    if(i == largest) {
      StringMatch sm(*i);
      sm.endToken = true;
      result->addStringInclude(sm);
    }
    else {
      result->addStringInclude(*i);
    }
  }
  result->addStringExcludes(match->getStringExcludes());

  return result;
}
