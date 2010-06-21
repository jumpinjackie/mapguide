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
 * $Id: FunctionIndexOf.cpp,v 1.14 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionIndexOf.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATAnyURIOrDerived.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

const XMLCh FunctionIndexOf::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionIndexOf::minArgs = 2;
const unsigned int FunctionIndexOf::maxArgs = 3;

/**
 * fn:index-of($seqParam as xdt:anyAtomicType*, $srchParam as xdt:anyAtomicType) as xs:integer*
 * fn:index-of($seqParam as xdt:anyAtomicType*, $srchParam as xdt:anyAtomicType, $collation as xs:string) as xs:integer*
**/

FunctionIndexOf::FunctionIndexOf(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "anyAtomicType*, anyAtomicType, string", args, memMgr)
{
  _src.getStaticType().flags = StaticType::DECIMAL_TYPE;
}

Sequence FunctionIndexOf::indexOf(Sequence &list, const Item::Ptr &item, Collation* collation, DynamicContext* context) const
{
  Sequence result = Sequence(list.getLength(),context->getMemoryManager());
  int index = 1;
  AnyAtomicType::Ptr atom=(const AnyAtomicType::Ptr )item;
  // need to manually convert xdt:untypedAtomic to xs:string
  if(atom->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC)
    atom = atom->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                        XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  for(Sequence::iterator i = list.begin(); i != list.end(); ++i,++index) {
    AnyAtomicType::Ptr current = (const AnyAtomicType::Ptr )*i;
    try {
        // need to manually convert xdt:untypedAtomic to xs:string
        if(current->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC)
            current = current->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);

        if(Equals::equals(current,atom,collation,context,this))
        result.addItem(context->getItemFactory()->createInteger(index, context));
    } catch (IllegalArgumentException &e) {
        // if eq is not defined, they are different
    } catch (XPath2ErrorException &e) {
        // if eq is not defined, they are different
    }
  }
  return result;
}


Sequence FunctionIndexOf::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();
	Sequence list=getParamNumber(1,context)->toSequence(context);
	if(list.isEmpty()) return Sequence(memMgr);
  const Item::Ptr srchparam = (const Item::Ptr )getParamNumber(2,context)->next(context);
  
  Collation* collation=NULL;
  if (getNumArgs() > 2) {
    const XMLCh* collName=getParamNumber(3,context)->next(context)->asString(context);
    try {
      context->getItemFactory()->createAnyURI(collName, context);
    } catch(XPath2ErrorException &e) {
      XQThrow(FunctionException, X("FunctionIndexOf::collapseTreeInternal"), X("Invalid collationURI"));  
    }
	  collation=context->getCollation(collName, this);
  }
  else
    collation = context->getDefaultCollation(this);
  return indexOf(list, srchparam, collation, context);
}
