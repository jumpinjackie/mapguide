/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/operators/Range.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/Numeric.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>
#include <assert.h>

/*static*/ const XMLCh Range::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Range::Range(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemOperator(name, args, memMgr)
{
}

DataItem* Range::staticResolution(StaticContext *context)
{
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
  return DataItemOperator::staticResolution(context);
}

Result Range::createResult(DynamicContext* context, int flags) const
{
  return new RangeResult(this, context);
}

Range::RangeResult::RangeResult(const Range *op, DynamicContext *context)
  : ResultImpl(context),
    _op(op),
    _last(0),
    _step(context->getPathanFactory()->createInteger(1, context)),
    _end(0)
{  
}

Item::Ptr Range::RangeResult::next(DynamicContext *context)
{
  static SequenceType integerType(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                  XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER,
                                  SequenceType::QUESTION_MARK);

  if(_end == NULLRCP) {
    // initialise
    _last = (const Numeric::Ptr )_op->getArgument(0)->collapseTree(context, DataItem::UNORDERED|DataItem::RETURN_TWO).
      convertFunctionArg(&integerType, _op->getArgument(0)->getStaticResolutionContext().getStaticType(), context).next(context);
    _end = (const Numeric::Ptr )_op->getArgument(1)->collapseTree(context, DataItem::UNORDERED|DataItem::RETURN_TWO).
      convertFunctionArg(&integerType, _op->getArgument(1)->getStaticResolutionContext().getStaticType(), context).next(context);
    if(_last.isNull() || _end.isNull() || _last->greaterThan(_end, context))
      _last = 0;
  }
  else if(_last != NULLRCP) {
    if(_last->equals((const AnyAtomicType::Ptr)_end, context)) {
      _last = 0;
    }
    else {
      _last = _last->add(_step, context);
    }
  }

  return (const Item::Ptr)_last;
}

std::string Range::RangeResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<range/>" << std::endl;

  return oss.str();
}
