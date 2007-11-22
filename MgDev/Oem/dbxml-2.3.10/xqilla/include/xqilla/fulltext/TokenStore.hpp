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
 * $Id: TokenStore.hpp,v 1.2 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _TOKENSTORE_HPP
#define _TOKENSTORE_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/framework/ReferenceCounted.hpp>
#include <xqilla/fulltext/TokenInfo.hpp>

class Tokenizer;

class XQILLA_API TokenStore : public ReferenceCounted
{
public:
  typedef RefCountPointer<const TokenStore> Ptr;

  virtual ~TokenStore() {}

  virtual TokenStream::Ptr findTokens(const XMLCh *searchString) const = 0;

  virtual unsigned int getStartTokenPosition() const = 0;
  virtual unsigned int getEndTokenPosition() const = 0;

protected:
  TokenStore() {}
};

#endif
