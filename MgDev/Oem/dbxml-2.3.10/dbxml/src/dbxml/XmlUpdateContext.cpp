//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlUpdateContext.cpp,v 1.13 2006/10/30 17:45:54 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlUpdateContext.hpp"
#include "dbxml/XmlContainer.hpp"
#include "UpdateContext.hpp"
#include "Container.hpp"

using namespace DbXml;

static const char *className = "XmlUpdateContext";
#define CHECK_POINTER checkNullPointer(updateContext_,className)

XmlUpdateContext::XmlUpdateContext()
	: updateContext_(0)
{
}

XmlUpdateContext::XmlUpdateContext(UpdateContext *uc)
	: updateContext_(uc)
{
	if (updateContext_)
		updateContext_->acquire();
}

XmlUpdateContext::~XmlUpdateContext()
{
	if (updateContext_)
		updateContext_->release();
}

XmlUpdateContext::XmlUpdateContext(const XmlUpdateContext &o)
	: updateContext_(o.updateContext_)
{
	if (updateContext_ != 0)
		updateContext_->acquire();
}

XmlUpdateContext &XmlUpdateContext::operator=(const XmlUpdateContext &o)
{
	if (this != &o && updateContext_ != o.updateContext_) {
		if (updateContext_ != 0)
			updateContext_->release();
		updateContext_ = o.updateContext_;
		if (updateContext_ != 0)
			updateContext_->acquire();
	}
	return *this;
}

void XmlUpdateContext::setApplyChangesToContainers(bool applyChanges)
{
	CHECK_POINTER;
	updateContext_->setApplyChanges(applyChanges);
}

bool XmlUpdateContext::getApplyChangesToContainers() const
{
	CHECK_POINTER;
	return updateContext_->getApplyChanges();
}
