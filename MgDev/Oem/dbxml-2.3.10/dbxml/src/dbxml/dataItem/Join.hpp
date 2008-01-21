//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Join.hpp,v 1.2 2006/10/30 17:45:55 bostic Exp $
//

#ifndef __JOIN_HPP_78390
#define	__JOIN_HPP_78390

#include "DbXmlASTNode.hpp"

#include <xqilla/ast/XQStep.hpp>

#include <vector>

namespace DbXml
{

class Join : public DbXmlASTNode
{
public:
	enum Type {
		ANCESTOR = XQStep::ANCESTOR,
		ANCESTOR_OR_SELF = XQStep::ANCESTOR_OR_SELF,
		ATTRIBUTE = XQStep::ATTRIBUTE,
		CHILD = XQStep::CHILD,
		DESCENDANT = XQStep::DESCENDANT,
		DESCENDANT_OR_SELF = XQStep::DESCENDANT_OR_SELF,
		FOLLOWING = XQStep::FOLLOWING,
		FOLLOWING_SIBLING = XQStep::FOLLOWING_SIBLING,
		NAMESPACE = XQStep::NAMESPACE,
		PARENT = XQStep::PARENT,
		PRECEDING = XQStep::PRECEDING,
		PRECEDING_SIBLING = XQStep::PRECEDING_SIBLING,
		SELF = XQStep::SELF,

		PARENT_A, // Parent of an attribute
		PARENT_C, // Parent of a child (not an attribute)
		ATTRIBUTE_OR_CHILD, // Attributes and children of a node

		NONE // No join
	};

	Join(Type type, bool returnArg, ASTNode *arg, XPath2MemoryManager* memMgr);
	Join(Type type, const Join *join, XPath2MemoryManager* memMgr);

	virtual ASTNode *staticTyping(StaticContext *context);
	virtual DbXmlResult createResult(const DbXmlResult &contextItems, unsigned int props,
		DynamicContext* context) const;

	Type getJoinType() const { return joinType_; }
	bool getReturnArg() const { return returnArg_; }

	const ASTNode *getArgument() const { return arg_; }
	void setArgument(ASTNode *arg) { arg_ = arg; }

	static bool isSupported(Type join);
	static unsigned int getJoinTypeProperties(Type join);
	static Type inverse(Type join);

private:
	Type joinType_;
	bool returnArg_;
	ASTNode *arg_;
};

class DescendantJoin : public DbXmlResultImpl
{
public:
	DescendantJoin(bool orSelf, const DbXmlResult &ancestors, const DbXmlResult &descendants, const LocationInfo *o);

	virtual Item::Ptr next(DynamicContext *context);
	virtual Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual Item::Ptr doJoin(Item::Ptr &descendant, DynamicContext *context);

	bool orSelf_;

	DbXmlResult ancestors_;
	DbXmlResult descendants_;

	Item::Ptr ancestor_;

	enum { INIT, RUNNING, DONE } state_;
};

class ChildJoin : public DescendantJoin
{
public:
	ChildJoin(const DbXmlResult &ancestors, const DbXmlResult &descendants, const LocationInfo *o);

protected:
	virtual Item::Ptr doJoin(Item::Ptr &descendant, DynamicContext *context);

	std::vector<Item::Ptr> ancestorStack_;
};

class AttributeOrChildJoin : public DescendantJoin
{
public:
	AttributeOrChildJoin(const DbXmlResult &ancestors, const DbXmlResult &descendants, const LocationInfo *o);

protected:
	virtual Item::Ptr doJoin(Item::Ptr &descendant, DynamicContext *context);

	std::vector<Item::Ptr> ancestorStack_;
};

class AncestorJoin : public DbXmlResultImpl
{
public:
	AncestorJoin(bool orSelf, const DbXmlResult &descendants, const DbXmlResult &ancestors, const LocationInfo *o);

	virtual Item::Ptr next(DynamicContext *context);
	virtual Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual Item::Ptr doJoin(Item::Ptr &ancestor, DynamicContext *context);

	bool orSelf_;

	DbXmlResult ancestors_;
	DbXmlResult descendants_;

	Item::Ptr descendant_;

	enum { INIT, RUNNING, DONE } state_;
};

class ParentJoin : public AncestorJoin
{
public:
	ParentJoin(const DbXmlResult &descendants, const DbXmlResult &ancestors, const LocationInfo *o);

protected:
	virtual Item::Ptr doJoin(Item::Ptr &ancestor, DynamicContext *context);
};

class ParentCJoin : public AncestorJoin
{
public:
	ParentCJoin(const DbXmlResult &descendants, const DbXmlResult &ancestors, const LocationInfo *o);

protected:
	virtual Item::Ptr doJoin(Item::Ptr &ancestor, DynamicContext *context);
};

class SelfJoin : public DbXmlResultImpl
{
public:
	SelfJoin(const DbXmlResult &left, const DbXmlResult &right, const LocationInfo *o);

	virtual Item::Ptr next(DynamicContext *context);
	virtual Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual Item::Ptr doJoin(Item::Ptr &li, Item::Ptr &ri, DynamicContext *context);

	DbXmlResult left_;
	DbXmlResult right_;

	enum { RUNNING, DONE } state_;
};

class AttributeJoin : public SelfJoin
{
public:
	AttributeJoin(const DbXmlResult &parents, const DbXmlResult &attributes, const LocationInfo *o);

protected:
	virtual Item::Ptr doJoin(Item::Ptr &pa, Item::Ptr &at, DynamicContext *context);
};

class ParentAJoin : public SelfJoin
{
public:
	ParentAJoin(const DbXmlResult &attributes, const DbXmlResult &parents, const LocationInfo *o);

protected:
	virtual Item::Ptr doJoin(Item::Ptr &at, Item::Ptr &pa, DynamicContext *context);
};

}

#endif
