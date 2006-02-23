/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <sstream>

#include <pathan/dataItem/DataItemCastAs.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/internal/parser/QName.hpp>
#include <pathan/exceptions/TypeErrorException.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>
#include <pathan/AnyAtomicType.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DataItemCastAs::DataItemCastAs(DataItem* expr, SequenceType* exprType, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr),
  _expr(expr),
  _exprType(exprType)
{
	setType(DataItem::CAST_AS);
}

Result DataItemCastAs::createResult(DynamicContext* context, int flags) const
{
  return new CastAsResult(this, context);
}

DataItem* DataItemCastAs::staticResolution(StaticContext *context) {
  return resolveDataItem(_expr, context, true);
}

const DataItem *DataItemCastAs::getExpression() const {
  return _expr;
}

const SequenceType *DataItemCastAs::getSequenceType() const {
  return _exprType;
}

void DataItemCastAs::setExpression(DataItem *item) {
  _expr = item;
}

DataItemCastAs::CastAsResult::CastAsResult(const DataItemCastAs *di, DynamicContext *context)
  : SingleResult(context),
    _di(di)
{
}

Item::Ptr DataItemCastAs::CastAsResult::getSingleResult(DynamicContext *context) const
{
  // The semantics of the cast expression are as follows:
  //    1. Atomization is performed on the input expression.
	Result toBeCasted(_di->getExpression()->collapseTree(context, DataItem::RETURN_TWO|DataItem::UNORDERED));

  if(_di->getExpression()->getStaticResolutionContext().getStaticType().flags & StaticType::NODE_TYPE) {
    toBeCasted = toBeCasted.atomize(context);
  }

  const Item::Ptr first = toBeCasted.next(context);

	if(first == NULLRCP) {
    //    3. If the result of atomization is an empty sequence:
    //       1. If ? is specified after the target type, the result of the cast expression is an empty sequence.
    //       2. If ? is not specified after the target type, a type error is raised [err:XPTY0004].
    if(_di->getSequenceType()->getOccurrenceIndicator() == SequenceType::EXACTLY_ONE) {
      DSLthrow(TypeErrorException,X("DataItemCastAs::CastAsResult::getSingleResult"),X("The input to a non-optional cast as expression is an empty sequence [err:XPTY0004]"));
    }
    else {
      return 0;
    }
  }

  const Item::Ptr second = toBeCasted.next(context);

  //    2. If the result of atomization is a sequence of more than one atomic value, a type error is raised.[err:XPTY0004]
  if(second != NULLRCP) {
    DSLthrow(TypeErrorException,X("DataItemCastAs::CastAsResult::getSingleResult"),X("The input to a cast as expression is more than one atomic value [err:XPTY0004]"));
  }

  if(_di->getSequenceType()->getItemTestType() != SequenceType::ItemType::TEST_ATOMIC_TYPE)
    DSLthrow(TypeErrorException,X("DataItemCastAs::CastAsResult::getSingleResult"),X("Cannot cast to a non atomic type"));
  //    4. If the result of atomization is a single atomic value, the result of the cast expression depends on the input type and the target type.
  //       The normative definition of these rules is given in [XQuery 1.0 and XPath 2.0 Functions and Operators].
  return (const Item::Ptr)((const AnyAtomicType::Ptr)first)->castAs(_di->getSequenceType()->getTypeURI(context), _di->getSequenceType()->getConstrainingType()->getName(), context);
}

std::string DataItemCastAs::CastAsResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<castas/>" << std::endl;

  return oss.str();

}
