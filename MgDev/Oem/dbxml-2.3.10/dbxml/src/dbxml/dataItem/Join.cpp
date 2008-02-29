//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Join.cpp,v 1.4 2006/10/30 17:45:55 bostic Exp $
//

#include "Join.hpp"
#include "DbXmlNodeImpl.hpp"
#include "DbXmlNav.hpp"

#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

Join::Join(Type type, bool returnArg, ASTNode *arg, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(JOIN, memMgr),
	  joinType_(type),
	  returnArg_(returnArg),
	  arg_(arg)
{
	staticTyping(0);
}

Join::Join(Type type, const Join *join, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(JOIN, memMgr),
	  joinType_(type),
	  returnArg_(join->returnArg_),
	  arg_(join->arg_)
{
	staticTyping(0);
}

ASTNode *Join::staticTyping(StaticContext *context)
{
	_src.clear();

	if(context != 0)
		arg_ = arg_->staticTyping(context);
	_src.add(arg_->getStaticResolutionContext());
	_src.contextItemUsed(true);

	Type join = joinType_;
	StaticType contextType = StaticType::NODE_TYPE;
	if(context != 0) contextType = context->getContextItemType();

	if(returnArg_) {
		_src.setProperties(Join::getJoinTypeProperties(joinType_));
		_src.getStaticType() = arg_->getStaticResolutionContext().getStaticType();
	} else {
		_src.setProperties(Join::getJoinTypeProperties(Join::SELF));
		_src.getStaticType() = contextType;

		join = inverse(join);
		contextType = arg_->getStaticResolutionContext().getStaticType();
	}

	switch(join) {
	case Join::SELF:
	case Join::DESCENDANT_OR_SELF:
	case Join::ANCESTOR_OR_SELF:
		_src.getStaticType().typeIntersect(contextType);
		break;
	case Join::ATTRIBUTE:
		_src.getStaticType().typeIntersect(StaticType::ATTRIBUTE_TYPE);
		break;
	case Join::NAMESPACE:
		_src.getStaticType().typeIntersect(StaticType::NAMESPACE_TYPE);
		break;
	case Join::ATTRIBUTE_OR_CHILD:
		_src.getStaticType().typeIntersect(StaticType::ELEMENT_TYPE | StaticType::TEXT_TYPE | StaticType::PI_TYPE |
			StaticType::COMMENT_TYPE | StaticType::ATTRIBUTE_TYPE);
		break;
	case Join::CHILD:
	case Join::DESCENDANT:
	case Join::FOLLOWING:
	case Join::FOLLOWING_SIBLING:
	case Join::PRECEDING:
	case Join::PRECEDING_SIBLING:
		_src.getStaticType().typeIntersect(StaticType::ELEMENT_TYPE | StaticType::TEXT_TYPE | StaticType::PI_TYPE |
			StaticType::COMMENT_TYPE);
		break;
	case Join::ANCESTOR:
	case Join::PARENT:
	case Join::PARENT_C:
		_src.getStaticType().typeIntersect(StaticType::DOCUMENT_TYPE | StaticType::ELEMENT_TYPE);
		break;
	case Join::PARENT_A:
		_src.getStaticType().typeIntersect(StaticType::ELEMENT_TYPE);
		break;
	case Join::NONE:
		break;
	}

	return this;
}

DbXmlResult Join::createResult(const DbXmlResult &contextItems, unsigned int props, DynamicContext* context) const
{
	DbXmlResult left = DbXmlNav::sort(contextItems, props, this, context);
	if(left.isNull()) left = new XQContextItem::ContextItemResult(this);

	DbXmlResult right = createDbXmlResult(arg_, context);
	unsigned int rprops = arg_->getStaticResolutionContext().getProperties();
	right = DbXmlNav::sort(right, rprops, this, context);

	Type join = joinType_;
	if(!returnArg_) {
		join = inverse(join);
		DbXmlResult tmp = left;
		left = right;
		right = tmp;
	}

	switch(join) {
	case ANCESTOR:
		return new AncestorJoin(/*orSelf*/false, left, right, this);
	case ANCESTOR_OR_SELF:
		return new AncestorJoin(/*orSelf*/true, left, right, this);
	case ATTRIBUTE:
		return new AttributeJoin(left, right, this);
	case CHILD:
		return new ChildJoin(left, right, this);
	case DESCENDANT:
		return new DescendantJoin(/*orSelf*/false, left, right, this);
	case DESCENDANT_OR_SELF:
		return new DescendantJoin(/*orSelf*/true, left, right, this);
	case PARENT:
		return new ParentJoin(left, right, this);
	case SELF:
		return new SelfJoin(left, right, this);
	case PARENT_A:
		return new ParentAJoin(left, right, this);
	case PARENT_C:
		return new ParentCJoin(left, right, this);
	case ATTRIBUTE_OR_CHILD:
		return new AttributeOrChildJoin(left, right, this);
	case NAMESPACE:
	case FOLLOWING:
	case FOLLOWING_SIBLING:
	case PRECEDING:
	case PRECEDING_SIBLING:
	case NONE:
		break;
	}

	DBXML_ASSERT(false);
	return contextItems;
}

unsigned int Join::getJoinTypeProperties(Type join)
{
	unsigned int properties = 0;
	// properties depend on the axis of the step
	switch (join) {
	case SELF:
		properties |= StaticResolutionContext::ONENODE | StaticResolutionContext::SELF;
		// Fall through
	case ATTRIBUTE_OR_CHILD:
	case CHILD:
	case ATTRIBUTE:
	case NAMESPACE:
		properties |= StaticResolutionContext::SUBTREE | StaticResolutionContext::PEER |
			StaticResolutionContext::DOCORDER;
		break;
	case DESCENDANT:
	case DESCENDANT_OR_SELF:
		properties |= StaticResolutionContext::SUBTREE | StaticResolutionContext::DOCORDER;
		break;
	case FOLLOWING_SIBLING:
		properties |= StaticResolutionContext::DOCORDER;
		// Fall through
	case PRECEDING_SIBLING:
		properties |= StaticResolutionContext::PEER;
		break;
	case PARENT_A:
	case PARENT_C:
	case PARENT:
		properties |= StaticResolutionContext::PEER | StaticResolutionContext::ONENODE |
			StaticResolutionContext::DOCORDER;
		break;
	case FOLLOWING:
		properties |= StaticResolutionContext::DOCORDER;
		break;
	case ANCESTOR:
	case ANCESTOR_OR_SELF:
	case PRECEDING:
	case NONE:
		break;
	}

	properties |= StaticResolutionContext::GROUPED | StaticResolutionContext::SAMEDOC;
	return properties;
}

bool Join::isSupported(Type join)
{
	switch(join) {
	case ANCESTOR:
	case ANCESTOR_OR_SELF:
	case ATTRIBUTE:
	case CHILD:
	case DESCENDANT:
	case DESCENDANT_OR_SELF:
	case PARENT:
	case SELF:
	case PARENT_A:
	case PARENT_C:
	case ATTRIBUTE_OR_CHILD:
		return true;
	case NAMESPACE:
	case FOLLOWING:
	case FOLLOWING_SIBLING:
	case PRECEDING:
	case PRECEDING_SIBLING:
	case NONE:
		break;
	}

	return false;
}

Join::Type Join::inverse(Type join)
{
  switch(join) {
  case Join::ANCESTOR:           return Join::DESCENDANT;
  case Join::DESCENDANT:         return Join::ANCESTOR;

  case Join::ANCESTOR_OR_SELF:   return Join::DESCENDANT_OR_SELF;
  case Join::DESCENDANT_OR_SELF: return Join::ANCESTOR_OR_SELF;

  case Join::PARENT:             return Join::ATTRIBUTE_OR_CHILD;
  case Join::ATTRIBUTE_OR_CHILD: return Join::PARENT;

  case Join::PRECEDING:          return Join::FOLLOWING;
  case Join::FOLLOWING:          return Join::PRECEDING;

  case Join::PRECEDING_SIBLING:  return Join::FOLLOWING_SIBLING;
  case Join::FOLLOWING_SIBLING:  return Join::PRECEDING_SIBLING;

  case Join::CHILD:              return Join::PARENT_C;
  case Join::PARENT_C:           return Join::CHILD;

  case Join::ATTRIBUTE:          return Join::PARENT_A;
  case Join::PARENT_A:           return Join::ATTRIBUTE;

  case Join::SELF:               return Join::SELF;

  case Join::NONE:
  case Join::NAMESPACE:
	  break;
  }

  return Join::NONE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DescendantJoin::DescendantJoin(bool orSelf, const DbXmlResult &ancestors, const DbXmlResult &descendants,
	const LocationInfo *o)
	: DbXmlResultImpl(o),
	  orSelf_(orSelf),
	  ancestors_(ancestors),
	  descendants_(descendants),
	  state_(INIT)
{
}

Item::Ptr DescendantJoin::next(DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		if((ancestor_ = ancestors_->next(context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		DbXmlNodeImpl *an = (DbXmlNodeImpl*)ancestor_.get();
		Item::Ptr descendant;
		if((descendant = descendants_->seek(an->getDocID(), *an->getNID(),
			    context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(descendant, context);
	}
	case RUNNING: {
		Item::Ptr descendant;
		if((descendant = descendants_->next(context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(descendant, context);
	}
	case DONE: break;
	}
	return 0;
}

Item::Ptr DescendantJoin::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		Item::Ptr descendant;
		if((descendant = descendants_->seek(did, nid, context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		if((ancestor_ = ancestors_->next(context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(descendant, context);
	}
	case RUNNING: {
		Item::Ptr descendant;
		if((descendant = descendants_->seek(did, nid, context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(descendant, context);
	}
	case DONE: break;
	}
	return 0;
}

Item::Ptr DescendantJoin::doJoin(Item::Ptr &descendant, DynamicContext *context)
{
	while(state_ != DONE) {
		context->testInterrupt();

		DbXmlNodeImpl *de = (DbXmlNodeImpl*)descendant.get();
		DbXmlNodeImpl *an = (DbXmlNodeImpl*)ancestor_.get();

		int cmp = de->isDescendantOf(an, orSelf_);
		if(cmp < 0) {
			if((descendant = descendants_->seek(an->getDocID(), *an->getNID(),
				    context)).isNull()) state_ = DONE;
		} else if(cmp > 0) {
			if((ancestor_ = ancestors_->seek(an->getDocID(), *an->getLastElemDescendantNID(),
				    context)).isNull()) state_ = DONE;
		} else {
			return descendant;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ChildJoin::ChildJoin(const DbXmlResult &ancestors, const DbXmlResult &descendants, const LocationInfo *o)
	: DescendantJoin(/*orSelf*/false, ancestors, descendants, o)
{
}

Item::Ptr ChildJoin::doJoin(Item::Ptr &descendant, DynamicContext *context)
{
	while(state_ != DONE) {
		context->testInterrupt();

		DbXmlNodeImpl *de = (DbXmlNodeImpl*)descendant.get();
		DbXmlNodeImpl *an = (DbXmlNodeImpl*)ancestor_.get();

		int cmp = de->isDescendantOf(an, orSelf_);
		if(cmp < 0) {
			if(ancestorStack_.empty()) {
				if((descendant = descendants_->seek(an->getDocID(), *an->getNID(),
					    context)).isNull()) state_ = DONE;
			} else {
				an = (DbXmlNodeImpl*)ancestorStack_.back().get();
				if(an->getLevel() == (de->getLevel() - 1)) {
					return descendant;
				}

				if((descendant = descendants_->next(context)).isNull())
					state_ = DONE;
			}
		} else if(cmp > 0) {
			if(ancestorStack_.empty()) {
				if((ancestor_ = ancestors_->seek(an->getDocID(), *an->getLastElemDescendantNID(),
					    context)).isNull()) state_ = DONE;
			} else {
				ancestor_ = ancestorStack_.back();
				ancestorStack_.pop_back();
			}
		} else {
			ancestorStack_.push_back(ancestor_);
			if((ancestor_ = ancestors_->next(context)).isNull()) {
				ancestor_ = ancestorStack_.back();
				ancestorStack_.pop_back();

				if(an->getLevel() == (de->getLevel() - 1)) {
					return descendant;
				}

				if((descendant = descendants_->next(context)).isNull())
					state_ = DONE;
			}
		}
	}

	ancestorStack_.clear();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AttributeOrChildJoin::AttributeOrChildJoin(const DbXmlResult &ancestors, const DbXmlResult &descendants,
	const LocationInfo *o)
	: DescendantJoin(/*orSelf*/true, ancestors, descendants, o)
{
}

Item::Ptr AttributeOrChildJoin::doJoin(Item::Ptr &descendant, DynamicContext *context)
{
	while(state_ != DONE) {
		context->testInterrupt();

		DbXmlNodeImpl *de = (DbXmlNodeImpl*)descendant.get();
		DbXmlNodeImpl *an = (DbXmlNodeImpl*)ancestor_.get();

		int cmp = de->isDescendantOf(an, orSelf_);
		if(cmp < 0) {
			if(ancestorStack_.empty()) {
				if((descendant = descendants_->seek(an->getDocID(), *an->getNID(),
					    context)).isNull()) state_ = DONE;
			} else {
				an = (DbXmlNodeImpl*)ancestorStack_.back().get();
				if(an->getLevel() == (de->getLevel() - 1) ||
					(de->getNodeType() == DOMNode::ATTRIBUTE_NODE && an->getLevel() == de->getLevel())) {
					return descendant;
				}

				if((descendant = descendants_->next(context)).isNull())
					state_ = DONE;
			}
		} else if(cmp > 0) {
			if(ancestorStack_.empty()) {
				if((ancestor_ = ancestors_->seek(an->getDocID(), *an->getLastElemDescendantNID(),
					    context)).isNull()) state_ = DONE;
			} else {
				ancestor_ = ancestorStack_.back();
				ancestorStack_.pop_back();
			}
		} else {
			ancestorStack_.push_back(ancestor_);
			if((ancestor_ = ancestors_->next(context)).isNull()) {
				ancestor_ = ancestorStack_.back();
				ancestorStack_.pop_back();

				if(an->getLevel() == (de->getLevel() - 1) ||
					(de->getNodeType() == DOMNode::ATTRIBUTE_NODE && an->getLevel() == de->getLevel())) {
					return descendant;
				}

				if((descendant = descendants_->next(context)).isNull())
					state_ = DONE;
			}
		}
	}

	ancestorStack_.clear();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AncestorJoin::AncestorJoin(bool orSelf, const DbXmlResult &descendants, const DbXmlResult &ancestors,
	const LocationInfo *o)
	: DbXmlResultImpl(o),
	  orSelf_(orSelf),
	  ancestors_(ancestors),
	  descendants_(descendants),
	  state_(INIT)
{
}

Item::Ptr AncestorJoin::next(DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		Item::Ptr ancestor;
		if((ancestor = ancestors_->next(context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		DbXmlNodeImpl *an = (DbXmlNodeImpl*)ancestor.get();
		if((descendant_ = descendants_->seek(an->getDocID(), *an->getNID(),
			    context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(ancestor, context);
	}
	case RUNNING: {
		Item::Ptr ancestor;
		if((ancestor = ancestors_->next(context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(ancestor, context);
	}
	case DONE: break;
	}
	return 0;
}

Item::Ptr AncestorJoin::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		Item::Ptr ancestor;
		if((ancestor = ancestors_->seek(did, nid, context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		DbXmlNodeImpl *an = (DbXmlNodeImpl*)ancestor.get();
		if((descendant_ = descendants_->seek(an->getDocID(), *an->getNID(),
			    context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(ancestor, context);
	}
	case RUNNING: {
		Item::Ptr ancestor;
		if((ancestor = ancestors_->seek(did, nid, context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(ancestor, context);
	}
	case DONE: break;
	}
	return 0;
}

Item::Ptr AncestorJoin::doJoin(Item::Ptr &ancestor, DynamicContext *context)
{
	while(state_ != DONE) {
		context->testInterrupt();

		DbXmlNodeImpl *de = (DbXmlNodeImpl*)descendant_.get();
		DbXmlNodeImpl *an = (DbXmlNodeImpl*)ancestor.get();

		int cmp = de->isDescendantOf(an, orSelf_);
		if(cmp < 0) {
			if((descendant_ = descendants_->seek(an->getDocID(), *an->getNID(),
				    context)).isNull()) state_ = DONE;
		} else if(cmp > 0) {
			if((ancestor = ancestors_->seek(an->getDocID(), *an->getLastElemDescendantNID(),
				    context)).isNull()) state_ = DONE;
		} else {
			return ancestor;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ParentJoin::ParentJoin(const DbXmlResult &descendants, const DbXmlResult &ancestors, const LocationInfo *o)
	: AncestorJoin(/*orSelf*/true, descendants, ancestors, o)
{
}

Item::Ptr ParentJoin::doJoin(Item::Ptr &ancestor, DynamicContext *context)
{
	while(state_ != DONE) {
		context->testInterrupt();

		DbXmlNodeImpl *de = (DbXmlNodeImpl*)descendant_.get();
		DbXmlNodeImpl *an = (DbXmlNodeImpl*)ancestor.get();

		int cmp = de->isDescendantOf(an, orSelf_);
		if(cmp < 0) {
			if((descendant_ = descendants_->seek(an->getDocID(), *an->getNID(),
				    context)).isNull()) state_ = DONE;
		} else if(cmp > 0) {
			if((ancestor = ancestors_->seek(an->getDocID(), *an->getLastElemDescendantNID(),
				    context)).isNull()) state_ = DONE;
		} else if(an->getLevel() == (de->getLevel() - 1) ||
			(de->getNodeType() == DOMNode::ATTRIBUTE_NODE && an->getLevel() == de->getLevel())) {
			return ancestor;
		} else {
			if((ancestor = ancestors_->next(context)).isNull()) state_ = DONE;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ParentCJoin::ParentCJoin(const DbXmlResult &descendants, const DbXmlResult &ancestors, const LocationInfo *o)
	: AncestorJoin(/*orSelf*/false, descendants, ancestors, o)
{
}

Item::Ptr ParentCJoin::doJoin(Item::Ptr &ancestor, DynamicContext *context)
{
	while(state_ != DONE) {
		context->testInterrupt();

		DbXmlNodeImpl *de = (DbXmlNodeImpl*)descendant_.get();
		DbXmlNodeImpl *an = (DbXmlNodeImpl*)ancestor.get();

		int cmp = de->isDescendantOf(an, orSelf_);
		if(cmp < 0) {
			if((descendant_ = descendants_->seek(an->getDocID(), *an->getNID(),
				    context)).isNull()) state_ = DONE;
		} else if(cmp > 0) {
			if((ancestor = ancestors_->seek(an->getDocID(), *an->getLastElemDescendantNID(),
				    context)).isNull()) state_ = DONE;
		} else if(an->getLevel() == (de->getLevel() - 1)) {
			return ancestor;
		} else {
			if((ancestor = ancestors_->next(context)).isNull()) state_ = DONE;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SelfJoin::SelfJoin(const DbXmlResult &left, const DbXmlResult &right, const LocationInfo *o)
	: DbXmlResultImpl(o),
	  left_(left),
	  right_(right),
	  state_(RUNNING)
{
}

Item::Ptr SelfJoin::next(DynamicContext *context)
{
	switch(state_) {
	case RUNNING: {
		Item::Ptr li, ri;
		if((li = left_->next(context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		DbXmlNodeImpl *l = (DbXmlNodeImpl*)li.get();
		if((ri = right_->seek(l->getDocID(), *l->getNID(), context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(li, ri, context);
	}
	case DONE: break;
	}
	return 0;
}

Item::Ptr SelfJoin::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	switch(state_) {
	case RUNNING: {
		Item::Ptr li, ri;
		if((li = left_->seek(did, nid, context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		DbXmlNodeImpl *l = (DbXmlNodeImpl*)li.get();
		if((ri = right_->seek(l->getDocID(), *l->getNID(), context)).isNull()) {
			state_ = DONE;
			return 0;
		}
		return doJoin(li, ri, context);
	}
	case DONE: break;
	}
	return 0;
}

Item::Ptr SelfJoin::doJoin(Item::Ptr &li, Item::Ptr &ri, DynamicContext *context)
{
	while(state_ != DONE) {
		context->testInterrupt();

		DbXmlNodeImpl *l = (DbXmlNodeImpl*)li.get();
		DbXmlNodeImpl *r = (DbXmlNodeImpl*)ri.get();

		int cmp = l->compare(r);
		if(cmp < 0) {
			if((li = left_->seek(r->getDocID(), *r->getNID(), context)).isNull())
				state_ = DONE;
		} else if(cmp > 0) {
			if((ri = right_->seek(l->getDocID(), *l->getNID(), context)).isNull())
				state_ = DONE;
		} else {
			return ri;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AttributeJoin::AttributeJoin(const DbXmlResult &parents, const DbXmlResult &attributes, const LocationInfo *o)
	: SelfJoin(parents, attributes, o)
{
}

Item::Ptr AttributeJoin::doJoin(Item::Ptr &pa, Item::Ptr &at, DynamicContext *context)
{
	while(state_ != DONE) {
		context->testInterrupt();

		DbXmlNodeImpl *p = (DbXmlNodeImpl*)pa.get();
		DbXmlNodeImpl *a = (DbXmlNodeImpl*)at.get();

		int cmp = p->isSameNID(a);
		if(cmp < 0) {
			if((pa = left_->seek(a->getDocID(), *a->getNID(), context)).isNull())
				state_ = DONE;
		} else if(cmp > 0) {
			if((at = right_->seek(p->getDocID(), *p->getNID(), context)).isNull())
				state_ = DONE;
		} else if(a->getNodeType() == DOMNode::ATTRIBUTE_NODE &&
			p->getNodeType() == DOMNode::ELEMENT_NODE) {
			return at;
		} else {
			if((at = right_->next(context)).isNull()) state_ = DONE;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ParentAJoin::ParentAJoin(const DbXmlResult &attributes, const DbXmlResult &parents, const LocationInfo *o)
	: SelfJoin(attributes, parents, o)
{
}

Item::Ptr ParentAJoin::doJoin(Item::Ptr &at, Item::Ptr &pa, DynamicContext *context)
{
	while(state_ != DONE) {
		context->testInterrupt();

		DbXmlNodeImpl *a = (DbXmlNodeImpl*)at.get();
		DbXmlNodeImpl *p = (DbXmlNodeImpl*)pa.get();

		int cmp = a->isSameNID(p);
		if(cmp < 0) {
			if((at = left_->seek(a->getDocID(), *a->getNID(), context)).isNull())
				state_ = DONE;
		} else if(cmp > 0) {
			if((pa = right_->seek(p->getDocID(), *p->getNID(), context)).isNull())
				state_ = DONE;
		} else if(a->getNodeType() == DOMNode::ATTRIBUTE_NODE &&
			p->getNodeType() == DOMNode::ELEMENT_NODE) {
			return p;
		} else {
			if((pa = right_->next(context)).isNull()) state_ = DONE;
		}
	}
	return 0;
}

