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
 * $Id: XQUserFunction.hpp,v 1.12.2.1 2007/01/12 18:04:46 gmfeinberg Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQFunction.hpp: interface for the XQFunction class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQFUNCTION_H__97943356_0D2D_4930_9D60_6E95AB67586A__INCLUDED_)
#define AFXQ_XQFUNCTION_H__97943356_0D2D_4930_9D60_6E95AB67586A__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/functions/FuncFactory.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/schema/DocumentCache.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/context/Scope.hpp>

class XQILLA_API XQUserFunction : public FuncFactory, public LocationInfo
{
public:
  class XQFunctionParameter
  {
  public:
    XQFunctionParameter(const XMLCh* paramName, SequenceType* type, XPath2MemoryManager* memMgr)
      : _src(memMgr)
    {
      _qname = memMgr->getPooledString(paramName);
      m_pType = type;
    }

    const XMLCh *_qname, *_uri, *_name;
    SequenceType* m_pType;
    StaticResolutionContext _src;
  };

  typedef std::vector<XQFunctionParameter*,XQillaAllocator<XQFunctionParameter*> > VectorOfFunctionParameters;

  class XQFunctionEvaluator : public XQFunction
  {
  public:
    XQFunctionEvaluator(const XQUserFunction* funcDef, const VectorOfASTNodes& args, XPath2MemoryManager* expr);

    void setSignature(const XMLCh* signature)
    {
      _signature=signature;
    }

    Result createResult(DynamicContext* context, int flags=0) const;
    ASTNode* staticResolution(StaticContext* context);
    virtual ASTNode *staticTyping(StaticContext *context);

    const XQUserFunction *getFunctionDefinition() const
    {
      return m_pFuncDef;
    }

  protected:
    class FunctionEvaluatorResult : public ResultImpl
    {
    public:
      FunctionEvaluatorResult(const XQFunctionEvaluator *di);

      Item::Ptr next(DynamicContext *context);
      std::string asString(DynamicContext *context, int indent) const;
    private:
      const XQFunctionEvaluator *_di;
      bool _toDo;

      Scope<Sequence> *_scope;
      Result _result;
      bool _scopeRemoved;
    };

    const XQUserFunction* m_pFuncDef;
  };

  XQUserFunction(const XMLCh* fnName, VectorOfFunctionParameters* params, ASTNode* body, SequenceType* returnValue, StaticContext* ctx);

  // from FuncFactory
  virtual ASTNode *createInstance(const VectorOfASTNodes &args, XPath2MemoryManager* expr) const;
  virtual const XMLCh* getName() const;
  virtual const XMLCh *getURI() const;
  virtual const XMLCh *getQName() const;
  virtual size_t getMinArgs() const;
  virtual size_t getMaxArgs() const;

  virtual const XMLCh* getFullName() const;
  const VectorOfFunctionParameters* getParams() const;
  const SequenceType* getReturnValue() const;

  /// Resolve URIs, give the function a default static type
  void staticResolutionStage1(StaticContext* context);
  /// Resolve the function body, work out a more static return type
  void staticResolutionStage2(StaticContext* context);
  void staticTyping(StaticContext *context);

  void setSignature(const XMLCh* signature);
  const XMLCh* getSignature();
  bool isExternal() const;
  void setFunctionBody(ASTNode* value);
  void setModuleDocumentCache(DocumentCache* docCache);

  void setURI(const XMLCh* uri);
  const XMLCh *getPrefix() const;

  const ASTNode *getFunctionBody() const;
  DocumentCache* getModuleDocumentCache() const;

  static const XMLCh XMLChXQueryLocalFunctionsURI[];

protected:
  ASTNode* m_body;
  const XMLCh* m_szPrefix,*m_szName,*m_szSignature,*m_szFullName,*m_szURI;
  SequenceType* m_pReturnPattern;
  VectorOfFunctionParameters* m_pParams;
  XPath2MemoryManager* m_pMemMgr;
  StaticResolutionContext _src;
  bool m_bCalculatingSRC;
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer m_qname;
  DocumentCache* m_moduleDocCache;

  friend class XQFunctionEvaluator;
};

#endif // !defined(AFXQ_XQFUNCTION_H__97943356_0D2D_4930_9D60_6E95AB67586A__INCLUDED_)
