/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  (string $uri-part, boolean $escape-reserved) => string
  Applies the URI escaping rules to the string supplied
  

*/

#ifndef _FUNCTIONESCAPEURI_HPP
#define _FUNCTIONESCAPEURI_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/util/XMLUTF8Transcoder.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Substring function. */
class PATHAN_EXPORT FunctionEscapeURI : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionEscapeURI(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Returns param1 after applying URI escaping rules - the value of param2 determines exactly which set of 
   *  characters is escaped**/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

private:
 //The UTF-8 XML Transcoder
 //XMLUTF8Transcoder transcoder;
  
 //Returns the escape character sequence for the given character
 XMLCh* escapeChar(XMLCh c) const;
 
};

#endif // _FUNCTIONESCAPEURI_HPP





