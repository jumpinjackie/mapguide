//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlAtomize.hpp,v 1.3 2006/10/30 17:45:54 bostic Exp $
//

#ifndef __DBXMLATOMIZE_HPP
#define	__DBXMLATOMIZE_HPP

#include <xqilla/ast/XQAtomize.hpp>

namespace DbXml
{

class DbXmlAtomize : public XQAtomize
{
public:
	DbXmlAtomize(XQAtomize *atom, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext *context);
	virtual ASTNode *staticTyping(StaticContext *context);
};


}

#endif
