#include "../config/pathan_config.h"
#include <pathan/internal/parser/QName.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>

//Parse qualifiedName into prefix and name
QualifiedName::QualifiedName(const XMLCh* const qualifiedName, XPath2MemoryManager* memMgr) : _bDeleteStrings(false)
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

QualifiedName::QualifiedName(const XMLCh* qualifiedName) : _bDeleteStrings(true)
{
  int colonPos = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf(qualifiedName, XERCES_CPP_NAMESPACE_QUALIFIER chColon, 0);
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

QualifiedName::QualifiedName(const XMLCh* const prefix, const XMLCh* const name) : _bDeleteStrings(false)
{
  _prefix = prefix;
  _name = name;
}

QualifiedName::~QualifiedName()
{
  if(_bDeleteStrings)
  {
    delete [] (XMLCh*)_prefix;
    delete [] (XMLCh*)_name;
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
