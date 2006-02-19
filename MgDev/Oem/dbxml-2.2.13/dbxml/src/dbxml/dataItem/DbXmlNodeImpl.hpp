//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlNodeImpl.hpp,v 1.18 2005/12/01 13:48:11 jsnelson Exp $
//

#ifndef __DBXMLNODEIMPL_HPP
#define	__DBXMLNODEIMPL_HPP

#include <dbxml/XmlDocument.hpp>

#include <pathan/internal/navigation/Axis.hpp>
#include <pathan/Node.hpp>

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

class DbXmlNodeImpl : public Node, private AxisNodeFactory
{
public:
	typedef RefCountPointer<const DbXmlNodeImpl> Ptr;

	/** The "DbXmlNodeImpl" node interface */
	static const XMLCh gDbXml[];

	DbXmlNodeImpl(Document *doc, const DynamicContext *context, const DbXmlFactoryImpl *factory);
	DbXmlNodeImpl(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, Document *doc, const DynamicContext *context, const DbXmlFactoryImpl *factory);
	virtual ~DbXmlNodeImpl();

	virtual void decrementRefCount() const;

	void init(Document *doc);
	void init(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, Document *doc);

	void setFactory(const DbXmlFactoryImpl *factory) { factory_ = factory; }
	void setIndexEntry(const IndexEntry::SharedPtr &ie) { ie_ = ie; }

	Item::Ptr getMetaData(const XMLCh *uri, const XMLCh *name, DynamicContext *context) const;

	virtual bool hasElemChildren() const;
	virtual u_int32_t getLevel() const;
	virtual const nid_t *getNID() const;
	// Returns 0 if unknown (never fetches)
	virtual const nid_t *getLastElemDescendantNID() const;
	virtual const nid_t *getLastElemChildNID(bool fetch) const;
	virtual DbXmlNodeImpl::Ptr getLastElemChild(const DynamicContext *context) const;
	virtual DbXmlNodeImpl::Ptr getAttribute(u_int32_t index, const DynamicContext *context) const;

	virtual DbXmlNodeImpl::Ptr getDescendantElement(const IndexEntry::SharedPtr &ie, const DynamicContext *context) const;

	virtual bool isNode() const { return true; }
	virtual bool isAtomicValue() const { return false; }

	virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getDOMNode() const;
	virtual const XmlDocument &getXmlDocument() const { return document_; }

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
	virtual Sequence dmTypedValue(const DynamicContext* context) const;

	virtual bool lessThan(const Node::Ptr &other, const DynamicContext *context) const;
	virtual bool equals(const Node::Ptr &other) const;
	virtual bool uniqueLessThan(const Node::Ptr &other, const DynamicContext *context) const;

	virtual Node::Ptr dmParent(const DynamicContext* context) const;
	virtual Result dmAttributes(const DynamicContext* context) const;
	virtual Result dmNamespaceNodes(const DynamicContext* context) const;
	virtual Result dmChildren(const DynamicContext *context) const;
	virtual Result getAxisResult(DataItemStep::Axis axis, const NodeTest *nodeTest, const DynamicContext *context) const;

	virtual const XMLCh* getTypeURI() const;
	virtual const XMLCh* getTypeName() const;

private:
	virtual const Node::Ptr createNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const DynamicContext *context) const;

protected:
	const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node_;
	IndexEntry::SharedPtr ie_;
	XmlDocument document_;

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
	virtual const nid_t *getNID() const { return 0; }
	// Returns 0 if unknown (never fetches)
	virtual const nid_t *getLastElemDescendantNID() const { return 0; }
	virtual const nid_t *getLastElemChildNID(bool fetch) const { return 0; }
	virtual DbXmlNodeImpl::Ptr getLastElemChild(const DynamicContext *context) const { return 0; }
	virtual DbXmlNodeImpl::Ptr getAttribute(u_int32_t index, const DynamicContext *context) const { return 0; }

	virtual DbXmlNodeImpl::Ptr getDescendantElement(const IndexEntry::SharedPtr &ie, const DynamicContext *context) const { return 0; }

	virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getDOMNode() const { return 0; }

	virtual const XMLCh* asString(const DynamicContext* context) const { return 0; }

	virtual const XMLCh* dmNodeKind() const { return namespace_string; }

	virtual ATQNameOrDerived::Ptr dmTypeName(const DynamicContext* context) const { return 0; }
	virtual ATBooleanOrDerived::Ptr dmNilled(const DynamicContext* context) const { return 0; }
	virtual ATBooleanOrDerived::Ptr dmIsId(const DynamicContext* context) const { return 0; }
	virtual ATBooleanOrDerived::Ptr dmIsIdRefs(const DynamicContext* context) const { return 0; }

	virtual ATQNameOrDerived::Ptr dmNodeName(const DynamicContext* context) const;
	virtual const XMLCh* dmStringValue(const DynamicContext* context) const;
	virtual Sequence dmTypedValue(const DynamicContext* context) const;

	virtual bool lessThan(const Node::Ptr &other, const DynamicContext *context) const { return false; }
	virtual bool equals(const Node::Ptr &other) const { return false; }
	virtual bool uniqueLessThan(const Node::Ptr &other, const DynamicContext *context) const { return false; }

	virtual Node::Ptr dmParent(const DynamicContext* context) const;
	virtual Result dmAttributes(const DynamicContext* context) const { return 0; }
	virtual Result dmNamespaceNodes(const DynamicContext* context) const { return 0; }
	virtual Result dmChildren(const DynamicContext *context) const { return 0; }
	virtual Result getAxisResult(DataItemStep::Axis axis, const NodeTest *nodeTest, const DynamicContext *context) const { return 0; }

	virtual const XMLCh* getTypeURI() const { return 0; }
	virtual const XMLCh* getTypeName() const { return 0; }

private:
	const XMLCh *prefix_;
	const XMLCh *uri_;
	const NsDomElement *parent_;
};

class DbXmlAxis : public ResultImpl
{
public:
	DbXmlAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
	Item::Ptr next(DynamicContext *context);

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
	DbXmlAttributeAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomNamedNodeMap *nodeMap_;
	int i_;
};

class DbXmlNamespaceAxis : public ResultImpl
{
public:
	DbXmlNamespaceAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
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
};

class DbXmlChildAxis : public DbXmlAxis
{
public:
	DbXmlChildAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomNode *child_;
};

class ElementChildAxis : public DbXmlAxis
{
public:
	ElementChildAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomElement *child_;
};

class DbXmlDescendantAxis : public DbXmlAxis
{
public:
	DbXmlDescendantAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomNode *descendant_;
};

class ElementDescendantAxis : public DbXmlAxis
{
public:
	ElementDescendantAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomElement *nscontext_;
	NsDomElement *descendant_;
};

class DbXmlDescendantOrSelfAxis : public DbXmlAxis
{
public:
	DbXmlDescendantOrSelfAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomNode *descendant_;
};

class ElementDescendantOrSelfAxis : public DbXmlAxis
{
public:
	ElementDescendantOrSelfAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
private:
	NsDomElement *nscontext_;
	NsDomElement *descendant_;
};

class DbXmlParentAxis : public DbXmlAxis
{
public:
	DbXmlParentAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
		DynamicContext *context);
	const NsDomNode *nextNode();
	std::string asString(DynamicContext *context, int indent) const;
};



}

#endif
