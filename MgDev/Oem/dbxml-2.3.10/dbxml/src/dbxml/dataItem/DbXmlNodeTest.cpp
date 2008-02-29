//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlNodeTest.cpp,v 1.12 2006/10/30 17:45:55 bostic Exp $
//

#include "DbXmlNodeTest.hpp"
#include "QueryPlanFunction.hpp"
#include "../nodeStore/NsUtil.hpp"
#include "../Manager.hpp"

#include <xercesc/util/XMLUniDefs.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh DbXmlNodeTest::gDbXml[] =
{
	chLatin_D, chLatin_b,
	chLatin_X, chLatin_m,
	chLatin_l,
	chNull
};

DbXmlNodeTest::DbXmlNodeTest(const XMLCh *nodeType)
        : name8_(0),
          uri8_(0)
{
        _type = nodeType;
        _wildcardName = true;
        _wildcardNamespace = true;
}

DbXmlNodeTest::DbXmlNodeTest(const XMLCh *nodeType, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager *mm)
        : name8_(0),
          uri8_(0)
{
        _type = nodeType;
	_uri = uri;
	_name = name;

	NsDonator donator(mm, _name, _uri);
	name8_ = donator.getStr();
	uri8_ = donator.getStr2();
}

DbXmlNodeTest::DbXmlNodeTest(const NodeTest *other, XPath2MemoryManager *mm)
	: name8_(0),
	  uri8_(0)
{
	_name = other->getNodeName();
	_uri = other->getNodeUri();
	_prefix = other->getNodePrefix();
	_type = other->getNodeType();
	_wildcardName = other->getNameWildcard();
	_wildcardNamespace = other->getNamespaceWildcard();
	_wildcardType = other->getTypeWildcard();
	_usePrefix = other->isNodePrefixSet();
	_hasChildren = other->getHasChildren();
	_itemType = other->getItemType();

	NsDonator donator(mm, _name, _uri);
	name8_ = donator.getStr();
	uri8_ = donator.getStr2();
}

void *DbXmlNodeTest::getInterface(const XMLCh *name) const
{
	if(name == gDbXml) {
		return (void*)this;
	}
	return 0;
}
