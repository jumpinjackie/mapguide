//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Syntax.hpp,v 1.20 2005/11/22 18:09:02 gmf Exp $
//

#ifndef __SYNTAX_HPP
#define	__SYNTAX_HPP

#include "db_cxx.h"
#include "DbWrapper.hpp"
#include "KeyGenerator.hpp"

#include <xercesc/validators/datatype/DatatypeValidator.hpp>

namespace DbXml
{

class Index;
class Buffer;

class Syntax
{
public:
	virtual ~Syntax() {}
	enum Type
	{
		NONE,
		STRING,
		ANY_URI,
		BASE_64_BINARY,
		BOOLEAN,
		DATE,
		DATE_TIME,
		DAY,
		DECIMAL,
		DOUBLE,
		DURATION,
		FLOAT,
		HEX_BINARY,
		MONTH,
		MONTH_DAY,
		NOTATION,
		QNAME,
		TIME,
		YEAR,
		YEAR_MONTH
	};
	
	virtual Type getType() const = 0;
	virtual const char *getName() const = 0;
	virtual bool test(const char *v, size_t len) const = 0;
	virtual int bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2) const = 0;
	virtual DbWrapper::bt_compare_fn get_bt_compare() const = 0;
	virtual KeyGenerator::Ptr getKeyGenerator(
		const Index &index, const char *valueBuffer,
		size_t valueLength) const = 0;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l) const = 0;
	virtual void unmarshal(Buffer *buffer, const char *p, size_t l) const = 0;
};

class NoneSyntax : public Syntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual bool test(const char *v, size_t len) const;
	virtual int bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2) const;
	virtual DbWrapper::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
	virtual KeyGenerator::Ptr getKeyGenerator(const Index &index,
						  const char *valueBuffer,
						  size_t valueLength) const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l) const;
	virtual void unmarshal(Buffer *buffer, const char *p, size_t l) const;
};

// StringSyntax derives directly from Syntax due to the special case of string
// indices
class StringSyntax : public Syntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual bool test(const char *v, size_t len) const;
	virtual int bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2) const;
	virtual DbWrapper::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
	virtual KeyGenerator::Ptr getKeyGenerator(
		const Index &index, const char *valueBuffer,
		size_t valueLength) const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l) const;
	virtual void unmarshal(Buffer *buffer, const char *p, size_t l) const;
};

// Common functionality for all Syntaxes that correspond to non-string basic
// types
class SimpleTypeSyntax : public Syntax
{
public:
	/// Returns the DatatypeValidator for the syntax type
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator
	      *getDatatypeValidator() const = 0;

	virtual bool test(const char *v, size_t len) const;
	virtual int bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2) const;
	virtual KeyGenerator::Ptr getKeyGenerator(const Index &index,
						  const char *valueBuffer,
						  size_t valueLength) const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l) const;
	virtual void unmarshal(Buffer *buffer, const char *p, size_t l) const;
};

class AnyURISyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator
	     *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class Base64BinarySyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class BooleanSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class DateSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};
	
class DateTimeSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class DaySyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class DecimalSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class DoubleSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class DurationSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class FloatSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class HexBinarySyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class MonthSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class MonthDaySyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class NotationSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class QNameSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class TimeSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class YearSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

class YearMonthSyntax : public SimpleTypeSyntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *getDatatypeValidator() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	static int static_bt_compare(Db *db, const Dbt *dbt1, const Dbt *dbt2);
};

}

#endif
