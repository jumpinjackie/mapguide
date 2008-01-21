//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlFactoryImpl.hpp,v 1.19.2.1 2007/01/19 14:56:01 jsnelson Exp $
//

#ifndef __DBXMLFACTORYIMPL_HPP
#define	__DBXMLFACTORYIMPL_HPP

#include <dbxml/XmlDocument.hpp>

#include <xqilla/context/ItemFactory.hpp>

#include <xqilla/items/DatatypeLookup.hpp>

#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include "../IndexEntry.hpp"

namespace DbXml
{

class DbXmlFactoryImpl : public ItemFactory
{
public:
	DbXmlFactoryImpl(const DocumentCache* dc, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
	virtual ~DbXmlFactoryImpl();

	virtual void release();

	/* @name Node factory methods */

	void addToNodePool(DbXmlNodeImpl *node) const;

	Node::Ptr createNode(Document *doc, const DynamicContext *context) const;
	Node::Ptr createNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, Document *doc,
		const DynamicContext *context) const;
	Node::Ptr createNode(const IndexEntry::Ptr &ie, Document *doc, const DynamicContext *context) const;
	Node::Ptr createNode(const IndexEntry::Ptr &ie, const Container *container,
		const XMLCh *documentUri, const DynamicContext *context) const;

	virtual Node::Ptr cloneNode(const Node::Ptr node, const DynamicContext *context) const;
	virtual Node::Ptr createTextNode(const XMLCh *value, const DynamicContext *context) const;
	virtual Node::Ptr createCommentNode(const XMLCh *value, const DynamicContext *context) const;
	virtual Node::Ptr createPINode(const XMLCh *name, const XMLCh *value, const DynamicContext *context) const;
	virtual Node::Ptr createAttributeNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name,
		const XMLCh *value, const DynamicContext *context) const;
	virtual Node::Ptr createElementNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name,
		const std::vector<Node::Ptr> &attrList, const std::vector<ItemFactory::ElementChild> &childList,
		const DynamicContext *context) const;
	virtual Node::Ptr createDocumentNode(const std::vector<Node::Ptr> &childList, const DynamicContext *context) const;
	virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* createNamespaceNode(const XMLCh* prefix, const XMLCh* uri,
		const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* parentNode, const DynamicContext *context) const;

	/* @name Atomic type factory methods */

	virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex(const XMLCh* typeURI, const XMLCh* typeName, bool &isPrimitive) const;
	virtual AnyAtomicType::Ptr createDerivedFromAtomicType(AnyAtomicType::AtomicObjectType typeIndex, const XMLCh* value,
		const DynamicContext* context);
	virtual AnyAtomicType::Ptr createDerivedFromAtomicType(AnyAtomicType::AtomicObjectType typeIndex, const XMLCh* typeURI,
		const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
	virtual AnyAtomicType::Ptr createDerivedFromAtomicType(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	/* @name Number factory methods */

	virtual ATDoubleOrDerived::Ptr createDouble(const MAPM value, const DynamicContext* context);
	virtual ATDoubleOrDerived::Ptr createDouble(const XMLCh* value, const DynamicContext* context);
	virtual ATDoubleOrDerived::Ptr createDoubleOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context);
	virtual ATDoubleOrDerived::Ptr createDoubleOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	virtual ATFloatOrDerived::Ptr createFloat(const MAPM value, const DynamicContext* context);
	virtual ATFloatOrDerived::Ptr createFloat(const XMLCh* value, const DynamicContext* context);
	virtual ATFloatOrDerived::Ptr createFloatOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context);
	virtual ATFloatOrDerived::Ptr createFloatOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
	
	virtual ATDecimalOrDerived::Ptr createInteger(const int value, const DynamicContext* context);
	virtual ATDecimalOrDerived::Ptr createInteger(const MAPM value, const DynamicContext* context);
	virtual ATDecimalOrDerived::Ptr createInteger(const XMLCh* value, const DynamicContext* context);
	virtual ATDecimalOrDerived::Ptr createNonNegativeInteger(const MAPM value, const DynamicContext* context);

	virtual ATDecimalOrDerived::Ptr createDecimal(const MAPM value, const DynamicContext* context);
	virtual ATDecimalOrDerived::Ptr createDecimal(const XMLCh* value, const DynamicContext* context);
	virtual ATDecimalOrDerived::Ptr createDecimalOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context);
	virtual ATDecimalOrDerived::Ptr createDecimalOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	/* @name Date / time factory methods */

	virtual ATDateOrDerived::Ptr createDate(const XMLCh* value, const DynamicContext* context);
	virtual ATDateOrDerived::Ptr createDateOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	virtual ATDateTimeOrDerived::Ptr createDateTime(const XMLCh* value, const DynamicContext* context);
	virtual ATDateTimeOrDerived::Ptr createDateTimeOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	virtual ATTimeOrDerived::Ptr createTime(const XMLCh* value, const DynamicContext* context);
	virtual ATTimeOrDerived::Ptr createTimeOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	virtual ATGDayOrDerived::Ptr createGDayOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
	virtual ATGMonthDayOrDerived::Ptr createGMonthDayOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
	virtual ATGMonthOrDerived::Ptr createGMonthOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
	virtual ATGYearMonthOrDerived::Ptr createGYearMonthOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
	virtual ATGYearOrDerived::Ptr createGYearOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	virtual ATDurationOrDerived::Ptr createDayTimeDuration(const XMLCh* value, const DynamicContext* context);
	virtual ATDurationOrDerived::Ptr createDayTimeDuration(const MAPM &seconds, const DynamicContext* context);
	virtual ATDurationOrDerived::Ptr createYearMonthDuration(const XMLCh* value, const DynamicContext* context);
	virtual ATDurationOrDerived::Ptr createYearMonthDuration(const MAPM &seconds, const DynamicContext* context);
	virtual ATDurationOrDerived::Ptr createDurationOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	/* @name Other factory methods */

	virtual ATStringOrDerived::Ptr createString(const XMLCh* value, const DynamicContext* context);
	virtual ATStringOrDerived::Ptr createStringOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	virtual ATUntypedAtomic::Ptr createUntypedAtomic(const XMLCh* value, const DynamicContext* context);

	virtual ATBooleanOrDerived::Ptr createBoolean(bool value, const DynamicContext* context);
	virtual ATBooleanOrDerived::Ptr createBoolean(const XMLCh* value, const DynamicContext* context);
	virtual ATBooleanOrDerived::Ptr createBooleanOrDerived(const XMLCh* typeURI, const XMLCh* typeName, bool value, const DynamicContext* context);
	virtual ATBooleanOrDerived::Ptr createBooleanOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

	virtual ATAnyURIOrDerived::Ptr createAnyURI(const XMLCh* value, const DynamicContext* context);

	virtual ATQNameOrDerived::Ptr createQName(const XMLCh* uri, const XMLCh* prefix, const XMLCh* name, const DynamicContext* context);
	virtual ATQNameOrDerived::Ptr createQNameOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* uri, const XMLCh* prefix,
		const XMLCh* name, const DynamicContext* context);

private:
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getOutputDocument(const DynamicContext *context) const;

	DatatypeLookup datatypeLookup_;
	mutable XmlDocument outputDocument_;
	XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr_;

	mutable DbXmlNodeImpl *pooledNodes_;
	mutable DbXmlNodeImpl *allocatedNodes_;
};

}

#endif
