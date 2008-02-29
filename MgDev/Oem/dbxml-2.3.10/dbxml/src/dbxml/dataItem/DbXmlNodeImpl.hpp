//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlNodeImpl.hpp,v 1.33.2.1 2007/01/19 14:56:01 jsnelson Exp $
//

#ifndef __DBXMLNODEIMPL_HPP
#define	__DBXMLNODEIMPL_HPP

#include <dbxml/XmlDocument.hpp>

#include <xqilla/axis/Axis.hpp>
#include <xqilla/items/Node.hpp>

#include "DbXmlResult.hpp"
#include "../IndexEntry.hpp"
#include "../nodeStore/NsTypes.hpp"

namespace DbXml
{

class NsDomNode;
class NsDomElement;
class NsDomNamedNodeMap;
class NsDomAttr;
class QueryContext;
class Value;
class DbXmlNodeTest;
class DbXmlFactoryImpl;
class NsNid;
	
class DbXmlNodeImpl : public Node, private AxisNodeFactory
{
public:
	typedef RefCountPointer<const DbXmlNodeImpl> Ptr;

	/** The "DbXmlNodeImpl" node interface */
	static const XMLCh gDbXml[];

	DbXmlNodeImpl(Document *doc, const DynamicContext *context, const DbXmlFactoryImpl *factory);
	DbXmlNodeImpl(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, Document *doc, const DynamicContext *context,
		const DbXmlFactoryImpl *factory);
	DbXmlNodeImpl(const IndexEntry::Ptr &ie, Document *doc, const DynamicContext *context,
		const DbXmlFactoryImpl *factory);
	DbXmlNodeImpl(const IndexEntry::Ptr &ie, const Container *container, const XMLCh *documentUri,
		const DynamicContext *context, const DbXmlFactoryImpl *factory);
	virtual ~DbXmlNodeImpl();

	virtual void decrementRefCount() const;

	void init(Document *doc);
	void init(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, Document *doc);
	void init(const IndexEntry::Ptr &ie, Document *doc);
	void init(const IndexEntry::Ptr &ie, const Container *container, const XMLCh *documentUri,
		const DynamicContext *context);

	void setFactory(const DbXmlFactoryImpl *factory) { factory_ = factory; }
	void setIndexEntry(const IndexEntry::Ptr &ie) { ie_ = ie; }

	short getNodeType() const;

	Item::Ptr getMetaData(const XMLCh *uri, const XMLCh *name, DynamicContext *context) const;

	int isSameNID(const DbXmlNodeImpl *other) const;
	int isDescendantOf(const DbXmlNodeImpl *parent, bool orSelf) const;

	virtual u_int32_t getLevel() const;
	virtual TransactedContainer *getContainer() const;
	virtual DocID getDocID() const;
	virtual const NsNid *getNID() const;
	virtual int getIndex() const;
	virtual bool isLeadingText() const;
	virtual bool isChildText() const;
	// Returns 0 if unknown (never fetches)
	virtual const NsNid *getLastElemDescendantNID() const;
	virtual DbXmlNodeImpl::Ptr getAttribute(u_int32_t index, const DynamicContext *context) const;

	virtual DbXmlNodeImpl::Ptr getDescendantElement(const IndexEntry::Ptr &ie, const DynamicContext *context) const;
	virtual std::string getNodeHandle() const;

	virtual bool isNode() const { return true; }
	virtual bool isAtomicValue() const { return false; }

	virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getDOMNode() const;
	virtual const XmlDocument &getXmlDocument() const;

	virtual void *getInterface(const XMLCh *name) const;

	virtual const XMLCh* asString(const DynamicContext* context) const;

	virtual Sequence dmBaseURI(const DynamicContext* context) const;
	virtual Sequence dmDocumentURI(const DynamicContext* context) const;

	virtual const XMLCh* dmNodeKind() const;

	virtual ATQNameOrDerived::Ptr dmTypeName(const DynamicContext* context) const;
	virtual ATBooleanOrDerived::Ptr dmNilled(const DynamicContext* context) const;
	virtual ATBooleanOrDerived::Ptr dmIsId(const DynamicContext* context) const;
	virtual ATBooleanOrDerived::Ptr dmIsIdRefs(const DynamicContext* context) const;
	virtual bool hasInstanceOfType(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const;

	virtual ATQNameOrDerived::Ptr dmNodeName(const DynamicContext* context) const;
	virtual const XMLCh* dmStringValue(const DynamicContext* context) const;
	virtual Sequence dmTypedValue(DynamicContext* context) const;

	virtual int compare(const DbXmlNodeImpl *other) const;
	virtual bool lessThan(const Node::Ptr &other, const DynamicContext *context) const;
	virtual bool equals(const Node::Ptr &other) const;
	virtual bool uniqueLessThan(const Node::Ptr &other, const DynamicContext *context) const;

	virtual Node::Ptr dmParent(const DynamicContext* context) const;
	virtual Result dmAttributes(const DynamicContext* context, const LocationInfo *location) const;
	virtual Result dmNamespaceNodes(const DynamicContext* context, const LocationInfo *location) const;
	virtual Result dmChildren(const DynamicContext *context, const LocationInfo *location) const;
	virtual Result getAxisResult(XQStep::Axis axis, const NodeTest *nodeTest, const DynamicContext *context, const LocationInfo *location) const;
	virtual DbXmlResult getAxisDbXmlResult(XQStep::Axis axis, const NodeTest *nodeTest, const DynamicContext *context, const LocationInfo *location) const;

	virtual const XMLCh* getTypeURI() const;
	virtual const XMLCh* getTypeName() const;

private:
	virtual const Node::Ptr createNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const DynamicContext *context) const;

protected:
	// Before materialisation
	IndexEntry::Ptr ie_;
	const Container *container_;
	QueryContext *qc_;
	const XMLCh *documentUri_;

	// After materialisation
	XmlDocument document_;
	const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node_;

	// So we can go back in the factory pool
	const DbXmlFactoryImpl *factory_;
public:
	// For linking the nodes in the factory pool
	DbXmlNodeImpl *poolNext;
	DbXmlNodeImpl *allocNext;

};

/// A (hacky) namespace node, that does as much as we need for XQuery.
/// This may need to be changed if we start supporting XPath 2.
class DbXmlNamespaceNodeImpl : public DbXmlNodeImpl
{
public:
	typedef RefCountPointer<const DbXmlNamespaceNodeImpl> Ptr;

	DbXmlNamespaceNodeImpl(const XMLCh *prefix, const XMLCh *uri, NsDomElement *parent, Document *doc, const DynamicContext *context);
	~DbXmlNamespaceNodeImpl() {}

	const XMLCh *getPrefix() const { return prefix_; }

	virtual bool hasElemChildren() const { return false; }
	virtual u_int32_t getLevel() const { return 0; }
	virtual const NsNid *getNID() const { return 0; }
	virtual int getIndex() const { return -1; }
	virtual bool isLeadingText() const { return false; }
	// Returns 0 if unknown (never fetches)
	virtual const NsNid *getLastElemDescendantNID() const { return 0; }
	virtual DbXmlNodeImpl::Ptr getAttribute(u_int32_t index, const DynamicContext *context) const { return 0; }

	virtual DbXmlNodeImpl::Ptr getDescendantElement(const IndexEntry::Ptr &ie, const DynamicContext *context) const { return 0; }

	virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getDOMNode() const { return 0; }

	virtual const XMLCh* asString(const DynamicContext* context) const { return 0; }

	virtual Sequence dmBaseURI(const DynamicContext* context) const;
	virtual Sequence dmDocumentURI(const DynamicContext* context) const;

	virtual const XMLCh* dmNodeKind() const { return namespace_string; }

	virtual ATQNameOrDerived::Ptr dmTypeName(const DynamicContext* context) const { return 0; }
	virtual ATBooleanOrDerived::Ptr dmNilled(const DynamicContext* context) const { return 0; }
	virtual ATBooleanOrDerived::Ptr dmIsId(const DynamicContext* context) const { return 0; }
	virtual ATBooleanOrDerived::Ptr dmIsIdRefs(const DynamicContext* context) const { return 0; }

	virtual ATQNameOrDerived::Ptr dmNodeName(const DynamicContext* context) const;
	virtual const XMLCh* dmStringValue(const DynamicContext* context) const;
	virtual Sequence dmTypedValue(const DynamicContext* context) const;

	virtual int compare(const Node::Ptr &o) const { return 0; }
	virtual bool lessThan(const Node::Ptr &other, const DynamicContext *context) const { return false; }
	virtual bool equals(const Node::Ptr &other) const { return false; }
	virtual bool uniqueLessThan(const Node::Ptr &other, const DynamicContext *context) const { return false; }

	virtual Node::Ptr dmParent(const DynamicContext* context) const;
	virtual Result dmAttributes(const DynamicContext* context) const { return 0; }
	virtual Result dmNamespaceNodes(const DynamicContext* context) const { return 0; }
	virtual Result dmChildren(const DynamicContext *context) const { return 0; }
	virtual Result getAxisResult(XQStep::Axis axis, const NodeTest *nodeTest, const DynamicContext *context) const { return 0; }

	virtual const XMLCh* getTypeURI() const { return 0; }
	virtual const XMLCh* getTypeName() const { return 0; }

private:
	const XMLCh *prefix_;
	const XMLCh *uri_;
	const NsDomElement *parent_;
};

class DbXmlAxis : public DbXmlResultImpl
{
public:
	DbXmlAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr next(DynamicContext *context);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context)
	{
		// This is a sufficient implementation for now
		return next(context);
	}

	virtual const NsDomNode *nextNode() = 0;

protected:
	DbXmlNodeImpl::Ptr nodeObj_;

	NsDomNode *contextNode_;
	const DbXmlNodeTest *nodeTest_;
	bool toDo_;
};

class DbXmlAttributeAxis : public DbXmlAxis
{
public:
	DbXmlAttributeAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomNamedNodeMap *nodeMap_;
	int i_;
};

class DbXmlNamespaceAxis : public ResultImpl
{
public:
	DbXmlNamespaceAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr next(DynamicContext *context);
	DbXmlNamespaceNodeImpl::Ptr nextNode(DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
private:
	DbXmlNodeImpl::Ptr nodeObj_;

	NsDomNode *contextNode_;
	const DbXmlNodeTest *nodeTest_;
	bool toDo_;

	NsDomElement *node_;
	NsDomNamedNodeMap *nodeMap_;
	int i_;

	typedef std::set<const XMLCh*, XMLChSort> DoneSet;
	DoneSet done_;
	bool defNsTested_;
};

class DbXmlAttributeOrChildAxis : public DbXmlAxis
{
public:
	DbXmlAttributeOrChildAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode,
		const DbXmlNodeTest *nodeTest);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomNamedNodeMap *nodeMap_;
	int i_;
	NsDomNode *child_;
};

class DbXmlChildAxis : public DbXmlAxis
{
public:
	DbXmlChildAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomNode *child_;
};

class ElementChildAxis : public DbXmlAxis
{
public:
	ElementChildAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomElement *child_;
};

class DbXmlDescendantAxis : public DbXmlAxis
{
public:
	DbXmlDescendantAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomNode *descendant_;
};

class ElementDescendantAxis : public DbXmlAxis
{
public:
	ElementDescendantAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomElement *nscontext_;
	NsDomElement *descendant_;
};

class DbXmlDescendantOrSelfAxis : public DbXmlAxis
{
public:
	DbXmlDescendantOrSelfAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomNode *descendant_;
};

class ElementDescendantOrSelfAxis : public DbXmlAxis
{
public:
	ElementDescendantOrSelfAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomElement *nscontext_;
	NsDomElement *descendant_;
};

class DbXmlParentAxis : public DbXmlAxis
{
public:
	DbXmlParentAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
};



}

#endif
