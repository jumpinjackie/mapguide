/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

   document-uri function

*/

#ifndef _FUNCTIONDOCUMENTURI_HPP
#define _FUNCTIONDOCUMENTURI_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class Sequence;

/* document-uri function */
class PATHAN_EXPORT FunctionDocumentURI : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDocumentURI(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
	/* returns the document-URI property of the node */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif


