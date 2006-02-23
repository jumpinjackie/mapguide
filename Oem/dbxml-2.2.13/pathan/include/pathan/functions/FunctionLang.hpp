/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  lang function

*/

#ifndef _FUNCTIONLANG_HPP
#define _FUNCTIONLANG_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

/** lang function. */
class PATHAN_EXPORT FunctionLang : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionLang(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);

  /** The lang function returns true or false depending on whether the language of
      the context node as specified by xml:lang attributes is the same as or is a
      sublanguage of the language specified by the argument string. The language of 
      the context node is determined by the value of the xml:lang attribute on the 
      context node, or, if the context node has no xml:lang attribute, by the value 
      of the xml:lang attribute on the nearest ancestor of the context node that has 
      an xml:lang attribute.**/

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONLANG_HPP

