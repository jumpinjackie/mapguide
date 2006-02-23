//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlModify.hpp,v 1.11 2005/04/20 18:31:21 bostic Exp $
//

#ifndef __XMLMODIFY_HPP
#define __XMLMODIFY_HPP

#include <string>

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"

namespace DbXml
{

class Modify;

class DBXML_EXPORT XmlModify
{
public:
	enum XmlObject {
		Element,               ///< requires name
		Attribute,             ///< requires name
		Text,                  ///< requires content
		ProcessingInstruction, ///< requires name and content
		Comment                ///< requires content
	};

	/** @name For Reference Counting */
	XmlModify();
	XmlModify(const XmlModify &o);
	XmlModify &operator=(const XmlModify &o);
	~XmlModify();
	bool isNull() const { return modify_ == 0; }

	/** @name Modification Specifying Methods */

	void addInsertBeforeStep(const XmlQueryExpression &selectionExpr, XmlObject type, const std::string &name, const std::string &content);
	void addInsertAfterStep(const XmlQueryExpression &selectionExpr, XmlObject type, const std::string &name, const std::string &content);
	void addAppendStep(const XmlQueryExpression &selectionExpr, XmlObject type, const std::string &name, const std::string &content, int location = -1);
	void addUpdateStep(const XmlQueryExpression &selectionExpr, const std::string &content);
	void addRemoveStep(const XmlQueryExpression &selectionExpr);
	void addRenameStep(const XmlQueryExpression &selectionExpr, const std::string &newName);

	/// optionally rewrite content with new encoding
	void setNewEncoding(const std::string &newEncoding);

	/** @name Modification Methods */
	unsigned int execute(XmlValue &toModify, XmlQueryContext &context, XmlUpdateContext &uc) const;
	unsigned int execute(XmlResults &toModify, XmlQueryContext &context, XmlUpdateContext &uc) const;

	/** @name Modification Methods (Transacted) */
	unsigned int execute(XmlTransaction &txn, XmlValue &toModify, XmlQueryContext &context, XmlUpdateContext &uc) const;
	unsigned int execute(XmlTransaction &txn, XmlResults &toModify, XmlQueryContext &context, XmlUpdateContext &uc) const;

	/** @name Private Methods (for internal use) */
	// @{

	XmlModify(Modify *m);
	operator Modify &() const
	{
		return *modify_;
	}

	//@}

private:
	Modify *modify_;
};

}
#endif
