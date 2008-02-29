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
 * $Id: DefaultTokenStore.cpp,v 1.3 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/DefaultTokenStore.hpp>
#include <xqilla/fulltext/Tokenizer.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/utils/UCANormalizer.hpp>

#include <xercesc/util/XMLString.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

DefaultTokenStore::DefaultTokenStore(const Node::Ptr &node, const Tokenizer *tokenizer,
                                     DynamicContext *context)
  : tokens_(73, /*adoptElems*/true, context->getMemoryManager()),
    numTokens_(0),
    mm_(context->getMemoryManager()),
    buffer_(1023, context->getMemoryManager())
{
  TokenStream::Ptr stream = tokenizer->tokenize(node, context);
  TokenInfo::Ptr token;
  while((token = stream->next()).notNull()) {
    ++numTokens_;
    buffer_.reset();
    Normalizer::caseFoldAndRemoveDiacritics(token->getWord(), buffer_);

    TokenEntry *entry = tokens_.get(buffer_.getRawBuffer());
    if(entry == 0) {
      entry = new TokenEntry();
      tokens_.put((void*)mm_->getPooledString(buffer_.getRawBuffer()), entry);
    }
    entry->addToken(token);
  }
}

DefaultTokenStore::DefaultTokenStore(XPath2MemoryManager *mm)
  : tokens_(3, /*adoptElems*/true, mm),
    numTokens_(0),
    mm_(mm),
    buffer_(0, mm)
{
}

TokenStream::Ptr DefaultTokenStore::findTokens(const XMLCh *searchString) const
{
  buffer_.reset();
  Normalizer::caseFoldAndRemoveDiacritics(searchString, buffer_);

  const TokenEntry *entry = tokens_.get(buffer_.getRawBuffer());
  if(entry == 0) return 0;
  return entry->getTokenStream();
}
