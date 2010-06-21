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
 * $Id: XQUserFunction.cpp,v 1.21.2.2 2007/01/23 11:58:21 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQUserFunction.cpp: implementation of the XQUserFunction class.
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/functions/XQUserFunction.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/ast/XQTreatAs.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/validators/datatype/DatatypeValidator.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

 /* http://www.w3.org/2005/xquery-local-functions */
const XMLCh XQUserFunction::XMLChXQueryLocalFunctionsURI[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,       XERCES_CPP_NAMESPACE_QUALIFIER chColon,         XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,       XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3,       XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g,       XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2,       XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,       XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_5,       XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y,       XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,       XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
    XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

XQUserFunction::XQUserFunction(const XMLCh* fnName, VectorOfFunctionParameters* params, ASTNode* body, SequenceType* returnValue, StaticContext* ctx)
	: m_body(body),
    m_szPrefix(NULL),
    m_szName(NULL),
    m_szSignature(NULL),
    m_szFullName(fnName),
    m_szURI(NULL),
    m_pMemMgr(ctx->getMemoryManager()),
    _src(ctx->getMemoryManager()),
    m_bCalculatingSRC(false),
    m_qname(1023, m_pMemMgr),
    m_moduleDocCache(NULL)
{
  int nColon=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf(fnName,':');
  if(nColon==-1)
  {
      m_szURI=ctx->getDefaultFuncNS();
      m_szName=m_pMemMgr->getPooledString(fnName);
  }
  else
  {
      XMLCh* tempPrefix = new XMLCh[nColon + 1];
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::subString(tempPrefix, fnName, 0, nColon);
      tempPrefix[nColon] = 0;
      m_szPrefix = m_pMemMgr->getPooledString(tempPrefix);
      delete [] tempPrefix;
      m_szURI=ctx->getUriBoundToPrefix(m_szPrefix, this);
      m_szName=m_pMemMgr->getPooledString(fnName+nColon+1);
  }
  if(XPath2Utils::equals(m_szURI, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLURIName) ||
     XPath2Utils::equals(m_szURI, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA) ||
     XPath2Utils::equals(m_szURI, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_XSI) ||
     XPath2Utils::equals(m_szURI, XQFunction::XMLChFunctionURI) ||
     XPath2Utils::equals(m_szURI, FunctionConstructor::XMLChXPath2DatatypesURI))
  {
      XQThrow(FunctionException,X("User-defined Function"), X("A user defined function must not be in the namespaces xml, xsd, xsi, fn or xdt [err:XQST0045]"));
  }
  else if(m_szURI==NULL || *m_szURI==0)
      XQThrow(FunctionException,X("User-defined Function"), X("A user defined function must have a non-null namespaces [err:XQST0060]"));

  m_pReturnPattern=returnValue;
  m_pParams=params;
  m_qname.set(m_szName);
  m_qname.append(m_szURI);
}

const XMLCh* XQUserFunction::getFullName() const
{
  return m_szFullName;
}

const XMLCh* XQUserFunction::getName() const
{
  return m_szName;
}

const XMLCh *XQUserFunction::getURI() const
{
  return m_szURI;
}
const XMLCh *XQUserFunction::getQName() const
{
  return m_qname.getRawBuffer();
}

size_t XQUserFunction::getMinArgs() const
{
  return m_pParams==NULL?0:m_pParams->size();
}

size_t XQUserFunction::getMaxArgs() const
{
  return m_pParams==NULL?0:m_pParams->size();
}

const XQUserFunction::VectorOfFunctionParameters* XQUserFunction::getParams() const
{
  return m_pParams;
}

const SequenceType* XQUserFunction::getReturnValue() const
{
  return m_pReturnPattern;
}

void XQUserFunction::setURI(const XMLCh* uri)
{
  m_szURI=uri;
}

const XMLCh *XQUserFunction::getPrefix() const
{
  return m_szPrefix;
}

bool XQUserFunction::isExternal() const
{
  return m_body==NULL;
}

void XQUserFunction::setFunctionBody(ASTNode* value)
{
  m_body=value;
}

void XQUserFunction::setModuleDocumentCache(DocumentCache* docCache)
{
  m_moduleDocCache=docCache;
}

const ASTNode *XQUserFunction::getFunctionBody() const
{
  return m_body;
}

DocumentCache* XQUserFunction::getModuleDocumentCache() const
{
  return m_moduleDocCache;
}

ASTNode* XQUserFunction::createInstance(const VectorOfASTNodes &args, XPath2MemoryManager* expr) const
{
  XQFunctionEvaluator* fnInstance=new (expr) XQFunctionEvaluator(this, args, expr);
  fnInstance->setSignature(m_szSignature);
  return fnInstance;
}

void XQUserFunction::staticResolutionStage1(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  // Resolve the parameter names
  if(m_pParams) {
    VectorOfFunctionParameters::iterator it;
    for (it = m_pParams->begin(); it != m_pParams->end (); ++it) {
      (*it)->_uri = context->getUriBoundToPrefix(XPath2NSUtils::getPrefix((*it)->_qname, mm), this);
      (*it)->_name = XPath2NSUtils::getLocalName((*it)->_qname);
    }
    // check for duplicate parameters
    for (it = m_pParams->begin(); it != m_pParams->end ()-1; ++it) {
      for (VectorOfFunctionParameters::iterator it2 = it+1; it2 != m_pParams->end (); ++it2) {
        if(XPath2Utils::equals((*it)->_uri,(*it2)->_uri) && 
           XPath2Utils::equals((*it)->_name,(*it2)->_name)) {
          XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf;
          buf.set(X("User-defined function '"));
          buf.append(m_szName);
          buf.append(X("' has two parameters named '"));
          if((*it)->_uri && *((*it)->_uri)) {
            buf.append(X("{"));
            buf.append((*it)->_uri);
            buf.append(X("}:"));
          }
          buf.append((*it)->_name);
          buf.append(X("' [err:XQST0039]"));
          XQThrow(StaticErrorException, X("XQUserFunction::staticResolution"), buf.getRawBuffer());
        }
      }
    }
  }

  // Set up a default StaticType and StaticResolutionContext
  if(m_pReturnPattern != NULL) {
    if(m_body != NULL) {
      m_body = m_pReturnPattern->convertFunctionArg(m_body, context, /*numericfunction*/false, m_pReturnPattern);
    }

    const SequenceType::ItemType *itemType = m_pReturnPattern->getItemType();
    if(itemType != 0) {
      bool isPrimitive;
      itemType->getStaticType(_src.getStaticType(), context, isPrimitive, m_pReturnPattern);
    }
    else {
      _src.getStaticType().flags = 0;
    }
  }
  else {
    // Default type is item()*
    _src.getStaticType().flags = StaticType::ITEM_TYPE;
  }
  _src.forceNoFolding(true);
}

void XQUserFunction::staticResolutionStage2(StaticContext *context)
{
  if(m_body != NULL) {
    m_body = m_body->staticResolution(context);
  }
}

void XQUserFunction::staticTyping(StaticContext *context)
{
  // Avoid inifinite recursion for recursive functions
  if(m_bCalculatingSRC) return;
  m_bCalculatingSRC = true;

  XPath2MemoryManager *mm = context->getMemoryManager();

  // define the new variables in a new scope and assign them the proper values
  VariableTypeStore* varStore=context->getVariableTypeStore();
  varStore->addLocalScope();

  // Declare the parameters
  if(m_pParams) {
    VectorOfFunctionParameters::iterator it;
    for(it = m_pParams->begin(); it != m_pParams->end (); ++it) {
      bool isPrimitive;
      (*it)->m_pType->getItemType()->getStaticType((*it)->_src.getStaticType(), context, isPrimitive, (*it)->m_pType);
      varStore->declareVar((*it)->_uri, (*it)->_name, (*it)->_src);
    }
  }

  StaticResolutionContext m_src(mm);
  if(m_body != NULL) {
    m_body = m_body->staticTyping(context);
    m_src.copy(m_body->getStaticResolutionContext());
  }

  // Remove the parameter variables from the stored StaticResolutionContext
  if(m_pParams) {
    for(VectorOfFunctionParameters::iterator it = m_pParams->begin(); it != m_pParams->end (); ++it) {
      if(!m_src.removeVariable((*it)->_uri, (*it)->_name)) {
        // The parameter isn't used, so set it to null, so that we don't bother to evaluate it
        (*it)->_qname = 0;
      }
    }
  }

  // Swap m_src with our StaticResolutionContext
  _src.clear();
  _src.copy(m_src);

  varStore->removeScope();

  m_bCalculatingSRC = false;
}

void XQUserFunction::setSignature(const XMLCh* signature)
{
  m_szSignature=m_pMemMgr->getPooledString(signature);
}

const XMLCh* XQUserFunction::getSignature()
{
  return m_szSignature;
}

///////////////////////////////////////////////////////////////
// XQUserFunction::XQFunctionEvaluator

typedef std::pair<XQUserFunction::XQFunctionParameter*, Sequence> ParamBinding;
typedef std::vector<ParamBinding> VectorOfParamBindings;

XQUserFunction::XQFunctionEvaluator::XQFunctionEvaluator(const XQUserFunction* funcDef, const VectorOfASTNodes& args, XPath2MemoryManager* expr) 
  : XQFunction(funcDef->getName(),0,UNLIMITED,"",args,expr)
{
  setType(ASTNode::USER_FUNCTION);
  m_pFuncDef = funcDef;
  _fURI = funcDef->getURI();
}

Result XQUserFunction::XQFunctionEvaluator::createResult(DynamicContext* context, int flags) const
{
  return new FunctionEvaluatorResult(this);
}

ASTNode* XQUserFunction::XQFunctionEvaluator::staticResolution(StaticContext* context)
{
  size_t nDefinedArgs = m_pFuncDef->m_pParams ? m_pFuncDef->m_pParams->size() : 0;
  if(_args.size() != nDefinedArgs) {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf;
    buf.set(X("Function '"));
    buf.append(m_pFuncDef->m_szName);
    buf.append(X("' has been called with a number of arguments that doesn't match the function signature"));
    XQThrow(FunctionException,X("User-defined Function"), buf.getRawBuffer());
  }

  if(nDefinedArgs > 0) {
    VectorOfASTNodes::iterator argIt = _args.begin();
    for(VectorOfFunctionParameters::iterator defIt = m_pFuncDef->m_pParams->begin();
        defIt != m_pFuncDef->m_pParams->end() && argIt != _args.end(); ++defIt, ++argIt) {
      // The doesn't allow us to skip static errors, so we have to check even if
      // the parameter isn't used
      *argIt = (*defIt)->m_pType->convertFunctionArg(*argIt, context, /*numericfunction*/false, *argIt);
      *argIt = (*argIt)->staticResolution(context);
    }
  }

  return this;
}

ASTNode* XQUserFunction::XQFunctionEvaluator::staticTyping(StaticContext* context)
{
  _src.clear();

  if(m_pFuncDef->m_body) {
    // See if we can work out a better return type for the user defined function.
    // This call will just return if staticTyping is in our call stack.
    const_cast<XQUserFunction*>(m_pFuncDef)->staticTyping(context);
    _src.copy(m_pFuncDef->_src);
  }

  size_t nDefinedArgs = m_pFuncDef->m_pParams ? m_pFuncDef->m_pParams->size() : 0;
  if(nDefinedArgs > 0) {
    VectorOfASTNodes::iterator argIt = _args.begin();
    for(VectorOfFunctionParameters::iterator defIt = m_pFuncDef->m_pParams->begin();
        defIt != m_pFuncDef->m_pParams->end() && argIt != _args.end(); ++defIt, ++argIt) {
      // The doesn't allow us to skip static errors, so we have to check even if
      // the parameter isn't used

      // TBD Check all static errors in staticResolution, so we can skip static typing - jpcs
      // if((*defIt)->_qname || context->isDebuggingEnabled()) {
      *argIt = (*argIt)->staticTyping(context);
      _src.add((*argIt)->getStaticResolutionContext());
    }
  }

  // don't constant fold if it's an imported or an external function
  if(m_pFuncDef->m_moduleDocCache==NULL && m_pFuncDef->m_body!=0 && !_src.isUsed()) {
    return constantFold(context);
  }
  return this;
}

XQUserFunction::XQFunctionEvaluator::FunctionEvaluatorResult::FunctionEvaluatorResult(const XQFunctionEvaluator *di)
  : ResultImpl(di),
    _di(di),
    _toDo(true),
    _scope(0),
    _result(0),
    _scopeRemoved(false)
{
}

Item::Ptr XQUserFunction::XQFunctionEvaluator::FunctionEvaluatorResult::next(DynamicContext *context)
{
  context->testInterrupt();	
  VariableStore* varStore=context->getVariableStore();
  Scope<Sequence> *oldScope = varStore->getScopeState();

  AutoDocumentCacheReset reset(context);

  DocumentCache* docCache=_di->getFunctionDefinition()->getModuleDocumentCache();
  if(docCache!=NULL)
  {
    context->setDocumentCache(docCache);
  }
  if(_toDo) {
    _toDo = false;

    size_t nDefinedArgs = _di->getFunctionDefinition()->getParams() ? _di->getFunctionDefinition()->getParams()->size() : 0;

    VectorOfParamBindings varValues;
    if(nDefinedArgs > 0) {
      // the variables should be evaluated in the calling context
      // (before the VariableStore::addLocalScope call: after this call, the variables that can be seen are only the local ones)
      int index = 0;
      for(VectorOfFunctionParameters::const_iterator it = _di->getFunctionDefinition()->getParams()->begin();
          it != _di->getFunctionDefinition()->getParams()->end(); ++it, ++index) {
        if((*it)->_qname || context->isDebuggingEnabled()) {
          Sequence argValue(_di->getArguments()[index]->collapseTree(context)->toSequence(context));
          varValues.push_back(ParamBinding(*it, argValue));
        }
        else {
          // Skip evaluation of the parameter, since it isn't used, and debugging isn't enabled
        }
      }
    }
    // define the new variables in a new scope and assign them the proper values
    varStore->addLocalScope();
    for(VectorOfParamBindings::iterator it2 = varValues.begin(); it2 != varValues.end(); ++it2) {
      varStore->declareVar(it2->first->_uri, it2->first->_name, it2->second, context);
    }

    if(_di->getFunctionDefinition()->getFunctionBody() == NULL) {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf;
      buf.set(X("External function '"));
      buf.append(_di->getFunctionDefinition()->getName());
      buf.append(X("' has not been bound to an implementation"));
      XQThrow(FunctionException,X("User-defined Function"), buf.getRawBuffer());
    }

    _result = _di->getFunctionDefinition()->getFunctionBody()->collapseTree(context);
  }
  else if(_scope != 0) {
    varStore->setScopeState(_scope);
  }

  Item::Ptr item = _result->next(context);

  if(!_scopeRemoved) {
    if(item == NULLRCP) {
      varStore->removeScope();
      _scope = 0;
    }
    else {
      _scope = varStore->getScopeState();
      varStore->setScopeState(oldScope);
    }
  }

  // if we had to switch document cache, check that the returned types are known also in the original context; if not, upgrade them to the base type 
  if(docCache!=NULL)
  {
    if(item!=NULLRCP && !reset.oldDC->isTypeDefined(item->getTypeURI(), item->getTypeName()))
    {
      if(item->isNode())
      {
        Node::Ptr node=item;
        // TODO: change the annotation in the DOM elements and attributes
      }
      else
      {
        AnyAtomicType::Ptr atom=item;
        const XMLCh* uri=atom->getTypeURI(), *name=atom->getTypeName();
        while(!reset.oldDC->isTypeDefined(uri, name))
        {
            XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator* pDV=docCache->getDatatypeValidator(uri, name);
            assert(pDV!=NULL);
            XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator* pBaseDV=pDV->getBaseValidator();
            if(pBaseDV==NULL)
                break;
            uri=pBaseDV->getTypeUri();
            name=pBaseDV->getTypeLocalName();
        }
        item=context->getItemFactory()->createDerivedFromAtomicType(uri, name, atom->asString(context), context);
      }
    }
  }
  return item;
}

std::string XQUserFunction::XQFunctionEvaluator::FunctionEvaluatorResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<functionevaluator/>" << std::endl;

  return oss.str();
}
