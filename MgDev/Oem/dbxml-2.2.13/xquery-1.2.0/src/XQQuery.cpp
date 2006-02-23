//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQQuery.cpp,v 1.27 2005/07/14 15:21:12 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQQuery.cpp: implementation of the XQQuery class.
//////////////////////////////////////////////////////////////////////

#include "xquery/XQQuery.hpp"
#include "xquery/XQEngine.hpp"
#include "xquery/functions/XQFunction.hpp"
#include "xquery/dataItem/XQGlobalVariable.hpp"
#include "xquery/context/XQContextImpl.hpp"
#include "xquery/XQException.hpp"
#include "xquery/XQEvaluator.hpp"
#include "xquery/XQDebugCallback.hpp"
#include "xquery/dom-extensions/XQScopedNamespace.hpp"
#include <pathan/functionAPI/FunctionLookup.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/exceptions/ContextException.hpp>
#include <pathan/Node.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/dataItem/Result.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/XMLResourceIdentifier.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/util/Janitor.hpp>

#include <sstream>
#include <list>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQQuery::XQQuery(const XMLCh* queryText, XPath2MemoryManager* memMgr) :
  m_userDefFns(PathanAllocator<XQFunction*>(memMgr)),
  m_userDefVars(PathanAllocator<XQGlobalVariable*>(memMgr))
{
  m_memMgr=memMgr;
  m_query=NULL;
  m_bIsLibraryModule=false;
  m_szTargetNamespace=NULL;
  m_szCurrentFile = NULL;
  m_szQueryText=m_memMgr->getPooledString(queryText);
}

Result XQQuery::evaluate(XQContext* context) const
{
  if(context->getDebugCallback()) {
    return new DebugResult(this, context);
  }
  else {
    return new QueryResult(this, context);
  }
}

void XQQuery::staticResolution(XQContext *context)
{
  for(vector<XQGlobalVariable*, PathanAllocator<XQGlobalVariable*> >::iterator it = m_userDefVars.begin();
      it != m_userDefVars.end(); ++it) {
    (*it) = (XQGlobalVariable*)(*it)->staticResolution(context);
  }
  for(vector<XQFunction*, PathanAllocator<XQFunction*> >::iterator i = m_userDefFns.begin();
      i != m_userDefFns.end(); ++i) {
    (*i)->staticResolution(context);
  }
  if(m_query) m_query = m_query->staticResolution(context);
}

DataItem* XQQuery::getQueryBody() const
{
  return m_query;
}

void XQQuery::setQueryBody(DataItem* query)
{
  m_query=query;
}

void XQQuery::addFunction(XQFunction* fnDef)
{
  m_userDefFns.push_back(fnDef);
}

void XQQuery::addVariable(XQGlobalVariable* varDef)
{
  m_userDefVars.push_back(varDef);
}

void XQQuery::setIsLibraryModule(bool bIsModule/*=true*/)
{
  m_bIsLibraryModule=bIsModule;
}

bool XQQuery::getIsLibraryModule() const
{
  return m_bIsLibraryModule;
}

void XQQuery::setModuleTargetNamespace(const XMLCh* uri)
{
  m_szTargetNamespace=uri;
}

const XMLCh* XQQuery::getModuleTargetNamespace() const
{
  return m_szTargetNamespace;
}

void XQQuery::importModule(const XMLCh* szUri, VectorOfStrings* locations, StaticContext* ctx)
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);

  XQContextImpl moduleCtx(context->getMemoryManager());
  // force the context to use our memory manager
  moduleCtx.setMemoryManager(context->getMemoryManager());
  // we also need to fix the namespace resolver, because it has already been initialized using the internal memory manager
  moduleCtx.setNSResolver(new (context->getMemoryManager()) XQScopedNamespace(context->getMemoryManager(), moduleCtx.getNSResolver()));
  // propagate debug settings
  moduleCtx.enableDebugging(context->isDebuggingEnabled());

  bool bFound=false;
  for(VectorOfStrings::iterator it=locations->begin();it!=locations->end();it++)
  {
      const XMLCh* fullPath=NULL;
      XERCES_CPP_NAMESPACE_QUALIFIER InputSource* srcToUse = 0;
      if (context->getDocumentCache()->getXMLEntityResolver()){
        XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier resourceIdentifier(XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier::UnKnown,
                                                                                *it, 
                                                                                szUri, 
                                                                                XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, 
                                                                                context->getBaseURI());
        srcToUse = context->getDocumentCache()->getXMLEntityResolver()->resolveEntity(&resourceIdentifier);
      }

      if(srcToUse==0)
      {
          try {
            XERCES_CPP_NAMESPACE_QUALIFIER XMLURL urlTmp(context->getBaseURI(), *it);
            if (urlTmp.isRelative()) {
              throw XERCES_CPP_NAMESPACE_QUALIFIER MalformedURLException(__FILE__, __LINE__, XERCES_CPP_NAMESPACE_QUALIFIER XMLExcepts::URL_NoProtocolPresent);
            }
            srcToUse = new XERCES_CPP_NAMESPACE_QUALIFIER URLInputSource(urlTmp);
          }
          catch(const XERCES_CPP_NAMESPACE_QUALIFIER MalformedURLException&) {
            // It's not a URL, so let's assume it's a local file name.
            const XMLCh* baseUri=context->getBaseURI();
            if(baseUri && baseUri[0]) {
              XMLCh* tmpBuf = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::weavePaths(baseUri, *it);
              srcToUse = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileInputSource(tmpBuf);
              XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager->deallocate(tmpBuf);
            }
            else {
              srcToUse = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileInputSource(*it);
            }
          }
      }
      XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER InputSource> janIS(srcToUse);
      moduleCtx.setBaseURI(srcToUse->getSystemId());
      try {
        XQQuery* pParsedQuery = XQEvaluator::parse(*srcToUse, &moduleCtx, false);
        if(!pParsedQuery->getIsLibraryModule()) {
          XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
          errMsg.set(X("The module at "));
          errMsg.append(*it);
          errMsg.append(X(" is not a module"));
              DSLthrow(ContextException,X("XQQuery::ImportModule"), errMsg.getRawBuffer());
        }
        if(!XERCES_CPP_NAMESPACE::XMLString::equals(szUri,pParsedQuery->getModuleTargetNamespace())) {
          XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
          errMsg.set(X("The module at "));
          errMsg.append(*it);
          errMsg.append(X(" specifies a different namespace"));
          DSLthrow(ContextException,X("XQQuery::ImportModule"), errMsg.getRawBuffer());
        }
        // now move the variable declarations and the function definitions into my context
        for(vector<XQFunction*, PathanAllocator<XQFunction*> >::iterator itFn = pParsedQuery->m_userDefFns.begin();
            itFn != pParsedQuery->m_userDefFns.end(); ++itFn) {
          m_userDefFns.push_back(*itFn);
          context->addCustomFunction(*itFn);
        }
        for(vector<XQGlobalVariable*, PathanAllocator<XQGlobalVariable*> >::iterator itVar = pParsedQuery->m_userDefVars.begin();
            itVar != pParsedQuery->m_userDefVars.end(); ++itVar) {
          // Should this set a global variable in the context? - jpcs
          m_userDefVars.push_back(*itVar);
        }
        bFound=true;
        break;
      }
      catch(XQException& e) {
        if(e.m_szFile==NULL) {
          e.m_szFile=fullPath;
        }
	if (e.m_nLine != 0 ||
	    e.m_nColumn != 0) {
		// found module, parse error
		throw;
	}
	
      }
  }
  if(!bFound)
      DSLthrow(ContextException,X("XQQuery::ImportModule"), X("Cannot locate the requested module"));
}

const XMLCh* XQQuery::getFile() const
{
	return m_szCurrentFile;
}

void XQQuery::setFile(const XMLCh* file)
{
	m_szCurrentFile=m_memMgr->getPooledString(file);
}

const XMLCh* XQQuery::getQueryText() const
{
    return m_szQueryText;
}

XQQuery::QueryResult::QueryResult(const XQQuery *query, DynamicContext *context)
  : ResultImpl(context),
    _query(query),
    _parent(0),
    _toDo(true)
{
}

Item::Ptr XQQuery::QueryResult::next(DynamicContext *context)
{
  if(_toDo) {
    _toDo = false;

    // define global variables
    for(vector<XQGlobalVariable*, PathanAllocator<XQGlobalVariable*> >::const_iterator it = _query->m_userDefVars.begin();
        it != _query->m_userDefVars.end(); ++it) {
      (*it)->collapseTree(context).toSequence(context);
    }
    if(_query->getQueryBody() != NULL) {
      _parent = _query->getQueryBody()->collapseTree(context);
    }
  }

  Item::Ptr item = _parent.next(context);

  if(item == NULLRCP) {
    _parent = 0;
  }

  return item;
}

std::string XQQuery::QueryResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<queryresult>" << std::endl;
  oss << _parent.asString(context, indent + 1);
  oss << in << "</queryresult>" << std::endl;

  return oss.str();
}

XQQuery::DebugResult::DebugResult(const XQQuery *query, DynamicContext *context)
  : LazySequenceResult(context),
    _query(query)
{
}

void XQQuery::DebugResult::getResult(Sequence &toFill, DynamicContext *ctx) const
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);

  static XMLCh szMain[]= { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

  if(context->getDebugCallback()) {
    context->getDebugCallback()->NotifyQueryBegin(context, _query->getQueryText());
    context->getDebugCallback()->EnterFunction(context, _query->getFile(), szMain, 0, 0);
  }

  try
  {
    // define global variables
    for(vector<XQGlobalVariable*, PathanAllocator<XQGlobalVariable*> >::const_iterator it = _query->m_userDefVars.begin();
        it != _query->m_userDefVars.end(); ++it) {
      (*it)->collapseTree(context).toSequence(context);
    }

    if(_query->getQueryBody() != NULL) {
      toFill = _query->getQueryBody()->collapseTree(context);
    }
  }
  catch(XQException& e)
  {
    throw XQException(e.getError(),e.m_szFile?e.m_szFile:_query->getFile(),e.m_nLine,e.m_nColumn);
  }
  catch(DSLException& e)
  {
    if(context->getDebugCallback() && context->isDebuggingEnabled()) 
      context->getDebugCallback()->ReportFirstError(context, e.getError(), _query->getFile(), 0);
    throw XQException(e.getError(),_query->getFile(),0,0);
  }

  if(context->getDebugCallback()) {
    context->getDebugCallback()->ExitFunction(context, szMain);
    context->getDebugCallback()->NotifyQueryEnd(context, toFill);
  }
}

std::string XQQuery::DebugResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<debugresult/>" << std::endl;

  return oss.str();
}
