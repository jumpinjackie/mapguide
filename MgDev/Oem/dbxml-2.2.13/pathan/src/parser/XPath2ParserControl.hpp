/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Definition of parser control struct.

  This is needed by Xparser.y, test.cpp and ../lexer/XPathLexer.cpp.
*/

#ifndef _XPATHPARSERCONTROL_HPP
#define _XPATHPARSERCONTROL_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItem.hpp>
#include <pathan/StaticContext.hpp>

class xxFlexLexer;
class XPath2MemoryManager;

class PATHAN_EXPORT XPathParserControl {

public:
  DataItem* result;
  xxFlexLexer *lexer;
  const StaticContext* context;
	XPath2MemoryManager* memMgr;

  XPathParserControl()
    : result(NULL),
      lexer(NULL),
      context(NULL),
      memMgr(NULL)
  {
  };

};

#endif // _XPATHPARSERC:ONTROL_HPP
