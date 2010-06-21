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
 * $Id: QName.cpp,v 1.10 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/parser/QName.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>

//Parse qualifiedName into prefix and name
QualifiedName::QualifiedName(const XMLCh* qualifiedName, XPath2MemoryManager* memMgr) : _bDeleteStrings(false)
{
  if(qualifiedName && *qualifiedName)
  {
    int colonPos = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf(qualifiedName, XERCES_CPP_NAMESPACE_QUALIFIER chColon, 0);
    if(colonPos == -1) {
      _name = memMgr->getPooledString(qualifiedName);
      _prefix = 0;
    }
    else {
      XMLCh* tempPrefix = new XMLCh[colonPos + 1];
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::subString(tempPrefix, qualifiedName, 0, colonPos);
      tempPrefix[colonPos] = 0;
      _prefix = memMgr->getPooledString(tempPrefix);
      delete [] tempPrefix;

      unsigned int length = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(qualifiedName);
      XMLCh* tempName = new XMLCh[length - colonPos];
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::subString(tempName, qualifiedName, colonPos + 1, length);
      tempName[length - colonPos - 1] = 0;
      _name = memMgr->getPooledString(tempName);
      delete [] tempName;
    }
  }
  else
  {
    _name = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
    _prefix = 0;
  }
}

QualifiedName::QualifiedName(const XMLCh* qualifiedName) : _bDeleteStrings(true)
{
  if(qualifiedName && *qualifiedName)
  {
    int colonPos=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf(qualifiedName, XERCES_CPP_NAMESPACE_QUALIFIER chColon, 0);
    if(colonPos == -1) {
      _name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(qualifiedName);
      _prefix = 0;
    }
    else
    {
      XMLCh* tempPrefix = new XMLCh[colonPos + 1];
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::subString(tempPrefix, qualifiedName, 0, colonPos);
      tempPrefix[colonPos] = 0;
      _prefix = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(tempPrefix);
      delete [] tempPrefix;

      unsigned int length = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(qualifiedName);
      XMLCh* tempName = new XMLCh[length - colonPos];
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::subString(tempName, qualifiedName, colonPos + 1, length);
      tempName[length - colonPos - 1] = 0;
      _name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(tempName);
      delete [] tempName;
    }
  }
  else
  {
    _name = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
    _prefix = 0;
    _bDeleteStrings = false;
  }
}

QualifiedName::QualifiedName(const XMLCh* prefix, const XMLCh* name) : _bDeleteStrings(false)
{
  _prefix = prefix;
  _name = name;
}

QualifiedName::~QualifiedName()
{
  if(_bDeleteStrings)
  {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release((XMLCh**)&_prefix);
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release((XMLCh**)&_name);
  }
}

const XMLCh* QualifiedName::getPrefix() const {
	return _prefix;
}

const XMLCh* QualifiedName::getName() const {
	return _name;
}

const XMLCh* QualifiedName::getFullName(XPath2MemoryManager* memMgr) const {
	if(_prefix != NULL) {
		XMLCh szColon[2] = {XERCES_CPP_NAMESPACE_QUALIFIER chColon, XERCES_CPP_NAMESPACE_QUALIFIER chNull};
		return XPath2Utils::concatStrings(_prefix, szColon, _name, memMgr);
	}

	return memMgr->getPooledString(_name);
}
