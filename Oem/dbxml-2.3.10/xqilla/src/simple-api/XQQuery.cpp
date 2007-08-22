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
 * $Id: XQQuery.cpp,v 1.31.2.2 2007/01/03 19:19:51 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQQuery.cpp: implementation of the XQQuery class.
//////////////////////////////////////////////////////////////////////

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/simple-api/XQilla.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/impl/XQContextImpl.hpp>
#include <xqilla/context/XQDebugCallback.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/functions/FunctionLookup.hpp>
#include <xqilla/functions/XQUserFunction.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/PrintAST.hpp>

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

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQQuery::XQQuery(const XMLCh* queryText, DynamicContext *context, bool contextOwned, MemoryManager *memMgr)
  : m_memMgr(memMgr),
    m_context(context),
    m_contextOwned(contextOwned),
    m_query(NULL),
    m_bIsLibraryModule(false),
    m_szTargetNamespace(NULL),
    m_szQueryText(m_context->getMemoryManager()->getPooledString(queryText)),
    m_szCurrentFile(NULL),
    m_userDefFns(XQillaAllocator<XQUserFunction*>(memMgr)),
    m_userDefVars(XQillaAllocator<XQGlobalVariable*>(memMgr)),
    m_importedModules(XQillaAllocator<XQQuery*>(memMgr))
{
}

XQQuery::~XQQuery()
{
  for(ImportedModules::iterator it = m_importedModules.begin();
      it != m_importedModules.end(); ++it) {
    delete *it;
  }
	if(m_contextOwned)
		delete m_context;
}

DynamicContext *XQQuery::createDynamicContext(MemoryManager *memMgr) const
{
  return m_context->createDynamicContext(memMgr);
}

Result XQQuery::execute(DynamicContext* context) const
{
  if(context->getDebugCallback()) {
    return new DebugResult(this, context);
  }
  else {
    return new QueryResult(this);
  }
}

void XQQuery::staticResolution(StaticContext *context)
{
  if(context == 0) context = m_context;

  // Run staticResolutionStage1 on the user defined functions,
  // which gives them the static type they were defined with
  UserFunctions::iterator i;
  for(i = m_userDefFns.begin(); i != m_userDefFns.end(); ++i) {
    if(getIsLibraryModule() && !XERCES_CPP_NAMESPACE::XMLString::equals((*i)->getURI(), getModuleTargetNamespace()))
      XQThrow3(StaticErrorException,X("XQQuery::staticResolution"), X("Every function in a module must be in the module namespace [err:XQST0048]."), *i);
    (*i)->staticResolutionStage1(context);
  }

  // Define types for the imported variables
  for(ImportedModules::const_iterator modIt = m_importedModules.begin();
      modIt != m_importedModules.end(); ++modIt) {
    for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
        varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
      context->getVariableTypeStore()->
        declareGlobalVar((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName(),
                         (*varIt)->getStaticResolutionContext());
    }
  }

  // Run staticResolution on the global variables
  if(!m_userDefVars.empty())
  {
    GlobalVariables::iterator itVar, itVar2;
    // declare all the global variables with a special StaticResolutionContext, in order to recognize 'variable is defined later' errors 
    // instead of more generic 'variable not found'
    // In order to catch references to not yet defined variables (but when no recursion happens) we also create a scope where we undefine
    // the rest of the variables (once we enter in a function call, the scope will disappear and the forward references to the global variables 
    // will happear)
    StaticResolutionContext forwardRef(context->getMemoryManager());
    forwardRef.setProperties(StaticResolutionContext::FORWARDREF);
    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) {
      const XMLCh* varName=(*itVar)->getVariableName();
      const XMLCh* prefix=XPath2NSUtils::getPrefix(varName, context->getMemoryManager());
      const XMLCh* uri=NULL;
      if(prefix && *prefix)
        uri = context->getUriBoundToPrefix(prefix, *itVar);
      const XMLCh* name= XPath2NSUtils::getLocalName(varName);
      context->getVariableTypeStore()->declareGlobalVar(uri, name, forwardRef);
    }
    StaticResolutionContext forwardRef2(context->getMemoryManager());
    forwardRef2.setProperties(StaticResolutionContext::UNDEFINEDVAR);
    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) {
      context->getVariableTypeStore()->addLogicalBlockScope();
      for(itVar2 = itVar; itVar2 != m_userDefVars.end(); ++itVar2) {
          const XMLCh* varName=(*itVar2)->getVariableName();
          const XMLCh* prefix=XPath2NSUtils::getPrefix(varName, context->getMemoryManager());
          const XMLCh* uri=NULL;
          if(prefix && *prefix)
            uri = context->getUriBoundToPrefix(prefix, *itVar2);
          const XMLCh* name= XPath2NSUtils::getLocalName(varName);
          context->getVariableTypeStore()->declareVar(uri,name,forwardRef2);
      }
      (*itVar)->staticResolution(context);
      context->getVariableTypeStore()->removeScope();
      if(getIsLibraryModule() && !XERCES_CPP_NAMESPACE::XMLString::equals((*itVar)->getVariableURI(),
		 getModuleTargetNamespace()))
        XQThrow3(StaticErrorException,X("XQQuery::staticResolution"),
		X("Every global variable in a module must be in the module namespace [err:XQST0048]."), *itVar);
    }
    // check for duplicate variable declarations
    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) 
    {
      for (GlobalVariables::iterator it2 = itVar+1; it2 != m_userDefVars.end(); ++it2) 
      {
        if(XPath2Utils::equals((*itVar)->getVariableURI(), (*it2)->getVariableURI()) &&
           XPath2Utils::equals((*itVar)->getVariableLocalName(), (*it2)->getVariableLocalName()))
        {
          XMLBuffer errMsg(1023, context->getMemoryManager());
          errMsg.set(X("A variable with name {"));
            errMsg.append((*itVar)->getVariableURI());
          errMsg.append(X("}"));
            errMsg.append((*itVar)->getVariableLocalName());
          errMsg.append(X(" has already been declared [err:XQST0049]"));
          XQThrow3(StaticErrorException,X("XQQuery::staticResolution"), errMsg.getRawBuffer(), *itVar);
        }
      }
      for(ImportedModules::const_iterator modIt = m_importedModules.begin();
          modIt != m_importedModules.end(); ++modIt) {
        for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
            varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
          if(XPath2Utils::equals((*itVar)->getVariableURI(), (*varIt)->getVariableURI()) &&
             XPath2Utils::equals((*itVar)->getVariableLocalName(), (*varIt)->getVariableLocalName())) {
            XMLBuffer errMsg(1023, context->getMemoryManager());
            errMsg.set(X("A variable with name {"));
            errMsg.append((*itVar)->getVariableURI());
            errMsg.append(X("}"));
            errMsg.append((*itVar)->getVariableLocalName());
            errMsg.append(X(" has already been imported from a module [err:XQST0049]"));
            XQThrow3(StaticErrorException,X("XQQuery::staticResolution"), errMsg.getRawBuffer(), *varIt);
          }
        }
      }
    }
  }

  // Run staticResolutionStage2 on the user defined functions,
  // which statically resolves their function bodies
  for(i = m_userDefFns.begin(); i != m_userDefFns.end(); ++i) {
      try {
        (*i)->staticResolutionStage2(context);
      } catch(XQException& e) {
        XMLBuffer errMsg(1023, context->getMemoryManager());
        errMsg.set(X("Error while running static resolution on user-defined function {"));
        errMsg.append((*i)->getURI());
        errMsg.append(X("}"));
        errMsg.append((*i)->getName());
        errMsg.append(X(": "));
        errMsg.append(e.getError());
        XQThrow3(StaticErrorException,X("XQQuery::staticResolution"), errMsg.getRawBuffer(), *i);
      }
  }
  if(m_query) m_query = m_query->staticResolution(context);

  staticTyping(context);
}

void XQQuery::staticTyping(StaticContext *context)
{
  if(context == 0) context = m_context;

  VariableTypeStore* varStore = context->getVariableTypeStore();

  // Define types for the imported variables
  for(ImportedModules::const_iterator modIt = m_importedModules.begin();
      modIt != m_importedModules.end(); ++modIt) {
    for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
        varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
      varStore->declareGlobalVar((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName(),
                                 (*varIt)->getStaticResolutionContext());
    }
  }

  // Run staticTyping on the global variables
  if(!m_userDefVars.empty())
  {
    // declare all the global variables with a special StaticResolutionContext, in order to recognize 'variable is defined
    // later' errors instead of more generic 'variable not found'. In order to catch references to not yet defined
    // variables (but when no recursion happens) we also create a scope where we undefine the rest of the variables (once
    // we enter in a function call, the scope will disappear and the forward references to the global variables will
    // appear).
    StaticResolutionContext forwardRef(context->getMemoryManager());
    forwardRef.setProperties(StaticResolutionContext::FORWARDREF);

    StaticResolutionContext undefinedVar(context->getMemoryManager());
    undefinedVar.setProperties(StaticResolutionContext::UNDEFINEDVAR);

    GlobalVariables::iterator itVar, itVar2;
    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) {
      varStore->declareGlobalVar((*itVar)->getVariableURI(), (*itVar)->getVariableLocalName(),
                                 forwardRef);
    }

    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) {
      varStore->addLogicalBlockScope();
      for(itVar2 = itVar; itVar2 != m_userDefVars.end(); ++itVar2) {
        varStore->declareVar((*itVar2)->getVariableURI(), (*itVar2)->getVariableLocalName(),
                             undefinedVar);
      }
      (*itVar)->staticTyping(context);
      varStore->removeScope();
    }
  }

  // Run staticTyping on the user defined functions,
  // which calculates a better type for them
  UserFunctions::iterator i;
  for(i = m_userDefFns.begin(); i != m_userDefFns.end(); ++i) {
      try {
        (*i)->staticTyping(context);
      } catch(XQException& e) {
        XMLBuffer errMsg(1023, context->getMemoryManager());
        errMsg.set(X("Error while calculating static type of user-defined function {"));
        errMsg.append((*i)->getURI());
        errMsg.append(X("}"));
        errMsg.append((*i)->getName());
        errMsg.append(X(": "));
        errMsg.append(e.getError());
        XQThrow3(StaticErrorException,X("XQQuery::staticTyping"), errMsg.getRawBuffer(), *i);
      }
  }
  if(m_query) m_query = m_query->staticTyping(context);
}

std::string XQQuery::getQueryPlan() const
{
  return PrintAST::print(this, m_context);
}

ASTNode* XQQuery::getQueryBody() const
{
  return m_query;
}

void XQQuery::setQueryBody(ASTNode* query)
{
  m_query=query;
}

void XQQuery::addFunction(XQUserFunction* fnDef)
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

class LoopDetector : public XMLEntityResolver
{
public:
    LoopDetector(XMLEntityResolver* pParent, const XMLCh* myModuleURI) 
    {
        m_pParentResolver=pParent;
        m_PreviousModuleNamespace=myModuleURI;
    }

    virtual InputSource* resolveEntity(XMLResourceIdentifier* resourceIdentifier)
    {
        if(resourceIdentifier->getResourceIdentifierType()==XMLResourceIdentifier::UnKnown &&
            XPath2Utils::equals(resourceIdentifier->getNameSpace(), m_PreviousModuleNamespace))
        {
            XMLBuffer buf;
            buf.set(X("The graph of module imports contains a cycle for namespace '"));
            buf.append(resourceIdentifier->getNameSpace());
            buf.append(X("' [err:XQST0073]"));
            XQThrow2(StaticErrorException, X("XQQuery::ImportModule"), buf.getRawBuffer());
        }
        if(m_pParentResolver)
            return m_pParentResolver->resolveEntity(resourceIdentifier);
        return NULL;
    }

protected:
    XMLEntityResolver* m_pParentResolver;
    const XMLCh* m_PreviousModuleNamespace;
};

void XQQuery::importModule(XQilla::Language language, const XMLCh* szUri,
                           VectorOfStrings* locations, StaticContext* context)
{
  for(ImportedModules::iterator modIt = m_importedModules.begin();
      modIt != m_importedModules.end(); ++modIt) {
      if(XPath2Utils::equals((*modIt)->getModuleTargetNamespace(),szUri))
      {
          XMLBuffer buf(1023,context->getMemoryManager());
          buf.set(X("Module for namespace '"));
          buf.append(szUri);
          buf.append(X("' has already been imported [err:XQST0047]"));
          XQThrow2(StaticErrorException, X("XQQuery::ImportModule"), buf.getRawBuffer());
      }
  }
  if(locations==NULL)
    locations=context->resolveModuleURI(szUri);
  if(locations==NULL || locations->empty())
  {
    XMLBuffer buf(1023,context->getMemoryManager());
    buf.set(X("Cannot locate module for namespace "));
    buf.append(szUri);
    buf.append(X(" without the 'at <location>' keyword [err:XQST0059]"));
    XQThrow2(StaticErrorException,X("XQQuery::ImportModule"), buf.getRawBuffer());
  }

  bool bFound=false;
  for(VectorOfStrings::iterator it=locations->begin();it!=locations->end();it++)
  {
    InputSource* srcToUse = 0;
    if (context->getDocumentCache()->getXMLEntityResolver()){
      XMLResourceIdentifier resourceIdentifier(XMLResourceIdentifier::UnKnown,
                                               *it, szUri, XMLUni::fgZeroLenString, 
                                               context->getBaseURI());
      srcToUse = context->getDocumentCache()->getXMLEntityResolver()->resolveEntity(&resourceIdentifier);
    }

    if(srcToUse==0)
    {
      try {
        XMLURL urlTmp(context->getBaseURI(), *it);
        if (urlTmp.isRelative()) {
          throw MalformedURLException(__FILE__, __LINE__, XMLExcepts::URL_NoProtocolPresent);
        }
        srcToUse = new URLInputSource(urlTmp);
      }
      catch(const MalformedURLException&) {
        // It's not a URL, so let's assume it's a local file name.
        const XMLCh* baseUri=context->getBaseURI();
        if(baseUri && baseUri[0]) {
          XMLCh* tmpBuf = XMLPlatformUtils::weavePaths(baseUri, *it);
          srcToUse = new LocalFileInputSource(tmpBuf);
          XMLPlatformUtils::fgMemoryManager->deallocate(tmpBuf);
        }
        else {
          srcToUse = new LocalFileInputSource(*it);
        }
      }
    }
    Janitor<InputSource> janIS(srcToUse);

    DynamicContext* moduleCtx = context->createModuleContext();
    moduleCtx->setBaseURI(srcToUse->getSystemId());
    LoopDetector loopDetector(context->getXMLEntityResolver(), szUri);
    moduleCtx->setXMLEntityResolver(&loopDetector);
    moduleCtx->setModuleResolver(context->getModuleResolver());

    XQQuery* pParsedQuery = XQilla::parse(*srcToUse, language, moduleCtx);

    if(!pParsedQuery->getIsLibraryModule()) {
      XMLBuffer buf(1023,context->getMemoryManager());
      buf.set(X("The module at "));
      buf.append(srcToUse->getSystemId());
      buf.append(X(" is not a module"));
      XQThrow2(StaticErrorException, X("XQQuery::ImportModule"), buf.getRawBuffer());
    }
    if(!XERCES_CPP_NAMESPACE::XMLString::equals(szUri,pParsedQuery->getModuleTargetNamespace())) {
      XMLBuffer buf(1023,context->getMemoryManager());
      buf.set(X("The module at "));
      buf.append(srcToUse->getSystemId());
      buf.append(X(" specifies a different namespace [err:XQST0059]"));
      XQThrow2(StaticErrorException, X("XQQuery::ImportModule"), buf.getRawBuffer());
    }
    // now move the variable declarations and the function definitions into my context
    for(UserFunctions::iterator itFn = pParsedQuery->m_userDefFns.begin(); itFn != pParsedQuery->m_userDefFns.end(); ++itFn) {
      (*itFn)->setModuleDocumentCache(const_cast<DocumentCache*>(moduleCtx->getDocumentCache()));
      context->addCustomFunction(*itFn);
    }
    for(GlobalVariables::iterator itVar = pParsedQuery->m_userDefVars.begin(); itVar != pParsedQuery->m_userDefVars.end(); ++itVar) {
      for(ImportedModules::const_iterator modIt = m_importedModules.begin();
          modIt != m_importedModules.end(); ++modIt) {
        for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
            varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
          if(XPath2Utils::equals((*varIt)->getVariableURI(), (*itVar)->getVariableURI()) &&
             XPath2Utils::equals((*varIt)->getVariableLocalName(), (*itVar)->getVariableLocalName())) {
            XMLBuffer buf(1023,context->getMemoryManager());
            buf.set(X("An imported variable {"));
            buf.append((*itVar)->getVariableURI());
            buf.append(X("}"));
            buf.append((*itVar)->getVariableLocalName());
            buf.append(X(" conflicts with an already defined global variable [err:XQST0049]."));
            XQThrow3(StaticErrorException, X("XQQuery::ImportModule"), buf.getRawBuffer(), *varIt);
          }
        }
      }
    }

    moduleCtx->setXMLEntityResolver(NULL);
    moduleCtx->setModuleResolver(NULL);
    m_importedModules.push_back(pParsedQuery);

    bFound=true;
  }
  if(!bFound)
  {
    XMLBuffer buf(1023,context->getMemoryManager());
    buf.set(X("Cannot locate the module for namespace \""));
    buf.append(szUri);
    buf.append(X("\" [err:XQST0059]"));
    XQThrow2(StaticErrorException,X("XQQuery::ImportModule"), buf.getRawBuffer());
  }
}

const XMLCh* XQQuery::getFile() const
{
	return m_szCurrentFile;
}

void XQQuery::setFile(const XMLCh* file)
{
	m_szCurrentFile=m_context->getMemoryManager()->getPooledString(file);
}

const XMLCh* XQQuery::getQueryText() const
{
    return m_szQueryText;
}

XQQuery::QueryResult::QueryResult(const XQQuery *query)
  : ResultImpl(query->getQueryBody()),
    _query(query),
    _parent(0),
    _toDo(true)
{
}

Item::Ptr XQQuery::QueryResult::next(DynamicContext *context)
{
  if(_toDo) {
    _toDo = false;

    // Execute the imported modules
    for(ImportedModules::const_iterator modIt = _query->m_importedModules.begin();
        modIt != _query->m_importedModules.end(); ++modIt) {

      // Derive the module's execution context from it's static context
      AutoDelete<DynamicContext> moduleCtx((*modIt)->createDynamicContext(context->getMemoryManager()));
      moduleCtx->setMemoryManager(context->getMemoryManager());

      (*modIt)->execute(moduleCtx)->next(moduleCtx);

      // Copy the module's imported variables into our context
      for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
          varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
        std::pair<bool, Sequence> value = moduleCtx->getVariableStore()->
          getGlobalVar((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName(), moduleCtx);
        assert(value.first);
        Sequence newSeq(value.second.getLength());
        for(Sequence::iterator it=value.second.begin(); it!=value.second.end(); it++)
          if((*it)->isNode())
            newSeq.addItem(context->getItemFactory()->cloneNode((const Node::Ptr)(*it), context));
          else
            newSeq.addItem(*it);
        context->getVariableStore()->
          setGlobalVar((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName(), newSeq, context);
      }
    }

    // define global variables
    for(GlobalVariables::const_iterator it3 = _query->m_userDefVars.begin(); it3 != _query->m_userDefVars.end(); ++it3)
      (*it3)->execute(context);

    // execute the query body
    if(_query->getQueryBody() != NULL) {
      _parent = _query->getQueryBody()->collapseTree(context);
    }
  }

  Item::Ptr item = _parent->next(context);

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
  oss << _parent->asString(context, indent + 1);
  oss << in << "</queryresult>" << std::endl;

  return oss.str();
}

XQQuery::DebugResult::DebugResult(const XQQuery *query, DynamicContext *context)
  : LazySequenceResult(query->getQueryBody(), context),
    _query(query)
{
}

void XQQuery::DebugResult::getResult(Sequence &toFill, DynamicContext *context) const
{
  static XMLCh szMain[]= { chLatin_M, chLatin_a, chLatin_i, chLatin_n, chNull };

  if(context->getDebugCallback()) {
    context->getDebugCallback()->NotifyQueryBegin(context, _query->getQueryText());
    context->getDebugCallback()->EnterFunction(context, _query->getFile(), szMain, 0, 0);
  }

  try
  {
    // Execute the imported modules
    for(ImportedModules::const_iterator modIt = _query->m_importedModules.begin();
        modIt != _query->m_importedModules.end(); ++modIt) {

      // Derive the module's execution context from it's static context
      AutoDelete<DynamicContext> moduleCtx((*modIt)->createDynamicContext(context->getMemoryManager()));
      moduleCtx->setMemoryManager(context->getMemoryManager());
      // propagate debug settings
      moduleCtx->setDebugCallback(context->getDebugCallback());

      (*modIt)->execute(moduleCtx)->next(moduleCtx);

      // Copy the module's imported variables into our context
      for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
          varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
        std::pair<bool, Sequence> value = moduleCtx->getVariableStore()->
          getGlobalVar((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName(), moduleCtx);
        assert(value.first);
        Sequence newSeq(value.second.getLength());
        for(Sequence::iterator it=value.second.begin(); it!=value.second.end(); it++)
          if((*it)->isNode())
            newSeq.addItem(context->getItemFactory()->cloneNode((const Node::Ptr)(*it), context));
          else
            newSeq.addItem(*it);
        context->getVariableStore()->
          setGlobalVar((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName(), newSeq, context);
      }
    }

    // define global variables
    for(GlobalVariables::const_iterator it3 = _query->m_userDefVars.begin(); it3 != _query->m_userDefVars.end(); ++it3)
      (*it3)->execute(context);

    // execute the query body
    if(_query->getQueryBody() != NULL) {
      toFill = _query->getQueryBody()->collapseTree(context)->toSequence(context);
    }
  }
  catch(XQException& e)
  {
    if(!e.isErrorReported()) {
      if(context->getDebugCallback() && context->isDebuggingEnabled()) 
        context->getDebugCallback()->ReportFirstError(context, e.getError(), _query->getFile(), 0);
      e.setErrorReported();
      if(e.getXQueryLine() == 0) {
        e.setXQueryPosition(_query->getFile(), 1, 1);
      }
    }
    throw e;
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
