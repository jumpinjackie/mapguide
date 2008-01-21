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
 * $Id: FTSelection.hpp,v 1.5 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FTSELECTION_HPP
#define _FTSELECTION_HPP


#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/ast/LocationInfo.hpp>
#include <xqilla/fulltext/AllMatches.hpp>
#include <xqilla/fulltext/TokenStore.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>

#include <vector>

class DynamicContext;
class StaticContext;

class XQILLA_API FTContext
{
public:
  FTContext(const Tokenizer *tok, const TokenStore::Ptr &tStore, DynamicContext *cont)
    : queryPos(0), tokenizer(tok), tokenStore(tStore), context(cont) {}

  unsigned int queryPos;
  const Tokenizer *tokenizer;
  TokenStore::Ptr tokenStore;
  DynamicContext *context;
};

class XQILLA_API FTSelection : public LocationInfo
{
public:
  /// The type of the FTSelection node
  enum Type {
    OR,
    AND,
    MILD_NOT,
    UNARY_NOT,
    WORDS,
    WORD,
    ORDER,
    DISTANCE,
    DISTANCE_LITERAL,
    SCOPE,
    CONTENT,
    WINDOW,
    WINDOW_LITERAL
  };

  virtual ~FTSelection() {}

  Type getType() const { return type_; }

  virtual const StaticResolutionContext &getStaticResolutionContext() const { return src_; }

  virtual FTSelection *staticResolution(StaticContext *context) = 0;
  virtual FTSelection *staticTyping(StaticContext *context) = 0;
  virtual FTSelection *optimize(FTContext *context, bool execute) const = 0;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const = 0;

protected:
  FTSelection(Type type, XPath2MemoryManager *memMgr)
    : src_(memMgr), type_(type), memMgr_(memMgr) {}

  XPath2MemoryManager *getMemoryManager() const { return memMgr_; }

  StaticResolutionContext src_;

private:
  Type type_;
  XPath2MemoryManager *memMgr_;
};

typedef std::vector<FTSelection*,XQillaAllocator<FTSelection*> > VectorOfFTSelections;

#endif
