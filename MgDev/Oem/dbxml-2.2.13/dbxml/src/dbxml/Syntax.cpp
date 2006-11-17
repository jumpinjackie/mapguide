//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Syntax.cpp,v 1.44 2005/08/29 23:27:06 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include <iostream>
#include "IndexSpecification.hpp"
#include "UTF8.hpp"
#include "Value.hpp"
#include "Syntax.hpp"
#include "Globals.hpp"
#include "Key.hpp"
#include "db_utils.h"

#if defined(DBXML_DOM_XERCES2)
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/validators/datatype/DatatypeValidatorFactory.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif
#endif

#include <cassert>

using namespace DbXml;
using namespace std;


///////////////////////////////////////////////////////////////////////////
// NoneSyntax

Syntax::Type NoneSyntax::getType() const
{
	return Syntax::NONE;
}

const char *NoneSyntax::getName() const
{
	return "none";
}

bool NoneSyntax::test(const char *v, size_t len) const
{
	UNUSED(v);
	UNUSED(len);

	return true;
}

int NoneSyntax::bt_compare(Db *db, const Dbt *a, const Dbt *b) const
{
	UNUSED(db);

	size_t len = a->get_size() > b->get_size() ? b->get_size() : a->get_size();
	char *p1 = (char*)a->get_data();
	char *p2 = (char*)b->get_data();
	for (; len--; ++p1, ++p2)
		if (*p1 != *p2)
			return ((long)*p1 - (long)*p2);
	return ((long)a->get_size() - (long)b->get_size());
}

DbWrapper::bt_compare_fn NoneSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int NoneSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static NoneSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

KeyGenerator::Ptr NoneSyntax::getKeyGenerator(const Index &index, const char *valueBuffer, size_t valueLength) const
{
	DBXML_ASSERT(index.getKey()==Index::KEY_PRESENCE);
	return KeyGenerator::Ptr(new SingleKeyGenerator(valueBuffer, valueLength));
}

size_t NoneSyntax::marshal(Buffer *buffer, const char *p, size_t l) const
{
	return buffer->write(p, l);
}

void NoneSyntax::unmarshal(Buffer *buffer, const char *p, size_t l) const
{
	buffer->write(p, l);
}

///////////////////////////////////////////////////////////////////////////
// StringSyntax

Syntax::Type StringSyntax::getType() const
{
	return Syntax::STRING;
}

const char *StringSyntax::getName() const
{
	return "string";
}

bool StringSyntax::test(const char *v, size_t len) const
{
	UNUSED(v);
	UNUSED(len);

	return true;
}

int StringSyntax::bt_compare(Db *db, const Dbt *a, const Dbt *b) const
{
	UNUSED(db);

	size_t len = a->get_size() > b->get_size() ? b->get_size() : a->get_size();
	char *p1 = (char*)a->get_data();
	char *p2 = (char*)b->get_data();
	for (; len--; ++p1, ++p2)
		if (*p1 != *p2)
			return ((long)*p1 - (long)*p2);
	return ((long)a->get_size() - (long)b->get_size());
}

DbWrapper::bt_compare_fn StringSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int StringSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static StringSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

KeyGenerator::Ptr StringSyntax::getKeyGenerator(const Index &index, const char *valueBuffer, size_t valueLength) const
{
	KeyGenerator::Ptr r;
	switch (index.getKey()) {
	case Index::KEY_PRESENCE:
	case Index::KEY_EQUALITY:
		r.reset(new SingleKeyGenerator(valueBuffer, valueLength));
		break;
	case Index::KEY_SUBSTRING:
		// This is a special case. We don't need to call
		// test(), since it always returns true.
		r.reset(new SubstringKeyGenerator(valueBuffer, valueLength));
		break;
	default:
		DBXML_ASSERT(0);
		break;
	}
	return r;
}

size_t StringSyntax::marshal(Buffer *buffer, const char *p, size_t l) const
{
	return buffer->write(p, l);
}

void StringSyntax::unmarshal(Buffer *buffer, const char *p, size_t l) const
{
	buffer->write(p, l);
}

///////////////////////////////////////////////////////////////////////////
// SimpleTypeSyntax

bool SimpleTypeSyntax::test(const char *v, size_t len) const
{
	bool ret = false;

	DatatypeValidator *dtv = getDatatypeValidator();
	string str(v, len);
	try {
		dtv->validate(UTF8ToXMLCh(str).str());
		ret = true;
	}
	catch(XMLException &e) {
		// InvalidDatatype[Value|Facet]Exception - test fails
	}

	return ret;
}

int SimpleTypeSyntax::bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2) const
{
	UNUSED(db);

	const xmlbyte_t *p1 = (const xmlbyte_t*)dbt1->get_data();
	const xmlbyte_t *p2 = (const xmlbyte_t*)dbt2->get_data();

	// Calculate the end of the two keys
	const xmlbyte_t *e1 = p1 + dbt1->get_size();
	const xmlbyte_t *e2 = p2 + dbt2->get_size();

	// Compare the structure part of the key
	int res = Key::compareStructure(p1, e1, p2, e2);
	if(res != 0) return res;

	// Check to see if we've come to the end of a key
	if(p1 >= e1) {
		if(p2 >= e2) return 0;
		return -1;
	}
	if(p2 >= e2) return +1;

	// Compare the value component of the keys,
	// TBD: implement fixed-size stack buffers to avoid
	// new/delete for the UTF8* transcoding for
	// the majority of key comparisons
	DatatypeValidator *dtv = getDatatypeValidator();
	UTF8ToXMLCh b1((const char*)p1, e1-p1);
	UTF8ToXMLCh b2((const char*)p2, e2-p2);
	return dtv->compare(b1.str(), b2.str());
}

KeyGenerator::Ptr SimpleTypeSyntax::getKeyGenerator(const Index &index, const char *valueBuffer, size_t valueLength) const
{
	if(test(valueBuffer, valueLength)) {
		return KeyGenerator::Ptr(new SingleKeyGenerator(valueBuffer, valueLength));
	}
	else {
		return KeyGenerator::Ptr(new EmptyKeyGenerator());
	}
}

size_t SimpleTypeSyntax::marshal(Buffer *buffer, const char *p, size_t l) const
{
	if(p == 0 || l == 0) {
		return 0;
	}
	const char x = 0;
	buffer->write(p, l);
	buffer->write(&x, 1);
	return l + 1;
}

void SimpleTypeSyntax::unmarshal(Buffer *buffer, const char *p, size_t l) const
{
	if(p != 0 && l != 0) {
		buffer->write(p, l);
	}
}

///////////////////////////////////////////////////////////////////////////
// AnyURISyntax

Syntax::Type AnyURISyntax::getType() const
{
	return Syntax::ANY_URI;
}

const char *AnyURISyntax::getName() const
{
	return "anyURI";
}

DatatypeValidator *AnyURISyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_ANYURI);
}

DbWrapper::bt_compare_fn AnyURISyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int AnyURISyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static AnyURISyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// Base64BinarySyntax

Syntax::Type Base64BinarySyntax::getType() const
{
	return Syntax::BASE_64_BINARY;
}

const char *Base64BinarySyntax::getName() const
{
	return "base64Binary";
}

DatatypeValidator *Base64BinarySyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_BASE64BINARY);
}

DbWrapper::bt_compare_fn Base64BinarySyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int Base64BinarySyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static Base64BinarySyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// BooleanSyntax

Syntax::Type BooleanSyntax::getType() const
{
	return Syntax::BOOLEAN;
}

const char *BooleanSyntax::getName() const
{
	return "boolean";
}

DatatypeValidator *BooleanSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_BOOLEAN);
}

DbWrapper::bt_compare_fn BooleanSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int BooleanSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static BooleanSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// DateSyntax

Syntax::Type DateSyntax::getType() const
{
	return Syntax::DATE;
}

const char *DateSyntax::getName() const
{
	return "date";
}

DatatypeValidator *DateSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_DATE);
}

DbWrapper::bt_compare_fn DateSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int DateSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static DateSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// DateTimeSyntax

Syntax::Type DateTimeSyntax::getType() const
{
	return Syntax::DATE_TIME;
}

const char *DateTimeSyntax::getName() const
{
	return "dateTime";
}

DatatypeValidator *DateTimeSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_DATETIME);
}

DbWrapper::bt_compare_fn DateTimeSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int DateTimeSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static DateTimeSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// DaySyntax

Syntax::Type DaySyntax::getType() const
{
	return Syntax::DAY;
}

const char *DaySyntax::getName() const
{
	return "day";
}

DatatypeValidator *DaySyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_DAY);
}

DbWrapper::bt_compare_fn DaySyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int DaySyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static DaySyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// DecimalSyntax

Syntax::Type DecimalSyntax::getType() const
{
	return Syntax::DECIMAL;
}

const char *DecimalSyntax::getName() const
{
	return "decimal";
}

DatatypeValidator *DecimalSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_DECIMAL);
}

DbWrapper::bt_compare_fn DecimalSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int DecimalSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static DecimalSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// DoubleTypeSyntax

Syntax::Type DoubleSyntax::getType() const
{
	return Syntax::DOUBLE;
}

const char *DoubleSyntax::getName() const
{
	return "double";
}

DatatypeValidator *DoubleSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_DOUBLE);
}

DbWrapper::bt_compare_fn DoubleSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int DoubleSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static DoubleSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// DurationSyntax

Syntax::Type DurationSyntax::getType() const
{
	return Syntax::DURATION;
}

const char *DurationSyntax::getName() const
{
	return "duration";
}

DatatypeValidator *DurationSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_DURATION);
}

DbWrapper::bt_compare_fn DurationSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int DurationSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static DurationSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// FloatSyntax

Syntax::Type FloatSyntax::getType() const
{
	return Syntax::FLOAT;
}

const char *FloatSyntax::getName() const
{
	return "float";
}

DatatypeValidator *FloatSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_FLOAT);
}

DbWrapper::bt_compare_fn FloatSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int FloatSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static FloatSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// HexBinarySyntax

Syntax::Type HexBinarySyntax::getType() const
{
	return Syntax::HEX_BINARY;
}

const char *HexBinarySyntax::getName() const
{
	return "hexBinary";
}

DatatypeValidator *HexBinarySyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_HEXBINARY);
}

DbWrapper::bt_compare_fn HexBinarySyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int HexBinarySyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static HexBinarySyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// MonthSyntax

Syntax::Type MonthSyntax::getType() const
{
	return Syntax::MONTH;
}

const char *MonthSyntax::getName() const
{
	return "month";
}

DatatypeValidator *MonthSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_MONTH);
}

DbWrapper::bt_compare_fn MonthSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int MonthSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static MonthSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// MonthDaySyntax

Syntax::Type MonthDaySyntax::getType() const
{
	return Syntax::MONTH_DAY;
}

const char *MonthDaySyntax::getName() const
{
	return "monthDay";
}

DatatypeValidator *MonthDaySyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_MONTHDAY);
}

DbWrapper::bt_compare_fn MonthDaySyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int MonthDaySyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static MonthDaySyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// NotationSyntax

Syntax::Type NotationSyntax::getType() const
{
	return Syntax::NOTATION;
}

const char *NotationSyntax::getName() const
{
	return "NOTATION";
}

DatatypeValidator *NotationSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(XMLUni::fgNotationString);
}

DbWrapper::bt_compare_fn NotationSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int NotationSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static NotationSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// QNameSyntax

Syntax::Type QNameSyntax::getType() const
{
	return Syntax::QNAME;
}

const char *QNameSyntax::getName() const
{
	return "QName";
}

DatatypeValidator *QNameSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_QNAME);
}

DbWrapper::bt_compare_fn QNameSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int QNameSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static QNameSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// TimeSyntax

Syntax::Type TimeSyntax::getType() const
{
	return Syntax::TIME;
}

const char *TimeSyntax::getName() const
{
	return "time";
}

DatatypeValidator *TimeSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_TIME);
}

DbWrapper::bt_compare_fn TimeSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int TimeSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static TimeSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// YearSyntax

Syntax::Type YearSyntax::getType() const
{
	return Syntax::YEAR;
}

const char *YearSyntax::getName() const
{
	return "year";
}

DatatypeValidator *YearSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_YEAR);
}

DbWrapper::bt_compare_fn YearSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int YearSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static YearSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}

///////////////////////////////////////////////////////////////////////////
// YearMonthSyntax

Syntax::Type YearMonthSyntax::getType() const
{
	return Syntax::YEAR_MONTH;
}

const char *YearMonthSyntax::getName() const
{
	return "yearMonth";
}

DatatypeValidator *YearMonthSyntax::getDatatypeValidator() const
{
	return DatatypeValidatorFactory::getBuiltInRegistry()->get(SchemaSymbols::fgDT_YEARMONTH);
}

DbWrapper::bt_compare_fn YearMonthSyntax::get_bt_compare() const
{
	return static_bt_compare;
}

int YearMonthSyntax::static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2)
{
	static YearMonthSyntax syntax;
	return syntax.bt_compare(db, dbt1, dbt2);
}
