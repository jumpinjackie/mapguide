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
 * $Id: DefaultTokenizer.cpp,v 1.2 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/DefaultTokenizer.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include <xercesc/util/XMLString.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

TokenStream::Ptr DefaultTokenizer::tokenize(const Node::Ptr &node, DynamicContext *context) const
{
  return new DefaultTokenStream(node->dmStringValue(context), context->getMemoryManager());
}

TokenStream::Ptr DefaultTokenizer::tokenize(const XMLCh *str, XPath2MemoryManager *mm) const
{
  return new DefaultTokenStream(str, mm);
}

DefaultTokenizer::DefaultTokenStream::DefaultTokenStream(const XMLCh *str, XPath2MemoryManager *mm)
  : string_(XMLString::replicate(str, mm)),
    current_(string_),
    tokenStart_(0),
    position_(0),
    sentence_(0),
    paragraph_(0),
    seenEndOfSentence_(false),
    mm_(mm)
{
}

DefaultTokenizer::DefaultTokenStream::~DefaultTokenStream()
{
  mm_->deallocate(string_);
}

#define REPORT_TOKEN \
      if(tokenStart_ != 0) { \
        *current_ = 0; \
        result = new DefaultTokenInfo(mm_->getPooledString(tokenStart_), position_, sentence_, paragraph_); \
        ++position_; \
        tokenStart_ = 0; \
      }


TokenInfo::Ptr DefaultTokenizer::DefaultTokenStream::next()
{
  TokenInfo::Ptr result(0);
  while(result.isNull()) {
    switch(*current_) {
    case '\n': {
      REPORT_TOKEN;
      if(seenEndOfSentence_) {
        ++paragraph_;
        seenEndOfSentence_ = false;
      }
      break;
    }
    case '!':
    case '?':
    case ':':
    case '.': {
      REPORT_TOKEN;
      if(!seenEndOfSentence_) {
        ++sentence_;
        seenEndOfSentence_ = true;
      }
      break;
    }
    case '\r':
    case '\t':
    case ' ':
    case '"':
    case '\'':
    case '`':
    case ';':
    case ',': {
      REPORT_TOKEN;
      break;
    }
    case 0: {
      REPORT_TOKEN;
      return result;
    }
    default: {
      if(tokenStart_ == 0) {
        tokenStart_ = current_;
        seenEndOfSentence_ = false;
      }
      break;
    }
    }

    ++current_;
  }

  return result;
}
