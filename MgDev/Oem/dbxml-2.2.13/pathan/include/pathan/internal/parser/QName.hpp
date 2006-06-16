/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _QNAME_HPP
#define _QNAME_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <vector>

/*
Represents a Qualified name. Built in the parser.
 */
class PATHAN_EXPORT QualifiedName {

public:
	QualifiedName(const XMLCh* qualifiedName, XPath2MemoryManager* memMgr);
	// This constructor should be used on persistent strings (no memory manager involved)
	QualifiedName(const XMLCh* const prefix, const XMLCh* const name);
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

