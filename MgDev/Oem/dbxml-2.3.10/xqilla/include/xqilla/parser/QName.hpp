/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: QName.hpp,v 1.8 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _QNAME_HPP
#define _QNAME_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <vector>

/*
Represents a Qualified name. Built in the parser.
 */
class XQILLA_API QualifiedName {

public:
	QualifiedName(const XMLCh* qualifiedName, XPath2MemoryManager* memMgr);
	// This constructor should be used on persistent strings (no memory manager involved)
	QualifiedName(const XMLCh* prefix, const XMLCh* name);
	QualifiedName(const XMLCh* fullName);
	~QualifiedName();

	const XMLCh* getPrefix() const;
	const XMLCh* getName() const;

	const XMLCh* getFullName(XPath2MemoryManager* memMgr) const;

private:
	const XMLCh* _prefix,*_name;
  bool _bDeleteStrings;
};

#endif

