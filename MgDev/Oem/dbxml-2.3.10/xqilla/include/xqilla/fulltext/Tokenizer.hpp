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
 * $Id: Tokenizer.hpp,v 1.2 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _TOKENIZER_HPP
#define _TOKENIZER_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/fulltext/TokenInfo.hpp>
#include <xqilla/items/Node.hpp>

class XPath2MemoryManager;
class DynamicContext;

class XQILLA_API Tokenizer
{
public:
  virtual ~Tokenizer() {}

  virtual TokenStream::Ptr tokenize(const Node::Ptr &node, DynamicContext *context) const = 0;
  virtual TokenStream::Ptr tokenize(const XMLCh *string, XPath2MemoryManager *mm) const = 0;

protected:
  Tokenizer() {}
};

#endif
