//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQTypeswitch.hpp,v 1.12 2005/08/04 15:48:05 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQTypeswitch.hpp: interface for the XQTypeswitch class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQTYPESWITCH_H__90ED3AA0_5C63_437A_9B94_0CCDEF499124__INCLUDED_)
#define AFX_XQTYPESWITCH_H__90ED3AA0_5C63_437A_9B94_0CCDEF499124__INCLUDED_

#include "../XQEngine.hpp"
#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/SequenceType.hpp>
#include <vector>

class ResultBuffer;
template<class TYPE> class Scope;

class XQENGINE_API XQTypeswitch : public DataItemImpl
{
public:
  class Clause
  {
  public:
    Clause(SequenceType* type, DataItem* expr, const XMLCh* variable)
      : _expr(expr), _type(type), _variable(variable), _uri(0), _name(0) {}

    void staticResolution(const StaticResolutionContext &var_src, StaticContext* context, StaticResolutionContext &src);

    DataItem* _expr;
    SequenceType* _type;
    const XMLCh* _variable;
    const XMLCh* _uri;
    const XMLCh* _name;
  };

  typedef std::vector<Clause*, PathanAllocator<Clause*> > VectorOfClause;

  XQTypeswitch(DataItem* eval, VectorOfClause* clauses, Clause* defReturn, XPath2MemoryManager* expr);

  Result createResult(DynamicContext* context, int flags=0) const;
  DataItem* staticResolution(StaticContext *context);

  const DataItem *getExpression() const;
  const Clause *getDefaultClause() const;
  const VectorOfClause *getClauses() const;

  void setExpression(DataItem *expr);

protected:
  class TypeswitchResult : public ResultImpl
  {
  public:
    TypeswitchResult(const XQTypeswitch *di, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    int _flags;
    const XQTypeswitch *_di;

    Scope<Sequence> *_scope;
    Result _result;
    bool _scopeRemoved;
  };

  DataItem* _expr;
  Clause* _default;
  VectorOfClause* _clauses;
};

#endif // !defined(AFX_XQTYPESWITCH_H__90ED3AA0_5C63_437A_9B94_0CCDEF499124__INCLUDED_)
