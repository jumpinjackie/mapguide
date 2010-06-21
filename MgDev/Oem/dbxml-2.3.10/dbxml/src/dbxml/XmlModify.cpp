//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlModify.cpp,v 1.15 2006/10/30 17:45:54 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlModify.hpp"
#include "dbxml/XmlQueryExpression.hpp"
#include "dbxml/XmlTransaction.hpp"
#include "QueryExpression.hpp"
#include "Modify.hpp"
#include "dbxml/XmlResults.hpp"

using namespace DbXml;

static const char *className = "XmlModify";
#define CHECK_POINTER checkNullPointer(modify_,className)

XmlModify::XmlModify()
  : modify_(0)
{
}

XmlModify::XmlModify(Modify *m)
	: modify_(m)
{
	if (modify_ != 0)
		modify_->acquire();
}

XmlModify::XmlModify(const XmlModify &o)
	: modify_(o.modify_)
{
	if (modify_ != 0)
		modify_->acquire();
}

XmlModify &XmlModify::operator=(const XmlModify &o)
{
	if(modify_ != o.modify_) {
		if (modify_ != 0)
			modify_->release();
		modify_ = o.modify_;
		if (modify_ != 0)
			modify_->acquire();
	}
	
	return *this;
}

XmlModify::~XmlModify()
{
	if (modify_ != 0)
		modify_->release();
}

void XmlModify::setNewEncoding(const std::string &newEncoding)
{
	CHECK_POINTER;
	modify_->setNewEncoding(newEncoding);
}

void XmlModify::addInsertBeforeStep(const XmlQueryExpression &selectionExpr,
				    XmlObject type, const std::string &name,
				    const std::string &content)
{
	CHECK_POINTER;
	modify_->addStep(new InsertBeforeStep(selectionExpr, type, name,
					      content, modify_->getMemMgr(),
					      modify_->getDOMParser()));
}

void XmlModify::addInsertBeforeStep(const XmlQueryExpression &selectionExpr,
				    XmlObject type, const std::string &name,
				    XmlResults &content)
{
	CHECK_POINTER;
	modify_->addStep(new InsertBeforeStep(selectionExpr, type, name,
					      content));
}

void XmlModify::addInsertAfterStep(const XmlQueryExpression &selectionExpr,
				   XmlObject type, const std::string &name,
				   const std::string &content)
{
	CHECK_POINTER;
	modify_->addStep(new InsertAfterStep(selectionExpr, type, name,
					     content, modify_->getMemMgr(),
					     modify_->getDOMParser()));
}

void XmlModify::addInsertAfterStep(const XmlQueryExpression &selectionExpr,
				   XmlObject type, const std::string &name,
				   XmlResults &content)
{
	CHECK_POINTER;
	modify_->addStep(new InsertAfterStep(selectionExpr, type, name,
					     content));
}

void XmlModify::addAppendStep(const XmlQueryExpression &selectionExpr,
			      XmlObject type, const std::string &name,
			      const std::string &content, int location)
{
	CHECK_POINTER;
	modify_->addStep(new AppendStep(selectionExpr, type, name, content,
					location, modify_->getMemMgr(),
					modify_->getDOMParser()));
	
}

void XmlModify::addAppendStep(const XmlQueryExpression &selectionExpr,
			      XmlObject type, const std::string &name,
			      XmlResults &content, int location)
{
	CHECK_POINTER;
	modify_->addStep(new AppendStep(selectionExpr, type, name, content,
					location));
}

void XmlModify::addUpdateStep(const XmlQueryExpression &selectionExpr,
			      const std::string &content)
{
	CHECK_POINTER;
	modify_->addStep(new UpdateStep(selectionExpr, content));
}

void XmlModify::addRemoveStep(const XmlQueryExpression &selectionExpr)
{
	CHECK_POINTER;
	modify_->addStep(new RemoveStep(selectionExpr));
}

void XmlModify::addRenameStep(const XmlQueryExpression &selectionExpr,
			      const std::string &newName)
{
	CHECK_POINTER;
	modify_->addStep(new RenameStep(selectionExpr, newName));
}

unsigned int XmlModify::execute(XmlValue &toModify, XmlQueryContext &context,
				XmlUpdateContext &uc) const
{
	CHECK_POINTER;
	return modify_->execute(0, toModify, context, uc);
}

unsigned int XmlModify::execute(XmlResults &toModify, XmlQueryContext &context,
				XmlUpdateContext &uc) const
{
	CHECK_POINTER;
	return modify_->execute(0, toModify, context, uc);
}

unsigned int XmlModify::execute(XmlTransaction &txn, XmlValue &toModify,
				XmlQueryContext &context,
				XmlUpdateContext &uc) const
{
	CHECK_POINTER;
	return modify_->execute(txn, toModify, context, uc);
}

unsigned int XmlModify::execute(XmlTransaction &txn, XmlResults &toModify,
				XmlQueryContext &context,
				XmlUpdateContext &uc) const
{
	CHECK_POINTER;
	return modify_->execute(txn, toModify, context, uc);
}

