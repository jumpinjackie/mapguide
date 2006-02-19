/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functionAPI/AggregateFunction.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/exceptions/DSLException.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATFloatOrDerived.hpp>
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>
#include <pathan/ATTimeOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/SequenceType.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLString.hpp>

AggregateFunction::AggregateFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
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
      firstStep.addItem(atom->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                     XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE,
                                     context));
    }
    else if(isNumericNaN(*i))
      return Sequence(atom, context->getMemoryManager());
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
          if(context->isTypeOrDerivedFromType(atomTypeURI, atomTypeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER)) {
            sequenceTypeName = XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER;
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
        if (XPath2Utils::equals(sequenceTypeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE)) {
          // sequenceType is double, so do nothing since double is king
        } else if (XPath2Utils::equals(sequenceTypeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT)) {
          // sequenceType is float, so if atomType is double then so is sequenceType
          if (XPath2Utils::equals(atom->getPrimitiveTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE))
            sequenceTypeName = atom->getPrimitiveTypeName();
        } else if (context->isTypeOrDerivedFromType(sequenceTypeURI, sequenceTypeName,  XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL)) {
          if (!context->isTypeOrDerivedFromType(atomTypeURI, atomTypeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER)) {
            sequenceTypeName = atom->getPrimitiveTypeName();
          }          
        } else { 
          // we should never actually get here
          DSLthrow(IllegalArgumentException, X("AggregateFunction::validateSequence"), X("Invalid argument to aggregate function"));
        }
      } else { // we have incompatible types
        DSLthrow(IllegalArgumentException, X("AggregateFunction::validateSequence"), X("Invalid argument to aggregate function"));
      } 
    }
  }
 
  SequenceType sequenceType(sequenceTypeURI, sequenceTypeName, SequenceType::STAR);
  Sequence castedSequence(context->getMemoryManager());
  try {
    StaticType stype;
    stype.flags = StaticType::NODE_TYPE;
    castedSequence = sequenceType.convertFunctionArg(firstStep, stype, context).toSequence(context);
  } catch (XPath2ErrorException &e) {
    DSLthrow(IllegalArgumentException, X("AggregateFunction::validateSequence"), X("Invalid argument to aggregate function"));    
  }

  return castedSequence;
}

bool AggregateFunction::isNumericType(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const {
  return (context->isTypeOrDerivedFromType(typeURI,typeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL) || 
          context->isTypeOrDerivedFromType(typeURI,typeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT) || 
          context->isTypeOrDerivedFromType(typeURI,typeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE));
}  

bool AggregateFunction::isNumericNaN(const Item::Ptr &item) const
{
  if(item->isNode())
    return false;
  const AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )item;
  if(atom->getPrimitiveTypeIndex()==AnyAtomicType::DOUBLE && ((const ATDoubleOrDerived::Ptr)atom)->isNaN())
    return true;
  else if(atom->getPrimitiveTypeIndex()==AnyAtomicType::FLOAT && ((const ATFloatOrDerived::Ptr)atom)->isNaN())
    return true;
  return false;
}
