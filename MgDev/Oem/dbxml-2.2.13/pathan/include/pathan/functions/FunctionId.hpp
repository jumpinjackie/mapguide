/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONID_HPP
#define _FUNCTIONID_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/Sequence.hpp>
#include <vector>

/** 
 * Id function 
 *
 * fn:id(string* $srcval) => element*
 */
class PATHAN_EXPORT FunctionId : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionId(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);

  /** 
   * Returns the sequence of element nodes with ID values matching the value 
   * of one of the IDREF values supplied in $srcval. 
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONID_HPP

