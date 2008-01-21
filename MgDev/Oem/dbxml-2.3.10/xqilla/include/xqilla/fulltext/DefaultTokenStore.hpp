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
 * $Id: DefaultTokenStore.hpp,v 1.3 2006/11/17 18:08:07 jpcs Exp $
 */

#ifndef _DEFAULTTOKENSTORE_HPP
#define _DEFAULTTOKENSTORE_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/fulltext/TokenStore.hpp>
#include <xqilla/items/Node.hpp>

#include <xercesc/util/RefHashTableOf.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

#include <vector>

class XPath2MemoryManager;
class DynamicContext;

class XQILLA_API DefaultTokenStore : public TokenStore
{
public:
  typedef std::vector<TokenInfo::Ptr> Tokens;

  class TokenEntryStream : public TokenStream
  {
  public:
    TokenEntryStream(const Tokens &tokens)
      : it_(tokens.begin()), end_(tokens.end()) {}

    virtual TokenInfo::Ptr next()
    {
      if(it_ != end_)
        return *it_++;
      return 0;
    }

  private:
    Tokens::const_iterator it_;
    Tokens::const_iterator end_;
  };

  DefaultTokenStore(const Node::Ptr &node, const Tokenizer *tokenizer,
                    DynamicContext *context);
  DefaultTokenStore(XPath2MemoryManager *mm);

  virtual TokenStream::Ptr findTokens(const XMLCh *searchString) const;

  virtual unsigned int getStartTokenPosition() const { return 0; }
  virtual unsigned int getEndTokenPosition() const { return numTokens_ - 1; }

private:
  class TokenEntry
  {
  public:
    void addToken(const TokenInfo::Ptr &token)
    {
      tokens_.push_back(token);
    }
    TokenStream::Ptr getTokenStream() const
    {
      return new TokenEntryStream(tokens_);
    }

  private:
    Tokens tokens_;
  };

private:
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<TokenEntry> tokens_;
  unsigned int numTokens_;
  XPath2MemoryManager *mm_;

  mutable XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer_;
};

#endif
