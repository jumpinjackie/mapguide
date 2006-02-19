//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Value.hpp,v 1.55 2005/09/16 19:44:10 gmf Exp $
//

#ifndef __VALUE_H
#define	__VALUE_H

#include <map>
#include <string>
#include <dbxml/XmlException.hpp>
#include <dbxml/XmlValue.hpp>
#include "ReferenceCounted.hpp"
#include "dbxml/XmlDocument.hpp"
#include "Syntax.hpp"
#include "dbxml/XmlValue.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/validators/datatype/DatatypeValidatorFactory.hpp>

#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/Node.hpp>
#include <pathan/AnyAtomicType.hpp>

namespace DbXml
{
class QueryContext;
	
void initTypeConversions(); // in Value.cpp
void terminateTypeConversions(); // in Value.cpp

typedef std::vector<XmlValue> XmlValueVector;

class Value : public ReferenceCounted
{
public:
	static Value *create(const AnyAtomicType::Ptr &atom, DynamicContext *context);
	static Value *create(const Node::Ptr &item, QueryContext &qc);
	static Value *create(XmlValue::Type type, const std::string &v,
			     bool validate = false);
	static Value *create(XmlValue::Type type, const Dbt &dbt,
			     bool validate = false);
	static Item::Ptr convertToItem(const Value *value, DynamicContext *context);
	static Syntax::Type getSyntaxType(const AnyAtomicType::Ptr &atom);
	
	/// Abstract Base Class.
	explicit Value(XmlValue::Type t) : t_(t)
	{}
	virtual ~Value()
	{}
	/// What type is the Value: Document, Variable or one of the simple types
	virtual XmlValue::Type getType() const
	{
		return t_;
	}
	/// What syntax type is the Value
	virtual Syntax::Type getSyntaxType() const
	{
		return Syntax::NONE;
	}
 	/// Return the value as a Number.
	virtual double asNumber() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "Can't convert XmlValue to Number");
		return 0;
	}
	/// Return the value as a String.
	virtual std::string asString(const char *encoding = 0) const
	{
		UNUSED(encoding);
		throw XmlException(
			XmlException::INVALID_VALUE, "Can't convert XmlValue to String");
		return 0;
	}
	/// Return the value as a Boolean.
	virtual bool asBoolean() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "Can't convert XmlValue to Boolean");
		return false;
	}
	/// Return the value as a Node
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *asNode() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "Can't convert XmlValue to Node");
		return 0;
	}
	/// Return the value as a Binary.
	virtual const Dbt & asBinary() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "Can't convert XmlValue to Binary");
	}
	/// Return the value as a Document.
	virtual const XmlDocument &asDocument() const;

	/// Perform type validation of the node
	virtual void validate(
		const XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidatorFactory
		&dvFactory) const {
		UNUSED(dvFactory);
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Can't validate an XmlValue that is not an atomic type");
	}

	/// Compare two values for equality.
	virtual bool equals(const Value &v) const = 0;

	// Lexical representations of special numerical values
	static const std::string NaN_string;
	static const std::string PositiveInfinity_string;
	static const std::string NegativeInfinity_string;
	
protected:
	/// Assign a specific atomic type to the Value
	virtual void setType(XmlValue::Type type) {
		// only allow simple types
		if(type < XmlValue::ANY_SIMPLE_TYPE) {
			throw XmlException(
				XmlException::INVALID_VALUE,
				"The variable value must be of an atomic type.");
		}
		t_ = type;
	}

private:
	// no need for copy and assignment
	Value(const Value &);
	Value &operator = (const Value &);
	XmlValue::Type t_;
};

/**
 * \brief Represents a Node value.
 */
class NodeValue : public Value
{
public:
	/// Construct the Node value from a node list.
	NodeValue(const XmlDocument &d);
	/// Construct the Node value from a node list.
	NodeValue(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n, const XmlDocument &d);
	virtual ~NodeValue();

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getDOMNode() const { return n_; }

	/// Converts the Node to a number.
	virtual double asNumber() const;
	/// Converts the number to a string.
	virtual std::string asString(const char *encoding = 0) const;
	/// Converts the number to a boolean.
	virtual bool asBoolean() const;
	/// Returns the Node value.
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *asNode() const;
	virtual const XmlDocument &asDocument() const;
	virtual bool equals(const Value &v) const;

	/// Returns the node name as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getNodeName() const;
	/// Returns the node value as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getNodeValue() const;
	/// Returns the namespace URI as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getNamespaceURI() const;
	/// Returns the prefix as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getPrefix() const;
	/// Returns the local name as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getLocalName() const;
	/// Returns the node type as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual short getNodeType() const;
	/// Returns the parent node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getParentNode() const;
	/// Returns the first child node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getFirstChild() const;
	/// Returns the last child node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getLastChild() const;
	/// Returns the previous sibling node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getPreviousSibling() const;
	/// Returns the next sibling node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getNextSibling() const;
	/// Returns the attribute nodes if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlResults getAttributes() const;
	/// Returns the owner element if this value is a attribute 
	/// XMLExpection if this value is not a attribute
 	virtual XmlValue getOwnerElement() const;

private:
	// no need for copy and assignment
	NodeValue(const NodeValue &);
	NodeValue & operator = (const NodeValue &);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n_;
	XmlDocument d_;
};

/**
 * \brief Represents an atomic type
 */
class AtomicTypeValue : public Value
{
public:
	// Construct the value from native types
	AtomicTypeValue(bool v);
	AtomicTypeValue(double v);
	AtomicTypeValue(const std::string &v);
	AtomicTypeValue(const char *v);
	
	// Contruct the value from either a type enumeration or type name
	AtomicTypeValue(XmlValue::Type type, const std::string &v);
	AtomicTypeValue(const std::string &primitiveTypeName, const std::string &v);
	
	/// What syntax type is the Value
	virtual Syntax::Type getSyntaxType() const;
	/// Return the value as a Number.
	virtual double asNumber() const;
	/// Return the value as a String.
	virtual std::string asString(const char *encoding = 0) const;
	/// Return the value as a Boolean.
	virtual bool asBoolean() const;
	/// Compare two values for equality.
	virtual bool equals(const Value &v) const;
	/// Perform type validation of the node
	virtual void validate(
		const XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidatorFactory
		&dvFactory) const;

	/// convert a DB XML type to an XML Schema primitive type name
	static const XMLCh *convertToPrimitiveTypeName(XmlValue::Type valueType);
	static Syntax::Type convertToSyntaxType(XmlValue::Type valueType);
	static XmlValue::Type convertToValueType(Syntax::Type syntaxType);
	
private:
	// no need for copy and assignment
	AtomicTypeValue(const AtomicTypeValue &);
	AtomicTypeValue &operator = (const AtomicTypeValue &);
	
	std::string value_;
};

/**
 * \brief Represents a binary value
 */
class BinaryValue : public Value
{
public:
	// Construct the value from native types
	BinaryValue();
	BinaryValue(const Dbt &dbt);
	// Allow construction from string
	BinaryValue(const std::string &content);

	virtual ~BinaryValue();
	
	virtual const Dbt & asBinary() const { return dbt_; }
	/// Compare two values for equality.
	virtual bool equals(const Value &v) const;
private:
	void init(const void *, u_int32_t size);
	Dbt dbt_;
};
	
}

#endif

