//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Value.cpp,v 1.100 2005/09/16 19:44:09 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include <dbxml/XmlResults.hpp>
#include "Value.hpp"
#include "Results.hpp"
#include "QueryContext.hpp"
#include "TypeConversions.hpp"
#include "UTF8.hpp"
#include "SyntaxManager.hpp"
#include "Document.hpp"
#include "dataItem/DbXmlContextImpl.hpp"
#include "dataItem/DbXmlNodeImpl.hpp"
#include "dataItem/DbXmlFactoryImpl.hpp"
#include "Globals.hpp"
#include "ReferenceMinder.hpp"
#include "nodeStore/NsWriter.hpp"
#include "nodeStore/NsDomEventSource.hpp"
#include "nodeStore/NsXercesDom.hpp"

#include <sstream>
#include <float.h>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/PathanFactory.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/validators/datatype/DatatypeValidatorFactory.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

#include <math.h>

using namespace std;
using namespace DbXml;

const std::string Value::NaN_string("NaN");
const std::string Value::PositiveInfinity_string("INF");
const std::string Value::NegativeInfinity_string("-INF");

Value *Value::create(const AnyAtomicType::Ptr &atom, DynamicContext *context)
{
	// arw - not sure if Pathan will throw here
	try {
		std::string value(XMLChToUTF8(atom->asString(context)).str());
		std::string primitiveTypeName(
			XMLChToUTF8(atom->getPrimitiveTypeName()).str());
		return new AtomicTypeValue(primitiveTypeName, value);
	}
	catch(...) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Unable to store value returned from evaluation of XPath query.");
	}
}

Value *Value::create(const Node::Ptr &item, QueryContext &qc)
{
	const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)item->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(nodeImpl != 0);

	const XmlDocument &document = nodeImpl->getXmlDocument();

	const DOMNode *node = nodeImpl->getDOMNode();
	if(qc.getReturnType()==XmlQueryContext::DeadValues) {
		// return 'dead' nodes.
		if(node->getNodeType() == DOMNode::DOCUMENT_NODE ||
		   node->getNodeType() == DOMNode::DOCUMENT_TYPE_NODE) {
			// don't bother creating a copy of the node, as that
			// involves copying the entire tree...
			return new NodeValue(const_cast<DOMNode*>(node),
					     document);
		} else {
			// Make a fake temporary XmlDocument, containing a
			// DOMDocument that we will create our dead node on.
			XmlDocument tmpDocument =
				qc.getManager().createDocument();
			return new NodeValue(
				tmpDocument.getContentAsDOM()->
				importNode(const_cast<DOMNode*>(node),
					   /*deep*/true), tmpDocument);
		}
	} else {
		// return 'live' nodes.
		return new NodeValue(const_cast<DOMNode*>(node), document);
	}
}

Value *Value::create(XmlValue::Type type, const std::string &v, bool validate)
{
	switch (type){
	case XmlValue::NONE:
	case XmlValue::NODE:
		break;
	case XmlValue::ANY_URI:
	case XmlValue::BASE_64_BINARY:
	case XmlValue::BOOLEAN:
	case XmlValue::DATE:
	case XmlValue::DATE_TIME:
	case XmlValue::DAY_TIME_DURATION:
	case XmlValue::DECIMAL:
	case XmlValue::DOUBLE:
	case XmlValue::DURATION:
	case XmlValue::FLOAT:
	case XmlValue::G_DAY:
	case XmlValue::G_MONTH:
	case XmlValue::G_MONTH_DAY:
	case XmlValue::G_YEAR:
	case XmlValue::G_YEAR_MONTH:
	case XmlValue::HEX_BINARY:
	case XmlValue::NOTATION:
	case XmlValue::QNAME:
	case XmlValue::STRING:
	case XmlValue::TIME:
	case XmlValue::YEAR_MONTH_DURATION:
	case XmlValue::UNTYPED_ATOMIC: {
		Value *value = new AtomicTypeValue(type, v);
		if (validate) {
			try {
				value->validate(
					Globals::getDatatypeValidatorFactory());
			} catch (...) {
				delete value;
				throw; // re-throw
			}
		}
		return value;
		break;
	}
	case XmlValue::BINARY: {
		return new BinaryValue(v);
	}
	default: break;
	}
	return 0;
}

Value *Value::create(XmlValue::Type type, const Dbt &dbt, bool validate)
{
	if (type != XmlValue::BINARY)
		return Value::create(type,
				     std::string((const char *)dbt.get_data()),
				     validate);
	return new BinaryValue(dbt);
}

Item::Ptr Value::convertToItem(const Value *v, DynamicContext *context)
{
	if(v != 0) {
		switch (v->getType()) {
		case XmlValue::NONE: {
			break;
		}
		case XmlValue::NODE: {
			CAST_TO_DBXMLCONTEXT(
				context)->getQueryContext().getMinder()->
				addDocument(v->asDocument());
			return ((DbXmlFactoryImpl*)context->getPathanFactory())->createNode(((const NodeValue*)v)->getDOMNode(), v->asDocument(), context);
		}
		case XmlValue::ANY_URI:
		case XmlValue::BASE_64_BINARY:
		case XmlValue::BOOLEAN:
		case XmlValue::DATE:
		case XmlValue::DATE_TIME:
		case XmlValue::DAY_TIME_DURATION:
		case XmlValue::DECIMAL:
		case XmlValue::DOUBLE:
		case XmlValue::DURATION:
		case XmlValue::FLOAT:
		case XmlValue::G_DAY:
		case XmlValue::G_MONTH:
		case XmlValue::G_MONTH_DAY:
		case XmlValue::G_YEAR:
		case XmlValue::G_YEAR_MONTH:
		case XmlValue::HEX_BINARY:
		case XmlValue::NOTATION:
		case XmlValue::QNAME:
		case XmlValue::STRING:
		case XmlValue::TIME:
		case XmlValue::YEAR_MONTH_DURATION:
		case XmlValue::UNTYPED_ATOMIC: {
			// create a simple type atom
			const XMLCh *typeName =
				AtomicTypeValue::convertToPrimitiveTypeName(
					v->getType());

			return context->getPathanFactory()->
				createDerivedFromAtomicType(
					SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
					typeName,
					UTF8ToXMLCh(v->asString()).str(),
					context);
		}
		default: break;
		}
	}
	return 0;
}

const XmlDocument &Value::asDocument() const
{
	throw XmlException(
		XmlException::INVALID_VALUE, "Can't convert XmlValue to Document");
}

// NodeValue

NodeValue::NodeValue(const XmlDocument &d)
	: Value(XmlValue::NODE),
	  n_(0),
	  d_(d)
{
}

NodeValue::NodeValue(DOMNode *n, const XmlDocument &d)
	: Value(XmlValue::NODE),
	  n_(n),
	  d_(d)
{
}

NodeValue::~NodeValue()
{
}

double NodeValue::asNumber() const
{
	return strtod(asString().c_str(), 0);
}

static const XMLCh XMLChUTF8[] = {
	chLatin_U, chLatin_T, chLatin_F, chDash, chDigit_8, chNull
};

std::string NodeValue::asString(const char *encoding) const
{
	if(n_ == 0) {
		std::string content;
		return d_.getContent(content);
	}
	else if(n_->getNodeType() == DOMNode::ELEMENT_NODE) {
		// Make output writer
		ostringstream oss;
		OStreamNsStream8 output(oss);
		NsWriter writer;
		writer.setStream(&output);

		// Make event generator
		scoped_ptr<NsPullEventSource8> event_gen(
			((Document*)d_)->getElementAsSAX((DOMElement*)n_));

		while(event_gen->nextEvent(&writer)) {}

		// TBD: use encoding argument
		return oss.str();
	} else if(n_->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
		ostringstream oss;
		oss << "{";
		if(n_->getNamespaceURI() != 0) {
			oss << XMLChToUTF8(n_->getNamespaceURI()).str();
		}
		oss << "}";
		oss << XMLChToUTF8(n_->getLocalName()).str();
		oss << "=\"";
		oss << XMLChToUTF8(n_->getNodeValue()).str();
		oss << "\"";
		return oss.str();
	} else if(n_->getNodeType() == DOMNode::TEXT_NODE) {
		return XMLChToUTF8(n_->getNodeValue()).str();
	} else if(n_->getNodeType() == DOMNode::COMMENT_NODE) {
		ostringstream oss;
		oss << "<!--";
		oss << XMLChToUTF8(n_->getNodeValue()).str();
		oss << "-->";
		return oss.str();
	} else {
		DOMWriter *writer =
			DOMImplementation::getImplementation()->
			createDOMWriter();
		// TBD: use encoding argument
		writer->setEncoding(XMLChUTF8);
		MemBufFormatTarget memBuf; // Uses default memory manager
		writer->writeNode(&memBuf, *n_);
		writer->release();
		return string((char *)memBuf.getRawBuffer(), memBuf.getLen());
	}
}

bool NodeValue::asBoolean() const
{

	// jcm: The xpath spec says that an empty sting is false, and a non-empty
	// string is true. But, the Boolean::string() xpath function returns
	// 'true' for true, and 'false' for false.... this is a bit silly, so
	// we diverge from the spec here.
	std::string s(asString());
	return (!s.empty() && s.compare("false") != 0);
}

DOMNode *NodeValue::asNode() const
{
	if(n_ == 0) {
		return d_.getContentAsDOM();
	} else {
		return n_;
	}
}

const XmlDocument &NodeValue::asDocument() const
{

	return d_;
}

bool NodeValue::equals(const Value &v) const
{
	return (v.getType() == XmlValue::NODE) &&
		d_ == ((NodeValue&)v).d_ &&
		(n_ == 0 || n_->isEqualNode(((NodeValue&)v).n_));
}

// AtomicTypeValue

// static helper methods within file scope
namespace
{
	// These are copies of string values from Xerces.
	// There is a change for inconsistency.  They should
	// be validated (debug function)
	const char *anyURI = "anyURI"; // fgDT_ANYURI
	const char *base64Binary = "base64Binary"; // fgDT_BASE64BINARY
	const char *boolean = "boolean"; // fgDT_BOOLEAN
	const char *date = "date"; // fgDT_DATE
	const char *dateTime = "dateTime"; // fgDT_DATETIME
	const char *decimal = "decimal"; // fgDT_DECIMAL
	const char *dt_double = "double"; // fgDT_DOUBLE
	const char *duration = "duration"; // fgDT_DURATION
	const char *dt_float = "float"; // fgDT_FLOAT
	const char *day = "gDay"; // fgDT_DAY
	const char *month = "gMonth"; // fgDT_MONTH
	const char *monthDay = "gMonthDay"; // fgDT_MONTHDAY
	const char *year = "gYear"; // fgDT_YEAR
	const char *yearMonth = "gYearMonth"; // fgDT_YEARMONTH
	const char *hexBinary = "hexBinary"; // fgDT_HEXBINARY
	const char *notationString = "NOTATION"; // XMLUni::fgNotationString
	const char *dt_qname = "QName"; // fgDT_QNAME
	const char *dt_string = "string"; // fgDT_STRING
	const char *dt_time = "time"; // fgDT_TIME
	// next 3 from Pathan
	const char *yearMonthDuration = "yearMonthDuration"; // ATDurationOrDerived::fgDT_YEARMONTHDURATION
	const char *untypedAtomic = "untypedAtomic"; // ATUntypedAtomic::fgDT_UNTYPEDATOMIC
	const char *dayTimeDuration = "dayTimeDuration";  // ATDurationOrDerived::fgDT_DAYTIMEDURATION

	const std::string s_false("false");
	const std::string s_true("true");

	const double s_nan = sqrt(-2.01);
	const double s_positiveInfinity = HUGE_VAL;
	const double s_negativeInfinity = -s_positiveInfinity;

	// does the double represent an infinite value?
	bool isInfinite(double n)
	{
		return (memcmp(&n, &s_positiveInfinity, sizeof (n)) == 0) ||
			(memcmp(&n, &s_negativeInfinity, sizeof (n)) == 0);
	}

	// does the double represent a NaN?
	bool isNan(double n)
	{
		return memcmp(&n, &s_nan, sizeof(n)) == 0;
	}

	// type validation using the Xerces/Pathan datatype validators
	void validateType(XmlValue::Type type, const std::string &value,
			  const DatatypeValidatorFactory &dvFactory)
	{
		DatatypeValidator *dv =
			dvFactory.getDatatypeValidator(
				AtomicTypeValue::convertToPrimitiveTypeName(type));
		if(dv == 0) {
			std::ostringstream os;
			os << "Cannot get datatype validator for an XmlValue type of " << type;
			throw XmlException(XmlException::INVALID_VALUE, os.str());
		}
		try {
			UTF8ToXMLCh chval(value);
			dv->validate(chval.str());
		}
		catch (XmlException &xe) {
			// will catch transcoding problems
			// InvalidDatatype[Value|Facet]Exception
			const XMLCh *tname =
				AtomicTypeValue::convertToPrimitiveTypeName(
					type);
			XMLChToUTF8 tn(tname);
			std::ostringstream os;
			os << "Transcoding error - the value \"" << value <<
				"\" is not valid for type \"" << tn.str() <<
				"\"";
			throw XmlException(XmlException::INVALID_VALUE, os.str());
		}
		catch(XMLException &e) {
			// InvalidDatatype[Value|Facet]Exception
			const XMLCh *tname =
				AtomicTypeValue::convertToPrimitiveTypeName(
					type);
			XMLChToUTF8 tn(tname);
			std::ostringstream os;
			os << "Error - the value \"" << value <<
				"\" is not valid for type \"" << tn.str() <<
				"\"";
			throw XmlException(XmlException::INVALID_VALUE, os.str());
		}
	}

	// conversions from XML Schema primitive type name (UTF8) to DB XML type
	typedef std::map<std::string, XmlValue::Type> NameTypeMap;
	static NameTypeMap *primitiveNamesToTypes = 0;

	XmlValue::Type typeFromPrimitiveName(const std::string
					     &primitiveTypeName) {
		NameTypeMap::iterator it =
			primitiveNamesToTypes->find(primitiveTypeName);
		if(it != primitiveNamesToTypes->end()) {
			return it->second;
		}

		std::ostringstream os;
		os << "Cannot convert the type name \"" << primitiveTypeName
		   << "\" to a supported DB XML type";
		throw XmlException(XmlException::INVALID_VALUE, os.str());
	}

}

Syntax::Type Value::getSyntaxType(const AnyAtomicType::Ptr &atom)
{
	try {
		std::string primitiveTypeName(
			XMLChToUTF8(atom->getPrimitiveTypeName()).str());
		return AtomicTypeValue::convertToSyntaxType(
			typeFromPrimitiveName(primitiveTypeName));
	}
	catch(...) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Unable to obtain syntax type.");
	}
}

AtomicTypeValue::AtomicTypeValue(bool v)
	: Value(XmlValue::BOOLEAN), value_(v ? s_true : s_false)
{

}

AtomicTypeValue::AtomicTypeValue(double v)
	: Value(XmlValue::DOUBLE), value_("")
{
	if(isNan(v)) {
		value_.assign(NaN_string);
	} else if (isInfinite(v)) {
		if(v < 0) {
			value_.assign(NegativeInfinity_string);
		} else {
			value_.assign(PositiveInfinity_string);
		}
	} else {
		value_ = DbXml::toString(v);
	}
}

AtomicTypeValue::AtomicTypeValue(const std::string &v)
	: Value(XmlValue::STRING), value_(v)
{
}

AtomicTypeValue::AtomicTypeValue(const char *v)
	: Value(XmlValue::STRING)
{
  if(!v) {
		throw XmlException(
				   XmlException::INVALID_VALUE,
				   "NULL pointer cannot be used as a value");
  }
  value_.assign(v);
}

AtomicTypeValue::AtomicTypeValue(XmlValue::Type type, const std::string &v)
	: Value(type), value_(v)
{
}

AtomicTypeValue::AtomicTypeValue(const std::string &primitiveTypeName, const std::string &v)
	: Value(XmlValue::ANY_SIMPLE_TYPE), value_(v)
{

	setType(typeFromPrimitiveName(primitiveTypeName));
}

// Converting between type and syntax enumerations
typedef std::map<XmlValue::Type, Syntax::Type> ValueSyntaxMap;
static ValueSyntaxMap *valueTypesToSyntaxTypes = 0;

Syntax::Type AtomicTypeValue::convertToSyntaxType(XmlValue::Type valueType)
{
	ValueSyntaxMap::iterator it = valueTypesToSyntaxTypes->find(valueType);
	if(it == valueTypesToSyntaxTypes->end()) {
		return Syntax::NONE;  // assume NONE in the case of error
	}
	return it->second;
}

// Converting between syntax and type enumerations
typedef std::map<Syntax::Type, XmlValue::Type> SyntaxValueMap;
static SyntaxValueMap *syntaxTypesToValueTypes = 0;

XmlValue::Type AtomicTypeValue::convertToValueType(Syntax::Type syntaxType)
{
	SyntaxValueMap::iterator it = syntaxTypesToValueTypes->find(syntaxType);
	if(it == syntaxTypesToValueTypes->end()) {
		return XmlValue::NONE;  // assume NONE in the case of error
	}
	return it->second;
}

Syntax::Type AtomicTypeValue::getSyntaxType() const
{
	return convertToSyntaxType(getType());
}

/// Return as a Number.
double AtomicTypeValue::asNumber() const
{

	switch(getType()) {
	case XmlValue::BOOLEAN: {
		if(value_.compare(s_true) == 0)
			return 1;
		else
			return 0;
		break;
	}
	case XmlValue::FLOAT:
	case XmlValue::DOUBLE:
	case XmlValue::DECIMAL: {
		return strtod(value_.c_str(), 0);
		break;
	}
	case XmlValue::STRING: {
		bool numeric(false);
		DatatypeValidator *dtv =
			DatatypeValidatorFactory::getBuiltInRegistry()->
			get(AtomicTypeValue::convertToPrimitiveTypeName(
				    XmlValue::DOUBLE));
		if(dtv != 0) {
			try {
				dtv->validate(UTF8ToXMLCh(value_).str());
				numeric = true;
			}
			catch (XmlException &xe) {
				// catches transcoding issues
			}
			catch(XMLException &e) {
				// InvalidDatatype[Value|Facet]Exception -
				//   not numeric
			}

			if(numeric) {
				if (value_.compare(NaN_string)==0)
					return s_nan;
				else if (value_.compare(
						 NegativeInfinity_string) ==0 )
					return s_negativeInfinity;
				else if (value_.compare(
						 PositiveInfinity_string) ==0 )
					return s_positiveInfinity;
				else
					return strtod(value_.c_str(), 0);
			} else {
				return s_nan;
			}
		} else {
			// cannot validate
			throw XmlException(
				XmlException::INVALID_VALUE,
				"Unable to check if the requested type can be converted into a number.");
		}
	}
	default:
		throw XmlException(
			XmlException::INVALID_VALUE,
			"The requested type cannot be converted into a number.");
		break;
	}
}

// Return as a string
std::string AtomicTypeValue::asString(const char *encoding) const
{
	// TBD: use encoding
	return value_;

}

/// Return the value as a Boolean.
bool AtomicTypeValue::asBoolean() const
{
	switch(getType()) {
	case XmlValue::BOOLEAN: {
		return (value_.compare(s_true) == 0);
		break;
	}
	case XmlValue::DECIMAL: {
		return (strtod(value_.c_str(), 0) != 0);
		break;
	}
	case XmlValue::FLOAT:
	case XmlValue::DOUBLE: {
		if (value_.compare(NaN_string)==0) {
			return false;
		} else {
			if (value_.compare(NegativeInfinity_string) == 0 ||
			    value_.compare(PositiveInfinity_string) == 0)
				return true;
			else
				return (strtod(value_.c_str(), 0) != 0);
		}
		break;
	}
	case XmlValue::STRING: {
		// jcm: The xpath spec says that an empty sting is false, and
		// a non-empty string is true. But, the Boolean::string()
		// xpath function returns 'true' for true, and 'false' for
		// false.... this is a bit silly, so we diverge
		// from the spec here.
		return (!value_.empty() && value_.compare(s_false) != 0);
		break;
	}
	default:
		throw XmlException(XmlException::INVALID_VALUE,
				   "The requested type cannot be converted into a boolean.");
		break;
	}
}

bool AtomicTypeValue::equals(const Value &v) const
{
	return ((getType() == v.getType()) && (value_.compare(
						       v.asString()) == 0));
}

void AtomicTypeValue::validate(const DatatypeValidatorFactory &dvFactory) const
{
	validateType(getType(), value_, dvFactory);
}

/// convert from a DB XML type to a Pathan primitive type name
typedef std::map<XmlValue::Type, const XMLCh*> TypeNameMap;
static TypeNameMap *typesToPrimitiveNames = 0;

const XMLCh *AtomicTypeValue::convertToPrimitiveTypeName(XmlValue::Type type) {
	TypeNameMap::iterator it = typesToPrimitiveNames->find(type);
	if(it != typesToPrimitiveNames->end()) {
		return it->second;
	}

	std::ostringstream os;
	os << "Cannot convert the DB XML type " << type <<
		" to a Pathan primitive type name.";
	throw XmlException(XmlException::INVALID_VALUE, os.str());
}

namespace DbXml {
#ifdef DEBUG
// Debug-only code to validate string type names vs. Xerces/Pathan
struct verifyTypeNames {
	const char *sname;
	const XMLCh *xname;
};
struct verifyTypeNames verf[22] = {
	{anyURI,SchemaSymbols::fgDT_ANYURI},
	{boolean,SchemaSymbols::fgDT_BOOLEAN},
	{base64Binary,SchemaSymbols::fgDT_BASE64BINARY},
	{date,SchemaSymbols::fgDT_DATE},
	{dateTime,SchemaSymbols::fgDT_DATETIME},
	{decimal,SchemaSymbols::fgDT_DECIMAL},
	{dt_double,SchemaSymbols::fgDT_DOUBLE},
	{duration,SchemaSymbols::fgDT_DURATION},
	{dt_float,SchemaSymbols::fgDT_FLOAT},
	{day,SchemaSymbols::fgDT_DAY},
	{month,SchemaSymbols::fgDT_MONTH},
	{monthDay,SchemaSymbols::fgDT_MONTHDAY},
	{year,SchemaSymbols::fgDT_YEAR},
	{yearMonth,SchemaSymbols::fgDT_YEARMONTH},
	{hexBinary,SchemaSymbols::fgDT_HEXBINARY},
	{notationString,XMLUni::fgNotationString},
	{dt_qname,SchemaSymbols::fgDT_QNAME},
	{dt_string,SchemaSymbols::fgDT_STRING},
	{dt_time,SchemaSymbols::fgDT_TIME},
	{yearMonthDuration,ATDurationOrDerived::fgDT_YEARMONTHDURATION},
	{untypedAtomic,ATUntypedAtomic::fgDT_UNTYPEDATOMIC},
	{dayTimeDuration,ATDurationOrDerived::fgDT_DAYTIMEDURATION}
};

static void verifyNames()
{
	for (int i = 0; i < 22; i++) {
		XMLChToUTF8 x(verf[i].xname);
		if (x.str() != (std::string)verf[i].sname) {
			ostringstream oss;
			oss << "Mismatch of Xerces type names.  BDB XML name: "
			    << verf[i].sname << ", Xerces name: " << x.str();
			throw XmlException(XmlException::INTERNAL_ERROR,
					   oss.str());
		}
	}
}

#else
static void verifyNames() {}
#endif // DEBUG

// initializations of static type conversion maps
void initTypeConversions()
{
	if(primitiveNamesToTypes == 0) {
		verifyNames();
		primitiveNamesToTypes = new NameTypeMap;

		std::string name;
		typedef std::pair<std::string, XmlValue::Type> NameTypePair;
		primitiveNamesToTypes->insert(NameTypePair(anyURI,
							   XmlValue::ANY_URI));
		primitiveNamesToTypes->insert(
			NameTypePair(base64Binary,XmlValue::BASE_64_BINARY));

		primitiveNamesToTypes->insert(
			NameTypePair(boolean, XmlValue::BOOLEAN));
		primitiveNamesToTypes->insert(
			NameTypePair(date, XmlValue::DATE));
		primitiveNamesToTypes->insert(
			NameTypePair(dateTime, XmlValue::DATE_TIME));
		primitiveNamesToTypes->insert(
			NameTypePair(dayTimeDuration,
				     XmlValue::DAY_TIME_DURATION));
		primitiveNamesToTypes->insert(
			NameTypePair(decimal, XmlValue::DECIMAL));
		primitiveNamesToTypes->insert(
			NameTypePair(dt_double, XmlValue::DOUBLE));
		primitiveNamesToTypes->insert(
			NameTypePair(duration, XmlValue::DURATION));
		primitiveNamesToTypes->insert(
			NameTypePair(dt_float, XmlValue::FLOAT));
		primitiveNamesToTypes->insert(
			NameTypePair(day, XmlValue::G_DAY));
		primitiveNamesToTypes->insert(
			NameTypePair(month, XmlValue::G_MONTH));
		primitiveNamesToTypes->insert(
			NameTypePair(monthDay, XmlValue::G_MONTH_DAY));
		primitiveNamesToTypes->insert(
			NameTypePair(year, XmlValue::G_YEAR));
		primitiveNamesToTypes->insert(
			NameTypePair(yearMonth, XmlValue::G_YEAR_MONTH));
		primitiveNamesToTypes->insert(
			NameTypePair(hexBinary, XmlValue::HEX_BINARY));
		primitiveNamesToTypes->insert(
			NameTypePair(notationString, XmlValue::NOTATION));
		primitiveNamesToTypes->insert(
			NameTypePair(dt_qname, XmlValue::QNAME));
		primitiveNamesToTypes->insert(
			NameTypePair(dt_string, XmlValue::STRING));
		primitiveNamesToTypes->insert(
			NameTypePair(dt_time, XmlValue::TIME));
		primitiveNamesToTypes->insert(
			NameTypePair(yearMonthDuration,
				     XmlValue::YEAR_MONTH_DURATION));
		primitiveNamesToTypes->insert(
			NameTypePair(untypedAtomic,XmlValue::UNTYPED_ATOMIC));  }

	if(typesToPrimitiveNames == 0) {
		typesToPrimitiveNames = new TypeNameMap;
		typedef std::pair<XmlValue::Type, const XMLCh*> TypeNamePair;

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::ANY_URI,
					    SchemaSymbols::fgDT_ANYURI));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::BASE_64_BINARY,
					    SchemaSymbols::fgDT_BASE64BINARY));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::BOOLEAN,
					    SchemaSymbols::fgDT_BOOLEAN));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::DATE,
					    SchemaSymbols::fgDT_DATE));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::DATE_TIME,
					    SchemaSymbols::fgDT_DATETIME));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::DAY_TIME_DURATION,
					    ATDurationOrDerived::fgDT_DAYTIMEDURATION_XERCESHASH));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::DECIMAL,
					    SchemaSymbols::fgDT_DECIMAL));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::DOUBLE,
					    SchemaSymbols::fgDT_DOUBLE));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::DURATION,
					    SchemaSymbols::fgDT_DURATION));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::FLOAT,
					    SchemaSymbols::fgDT_FLOAT));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::G_DAY,
					    SchemaSymbols::fgDT_DAY));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::G_MONTH,
					    SchemaSymbols::fgDT_MONTH));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::G_MONTH_DAY,
					    SchemaSymbols::fgDT_MONTHDAY));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::G_YEAR,
					    SchemaSymbols::fgDT_YEAR));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::G_YEAR_MONTH,
					    SchemaSymbols::fgDT_YEARMONTH));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::HEX_BINARY,
					    SchemaSymbols::fgDT_HEXBINARY));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::NOTATION,
					    XMLUni::fgNotationString));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::QNAME,
					    SchemaSymbols::fgDT_QNAME));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::STRING,
					    SchemaSymbols::fgDT_STRING));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::TIME,
					    SchemaSymbols::fgDT_TIME));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::YEAR_MONTH_DURATION,
					    ATDurationOrDerived::fgDT_YEARMONTHDURATION_XERCESHASH));

		typesToPrimitiveNames->
			insert(TypeNamePair(XmlValue::UNTYPED_ATOMIC,
					    ATUntypedAtomic::fgDT_UNTYPEDATOMIC_XERCESHASH));
	}

	typedef std::pair<XmlValue::Type, Syntax::Type> ValueSyntaxPair;
	if(valueTypesToSyntaxTypes == 0) {
		valueTypesToSyntaxTypes = new ValueSyntaxMap;
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::ANY_URI,
					       Syntax::ANY_URI));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::BASE_64_BINARY,
					       Syntax::BASE_64_BINARY));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::BOOLEAN,
					       Syntax::BOOLEAN));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::DATE, Syntax::DATE));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::DATE_TIME,
					       Syntax::DATE_TIME));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::G_DAY, Syntax::DAY));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::DAY_TIME_DURATION,
					       Syntax::DURATION));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::DECIMAL,
					       Syntax::DECIMAL));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::DOUBLE,
					       Syntax::DOUBLE));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::DURATION,
					       Syntax::DURATION));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::FLOAT, Syntax::FLOAT));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::HEX_BINARY,
					       Syntax::HEX_BINARY));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::G_MONTH,
					       Syntax::MONTH));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::G_MONTH_DAY,
					       Syntax::MONTH_DAY));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::NOTATION,
					       Syntax::NOTATION));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::QNAME, Syntax::QNAME));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::STRING,
					       Syntax::STRING));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::TIME, Syntax::TIME));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::G_YEAR, Syntax::YEAR));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::G_YEAR_MONTH,
					       Syntax::YEAR_MONTH));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::YEAR_MONTH_DURATION,
					       Syntax::DURATION));
		valueTypesToSyntaxTypes->
			insert(ValueSyntaxPair(XmlValue::UNTYPED_ATOMIC,
					       Syntax::STRING));
	}

	typedef std::pair<Syntax::Type, XmlValue::Type> SyntaxValuePair;
	if(syntaxTypesToValueTypes == 0) {
		syntaxTypesToValueTypes = new SyntaxValueMap;
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::ANY_URI,
					       XmlValue::ANY_URI));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::BASE_64_BINARY,
					       XmlValue::BASE_64_BINARY));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::BOOLEAN,
					       XmlValue::BOOLEAN));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::DATE, XmlValue::DATE));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::DATE_TIME,
					       XmlValue::DATE_TIME));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::DAY, XmlValue::G_DAY));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::DURATION,
					       XmlValue::DAY_TIME_DURATION));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::DECIMAL,
					       XmlValue::DECIMAL));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::DOUBLE,
					       XmlValue::DOUBLE));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::DURATION,
					       XmlValue::DURATION));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::FLOAT, XmlValue::FLOAT));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::HEX_BINARY,
					       XmlValue::HEX_BINARY));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::MONTH,
					       XmlValue::G_MONTH));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::MONTH_DAY,
					       XmlValue::G_MONTH_DAY));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::NOTATION,
					       XmlValue::NOTATION));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::QNAME, XmlValue::QNAME));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::STRING,
					       XmlValue::STRING));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::TIME, XmlValue::TIME));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::YEAR, XmlValue::G_YEAR));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::YEAR_MONTH,
					       XmlValue::G_YEAR_MONTH));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::DURATION,
					       XmlValue::YEAR_MONTH_DURATION));
		syntaxTypesToValueTypes->
			insert(SyntaxValuePair(Syntax::STRING,
					       XmlValue::UNTYPED_ATOMIC));
	}
}

void terminateTypeConversions()
{
	if (typesToPrimitiveNames) {
		delete typesToPrimitiveNames;
		typesToPrimitiveNames = 0;
	}
	if (syntaxTypesToValueTypes) {
		delete syntaxTypesToValueTypes;
		syntaxTypesToValueTypes = 0;
	}
	if (primitiveNamesToTypes) {
		delete primitiveNamesToTypes;
		primitiveNamesToTypes = 0;
	}
	if (valueTypesToSyntaxTypes) {
		delete valueTypesToSyntaxTypes;
		valueTypesToSyntaxTypes = 0;
	}
}
}

std::string NodeValue::getNodeName() const {
	if(n_) {
		return XMLChToUTF8(n_->getNodeName()).str();
	} else {
		return "#document";
	}
}

std::string NodeValue::getNamespaceURI() const {
	if(n_) {
		return XMLChToUTF8(asNode()->getNamespaceURI()).str();
	} else {
		return "";
	}
}

std::string NodeValue::getPrefix() const {
	if(n_) {
		return XMLChToUTF8(n_->getPrefix()).str();
	} else {
		return "";
	}
}

std::string NodeValue::getLocalName() const {
	if(n_) {
		return XMLChToUTF8(n_->getLocalName()).str();
	} else {
		return "#document";
	}
}

std::string NodeValue::getNodeValue() const {
	if(n_) {
		return XMLChToUTF8(n_->getNodeValue()).str();
	} else {
		return "";
	}
}

short NodeValue::getNodeType() const {
	if(n_) {
		return n_->getNodeType();
	} else {
		return DOMNode::DOCUMENT_NODE;
	}
}

XmlValue NodeValue::getParentNode() const {
	DOMNode *parent = 0;
	if(n_) parent = n_->getParentNode();
	if(parent == 0) return XmlValue();
	else return new NodeValue(parent, d_);
}

XmlValue NodeValue::getFirstChild() const {
	DOMNode *child = asNode()->getFirstChild();
	if(child == 0) return XmlValue();
	else return new NodeValue(child, d_);
}

XmlValue NodeValue::getLastChild() const {
	DOMNode *child = asNode()->getLastChild();
	if(child == 0) return XmlValue();
	else return new NodeValue(child, d_);
}

XmlValue NodeValue::getPreviousSibling() const {
	DOMNode *sibling = 0;
	if(n_) sibling = n_->getPreviousSibling();
	if(sibling == 0) return XmlValue();
	else return new NodeValue(sibling, d_);
}

XmlValue NodeValue::getNextSibling() const {
	DOMNode *sibling = 0;
	if(n_) sibling = n_->getNextSibling();
	if(sibling == 0) return XmlValue();
	else return new NodeValue(sibling, d_);
}

XmlResults NodeValue::getAttributes() const {
	ValueResults *vr = new ValueResults();
	if(n_) {
		DOMNamedNodeMap *attrMap = n_->getAttributes();
		if(attrMap != 0) {
			int size = attrMap->getLength();
			for(int i = 0; i < size; ++i) {
				NodeValue *curNode =
					new NodeValue(attrMap->item(i), d_);
				vr->add(curNode);
			}
		}
	}
	return vr;
}

XmlValue NodeValue::getOwnerElement() const {

	if(n_ && n_->getNodeType() ==  DOMNode::ATTRIBUTE_NODE) {
		return new NodeValue(((DOMAttr*)n_)
				     ->getOwnerElement(), d_);
	}
	throw XmlException(XmlException::INVALID_VALUE,
			   "Node is not an attribute node");

}

BinaryValue::BinaryValue(const Dbt &dbt)
	: Value(XmlValue::BINARY)
{
	init(dbt.get_data(), dbt.get_size());
}

BinaryValue::BinaryValue(const std::string &content)
	: Value(XmlValue::BINARY)
{
	// include the terminting null
	init((void*)content.c_str(), (u_int32_t)(content.size()+1));
}

BinaryValue::~BinaryValue()
{
	if (dbt_.get_data())
		::free(dbt_.get_data());
}

void BinaryValue::init(const void *data, u_int32_t size)
{
	if (size && data) {
		void *bin = ::malloc(size);
		if (!bin)
			throw XmlException(XmlException::NO_MEMORY_ERROR,
					   "Cannot allocate memory for XmlValue");
		::memcpy(bin, data, size);
		dbt_.set_data(bin);
		dbt_.set_size(size);
	}
}
	
bool BinaryValue::equals(const Value &v) const
{
	if (v.getType() == XmlValue::BINARY) {
		const void *thisData = dbt_.get_data();
		const void *otherData = v.asBinary().get_data();
		u_int32_t thisSize = dbt_.get_size();
		u_int32_t otherSize = v.asBinary().get_size();
		if (thisSize == otherSize) {
			if (::memcmp(thisData, otherData, thisSize) == 0)
				return true;
		}
	}
	return false;
}
