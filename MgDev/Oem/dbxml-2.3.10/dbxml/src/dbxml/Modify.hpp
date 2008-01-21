//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Modify.hpp,v 1.20 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __MODIFY_HPP
#define __MODIFY_HPP

#include <string>
#include <vector>
#include <dbxml/XmlManager.hpp>
#include <dbxml/XmlModify.hpp>
#include <dbxml/XmlQueryContext.hpp>
#include "ReferenceCounted.hpp"
#include "nodeStore/NsDom.hpp"
#include "UTF8.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

namespace DbXml
{

/// ModifyStep base class
class ModifyStep
{
public:
	typedef std::vector<ModifyStep*> Vector;
	
	ModifyStep(const XmlQueryExpression &expr);
	virtual ~ModifyStep();
	
	unsigned int execute(Transaction *transaction, Value *toModify,
			     XmlQueryContext &context) const;
	virtual void modify(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node,
		XmlQueryContext &context) const = 0;
	QueryExpression &getExpression() const {
		return *expr_;
	}
	void setExpression(QueryExpression *expr) {
		expr_->release();
		expr_ = expr;
		expr->acquire();
	}
protected:
	XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr *
	importAttr(XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr *attr,
		   XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *newdoc,
		   XmlQueryContext &context) const;

private:
	ModifyStep(const ModifyStep &);
	ModifyStep &operator=(const ModifyStep &);
	
	QueryExpression *expr_;
};

/// Step that removes the given node
class RemoveStep : public ModifyStep
{
public:
	RemoveStep(const XmlQueryExpression &expr) : ModifyStep(expr) {}
	
	virtual void modify(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node,
		XmlQueryContext &context) const;
};

/// Step that renames the given node
class RenameStep : public ModifyStep
{
public:
	RenameStep(const XmlQueryExpression &expr, const std::string &name)
		: ModifyStep(expr), name_(name) {}
	
	virtual void modify(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node,
		XmlQueryContext &context) const;

private:
	UTF8ToXMLCh name_;
};

/// Step that updates the text content of a given node
class UpdateStep : public ModifyStep
{
public:
	UpdateStep(const XmlQueryExpression &expr, const std::string &content)
		: ModifyStep(expr), content_(content) {}
	
	virtual void modify(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node,
		XmlQueryContext &context) const;

private:
	UTF8ToXMLCh content_;
};

/// Intermediate step, that constructs the replacement content for it's derived steps
class DOMContentStep : public ModifyStep
{
public:
	DOMContentStep(
		const XmlQueryExpression &expr, XmlModify::XmlObject type,
		const std::string &name, const std::string &content,
		XPath2MemoryManager *memMgr,
		XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *domParser);
	DOMContentStep(
		const XmlQueryExpression &expr, XmlModify::XmlObject type,
		const std::string &name, XmlResults &content);
	virtual ~DOMContentStep();
protected:
	void insertChildren(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *parent,
			    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *target,
			    NsDomInsertType itype,
			    bool isAppend = false) const;
protected:
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *document_;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *content_;
	std::string name_;
	Results *results_;
};

/// Step that inserts content before the given node
class InsertBeforeStep : public DOMContentStep
{
public:
	InsertBeforeStep(
		const XmlQueryExpression &expr, XmlModify::XmlObject type,
		const std::string &name, const std::string &content,
		XPath2MemoryManager *memMgr,
		XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *domParser)
		: DOMContentStep(
			expr, type, name, content, memMgr, domParser) {}
	InsertBeforeStep(
		const XmlQueryExpression &expr, XmlModify::XmlObject type,
		const std::string &name, XmlResults &content)
		: DOMContentStep(
			expr, type, name, content) {}
	virtual void modify(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node,
		XmlQueryContext &context) const;
};

/// Step that inserts content after the given node
class InsertAfterStep : public DOMContentStep
{
public:
	InsertAfterStep(
		const XmlQueryExpression &expr, XmlModify::XmlObject type,
		const std::string &name, const std::string &content,
		XPath2MemoryManager *memMgr,
		XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *domParser)
		: DOMContentStep(
			expr, type, name, content, memMgr, domParser) {}
	
	InsertAfterStep(
		const XmlQueryExpression &expr, XmlModify::XmlObject type,
		const std::string &name, XmlResults &content)
		: DOMContentStep(
			expr, type, name, content) {}
	virtual void modify(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node,
		XmlQueryContext &context) const;
};

/// Step that appends content as a child of the given node, at the optional location
class AppendStep : public DOMContentStep
{
public:
	AppendStep(
		const XmlQueryExpression &expr, XmlModify::XmlObject type,
		const std::string &name, const std::string &content,
		int location, XPath2MemoryManager *memMgr,
		XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *domParser)
		: DOMContentStep(
			expr, type, name, content, memMgr, domParser),
		  location_(location) {}

	AppendStep(
		const XmlQueryExpression &expr, XmlModify::XmlObject type,
		const std::string &name, XmlResults &content,
		int location)
		: DOMContentStep(
			expr, type, name, content),
		  location_(location) {}
	virtual void modify(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node,
		XmlQueryContext &context) const;

private:
	int location_;
};

class Modify : public ReferenceCountedProtected
{
public:
	Modify(XmlManager &db) : db_(db), memMgr_(0), domParser_(0) {}
	virtual ~Modify();

	XPath2MemoryManager *getMemMgr() const;
	XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *getDOMParser();
	
	void addStep(ModifyStep *stepToAdopt);
	void setNewEncoding(const std::string &newEncoding);
	
	unsigned int execute(Transaction *txn, Value *toModify,
			     XmlQueryContext &context,
			     XmlUpdateContext &uc) const;
	unsigned int execute(Transaction *txn, XmlResults &toModify,
			     XmlQueryContext &context,
			     XmlUpdateContext &uc) const;
	
private:
	unsigned int executeInternal(Transaction *txn, Value *toModify,
				     XmlQueryContext &context) const;
	unsigned int changeEncoding(const XmlDocument &document) const;
	void updateDocument(
		Transaction *txn, const XmlDocument &document,
		XmlQueryContext &context, XmlUpdateContext &uc) const;
	
	mutable XmlManager db_;
	
	mutable XPath2MemoryManager *memMgr_;
	XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *domParser_;
	
	std::string newEncoding_;
	ModifyStep::Vector steps_;
};

}
#endif
