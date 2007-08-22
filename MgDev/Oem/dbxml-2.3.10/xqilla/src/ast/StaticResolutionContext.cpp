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
 * $Id: StaticResolutionContext.cpp,v 1.14 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/UTF8Str.hpp>

using namespace std;

StaticResolutionContext::StaticResolutionContext(XPath2MemoryManager* memMgr)
  : _dynamicVariables(17, false, memMgr),
    _uriPool(17, memMgr),
    _memMgr(memMgr)
{
  clear();
}

StaticResolutionContext::StaticResolutionContext(const StaticResolutionContext &o, XPath2MemoryManager* memMgr)
  : _dynamicVariables(17, false, memMgr),
    _uriPool(17, memMgr),
    _memMgr(memMgr)
{
  clear();
  copy(o);
}

void StaticResolutionContext::copy(const StaticResolutionContext &o)
{
  add(o);
  _properties = o._properties;
  _staticType = o._staticType;
}

void StaticResolutionContext::clear()
{
  _contextItem = false;
  _contextPosition = false;
  _contextSize = false;
  _currentTime = false;
  _implicitTimezone = false;
  _availableDocuments = false;
  _availableCollections = false;
  _forceNoFolding = false;
  _creative = false;

  _properties = 0;
  _staticType = StaticType();

  _dynamicVariables.removeAll();
  _uriPool.flushAll();
}

void StaticResolutionContext::contextItemUsed(bool value)
{
  _contextItem = value;
}

void StaticResolutionContext::contextPositionUsed(bool value)
{
  _contextPosition = value;
}

void StaticResolutionContext::contextSizeUsed(bool value)
{
  _contextSize = value;
}

bool StaticResolutionContext::isContextItemUsed() const
{
  return _contextItem;
}

bool StaticResolutionContext::isContextPositionUsed() const
{
  return _contextPosition;
}

bool StaticResolutionContext::isContextSizeUsed() const
{
  return _contextSize;
}

/** Returns true if any of the context item flags have been used */
bool StaticResolutionContext::areContextFlagsUsed() const
{
  return _contextItem || _contextPosition || _contextSize;
}

void StaticResolutionContext::currentTimeUsed(bool value)
{
  _currentTime = value;
}

void StaticResolutionContext::implicitTimezoneUsed(bool value)
{
  _implicitTimezone = value;
}

void StaticResolutionContext::availableDocumentsUsed(bool value)
{
  _availableDocuments = value;
}

void StaticResolutionContext::availableCollectionsUsed(bool value)
{
  _availableCollections = value;
}

bool StaticResolutionContext::areDocsOrCollectionsUsed() const
{
  return _availableDocuments || _availableCollections;
}

void StaticResolutionContext::forceNoFolding(bool value)
{
  _forceNoFolding = value;
}

bool StaticResolutionContext::isNoFoldingForced() const
{
  return _forceNoFolding;
}

void StaticResolutionContext::variableUsed(const XMLCh *namespaceURI, const XMLCh *name)
{
  namespaceURI = _memMgr->getPooledString(namespaceURI);
  name = _memMgr->getPooledString(name);
  unsigned int nsID = _uriPool.addOrFind(namespaceURI);
  _dynamicVariables.put((void*)name, nsID, 0);
}

vector<pair<const XMLCh*, const XMLCh*> > StaticResolutionContext::variablesUsed() const
{
  vector<pair<const XMLCh*, const XMLCh*> > result;

  const XMLCh* namespaceURI;
  const XMLCh* name;
  int nsID;
  VariableAccessSetEnumerator it(const_cast<VariableAccessSet *>(&_dynamicVariables));
  while(it.hasMoreElements()) {
    it.nextElementKey((void*&)name, nsID);
    namespaceURI = _uriPool.getValueForId(nsID);
    result.push_back(pair<const XMLCh*, const XMLCh*>(namespaceURI, name));
  }  

  return result;
}

bool StaticResolutionContext::removeVariable(const XMLCh *namespaceURI, const XMLCh *name)
{
  namespaceURI = _memMgr->getPooledString(namespaceURI);
  name = _memMgr->getPooledString(name);
  unsigned int nsID = _uriPool.getId(namespaceURI);
  if(nsID != 0 && _dynamicVariables.containsKey((void*)name, nsID)) {
    _dynamicVariables.removeKey((void*)name, nsID);
    return true;
  }
  return false;
}

bool StaticResolutionContext::isVariableUsed(const XMLCh *namespaceURI, const XMLCh *name) const
{
  namespaceURI = _memMgr->getPooledString(namespaceURI);
  name = _memMgr->getPooledString(name);
  unsigned int nsID = _uriPool.getId(namespaceURI);
  if(nsID != 0 && _dynamicVariables.containsKey((void*)name, nsID)) {
    return true;
  }
  return false;
}

/** Sets the members of this StaticResolutionContext from the given StaticResolutionContext */
void StaticResolutionContext::add(const StaticResolutionContext &o)
{
  if(o._contextItem) _contextItem = true;
  if(o._contextPosition) _contextPosition = true;
  if(o._contextSize) _contextSize = true;
  if(o._currentTime) _currentTime = true;
  if(o._implicitTimezone) _implicitTimezone = true;
  if(o._availableDocuments) _availableDocuments = true;
  if(o._availableCollections) _availableCollections = true;
  if(o._forceNoFolding) _forceNoFolding = true;

  const XMLCh* namespaceURI;
  const XMLCh* name;
  int nsID;
  // No const enumerator is provided...
  VariableAccessSetEnumerator it(const_cast<VariableAccessSet *>(&o._dynamicVariables));
  while(it.hasMoreElements()) {
    it.nextElementKey((void*&)name, nsID);
    namespaceURI = o._uriPool.getValueForId(nsID);
    variableUsed(namespaceURI, name);
  }
}

void StaticResolutionContext::addExceptContextFlags(const StaticResolutionContext &o)
{
  if(o._currentTime) _currentTime = true;
  if(o._implicitTimezone) _implicitTimezone = true;
  if(o._availableDocuments) _availableDocuments = true;
  if(o._availableCollections) _availableCollections = true;
  if(o._forceNoFolding) _forceNoFolding = true;

  const XMLCh* namespaceURI;
  const XMLCh* name;
  int nsID;
  // No const enumerator is provided...
  VariableAccessSetEnumerator it(const_cast<VariableAccessSet *>(&o._dynamicVariables));
  while(it.hasMoreElements()) {
    it.nextElementKey((void*&)name, nsID);
    namespaceURI = o._uriPool.getValueForId(nsID);
    variableUsed(namespaceURI, name);
  }
}


/** Returns true if flags are set, or variables have been used */
bool StaticResolutionContext::isUsed() const
{
  return _contextItem || _contextPosition || _contextSize
    || _currentTime || _implicitTimezone || _availableCollections
    || _availableDocuments || _forceNoFolding || !_dynamicVariables.isEmpty();
}

bool StaticResolutionContext::isUsedExceptContextFlags() const
{
  return _currentTime || _implicitTimezone || _availableCollections
    || _availableDocuments || _forceNoFolding || !_dynamicVariables.isEmpty();
}

bool StaticResolutionContext::areContextTimeUsed() const
{
  return _currentTime || _implicitTimezone;
}

void StaticResolutionContext::creative(bool value)
{
  _creative = value;
}

bool StaticResolutionContext::isCreative() const
{
  return _creative;
}

unsigned int StaticResolutionContext::getProperties() const
{
	return _properties;
}

void StaticResolutionContext::setProperties(unsigned int props)
{
	_properties = props;
}

const StaticType &StaticResolutionContext::getStaticType() const
{
	return _staticType;
}

StaticType &StaticResolutionContext::getStaticType()
{
	return _staticType;
}

std::string StaticResolutionContext::toString() const
{
  std::ostringstream s;

  s << "Context Item:          " << (_contextItem ? "true" : "false") << std::endl;
  s << "Context Position:      " << (_contextPosition ? "true" : "false") << std::endl;
  s << "Context Size:          " << (_contextSize ? "true" : "false") << std::endl;
  s << "Current Time:          " << (_currentTime ? "true" : "false") << std::endl;
  s << "Implicit Timezone:     " << (_implicitTimezone ? "true" : "false") << std::endl;
  s << "Available Documents:   " << (_availableDocuments ? "true" : "false") << std::endl;
  s << "Available Collections: " << (_availableCollections ? "true" : "false") << std::endl;
  s << "Force No Folding:      " << (_forceNoFolding ? "true" : "false") << std::endl;

  s << "Variables Used: [";
  const XMLCh* namespaceURI;
  const XMLCh* name;
  int nsID;
  bool first = true;
  VariableAccessSetEnumerator it(const_cast<VariableAccessSet *>(&_dynamicVariables));
  while(it.hasMoreElements()) {
		it.nextElementKey((void*&)name, nsID);
    namespaceURI = _uriPool.getValueForId(nsID);

    if(first) {
      first = false;
    }
    else {
      s << ", ";
    }

    s << "{" << UTF8(namespaceURI) << "}:" << UTF8(name);
	}
  s << "]" << std::endl;

  return s.str();
}
