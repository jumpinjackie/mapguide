/*
 * Copyright (c) 2001-2008
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2008
 *     Oracle. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Id$
 */

#include "../../config/xqilla_config.h"
#include <xqilla/items/impl/ATGMonthOrDerivedImpl.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/mapm/m_apm.h>
#include <xqilla/items/Timezone.hpp>
#include "../../utils/DateUtils.hpp"
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

#include <xercesc/util/XMLString.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

ATGMonthOrDerivedImpl::
ATGMonthOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context): 
    ATGMonthOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
     setGMonth(value);
}

void *ATGMonthOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gXQilla) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATGMonthOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATGMonthOrDerivedImpl::getPrimitiveName()  {
  return SchemaSymbols::fgDT_MONTH;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATGMonthOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATGMonthOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATGMonthOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::G_MONTH;
} 

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATGMonthOrDerivedImpl::asString(const DynamicContext* context) const {
  /* --MM format*/
  
  XMLBuffer buffer(1023, context->getMemoryManager());
  buffer.append(chDash);
  buffer.append(chDash);
  DateUtils::formatNumber(_gMonth, 2, buffer);
  if(_hasTimezone) {
    buffer.append(timezone_->asString(context));
  }
  return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
}

static inline MAPM referenceDateTime(const MAPM &MM, bool hasTimezone, const Timezone::Ptr &timezone)
{
  MAPM result = DateUtils::convertDMY2Absolute(DateUtils::maximumDayInMonthFor(1972, MM), MM, 1972) * DateUtils::g_secondsPerDay;

  if(hasTimezone) {
    result -= timezone->asSeconds();
  }

  return result;
}

MAPM ATGMonthOrDerivedImpl::buildReferenceDateTime(const DynamicContext *context) const
{
  MAPM result = referenceDateTime(_gMonth, _hasTimezone, timezone_);

  if(!_hasTimezone)
    result -= context->getImplicitTimezone()->asSeconds(context)->asMAPM();

  return result;
}

/* Returns true if and only if the xs:dateTimes representing the starting instants of equivalent occurrences of $arg1 and $arg2 
 * compare equal. The starting instants of equivalent occurrences of $arg1 and $arg2 are calculated by adding the missing components 
 * of $arg1 and $arg2 from an xs:dateTime template such as 1972-xx-ddT00:00:00 where dd represents the last day of the month 
 * component in $arg1 or $arg2. Returns false otherwise.
 */
bool ATGMonthOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
        XQThrow2(::IllegalArgumentException,X("ATGMonthOrDerivedImpl::equals"), X("Equality operator for given types not supported [err:XPTY0004]"));
  }
  return compare((const ATGMonthOrDerived *)target.get(), context) == 0;
}

int ATGMonthOrDerivedImpl::compare(const ATGMonthOrDerived::Ptr &other, const DynamicContext *context) const
{
  return buildReferenceDateTime(context).compare(((const ATGMonthOrDerivedImpl *)other.get())->buildReferenceDateTime(context));
}

/** Returns true if a timezone is defined for this.  False otherwise.*/
bool ATGMonthOrDerivedImpl::hasTimezone() const {
  return _hasTimezone;
}

/** Sets the timezone to the given timezone.*/
ATGMonthOrDerived::Ptr ATGMonthOrDerivedImpl::setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const {
  XMLBuffer buffer(1023, context->getMemoryManager());

  buffer.append(chDash);
  buffer.append(chDash);
  DateUtils::formatNumber(_gMonth, 2, buffer);
  if(timezone != NULLRCP) 
    buffer.append(timezone->asString(context));
  return context->getItemFactory()->createGMonthOrDerived(this->getTypeURI(), this->getTypeName(), buffer.getRawBuffer(), context);
}


AnyAtomicType::AtomicObjectType ATGMonthOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}

/* parse the gMonth */
void ATGMonthOrDerivedImpl::setGMonth(const XMLCh* const value) {
 
   unsigned int length = XPath2Utils::uintStrlen(value);
 
  if(value == NULL) {
    XQThrow2(XPath2TypeCastException,X("XSGMonthImpl::setGMonth"), X("Invalid representation of gMonth [err:FORG0001]"));
  }
	
	// State variables etc.

	bool gotDigit = false;
	unsigned int pos = 0;
	long int tmpnum = 0;
	unsigned int numDigit = 0;

	// defaulting values
	MAPM MM = 0;
	_hasTimezone = false;
	bool zonepos = false;
	int zonehh = 0;
	int zonemm = 0;

	int state = 0 ; // 0 = year / 1 = month / 2 = day / 3 = hour 
	                 // 4 = minutes / 5 = sec / 6 = timezonehour / 7 = timezonemin
	XMLCh tmpChar;
	
	bool wrongformat = false;

	if ( length  < 4 || value[0] != L'-' || value[1] != L'-' ) {
		wrongformat = true;
	}else{
		pos = 2;
		state = 1;
	} 
		
  while ( ! wrongformat && pos < length) {
    tmpChar = value[pos];
    pos++;
      switch(tmpChar) {
      case 0x0030:
      case 0x0031:
      case 0x0032:
      case 0x0033:
      case 0x0034:
      case 0x0035:
      case 0x0036:
      case 0x0037:
      case 0x0038:
      case 0x0039: {
        tmpnum *= 10;
        tmpnum +=  static_cast<int>(tmpChar - 0x0030);
        gotDigit = true;
        numDigit ++;
        break;
	}

      case L'-' : {
        if ( gotDigit && state == 1 && numDigit == 2) {
          MM = tmpnum;		
          tmpnum = 0;
          gotDigit = false;
          state = 6;
          numDigit = 0;
          _hasTimezone = true;
          zonepos = false;				
        } else {
          wrongformat = true;
        }
        break;			
      }
      case L'+' : {
        if ( gotDigit && state == 1 && numDigit == 2) {
          MM = tmpnum;                      
          tmpnum = 0;
          gotDigit = false;
          state = 6;
          numDigit = 0;
          _hasTimezone = true;
          zonepos = true;        
        } else {
          wrongformat = true;
        }
    	break;
      }
		case L':' : {
			if (gotDigit &&  state == 6 && numDigit == 2) {
				zonehh = tmpnum;
				tmpnum = 0;
				gotDigit = false;				
				state ++;
				numDigit = 0;
	            state = 7;
			}else {
				wrongformat = true;
			}
			break;
		}
		case L'Z' : {
            if ( gotDigit && state == 1 && numDigit == 2) {
				state = 8; // final state
                MM = tmpnum;                      
				_hasTimezone = true;
                gotDigit = false;
                tmpnum = 0;
                numDigit = 0;
			} else {
				wrongformat = true;
			}
			break;
		}
		default:
			wrongformat = true;
		}	
	}

	if (gotDigit) {
        if ( state == 1 && numDigit == 2)
            MM = tmpnum;                      
		else if( state == 7 && numDigit == 2)
			zonemm = tmpnum;
		else
			wrongformat = true;
	} 
	
	// check time format

	if ( MM > 12 || zonehh > 24 || zonemm > 60 ) {
			wrongformat = true;
	}

	if ( wrongformat) {
      XQThrow2(XPath2TypeCastException,X("ATGMonthOrDerivedImpl::setGMonth"), X("Invalid representation of gMonth [err:FORG0001]"));
	}

  _gMonth = MM;
  timezone_ = new Timezone(Timezone::convert(zonepos, zonehh, zonemm));
  
}

MAPM ATGMonthOrDerivedImpl::parseGMonth(const XMLCh* const value, const MAPM &implicitTimezone)
{
  ATGMonthOrDerivedImpl dt(0, 0, value);

  MAPM result = referenceDateTime(dt._gMonth, dt._hasTimezone, dt.timezone_);

  if(!dt._hasTimezone)
    result -= implicitTimezone;

  return result;
}
