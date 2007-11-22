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
 * $Id: FTOption.hpp,v 1.3 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FTOPTION_HPP
#define _FTOPTION_HPP

#include <xqilla/fulltext/FTSelection.hpp>
#include <xqilla/fulltext/TokenInfo.hpp>

class XQILLA_API FTOption : public FTSelection
{
public:
  enum FTUnit {
    WORDS,
    SENTENCES,
    PARAGRAPHS
  };

  virtual void setArgument(FTSelection *arg) = 0;

  static unsigned int tokenUnit(const TokenInfo::Ptr t, FTUnit unit)
  {
    switch(unit) {
    case WORDS:
      return t->getPosition();
    case SENTENCES:
      return t->getSentence();
    case PARAGRAPHS:
      return t->getParagraph();
    }
    return 0;
  }

  static unsigned int tokenDistance(const TokenInfo::Ptr a, const TokenInfo::Ptr b, FTUnit unit)
  {
    switch(unit) {
    case WORDS:
      return (unsigned int)labs((long)a->getPosition() - (long)b->getPosition()) - 1;
    case SENTENCES:
      return (unsigned int)labs((long)a->getSentence() - (long)b->getSentence());
    case PARAGRAPHS:
      return (unsigned int)labs((long)a->getParagraph() - (long)b->getParagraph());
    }
    return 0;
  }

protected:
  FTOption(Type type, XPath2MemoryManager *memMgr)
    : FTSelection(type, memMgr) {}
};

typedef std::vector<FTOption*,XQillaAllocator<FTOption*> > VectorOfFTOptions;

#endif
