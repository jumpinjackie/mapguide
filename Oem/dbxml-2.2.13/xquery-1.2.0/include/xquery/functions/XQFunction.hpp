//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQFunction.hpp,v 1.14 2005/08/04 15:48:06 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQFunction.hpp: interface for the XQFunction class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQFUNCTION_H__97943356_0D2D_4930_9D60_6E95AB67586A__INCLUDED_)
#define AFX_XQFUNCTION_H__97943356_0D2D_4930_9D60_6E95AB67586A__INCLUDED_

#include "../XQEngine.hpp"
#include <pathan/internal/factory/FuncFactory.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/simpleVariables/Scope.hpp>

class XQENGINE_API XQFunction : public FuncFactory
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

  typedef std::vector<XQFunctionParameter*,PathanAllocator<XQFunctionParameter*> > VectorOfFunctionParameters;

  class XQFunctionEvaluator : public DataItemFunction
  {
  public:
    XQFunctionEvaluator(const XQFunction* funcDef, const VectorOfDataItems& args, XPath2MemoryManager* expr);

    void setSignature(const XMLCh* signature)
    {
      _signature=signature;
    }

    Result createResult(DynamicContext* context, int flags=0) const;
    DataItem* staticResolution(StaticContext* context);

    const XQFunction *getFunctionDefinition() const
    {
      return m_pFuncDef;
    }

  protected:
    class FunctionEvaluatorResult : public ResultImpl
    {
    public:
      FunctionEvaluatorResult(const XQFunctionEvaluator *di, int flags, DynamicContext *context);

      Item::Ptr next(DynamicContext *context);
      std::string asString(DynamicContext *context, int indent) const;
    private:
      int _flags;
      const XQFunctionEvaluator *_di;

      Scope<Sequence> *_scope;
      Result _result;
      bool _scopeRemoved;
    };

    const XQFunction* m_pFuncDef;
  };

  XQFunction(const XMLCh* fnName, VectorOfFunctionParameters* params, DataItem* body, SequenceType* returnValue, XPath2MemoryManager* memMgr);

  // from FuncFactory
  virtual DataItem *createInstance(const VectorOfDataItems &args, XPath2MemoryManager* expr) const;
  virtual const XMLCh* getName() const;
  virtual const XMLCh *getURI() const;
  virtual unsigned int getMinArgs() const;
  virtual unsigned int getMaxArgs() const;

  virtual const XMLCh* getFullName() const;
  const VectorOfFunctionParameters* getParams() const;
  const SequenceType* getReturnValue() const;

  void staticResolution(StaticContext* context);

  void setSignature(const XMLCh* signature);
  const XMLCh* getSignature();
  bool isExternal() const;
  void setFunctionBody(DataItem* value);

  void setURI(const XMLCh* uri);
  const XMLCh *getPrefix() const;

  const DataItem *getFunctionBody() const;

  static const XMLCh XMLChXQueryLocalFunctionsURI[];

protected:
  DataItem* m_body;
  const XMLCh* m_szPrefix,*m_szName,*m_szSignature,*m_szFullName,*m_szURI;
  SequenceType* m_pReturnPattern;
  VectorOfFunctionParameters* m_pParams;
  XPath2MemoryManager* m_pMemMgr;
  bool m_bStaticallyResolved;

  friend class XQFunctionEvaluator;
};

#endif // !defined(AFX_XQFUNCTION_H__97943356_0D2D_4930_9D60_6E95AB67586A__INCLUDED_)
