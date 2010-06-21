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
 * $Id: VarStoreImpl.cpp,v 1.12 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../../config/xqilla_config.h"
#include <assert.h>
#include "VarStoreImpl.hpp"
#include <xqilla/context/impl/XQContextImpl.hpp>
#include <xqilla/context/impl/VarHashEntryImpl.hpp>
#include <xqilla/context/StaticContext.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xqilla/items/Item.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xqilla/context/DynamicContext.hpp>

VarStoreImpl::VarStoreImpl(XPath2MemoryManager* memMgr)
  : _store(memMgr)
{
}

VarStoreImpl::~VarStoreImpl()
{
  /* nothing to do */
}

void VarStoreImpl::clear()
{
  _store.clear();
}

void VarStoreImpl::addLocalScope()
{
  _store.addScope(Scope<Sequence>::LOCAL_SCOPE);
}

void VarStoreImpl::addLogicalBlockScope()
{
  _store.addScope(Scope<Sequence>::LOGICAL_BLOCK_SCOPE);
}

void VarStoreImpl::removeScope()
{
  _store.removeScope();
}

VariableStore::MyScope *VarStoreImpl::getScopeState()
{
  return _store.getCurrentScope();
}

void VarStoreImpl::setScopeState(VariableStore::MyScope *state)
{
  _store.setCurrentScope(state);
}

const XMLCh* VarStoreImpl::getVariableNsURI(const XMLCh* qName, const StaticContext* context,
                                            const LocationInfo *location) const
{
  const XMLCh* uri=NULL;
  const XMLCh* prefix=XPath2NSUtils::getPrefix(qName, context->getMemoryManager());
  if(prefix && *prefix)
    uri = context->getUriBoundToPrefix(prefix, location);
  return uri;
}

void VarStoreImpl::setGlobalVar(const XMLCh* ident,
                                const Sequence& value,
                                const StaticContext* context,
                                const LocationInfo *location)
{
  _store.setGlobalVar(getVariableNsURI(ident, context, location), XPath2NSUtils::getLocalName(ident), value);
}

void VarStoreImpl::setGlobalVar(const XMLCh* namespaceURI,
                                const XMLCh* name,
                                const Sequence &value, 
                                const StaticContext* context)
{
  _store.setGlobalVar(namespaceURI, name, value);
}

void VarStoreImpl::setVar(const XMLCh* ident,
                          const Sequence &value,
                          const StaticContext* context,
                          const LocationInfo *location)
{
  _store.setVar(getVariableNsURI(ident, context, location), XPath2NSUtils::getLocalName(ident), value);
}

void VarStoreImpl::setVar(const XMLCh* namespaceURI,
                          const XMLCh* name,
                          const Sequence &value, 
                          const StaticContext* context)
{
  _store.setVar(namespaceURI, name, value);
}

void VarStoreImpl::declareVar(const XMLCh* ident,
                              const Sequence &value,
                              const StaticContext* context,
                              const LocationInfo *location)
{
  _store.declareVar(getVariableNsURI(ident, context, location), XPath2NSUtils::getLocalName(ident), value);
}

void VarStoreImpl::declareVar(const XMLCh* namespaceURI,
                              const XMLCh* name,
                              const Sequence &value, 
                              const StaticContext* context)
{
  _store.declareVar(namespaceURI, name, value);
}

const std::pair<bool, Sequence> VarStoreImpl::getVar(const XMLCh* ident,
                                                     const StaticContext* context,
                                                     const LocationInfo *location) const
{
  return getVar(getVariableNsURI(ident, context, location), XPath2NSUtils::getLocalName(ident), context);
}

const std::pair<bool, Sequence> VarStoreImpl::getVar(const XMLCh* namespaceURI,
                                                       const XMLCh* name,
                                                       const StaticContext* context) const
{
  VariableStore::Entry* result = _store.getVar(namespaceURI, name);
  if(result)
    return std::pair<bool, Sequence>(true, result->getValue());
  return std::pair<bool, Sequence>(false, Sequence(context->getMemoryManager()));
}

VariableStore::Entry* VarStoreImpl::getReferenceVar(const XMLCh* ident,
                                                    const StaticContext* context,
                                                    const LocationInfo *location) const
{
  return _store.getVar(getVariableNsURI(ident, context, location), XPath2NSUtils::getLocalName(ident));
}

/** Returns a null VarHashEntry if unsuccessful */
VariableStore::Entry* VarStoreImpl::getReferenceVar(const XMLCh* namespaceURI,
                                                        const XMLCh* name,
                                                        const StaticContext* context) const
{
  return _store.getVar(namespaceURI, name);
}

/** Change getGlobalVar to return a null Sequence, rather than
    a pair with sucess boolean... */
const std::pair<bool, Sequence> VarStoreImpl::getGlobalVar(const XMLCh* ident,
                                                           const StaticContext* context,
                                                           const LocationInfo *location) const
{
  return getGlobalVar(getVariableNsURI(ident, context, location), XPath2NSUtils::getLocalName(ident), context);
}

const std::pair<bool, Sequence> VarStoreImpl::getGlobalVar(const XMLCh* namespaceURI,
                                                             const XMLCh* name,
                                                             const StaticContext* context) const
{
  VariableStore::Entry* result = _store.getGlobalVar(namespaceURI, name);
  if(result)
    return std::pair<bool, Sequence>(true, result->getValue());
  return std::pair<bool, Sequence>(false, Sequence(context->getMemoryManager()));
}

void VarStoreImpl::delVar(const XMLCh* ident,
                          const StaticContext* context,
                          const LocationInfo *location)
{
  _store.delVar(getVariableNsURI(ident, context, location), XPath2NSUtils::getLocalName(ident));
}

void VarStoreImpl::delVar( const XMLCh* namespaceURI, const XMLCh* name, const StaticContext* context )
{
  _store.delVar(namespaceURI, name);
}

void VarStoreImpl::delGlobalVar(const XMLCh* ident,
                                const StaticContext* context,
                                const LocationInfo *location)
{
  _store.delGlobalVar(getVariableNsURI(ident, context, location), XPath2NSUtils::getLocalName(ident));
}

void VarStoreImpl::delGlobalVar( const XMLCh* namespaceURI, const XMLCh* name, const StaticContext* context )
{
  _store.delGlobalVar(namespaceURI, name);
}


XMLCh* VarStoreImpl::print(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr) const
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, memMgr);
  buf.set(X("<VariableStore>\n"));
  const Scope<Sequence>* index=_store.getCurrentScope();
  if(index==NULL)
    index=_store.getGlobalScope();

  XQContextImpl xp2c(memMgr, 0, 0);

  while(index)
    {
      buf.append(X("<Scope type=\""));
      switch(index->getType()) {
      case Scope<Sequence>::GLOBAL_SCOPE: {
        buf.append(X("global"));
        break;
      }
      case Scope<Sequence>::LOCAL_SCOPE: {
        buf.append(X("local"));
        break;
      }
      case Scope<Sequence>::LOGICAL_BLOCK_SCOPE: {
        buf.append(X("logical_block"));
        break;
      }
      }
      buf.append(X("\">\n"));

      std::vector< std::pair<unsigned int, const XMLCh*> > vars = index->getVars();
      for(std::vector< std::pair<unsigned int, const XMLCh*> >::iterator it=vars.begin();it!=vars.end();it++)
        {
          buf.append(X("<Variable uri=\""));
          buf.append(_store.lookUpNamespace(it->first));
          buf.append(X("\" name=\""));
          buf.append(it->second);
          buf.append(X("\">"));

          VarHashEntry<Sequence>* pEntry=const_cast<Scope<Sequence>*>(index)->get(it->first,it->second);

          Sequence value(pEntry->getValue());
          size_t len=value.getLength();
          if(len>0) {
            if(len==1)
              buf.append(value.first()->asString(&xp2c));
            else {
              XMLCh szOpenParen[]={ XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
              XMLCh szCloseParen[]={ XERCES_CPP_NAMESPACE_QUALIFIER chCloseParen, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
              XMLCh szComma[]={ XERCES_CPP_NAMESPACE_QUALIFIER chComma, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
              buf.append(szOpenParen);
              Sequence::iterator end = value.end();
              for(Sequence::iterator i = value.begin(); i != end;) {
                buf.append((*i)->asString(&xp2c));
                if(++i != end)
                  buf.append(szComma);
              }
              buf.append(szCloseParen);
            }
          }

          buf.append(X("</Variable>\n"));
        }

      buf.append(X("</Scope>\n"));
      if(index==_store.getGlobalScope())
        break;
      else
        {
          index=const_cast<Scope<Sequence>*>(index)->getNext();
          if(index==NULL)
            index=_store.getGlobalScope();
        }
    }
  buf.append(X("</VariableStore>\n"));

  XMLCh* result = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(buf.getRawBuffer());
  return result;
}

std::vector< std::pair<const XMLCh*, const XMLCh*> > VarStoreImpl::getVars() const
{
  std::vector< std::pair<const XMLCh*, const XMLCh*> > result;
  // List the local scopes
  const Scope<Sequence>* index=_store.getCurrentScope();
  while(index)
    {
      std::vector< std::pair<unsigned int, const XMLCh*> > tmp=index->getVars();
      for(std::vector< std::pair<unsigned int, const XMLCh*> >::iterator it=tmp.begin();it!=tmp.end();it++)
        result.push_back(std::pair<const XMLCh*, const XMLCh*>(_store.lookUpNamespace(it->first),it->second));
      // stop the search when we are going out of scope
      if(index->getType() == Scope<Sequence>::LOCAL_SCOPE)
        break;
      index=const_cast<Scope<Sequence>*>(index)->getNext();
    }
  // Check the global scope
  std::vector< std::pair<unsigned int, const XMLCh*> > tmp=_store.getGlobalScope()->getVars();
  for(std::vector< std::pair<unsigned int, const XMLCh*> >::iterator it=tmp.begin();it!=tmp.end();it++)
    result.push_back(std::pair<const XMLCh*, const XMLCh*>(_store.lookUpNamespace(it->first),it->second));

  return result;
}
