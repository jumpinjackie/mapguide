//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlUpdateContext.hpp,v 1.11 2005/04/05 16:43:56 bostic Exp $
//

#ifndef __XMLUPDATECONTEXT_HPP
#define	__XMLUPDATECONTEXT_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include <string>

namespace DbXml
{

class UpdateContext;

class DBXML_EXPORT XmlUpdateContext
{
public:
	/** @name For Reference Counting */
	XmlUpdateContext();
	XmlUpdateContext(const XmlUpdateContext&);
	XmlUpdateContext &operator=(const XmlUpdateContext &);
	virtual ~XmlUpdateContext();
	bool isNull() const { return updateContext_ == 0; }

	/** @name Apply Changes Methods */
	void setApplyChangesToContainers(bool applyChanges);
	bool getApplyChangesToContainers() const;

	/** @name Private Methods (for internal use) */
	// @{

	XmlUpdateContext(UpdateContext *uc);
	operator UpdateContext&()
	{
		return *updateContext_;
	}
	operator UpdateContext*()
	{
		return updateContext_;
	}

	//@}

private:
	UpdateContext *updateContext_;
};

}
#endif
