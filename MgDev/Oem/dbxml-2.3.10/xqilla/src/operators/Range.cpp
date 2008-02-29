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
 * $Id: Range.cpp,v 1.17 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/operators/Range.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <assert.h>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

/*static*/ const XMLCh Range::name[]={ chLatin_t, chLatin_o, chNull };

Range::Range(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr)
{
}

ASTNode* Range::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    SequenceType *integerType = new (mm) SequenceType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                                      SchemaSymbols::fgDT_INTEGER,
                                                      SequenceType::QUESTION_MARK, mm);
    integerType->setLocationInfo(this);

    *i = integerType->convertFunctionArg(*i, context, /*numericfunction*/false, *i);

    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode* Range::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::DECIMAL_TYPE;

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticTyping(context);
    _src.add((*i)->getStaticResolutionContext());

    if((*i)->isDateOrTimeAndHasNoTimezone(context))
      _src.implicitTimezoneUsed(true);
  }

  if(!_src.isUsed()) {
    return constantFold(context);
  }
  return this;
}

Result Range::createResult(DynamicContext* context, int flags) const
{
  return new RangeResult(this, context);
}

Range::RangeResult::RangeResult(const Range *op, DynamicContext *context)
  : ResultImpl(op),
    _op(op),
    _last(0),
    _step(context->getItemFactory()->createInteger(1, context)),
    _end(0)
{  
}

Item::Ptr Range::RangeResult::next(DynamicContext *context)
{
  context->testInterrupt();

  if(_end == NULLRCP) {
    // initialise
    _last = (const Numeric::Ptr )_op->getArgument(0)->collapseTree(context)->next(context);
    _end = (const Numeric::Ptr )_op->getArgument(1)->collapseTree(context)->next(context);
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
