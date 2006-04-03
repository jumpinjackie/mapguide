/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONIDREF_HPP
#define _FUNCTIONIDREF_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/Sequence.hpp>
#include <vector>

/** 
 * Function idref
 * 
 * fn:idref(string* $srcval) => element*
 *
 */
class PATHAN_EXPORT FunctionIdref : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionIdref(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);

  /** 
   * Returns the sequence of elements nodes having either an IDREF attribute 
   * whose value matches the value of one of the items in the value of $srcval 
   * or an IDREFS attribute whose value contains an IDREF value that matches 
   * the value of one of the items in the value of $srcval. 
   *
   * This function allows reverse navigation from IDs to IDREFs.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONIDREF_HPP

