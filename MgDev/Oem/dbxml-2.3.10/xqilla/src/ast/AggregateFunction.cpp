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
 * $Id: AggregateFunction.cpp,v 1.14 2006/11/01 16:37:17 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/ast/AggregateFunction.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATFloatOrDerived.hpp>
#include <xqilla/items/ATDateOrDerived.hpp>
#include <xqilla/items/ATDateTimeOrDerived.hpp>
#include <xqilla/items/ATTimeOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLException.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

AggregateFunction::AggregateFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  :  ConstantFoldingFunction(name,argsFrom, argsTo, paramDecl, args, memMgr)
{ 
}

  /* 
   * returns a sequence with untypedAtomic values casted to xs:double
   * If a NaN value is found, a singleton sequence containing NaN is returned
   * All the items in the returned sequence are guaranteed to be of the same type
   */
Sequence AggregateFunction::validateSequence(Sequence sequence, DynamicContext* context) const {
  if(sequence.isEmpty())
    return sequence;
  Sequence::iterator i = sequence.begin();

  Sequence firstStep(context->getMemoryManager());
  // check for NaN and convert untypedAtomic
  for (; i != sequence.end(); ++i) {
    const AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )*i;
    const XMLCh* atomTypeURI = atom->getTypeURI();
    const XMLCh* atomTypeName = atom->getTypeName();
    
    if (XPath2Utils::equals(atomTypeName, ATUntypedAtomic::fgDT_UNTYPEDATOMIC) &&
        XPath2Utils::equals(atomTypeURI, FunctionConstructor::XMLChXPath2DatatypesURI )) {
      firstStep.addItem(atom->castAs(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                     SchemaSymbols::fgDT_DOUBLE,
                                     context));
    }
    else
      firstStep.addItem(atom);
  }  
  // check that sequence contains items of a single type or one if its subtypes.  
  // if there are different numeric types, promote them all to a single common type.
  const XMLCh* sequenceTypeURI = FunctionConstructor::XMLChXPath2DatatypesURI;
  const XMLCh* sequenceTypeName = ATUntypedAtomic::fgDT_UNTYPEDATOMIC;
  i = firstStep.begin();
  for (; i != firstStep.end(); ++i) {
    const AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )*i;
    const XMLCh* atomTypeURI = atom->getTypeURI();
    const XMLCh* atomTypeName = atom->getTypeName();
    if (! ( XPath2Utils::equals(sequenceTypeName, atomTypeName) &&
            XPath2Utils::equals(sequenceTypeURI, atomTypeURI)     ) ) { 
      // if the sequenceType and atomType are not equal we must determine what to do
      if (XPath2Utils::equals(sequenceTypeName, ATUntypedAtomic::fgDT_UNTYPEDATOMIC) &&
          XPath2Utils::equals(sequenceTypeURI, FunctionConstructor::XMLChXPath2DatatypesURI )) {
        // if the sequenceType is untypedAtomic then we are the first, so set the sequenceType to the atomType
        if (isNumericType(atomTypeURI, atomTypeName, context)) {
          sequenceTypeURI = atom->getPrimitiveTypeURI();
          if(context->isTypeOrDerivedFromType(atomTypeURI, atomTypeName, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_INTEGER)) {
            sequenceTypeName = SchemaSymbols::fgDT_INTEGER;
          } else {
            sequenceTypeName = atom->getPrimitiveTypeName();
          }
        } else {
          sequenceTypeURI = atomTypeURI;
          sequenceTypeName = atomTypeName;    
        }
      } else if (context->isTypeOrDerivedFromType(sequenceTypeURI, sequenceTypeName, atomTypeURI, atomTypeName)) {
        // if the sequenceType is derived from the atomType then we have a sequence of the atomType
        sequenceTypeURI = atomTypeURI;
        sequenceTypeName = atomTypeName;
      } else if (context->isTypeOrDerivedFromType(atomTypeURI, atomTypeName, sequenceTypeURI, sequenceTypeName)) {
        // if the atomType is derived from the sequenceType let it pass to get dealt with by the SequenceType class
      } else if (isNumericType(sequenceTypeURI, sequenceTypeName, context) && isNumericType(atomTypeURI, atomTypeName, context)) {
        // if we are dealing with numerics determine which common type to promote to
        if (XPath2Utils::equals(sequenceTypeName, SchemaSymbols::fgDT_DOUBLE)) {
          // sequenceType is double, so do nothing since double is king
        } else if (XPath2Utils::equals(sequenceTypeName, SchemaSymbols::fgDT_FLOAT)) {
          // sequenceType is float, so if atomType is double then so is sequenceType
          if (XPath2Utils::equals(atom->getPrimitiveTypeName(), SchemaSymbols::fgDT_DOUBLE))
            sequenceTypeName = atom->getPrimitiveTypeName();
        } else if (context->isTypeOrDerivedFromType(sequenceTypeURI, sequenceTypeName,  SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_DECIMAL)) {
          if (!context->isTypeOrDerivedFromType(atomTypeURI, atomTypeName, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_INTEGER)) {
            sequenceTypeName = atom->getPrimitiveTypeName();
          }          
        } else { 
          // we should never actually get here
          XQThrow(::IllegalArgumentException, X("AggregateFunction::validateSequence"), X("Invalid argument to aggregate function"));
        }
      } else { // we have incompatible types
        XQThrow(::IllegalArgumentException, X("AggregateFunction::validateSequence"), X("Invalid argument to aggregate function"));
      } 
    }
  }

  if(isNumericType(sequenceTypeURI, sequenceTypeName, context)) {
    Sequence castedSequence(context->getMemoryManager());
    try {
      i = firstStep.begin();
      for (; i != firstStep.end(); ++i) {
        const AnyAtomicType::Ptr atomic = (const AnyAtomicType *)i->get();
        if(atomic->isNumericValue()) {
          const Numeric::Ptr promotedType = ((const Numeric*)atomic.get())->promoteTypeIfApplicable(sequenceTypeURI, sequenceTypeName, context);
          if(promotedType != NULLRCP) {
            if(isNumericNaN(promotedType))
              return Sequence(promotedType, context->getMemoryManager());
            else
            castedSequence.addItem(promotedType);
          }
          else {
            XQThrow(::IllegalArgumentException, X("AggregateFunction::validateSequence"), X("Invalid argument to aggregate function"));    
          }
        }
      }
    } catch (XPath2TypeCastException &e) {
      XQThrow(::IllegalArgumentException, X("AggregateFunction::validateSequence"), X("Invalid argument to aggregate function"));    
    } catch (const XMLException& e) {
      XQThrow(::IllegalArgumentException, X("AggregateFunction::validateSequence"), X("Invalid argument to aggregate function"));    
    }

    return castedSequence;
  }

  return firstStep;
}

bool AggregateFunction::isNumericType(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const {
  return (context->isTypeOrDerivedFromType(typeURI,typeName, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_DECIMAL) || 
          context->isTypeOrDerivedFromType(typeURI,typeName, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_FLOAT) || 
          context->isTypeOrDerivedFromType(typeURI,typeName, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_DOUBLE));
}  

bool AggregateFunction::isNumericNaN(const Item::Ptr &item) const
{
  if(item->isNode())
    return false;
  const AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )item;
  return (atom->isNumericValue() && ((const Numeric::Ptr)atom)->isNaN());
}
