//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlContext.cpp,v 1.15 2006/10/30 17:45:55 bostic Exp $
//

#include <stdlib.h>

#include "DbXmlContext.hpp"
#include "../UTF8.hpp"
#include "../Manager.hpp"
#include <sstream>

#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/utils/UTF8Str.hpp>
#include <xqilla/items/Timezone.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DbXmlContext::DbXmlContext(QueryContext &qc, Transaction *txn)
	: qc_(&qc),
	  resolver_(qc.getManager(), txn),
	  runQPs_(true),
	  contextItem_(0),
	  contextPosition_(0),
	  contextSize_(0)
{
}

DbXmlContext::~DbXmlContext()
{
	contextItem_ = 0;
}

ATDurationOrDerived::Ptr DbXmlContext::getImplicitTimezone() const
{
	MAPM tz = ((Manager&)qc_->getManager()).getImplicitTimezone();
	Timezone::validate(tz);
	return getItemFactory()->createDayTimeDuration(tz, this);
}

void DbXmlContext::setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration)
{
	DBXML_ASSERT(false);
}

Item::Ptr DbXmlContext::getContextItem() const
{
	return contextItem_;
}

void DbXmlContext::setContextItem(const Item::Ptr &item)
{
	contextItem_ = item;
}

void DbXmlContext::setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	DBXML_ASSERT(false);
}

size_t DbXmlContext::getContextPosition() const
{
	return contextPosition_;
}

void DbXmlContext::setContextPosition(size_t pos)
{
	contextPosition_ = pos;
}

size_t DbXmlContext::getContextSize() const
{
	return contextSize_;
}

void DbXmlContext::setContextSize(size_t size)
{
	contextSize_ = size;
}

void DbXmlContext::trace(const XMLCh* message1, const XMLCh* message2) {
	XMLChToUTF8 m1(message1);
	XMLChToUTF8 m2(message2);
	ostringstream oss;
	oss << m1.str() << " " << m2.str();
	QueryContext &qc = getQueryContext();
	((Manager &)qc.getManager()).log(Log::C_QUERY, Log::L_INFO,
					 oss.str());
}

VectorOfStrings *DbXmlContext::resolveModuleURI(const XMLCh* uri) const
{
	VectorOfStrings* vect = new (getMemoryManager())
		VectorOfStrings(XQillaAllocator<const XMLCh*>(getMemoryManager()));
	moduleResolver_->resolveModuleLocation(vect, uri, this);
	return vect;
}

