/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/**
 * xs:TYP(item $srcval) => TYP 
**/

#ifndef _FUNCTIONCONSTRUCTOR_HPP
#define _FUNCTIONCONSTRUCTOR_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>
#include <pathan/internal/factory/FuncFactory.hpp>

#include <xercesc/util/XercesDefs.hpp>


class PATHAN_EXPORT FunctionConstructor : public ConstantFoldingFunction
{
public:
  ///Constructor. Sets arg def to NOT_OPTIONAL.
  FunctionConstructor(const XMLCh* nsURI, const XMLCh* typeName,
                      AnyAtomicType::AtomicObjectType primitiveType,
                      const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  static const XMLCh XMLChConstructorURI[];
  static const XMLCh XMLChXPath2DatatypesURI[];
};

#endif // _FUNCTIONCONSTRUCTOR_HPP
