/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMFOR_HPP
#define _DATAITEMFOR_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/dataItem/ResultBuffer.hpp>
#include <pathan/internal/parser/VariableBinding.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/VariableStore.hpp>
#include <vector>

typedef std::vector<VariableBinding* ,PathanAllocator<VariableBinding*> > VectorOfBindings;

template<class TYPE> class VarHashEntry;
template<class TYPE> class Scope;

/** For function. */
class PATHAN_EXPORT DataItemFor : public DataItemImpl
{
public:
  DataItemFor(VectorOfBindings* bindings, DataItem* returnExpr, XPath2MemoryManager* memMgr);
  
  virtual Result createResult(DynamicContext* context, int flags=0) const;

  virtual DataItem* staticResolution(StaticContext *context);

  class VB {
  public:
    VB(const XMLCh* const variable, DataItem* allValues, XPath2MemoryManager* memMgr);
    VB(const VB &o, XPath2MemoryManager* memMgr);

    const XMLCh *_variable, *_uri, *_name;
    DataItem* _allValues;
    bool _valuesExprDependsOnPreviousBindings;
    bool _needsNewScope;
    StaticResolutionContext _src;

  private: 
    VB(const VB &o);   
  };
  typedef std::vector<VB*, PathanAllocator<VB*> > Bindings;

  const Bindings &getBindings() const;
  const DataItem *getReturnExpr() const;

  void setReturnExpr(DataItem *item);

  class VBExecution {
  public:
    VBExecution(const VB *vb, DynamicContext *context);

    bool initialise(DynamicContext *context);
    bool next(DynamicContext *context);

  private:
    const VB *_vb;
    Result _values;
    ResultBuffer _valuesBuffer;
    VariableStore::Entry *_refVar;
  };
  typedef std::vector<VBExecution> ExecutionBindings;

  static bool nextState(ExecutionBindings &ebs, DynamicContext *context, bool initialisationState);

protected:
  virtual Result createResultImpl(Bindings::const_iterator it, Bindings::const_iterator end, DynamicContext* context, int flags = 0) const;
  void staticResolutionImpl(StaticContext* context);

  class ForResult : public ResultImpl
  {
  public:
    ForResult(Bindings::const_iterator it, Bindings::const_iterator end, const DataItem *returnExpr, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    bool _toInit;
    int _flags;
    ExecutionBindings _ebs;
    VariableStore::MyScope *_scope;

    const DataItem *_returnExpr;
    Result _returnResult;
  };

  Bindings _bindings;
  DataItem* _returnExpr;
};

#endif // _DATAITEMFOR_HPP

