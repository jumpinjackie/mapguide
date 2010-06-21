//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ImpliedSchemaNode.cpp,v 1.18.2.1 2007/01/10 18:28:43 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "ImpliedSchemaNode.hpp"
#include "UTF8.hpp"
#include "Value.hpp"
#include <xqilla/utils/XPath2Utils.hpp>
#include <sstream>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

ImpliedSchemaNode::ImpliedSchemaNode(Type type, MemoryManager *mm)
	: uri_(0),
	  name_(0),
	  wildcardURI_(false),
	  wildcardName_(false),
	  wildcardNodeType_(true),
	  type_(type),
	  generalComp_(false),
	  syntaxType_(Syntax::NONE),
	  astnode_(0),
	  utf8URI_(0),
	  utf8Name_(0),
	  parent_(0),
	  nextSibling_(0),
	  prevSibling_(0),
	  firstChild_(0),
	  lastChild_(0),
	  qph_(0),
	  memMgr_(mm)
{
}

ImpliedSchemaNode::ImpliedSchemaNode(Type type, bool generalComp, Syntax::Type syntaxType, const ASTNode *value,
	MemoryManager *mm)
	: uri_(0),
	  name_(0),
	  wildcardURI_(false),
	  wildcardName_(false),
	  wildcardNodeType_(false),
	  type_(type),
	  generalComp_(generalComp),
	  syntaxType_(syntaxType),
	  astnode_(value),
	  utf8URI_(0),
	  utf8Name_(0),
	  parent_(0),
	  nextSibling_(0),
	  prevSibling_(0),
	  firstChild_(0),
	  lastChild_(0),
	  qph_(0),
	  memMgr_(mm)
{
}

ImpliedSchemaNode::ImpliedSchemaNode(const XMLCh *uri, bool wildcardURI, const XMLCh *name, bool wildcardName,
                                     bool wildcardNodeType, Type type, MemoryManager *mm)
	: uri_(uri),
	  name_(name),
	  wildcardURI_(wildcardURI),
	  wildcardName_(wildcardName),
	  wildcardNodeType_(wildcardNodeType),
	  type_(type),
	  generalComp_(false),
	  syntaxType_(Syntax::NONE),
	  astnode_(0),
	  utf8URI_(0),
	  utf8Name_(0),
	  parent_(0),
	  nextSibling_(0),
	  prevSibling_(0),
	  firstChild_(0),
	  lastChild_(0),
	  qph_(0),
	  memMgr_(mm)
{
}

ImpliedSchemaNode::~ImpliedSchemaNode()
{
	// We don't need to do this now we're memory managed

	//   ImpliedSchemaNode *child = firstChild_;
	//   ImpliedSchemaNode *next;
	//   while(child) {
	//     next = child->nextSibling_;
	//     delete child;
	//     child = next;
	//   }
	//   if(utf8URI_) delete utf8URI_;
	//   if(utf8Name_) delete utf8Name_;
}

bool ImpliedSchemaNode::isWildcardURI() const
{
	return wildcardURI_;
}

bool ImpliedSchemaNode::isWildcardName() const
{
	return wildcardName_;
}

bool ImpliedSchemaNode::isWildcardNodeType() const
{
	return wildcardNodeType_;
}

bool ImpliedSchemaNode::isWildcard() const
{
	return wildcardName_ || wildcardURI_ ||
		(wildcardNodeType_ && type_ != ATTRIBUTE);
}

const XMLCh *ImpliedSchemaNode::getURI() const
{
	return uri_;
}

const XMLCh *ImpliedSchemaNode::getName() const
{
	return name_;
}

ImpliedSchemaNode::Type ImpliedSchemaNode::getType() const
{
	return type_;
}

void ImpliedSchemaNode::setType(Type t)
{
	type_ = t;
}

ImpliedSchemaNode *ImpliedSchemaNode::getParent()
{
	return parent_;
}

const ImpliedSchemaNode *ImpliedSchemaNode::getParent() const
{
	return parent_;
}

ImpliedSchemaNode *ImpliedSchemaNode::getNextSibling()
{
	return nextSibling_;
}

const ImpliedSchemaNode *ImpliedSchemaNode::getNextSibling() const
{
	return nextSibling_;
}

ImpliedSchemaNode *ImpliedSchemaNode::getPreviousSibling()
{
	return prevSibling_;
}

const ImpliedSchemaNode *ImpliedSchemaNode::getPreviousSibling() const
{
	return prevSibling_;
}

ImpliedSchemaNode *ImpliedSchemaNode::getFirstChild()
{
	return firstChild_;
}

const ImpliedSchemaNode *ImpliedSchemaNode::getFirstChild() const
{
	return firstChild_;
}

ImpliedSchemaNode *ImpliedSchemaNode::getLastChild()
{
	return lastChild_;
}

const ImpliedSchemaNode *ImpliedSchemaNode::getLastChild() const
{
	return lastChild_;
}

ImpliedSchemaNode *ImpliedSchemaNode::getBaseNode()
{
	switch(type_) {
	case ImpliedSchemaNode::ROOT:
	case ImpliedSchemaNode::METADATA:
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::DESCENDANT: {
		return this;
	}
	case ImpliedSchemaNode::EQUALS:
	case ImpliedSchemaNode::GTX:
	case ImpliedSchemaNode::GTE:
	case ImpliedSchemaNode::LTX:
	case ImpliedSchemaNode::LTE:
	case ImpliedSchemaNode::PREFIX:
	case ImpliedSchemaNode::SUBSTRING: {
		return parent_->getBaseNode();
	}
	}
	return 0;
}

ImpliedSchemaNode *ImpliedSchemaNode::getRoot()
{
	ImpliedSchemaNode *result = this;
	ImpliedSchemaNode *parent = parent_;
	while(parent) {
		result = parent;
		parent = parent->parent_;
	}
	return result;
}

const ImpliedSchemaNode *ImpliedSchemaNode::getRoot() const
{
	const ImpliedSchemaNode *result = this;
	const ImpliedSchemaNode *parent = parent_;
	while(parent) {
		result = parent;
		parent = parent->parent_;
	}
	return result;
}

ImpliedSchemaNode *ImpliedSchemaNode::appendChild(ImpliedSchemaNode *childToAdopt)
{
	if(childToAdopt->parent_ == this) return childToAdopt;

	if(childToAdopt->parent_ != 0) {
		childToAdopt->parent_->removeChild(childToAdopt, false);
	}

	// Search to see if the child exists already
	ImpliedSchemaNode *compare = firstChild_;
	for(; compare; compare = compare->nextSibling_) {
		if(compare->equals(childToAdopt)) {
			break;
		}
	}

	if(compare) {
		// Child exists already
		compare->stealChildren(childToAdopt);
		//     delete childToAdopt; - Not done, as we're memory managed now

		return compare;
	}
	else {
		// Child doesn't already exist, so add
		childToAdopt->parent_ = this;
		if(firstChild_ == 0) {
			firstChild_ = childToAdopt;
		}
		if(lastChild_) {
			lastChild_->nextSibling_ = childToAdopt;
			childToAdopt->prevSibling_ = lastChild_;
		}
		lastChild_ = childToAdopt;

		return childToAdopt;
	}
}

void ImpliedSchemaNode::removeChild(ImpliedSchemaNode *child, bool deleteChild)
{
	DBXML_ASSERT(child->parent_ == this);

	if(firstChild_ == child) {
		firstChild_ = child->nextSibling_;
	}

	if(lastChild_ == child) {
		lastChild_ = child->prevSibling_;
	}

	if(child->prevSibling_) {
		child->prevSibling_->nextSibling_ = child->nextSibling_;
	}

	if(child->nextSibling_) {
		child->nextSibling_->prevSibling_ = child->prevSibling_;
	}

	if(deleteChild) {
		//     delete child; - Not done, as we're memory managed now.
	}
	else {
		child->parent_ = 0;
		child->nextSibling_ = 0;
		child->prevSibling_ = 0;
	}
}

void ImpliedSchemaNode::stealChildren(ImpliedSchemaNode *victim)
{
	// Loop over the children of "victim", adding them to us
	for(ImpliedSchemaNode *i = victim->firstChild_; i != 0; i = victim->firstChild_) {
		appendChild(i); // Removes it from "victim"
	}
}

void ImpliedSchemaNode::markSubtree()
{
	appendChild(new (memMgr_) ImpliedSchemaNode(0, true, 0, true, true, ImpliedSchemaNode::DESCENDANT, memMgr_));
}

bool ImpliedSchemaNode::equals(const ImpliedSchemaNode *o) const
{
	return wildcardURI_ == o->wildcardURI_ &&
		wildcardName_ == o->wildcardName_ &&
		wildcardNodeType_ == o->wildcardNodeType_ &&
		type_ == o->type_ &&
		generalComp_ == o->generalComp_ &&
		syntaxType_ == o->syntaxType_ &&
		astnode_ == o->astnode_ &&
		(wildcardURI_ || XPath2Utils::equals(uri_, o->uri_)) &&
		(wildcardName_ || XPath2Utils::equals(name_, o->name_));
}

bool ImpliedSchemaNode::matches(const ImpliedSchemaNode *node) const
{
	return (wildcardURI_ || node->wildcardURI_ || XPath2Utils::equals(uri_, node->uri_)) &&
		(wildcardName_ || node->wildcardName_ || XPath2Utils::equals(name_, node->name_)) &&
		(wildcardNodeType_ || wildcardNodeType_ == node->wildcardNodeType_);
}

ImpliedSchemaNode *ImpliedSchemaNode::copy(MemoryManager *mm) const
{
	if(mm == 0) {
		mm = memMgr_;
	}

	ImpliedSchemaNode *result = new (mm) ImpliedSchemaNode(uri_, wildcardURI_, name_, wildcardName_, wildcardNodeType_, type_, mm);

	ImpliedSchemaNode *child;
	for(child = firstChild_; child; child = child->nextSibling_) {
		result->appendChild(child->copy(mm));
	}

	return result;
}

static string getIndent(int level)
{
	ostringstream s;

	for(int i = level; i != 0; --i) {
		s << "  ";
	}

	return s.str();
}

static string typeToString(ImpliedSchemaNode::Type type) {
	switch(type) {
	case ImpliedSchemaNode::ATTRIBUTE: {
		return "attribute";
		break;
	}
	case ImpliedSchemaNode::CHILD: {
		return "child";
		break;
	}
	case ImpliedSchemaNode::DESCENDANT: {
		return "descendant";
		break;
	}
	case ImpliedSchemaNode::ROOT: {
		return "root";
		break;
	}
	case ImpliedSchemaNode::METADATA: {
		return "metadata";
		break;
	}
	case ImpliedSchemaNode::EQUALS: {
		return "equals";
		break;
	}
	case ImpliedSchemaNode::GTX: {
		return "gtx";
		break;
	}
	case ImpliedSchemaNode::GTE: {
		return "gte";
		break;
	}
	case ImpliedSchemaNode::LTX: {
		return "ltx";
		break;
	}
	case ImpliedSchemaNode::LTE: {
		return "lte";
		break;
	}
	case ImpliedSchemaNode::PREFIX: {
		return "prefix";
		break;
	}
	case ImpliedSchemaNode::SUBSTRING: {
		return "substring";
		break;
	}
	}
	return "UNKNOWN";
}

string ImpliedSchemaNode::toString(int level) const
{
	ostringstream s;
	string in(getIndent(level));
	string type(typeToString(type_));

	s << in << "<" << type;

	if(wildcardURI_) {
		s << " uri=\"*\"";
	}
	else if(uri_ != 0) {
		s << " uri=\"" << XMLChToUTF8(uri_).str() << "\"";
	}
	if(wildcardName_) {
		s << " name=\"*\"";
	}
	else if(name_ != 0) {
		s << " name=\"" << XMLChToUTF8(name_).str() << "\"";
	}
	if(wildcardNodeType_) {
		s << " nodeType=\"*\"";
	}

	if(firstChild_) {
		s << ">" << endl;

		ImpliedSchemaNode *child = firstChild_;
		while(child) {
			s << child->toString(level + 1);
			child = child->nextSibling_;
		}

		s << in << "</" << type << ">" << endl;
	}
	else {
		s << "/>" << endl;
	}

	return s.str();
}

string ImpliedSchemaNode::getPath() const
{
	ostringstream s;

	if(parent_) {
		s << parent_->getPath();
	}

	if(type_ == CHILD || type_ == ATTRIBUTE || type_ == DESCENDANT) {
		s << "/";
	}

	s << getStepName();

	return s.str();
}

const char *ImpliedSchemaNode::getUTF8URI() const
{
	if(utf8URI_ == 0) {
		const_cast<ImpliedSchemaNode*>(this)->utf8URI_ = XMLString::replicate(XMLChToUTF8(uri_).str(), memMgr_);
	}
	return utf8URI_;
}

const char *ImpliedSchemaNode::getUTF8Name() const
{
	if(utf8Name_ == 0) {
		const_cast<ImpliedSchemaNode*>(this)->utf8Name_ = XMLString::replicate(XMLChToUTF8(name_).str(), memMgr_);
	}
	return utf8Name_;
}

string ImpliedSchemaNode::getStepName() const
{
	ostringstream s;

	switch(type_) {
	case ROOT: {
		s << "root()";
		break;
	}
	case GTX:
	case GTE:
	case LTX:
	case LTE:
	case EQUALS:
	case PREFIX:
	case SUBSTRING: {
		switch(type_) {
		case GTX: s << " > "; break;
		case GTE: s << " >= "; break;
		case LTX: s << " < "; break;
		case LTE: s << " <= "; break;
		case EQUALS: s << " = "; break;
		case PREFIX: s << " <prefix> "; break;
		case SUBSTRING: s << " <substring> "; break;
		default: break;
		}
		break;
	}
	default: {
		if(type_ == ATTRIBUTE) {
			s << "@";
		}
		else if(type_ != CHILD) {
			s << typeToString(type_) << "::";
		}

		if(wildcardNodeType_ && type_ != ATTRIBUTE) {
			s << "node()";
		}
		else {
			s << getUriName();
		}
		break;
	}
	}

	return s.str();
}

// uriname must be name:uri, to match
// Name.cpp usage
string ImpliedSchemaNode::getUriName() const
{
	ostringstream s;

	if(wildcardURI_ && wildcardName_) {
		s << "*";
	} else {
		if(wildcardName_) {
			s << "*";
		} else if(name_ != 0) {
			s << getUTF8Name();
		}
		if(wildcardURI_) {
			s << ":*";
		} else if(uri_ != 0) {
			s << ":" << getUTF8URI();
		}
	}

	return s.str();
}
