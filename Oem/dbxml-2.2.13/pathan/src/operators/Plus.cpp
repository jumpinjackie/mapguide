/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/operators/Plus.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/ATDurationOrDerived.hpp> 
#include <pathan/ATDateTimeOrDerived.hpp> 
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATTimeOrDerived.hpp> 
#include <pathan/Numeric.hpp>
#include <pathan/DynamicContext.hpp>

/*static*/ const XMLCh Plus::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Plus::Plus(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ArithmeticOperator(name, args, memMgr)
{
  // Nothing to do
}

Item::Ptr Plus::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  return plus(atom1, atom2, context);
}

Item::Ptr Plus::plus(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context)
{
  if(atom1 == NULLRCP || atom2 == NULLRCP) return 0;

  if(atom1->isNumericValue()) {
    if(atom2->isNumericValue()) {
      return (const Item::Ptr)((Numeric*)(const AnyAtomicType*)atom1)->add((const Numeric::Ptr )atom2, context);
    }
    else {
      DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An attempt to add a non numeric type to a numeric type has occurred [err:XPTY0004]"));
		}
	}

  switch(atom1->getPrimitiveTypeIndex()) {
    case AnyAtomicType::DATE : {
      switch(atom2->getPrimitiveTypeIndex()) {
        case AnyAtomicType::DURATION : {
          const ATDurationOrDerived* duration = (const ATDurationOrDerived*)(const AnyAtomicType*)atom2;
          if(duration->isYearMonthDuration()) {
            return (const Item::Ptr)((ATDateOrDerived*)(const AnyAtomicType*)atom1)->addYearMonthDuration(duration, context);
          }
          else if(duration->isDayTimeDuration()) {
            return (const Item::Ptr)((ATDateOrDerived*)(const AnyAtomicType*)atom1)->addDayTimeDuration(duration, context);
          } else {
            DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:date type has occurred [err:XPTY0004]"));
          }
        }
        default: {
          DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:date type has occurred [err:XPTY0004]"));
        }
      }// switch
    }
    case AnyAtomicType::TIME : {
      switch(atom2->getPrimitiveTypeIndex()) {
        case AnyAtomicType::DURATION : {
          // assume xdt:dayTimeDuration, otherwise function will throw
          return (const Item::Ptr)((ATTimeOrDerived*)(const AnyAtomicType*)atom1)->addDayTimeDuration( (const ATDurationOrDerived::Ptr )atom2, context );
        }
        default: {
          DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:time type has occurred [err:XPTY0004]"));
        }
      }// switch
    }
    case AnyAtomicType::DATE_TIME : {
      switch(atom2->getPrimitiveTypeIndex()) {
        case AnyAtomicType::DURATION : {
          const ATDurationOrDerived* duration = (const ATDurationOrDerived* )(const AnyAtomicType*)atom2;
          if(duration->isYearMonthDuration()) {
            return (const Item::Ptr)((ATDateTimeOrDerived*)(const AnyAtomicType*)atom1)->addYearMonthDuration(duration, context);
          }
          else if(duration->isDayTimeDuration()) {
            return (const Item::Ptr)((ATDateTimeOrDerived*)(const AnyAtomicType*)atom1)->addDayTimeDuration(duration, context);
          } else {
            DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:dateTime type has occurred [err:XPTY0004]"));
          }
        }
       default: {
          DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:dateTime type has occurred [err:XPTY0004]"));
        }

      }// switch
    }
    case AnyAtomicType::DURATION : {
      const ATDurationOrDerived* duration = (const ATDurationOrDerived* )(const AnyAtomicType*)atom1;
      switch(atom2->getPrimitiveTypeIndex()) {
        case AnyAtomicType::DURATION : {
          // this call will throw an error if the duration is of the wrong type
          return (const Item::Ptr)duration->add((const ATDurationOrDerived::Ptr )atom2, context);
        }
        case AnyAtomicType::DATE: {
          if(duration->isYearMonthDuration()) {
            return (const Item::Ptr)((ATDateOrDerived*)(const AnyAtomicType*)atom2)->addYearMonthDuration(duration, context);
          } else if (duration->isDayTimeDuration()) {
            return (const Item::Ptr)((ATDateOrDerived*)(const AnyAtomicType*)atom2)->addDayTimeDuration(duration, context);
          } else {
            DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:duration type has occurred [err:XPTY0004]"));
          }
        }
        case AnyAtomicType::DATE_TIME: {
          if(duration->isYearMonthDuration()) {
            return (const Item::Ptr)((ATDateTimeOrDerived*)(const AnyAtomicType*)atom2)->addYearMonthDuration(duration, context);
          } else if (duration->isDayTimeDuration()) {
            return (const Item::Ptr)((ATDateTimeOrDerived*)(const AnyAtomicType*)atom2)->addDayTimeDuration(duration, context);
          } else {
            DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:duration type has occurred [err:XPTY0004]"));
          }                                         
        }
        case AnyAtomicType::TIME: {
          // assume xdt:dayTimeDuration, otherwise will throw
          return (const Item::Ptr)((ATTimeOrDerived*)(const AnyAtomicType*)atom2)->addDayTimeDuration(duration, context);
        }
        default: {
          DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:duration type has occurred [err:XPTY0004]"));
        }
      }// switch
    }
    default: {
      DSLthrow(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("The operator add ('+') has been called on invalid operand types [err:XPTY0004]"));
    }
  }// switch
}

