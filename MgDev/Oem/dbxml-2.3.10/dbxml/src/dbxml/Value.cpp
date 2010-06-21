//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Value.cpp,v 1.115 2006/11/21 21:44:24 gmf Exp $
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
#include "nodeStore/NsXercesDom.hpp"
#include "nodeStore/NsDomReader.hpp"

#include <sstream>
#include <float.h>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/schema/DocumentCacheImpl.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
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

XmlValue::Type typeFromPrimitive(AnyAtomicType::AtomicObjectType type)
{
	switch(type) {
	case AnyAtomicType::ANY_SIMPLE_TYPE: return XmlValue::ANY_SIMPLE_TYPE;
	case AnyAtomicType::ANY_URI: return XmlValue::ANY_URI;
	case AnyAtomicType::BASE_64_BINARY: return XmlValue::BASE_64_BINARY;
	case AnyAtomicType::BOOLEAN: return XmlValue::BOOLEAN;
	case AnyAtomicType::DATE: return XmlValue::DATE;
	case AnyAtomicType::DATE_TIME: return XmlValue::DATE_TIME;
	case AnyAtomicType::DAY_TIME_DURATION: return XmlValue::DAY_TIME_DURATION;
	case AnyAtomicType::DECIMAL: return XmlValue::DECIMAL;
	case AnyAtomicType::DOUBLE: return XmlValue::DOUBLE;
	case AnyAtomicType::DURATION: return XmlValue::DURATION;
	case AnyAtomicType::FLOAT: return XmlValue::FLOAT;
	case AnyAtomicType::G_DAY: return XmlValue::G_DAY;
	case AnyAtomicType::G_MONTH: return XmlValue::G_MONTH;
	case AnyAtomicType::G_MONTH_DAY: return XmlValue::G_MONTH_DAY;
	case AnyAtomicType::G_YEAR: return XmlValue::G_YEAR;
	case AnyAtomicType::G_YEAR_MONTH: return XmlValue::G_YEAR_MONTH;
	case AnyAtomicType::HEX_BINARY: return XmlValue::HEX_BINARY;
	case AnyAtomicType::NOTATION: return XmlValue::NOTATION;
	case AnyAtomicType::QNAME: return XmlValue::QNAME;
	case AnyAtomicType::STRING: return XmlValue::STRING;
	case AnyAtomicType::TIME: return XmlValue::TIME;
	case AnyAtomicType::YEAR_MONTH_DURATION: return XmlValue::YEAR_MONTH_DURATION;
	case AnyAtomicType::UNTYPED_ATOMIC: return XmlValue::UNTYPED_ATOMIC;
	default: break;
	}
	std::ostringstream os;
	os << "Cannot convert the primitive type name " << type
	   << " to a supported DB XML type";
	throw XmlException(XmlException::INVALID_VALUE, os.str());
}

AnyAtomicType::AtomicObjectType primitiveFromType(XmlValue::Type type)
{
	switch(type) {
	case XmlValue::ANY_SIMPLE_TYPE: return AnyAtomicType::ANY_SIMPLE_TYPE;
	case XmlValue::ANY_URI: return AnyAtomicType::ANY_URI;
	case XmlValue::BASE_64_BINARY: return AnyAtomicType::BASE_64_BINARY;
	case XmlValue::BOOLEAN: return AnyAtomicType::BOOLEAN;
	case XmlValue::DATE: return AnyAtomicType::DATE;
	case XmlValue::DATE_TIME: return AnyAtomicType::DATE_TIME;
	case XmlValue::DAY_TIME_DURATION: return AnyAtomicType::DAY_TIME_DURATION;
	case XmlValue::DECIMAL: return AnyAtomicType::DECIMAL;
	case XmlValue::DOUBLE: return AnyAtomicType::DOUBLE;
	case XmlValue::DURATION: return AnyAtomicType::DURATION;
	case XmlValue::FLOAT: return AnyAtomicType::FLOAT;
	case XmlValue::G_DAY: return AnyAtomicType::G_DAY;
	case XmlValue::G_MONTH: return AnyAtomicType::G_MONTH;
	case XmlValue::G_MONTH_DAY: return AnyAtomicType::G_MONTH_DAY;
	case XmlValue::G_YEAR: return AnyAtomicType::G_YEAR;
	case XmlValue::G_YEAR_MONTH: return AnyAtomicType::G_YEAR_MONTH;
	case XmlValue::HEX_BINARY: return AnyAtomicType::HEX_BINARY;
	case XmlValue::NOTATION: return AnyAtomicType::NOTATION;
	case XmlValue::QNAME: return AnyAtomicType::QNAME;
	case XmlValue::STRING: return AnyAtomicType::STRING;
	case XmlValue::TIME: return AnyAtomicType::TIME;
	case XmlValue::YEAR_MONTH_DURATION: return AnyAtomicType::YEAR_MONTH_DURATION;
	case XmlValue::UNTYPED_ATOMIC: return AnyAtomicType::UNTYPED_ATOMIC;
	default: break;
	}
	std::ostringstream os;
	os << "Cannot convert the DB XML type " << type
	   << " to a supported primitive type";
	throw XmlException(XmlException::INVALID_VALUE, os.str());
}

const char *stringFromType(XmlValue::Type type)
{
	switch(type) {
	case XmlValue::ANY_SIMPLE_TYPE: return "anySimpleType";
	case XmlValue::ANY_URI: return "anyURI";
	case XmlValue::BASE_64_BINARY: return "base64Binary";
	case XmlValue::BOOLEAN: return "boolean";
	case XmlValue::DATE: return "date";
	case XmlValue::DATE_TIME: return "dateTime";
	case XmlValue::DAY_TIME_DURATION: return "dayTimeDuration";
	case XmlValue::DECIMAL: return "decimal";
	case XmlValue::DOUBLE: return "double";
	case XmlValue::DURATION: return "duration";
	case XmlValue::FLOAT: return "float";
	case XmlValue::G_DAY: return "gDay";
	case XmlValue::G_MONTH: return "gMonth";
	case XmlValue::G_MONTH_DAY: return "gMonthDay";
	case XmlValue::G_YEAR: return "gYear";
	case XmlValue::G_YEAR_MONTH: return "gYearMonth";
	case XmlValue::HEX_BINARY: return "hexBinary";
	case XmlValue::NOTATION: return "NOTATION";
	case XmlValue::QNAME: return "QName";
	case XmlValue::STRING: return "string";
	case XmlValue::TIME: return "time";
	case XmlValue::YEAR_MONTH_DURATION: return "yearMonthDuration";
	case XmlValue::UNTYPED_ATOMIC: return "untypedAtomic";
	default: break;
	}
	return "";
}

Syntax::Type syntaxFromType(XmlValue::Type type)
{
	switch(type) {
	case XmlValue::BASE_64_BINARY: return Syntax::BASE_64_BINARY;
	case XmlValue::BOOLEAN: return Syntax::BOOLEAN;
	case XmlValue::DATE: return Syntax::DATE;
	case XmlValue::DATE_TIME: return Syntax::DATE_TIME;
	case XmlValue::DAY_TIME_DURATION: return Syntax::DURATION;
	case XmlValue::DECIMAL: return Syntax::DECIMAL;
	case XmlValue::DOUBLE: return Syntax::DOUBLE;
	case XmlValue::DURATION: return Syntax::DURATION;
	case XmlValue::FLOAT: return Syntax::FLOAT;
	case XmlValue::G_DAY: return Syntax::DAY;
	case XmlValue::G_MONTH: return Syntax::MONTH;
	case XmlValue::G_MONTH_DAY: return Syntax::MONTH_DAY;
	case XmlValue::G_YEAR: return Syntax::YEAR;
	case XmlValue::G_YEAR_MONTH: return Syntax::YEAR_MONTH;
	case XmlValue::HEX_BINARY: return Syntax::HEX_BINARY;
	case XmlValue::STRING: return Syntax::STRING;
	case XmlValue::TIME: return Syntax::TIME;
	case XmlValue::YEAR_MONTH_DURATION: return Syntax::DURATION;

	// No syntax for the following
	case XmlValue::ANY_URI:
	case XmlValue::NOTATION:
	case XmlValue::QNAME:
	case XmlValue::UNTYPED_ATOMIC: return Syntax::STRING;
	default: break;
	}

	return Syntax::NONE;
}

XmlValue::Type typeFromSyntax(Syntax::Type type)
{
	switch(type) {
	case Syntax::BASE_64_BINARY: return XmlValue::BASE_64_BINARY;
	case Syntax::BOOLEAN: return XmlValue::BOOLEAN;
	case Syntax::DATE: return XmlValue::DATE;
	case Syntax::DATE_TIME: return XmlValue::DATE_TIME;
	case Syntax::DECIMAL: return XmlValue::DECIMAL;
	case Syntax::DOUBLE: return XmlValue::DOUBLE;
	case Syntax::DURATION: return XmlValue::DURATION;
	case Syntax::FLOAT: return XmlValue::FLOAT;
	case Syntax::DAY: return XmlValue::G_DAY;
	case Syntax::MONTH: return XmlValue::G_MONTH;
	case Syntax::MONTH_DAY: return XmlValue::G_MONTH_DAY;
	case Syntax::YEAR: return XmlValue::G_YEAR;
	case Syntax::YEAR_MONTH: return XmlValue::G_YEAR_MONTH;
	case Syntax::HEX_BINARY: return XmlValue::HEX_BINARY;
	case Syntax::STRING: return XmlValue::STRING;
	case Syntax::TIME: return XmlValue::TIME;

	case Syntax::DEPRECATED_ANY_URI:
	case Syntax::DEPRECATED_NOTATION:
	case Syntax::DEPRECATED_QNAME:
		DBXML_ASSERT(false);
		break;
	default: break;
	}

	return XmlValue::NONE;
}

Value *Value::create(const AnyAtomicType::Ptr &atom, DynamicContext *context)
{
	// arw - not sure if XQilla will throw here
	try {
		std::string typeURI(XMLChToUTF8(atom->getTypeURI()).str());
		std::string typeName(XMLChToUTF8(atom->getTypeName()).str());
		std::string value(XMLChToUTF8(atom->asString(context)).str());
		return new AtomicTypeValue(typeFromPrimitive(atom->getPrimitiveTypeIndex()),
			typeURI, typeName, value);
	}
	catch(...) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Unable to store value returned from evaluation of XPath query.");
	}
}

Value *Value::create(const Node::Ptr &item, QueryContext &qc, bool lazyDocs)
{
	const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)item->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(nodeImpl != 0);

	const XmlDocument &document = nodeImpl->getXmlDocument();

	if(!lazyDocs) {
		((Document&)document).setEagerMetaData();
	}

	if(nodeImpl->dmNodeKind() == Node::document_string) {
		// Even if this is DeadValues, don't bother creating a copy
		// of the node, as that involves copying the entire tree...
		return new NodeValue(document);
	}

	const DOMNode *node = nodeImpl->getDOMNode();
	if(qc.getReturnType()==XmlQueryContext::DeadValues) {
		// Make a fake temporary XmlDocument, containing a
		// DOMDocument that we will create our dead node on.
		XmlDocument tmpDocument =
			qc.getManager().createDocument();
		return new NodeValue(
			tmpDocument.getContentAsDOM()->
			importNode(const_cast<DOMNode*>(node),
				/*deep*/true), tmpDocument);
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
				value->validate();
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
			return ((DbXmlFactoryImpl*)context->getItemFactory())->createNode(((const NodeValue*)v)->getDOMNode(), v->asDocument(), context);
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
			return context->getItemFactory()->
				createDerivedFromAtomicType(
					primitiveFromType(v->getType()),
					context->getMemoryManager()->getPooledString(UTF8ToXMLCh(v->getTypeURI()).str()),
					context->getMemoryManager()->getPooledString(UTF8ToXMLCh(v->getTypeName()).str()),
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

string NodeValue::getTypeURI() const
{
	if(n_ != 0 && (n_->getNodeType() == DOMNode::ELEMENT_NODE ||
		   n_->getNodeType() == DOMNode::ATTRIBUTE_NODE ||
		   n_->getNodeType() == DOMNode::TEXT_NODE ||
		   n_->getNodeType() == DOMNode::CDATA_SECTION_NODE)) {
		return XMLChToUTF8(FunctionConstructor::XMLChXPath2DatatypesURI).str();
	}

	return "";
}

string NodeValue::getTypeName() const
{
	if(n_ != 0 && n_->getNodeType() == DOMNode::ELEMENT_NODE) {
		return XMLChToUTF8(DocumentCacheParser::g_szUntyped).str();
	} else if(n_ != 0 && (n_->getNodeType() == DOMNode::ATTRIBUTE_NODE ||
		   n_->getNodeType() == DOMNode::TEXT_NODE ||
		   n_->getNodeType() == DOMNode::CDATA_SECTION_NODE)) {
		return XMLChToUTF8(ATUntypedAtomic::fgDT_UNTYPEDATOMIC).str();
	}

	return "";
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
	if(n_ == 0 || n_->getNodeType() == DOMNode::DOCUMENT_NODE) {
		std::string content;
		return d_.getContent(content);
	}
	else if(n_->getNodeType() == DOMNode::ELEMENT_NODE) {
		// Make output writer
		std::string outstr;
		StringNsStream output(outstr);
		NsWriter writer(&output);

		// Make an event reader
		ScopedPtr<EventReader> reader(
			((Document*)d_)->getElementAsReader((DOMElement*)n_));

		writer.writeFromReader(*reader);

		// TBD: use encoding argument
		return outstr;
	} else if(n_->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
		std::string str = "{";
		if(n_->getNamespaceURI() != 0) {
			str.append(XMLChToUTF8(n_->getNamespaceURI()).str());
		}
		str.append("}");
		str.append(XMLChToUTF8(n_->getLocalName()).str());
		str.append("=\"");
		str.append(XMLChToUTF8(n_->getNodeValue()).str());
		str.append("\"");
		return str;
	} else if(n_->getNodeType() == DOMNode::TEXT_NODE) {
		return XMLChToUTF8(n_->getNodeValue()).str();
	} else if(n_->getNodeType() == DOMNode::COMMENT_NODE) {
		std::string str = "<!--";
		str.append(XMLChToUTF8(n_->getNodeValue()).str());
		str.append("-->");
		return str;
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

XmlEventReader &NodeValue::asEventReader() const
{
	if((n_ == 0 || n_->getNodeType() == DOMNode::DOCUMENT_NODE) ||
	   (n_->getNodeType() == DOMNode::ELEMENT_NODE)) {
		if (n_ == 0) {
			DBXML_ASSERT(d_);
			return d_.getContentAsEventReader();
		} else {
			// Make an event reader
			NsDomElement *el =
				(NsDomElement*)((NsDomNode*)n_->
						getInterface(_nsDomString));
			return *(new NsDomReader(el));
		}
	}
	// The only way to support asEventReader on non-elements is if
	// the XmlEventReader interface were able to iterate through attributes
	// one at a time, rather than using an indexed interface.
	throw XmlException(XmlException::INVALID_VALUE,
			   "XmlValue::asEventReader requires an element node");
}

bool NodeValue::equals(const Value &v) const
{
	return (v.getType() == XmlValue::NODE) &&
		d_ == ((NodeValue&)v).d_ &&
		(n_ == 0 || n_->isSameNode(((NodeValue&)v).n_));
}

string NodeValue::getNodeHandle() const
{
	IndexEntry ie;
	ie.setDocID(((Document&)d_).getID());

	short type = getNodeType();
	if(type != DOMNode::DOCUMENT_NODE) {
		ie.setNodeID(fakeDynamicCastNsDomNode(n_)->getNodeId());

		if(type == DOMNode::ELEMENT_NODE) {
			ie.setFormat(IndexEntry::NH_ELEMENT_FORMAT);
		} else if(type == DOMNode::ATTRIBUTE_NODE) {
			ie.setFormat(IndexEntry::NH_ATTRIBUTE_FORMAT);
			ie.setIndex(fakeDynamicCastNsDomNode(n_)->getIndex());
		} else if(type == DOMNode::TEXT_NODE ||
			type == DOMNode::CDATA_SECTION_NODE) {
			ie.setFormat(IndexEntry::NH_TEXT_FORMAT);
			ie.setIndex(fakeDynamicCastNsDomNode(n_)->getIndex());
		} else if(type == DOMNode::COMMENT_NODE) {
			ie.setFormat(IndexEntry::NH_COMMENT_FORMAT);
			ie.setIndex(fakeDynamicCastNsDomNode(n_)->getIndex());
		} else if(type == DOMNode::PROCESSING_INSTRUCTION_NODE) {
			ie.setFormat(IndexEntry::NH_PI_FORMAT);
			ie.setIndex(fakeDynamicCastNsDomNode(n_)->getIndex());
		} else {
			throw XmlException(XmlException::INVALID_VALUE,
				"Node handle unavailable for node type");
		}
	} else {
		ie.setFormat(IndexEntry::NH_DOCUMENT_FORMAT);
	}

	return ie.getNodeHandle();
}

// AtomicTypeValue

// static helper methods within file scope
namespace
{
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

	// type validation using the Xerces/XQilla datatype validators
	void validateType(XmlValue::Type type, const std::string &typeURI,
		const std::string &typeName, const std::string &value)
	{
		bool isPrimitive;
		const DatatypeFactory *factory = Globals::getDatatypeLookup()->
			lookupDatatype(UTF8ToXMLCh(typeURI).str(), UTF8ToXMLCh(typeName).str(),
				isPrimitive);

		if(factory == 0) {
			std::ostringstream os;
			os << "Cannot get datatype validator for an XmlValue type of {"
			   << typeURI << "}" << typeName << " (\"" << stringFromType(type) << "\")";
			throw XmlException(XmlException::INVALID_VALUE, os.str());
		}

		if(factory->getPrimitiveTypeIndex() != primitiveFromType(type)) {
			std::ostringstream os;
			os << "XmlValue enumeration \"" << stringFromType(type) << "\" does not match type name of {"
			   << typeURI << "}" << typeName;
			throw XmlException(XmlException::INVALID_VALUE, os.str());
		}

		try {
			if(!factory->checkInstance(UTF8ToXMLCh(value).str(),
				   Globals::defaultMemoryManager)) {
				// InvalidDatatype[Value|Facet]Exception
				std::ostringstream os;
				os << "Error - the value \"" << value <<
					"\" is not valid for type \"" << stringFromType(type) <<
					"\"";
				throw XmlException(XmlException::INVALID_VALUE, os.str());
			}
		}
		catch (XmlException &xe) {
			// will catch transcoding problems
			// InvalidDatatype[Value|Facet]Exception
			std::ostringstream os;
			os << "The value \"" << value <<
				"\" is not valid for type \"" << stringFromType(type) <<
				"\"";
			throw XmlException(XmlException::INVALID_VALUE, os.str());
		}
	}
}

Syntax::Type Value::getSyntaxType(const AnyAtomicType::Ptr &atom)
{
	try {
		return syntaxFromType(typeFromPrimitive(atom->getPrimitiveTypeIndex()));
	}
	catch(...) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Unable to obtain syntax type.");
	}
}

AtomicTypeValue::AtomicTypeValue(bool v)
	: Value(XmlValue::BOOLEAN), value_(v ? s_true : s_false)
{
	setTypeNameFromEnumeration();
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
	setTypeNameFromEnumeration();
}

AtomicTypeValue::AtomicTypeValue(const std::string &v)
	: Value(XmlValue::STRING), value_(v)
{
	setTypeNameFromEnumeration();
}

AtomicTypeValue::AtomicTypeValue(const char *v)
	: Value(XmlValue::STRING)
{
	if(!v) {
		throw XmlException(XmlException::INVALID_VALUE,
			"NULL pointer cannot be used as a value");
	}
	value_.assign(v);
	setTypeNameFromEnumeration();
}

AtomicTypeValue::AtomicTypeValue(XmlValue::Type type, const std::string &v)
	: Value(type), value_(v)
{
	setTypeNameFromEnumeration();
}

AtomicTypeValue::AtomicTypeValue(XmlValue::Type type, const std::string &typeURI,
	const std::string &typeName, const std::string &v)
	: Value(type),
	  typeURI_(typeURI),
	  typeName_(typeName),
	  value_(v)
{
}

void AtomicTypeValue::setTypeNameFromEnumeration()
{
	const DatatypeFactory *factory = Globals::getDatatypeLookup()->
		lookupDatatype(primitiveFromType(getType()));

	if(factory == 0) {
		std::ostringstream os;
		os << "Cannot get datatype validator for an XmlValue type of " << getType();
		throw XmlException(XmlException::INVALID_VALUE, os.str());
	}

	typeURI_ = XMLChToUTF8(factory->getPrimitiveTypeURI()).str();
	typeName_ = XMLChToUTF8(factory->getPrimitiveTypeName()).str();
}

Syntax::Type AtomicTypeValue::convertToSyntaxType(XmlValue::Type valueType)
{
	return syntaxFromType(valueType);
}

XmlValue::Type AtomicTypeValue::convertToValueType(Syntax::Type syntaxType)
{
	return typeFromSyntax(syntaxType);
}

const char *AtomicTypeValue::getTypeString(Syntax::Type syntaxType)
{
	return stringFromType(typeFromSyntax(syntaxType));
}

const char *AtomicTypeValue::getValueTypeString(XmlValue::Type valueType)
{
	return stringFromType(valueType);
}

Syntax::Type AtomicTypeValue::getSyntaxType() const
{
	return syntaxFromType(getType());
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
	case XmlValue::UNTYPED_ATOMIC:
	case XmlValue::STRING: {
		bool numeric = false;
		try {
			numeric = Globals::getDatatypeLookup()->
				lookupDatatype(AnyAtomicType::DOUBLE)->
				checkInstance(UTF8ToXMLCh(value_).str(),
					Globals::defaultMemoryManager);
		}
		catch (XmlException &xe) {
			// will catch transcoding problems
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

void AtomicTypeValue::validate() const
{
	validateType(getType(), typeURI_, typeName_, value_);
}

namespace DbXml {

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

}
