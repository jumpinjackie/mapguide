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
 * $Id: FTContains.cpp,v 1.4 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/fulltext/FTContains.hpp>
#include <xqilla/fulltext/FTSelection.hpp>
#include <xqilla/fulltext/DefaultTokenizer.hpp>
#include <xqilla/fulltext/DefaultTokenStore.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>

FTContains::FTContains(ASTNode *argument, FTSelection *selection, ASTNode *ignore, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    argument_(argument),
    selection_(selection),
    ignore_(ignore)
{
  setType(ASTNode::FTCONTAINS);
}

FTContains::~FTContains()
{
  //no-op
}

ASTNode* FTContains::staticResolution(StaticContext *context)
{
  argument_ = argument_->staticResolution(context);
  selection_ = selection_->staticResolution(context);

  if(ignore_ != NULL) {
    ignore_ = ignore_->staticResolution(context);
  }

  return this;
}

ASTNode *FTContains::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;

  argument_ = argument_->staticTyping(context);
  _src.add(argument_->getStaticResolutionContext());

  selection_ = selection_->staticTyping(context);
  _src.add(selection_->getStaticResolutionContext());

  {
    AutoDelete<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());
    DefaultTokenizer tokenizer;
    FTContext ftcontext(&tokenizer, 0, dContext);
    selection_ = selection_->optimize(&ftcontext, /*execute*/false);
  }

  if(ignore_ != NULL) {
    ignore_ = ignore_->staticTyping(context);
    _src.add(ignore_->getStaticResolutionContext());
  }

  return this;
}

Result FTContains::createResult(DynamicContext* context, int flags) const
{
  return new FTContainsResult(this);
}

FTContains::FTContainsResult::FTContainsResult(const FTContains *parent)
  : SingleResult(parent),
    parent_(parent)
{
}

Item::Ptr FTContains::FTContainsResult::getSingleResult(DynamicContext *context) const
{
  // TBD deal with ignore nodes

  DefaultTokenizer tokenizer;

  Result argNodes = parent_->getArgument()->collapseTree(context);
  Item::Ptr item;
  while((item = argNodes->next(context)).notNull()) {
    if(!item->isNode())
      XQThrow(XPath2TypeMatchException, X("FTContains::FTContainsResult::getSingleResult"),
              X("The argument to ftcontains contains non nodes [err:XPTY0004]"));

    FTContext ftcontext(&tokenizer, new DefaultTokenStore((Node*)item.get(), &tokenizer, context), context);

    FTSelection *optimizedSelection = parent_->getSelection()->optimize(&ftcontext, /*execute*/true);

    AllMatches::Ptr matches = optimizedSelection->execute(&ftcontext);
    if(matches.notNull()) {
      StringMatches::const_iterator i;
      StringMatches::const_iterator end;

      Match::Ptr match(0);
      while((match = matches->next(context)).notNull()) {

        bool fail = false;
        i = match->getStringIncludes().begin();
        end = match->getStringIncludes().end();
        for(; i != end; ++i) {
          if(i->startToken && i->tokenInfo->getPosition() !=
             ftcontext.tokenStore->getStartTokenPosition()) {
            fail = true;
            break;
          }
          if(i->endToken && i->tokenInfo->getPosition() !=
             ftcontext.tokenStore->getEndTokenPosition()) {
            fail = true;
            break;
          }
        }

        if(fail) continue;

        i = match->getStringExcludes().begin();
        end = match->getStringExcludes().end();
        for(; i != end; ++i) {
          if(i->startToken && i->tokenInfo->getPosition() !=
             ftcontext.tokenStore->getStartTokenPosition()) {
            continue;
          }
          if(i->endToken && i->tokenInfo->getPosition() !=
             ftcontext.tokenStore->getEndTokenPosition()) {
            continue;
          }
          fail = true;
          break;
        }

        if(fail) continue;

        return context->getItemFactory()->createBoolean(true, context);
      }
    }
  }

  return context->getItemFactory()->createBoolean(false, context);
}

std::string FTContains::FTContainsResult::asString(DynamicContext *context, int indent) const
{
  return "<ftcontainsresult/>";
}
