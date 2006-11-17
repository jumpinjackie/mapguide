//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQFLWOR.hpp,v 1.20 2005/07/14 15:21:09 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQFLWOR.hpp: interface for the XQFLWOR class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQFLWOR_H__6E34F36F_797C_4A12_AF0B_5538F1D650B1__INCLUDED_)
#define AFX_XQFLWOR_H__6E34F36F_797C_4A12_AF0B_5538F1D650B1__INCLUDED_

#include "../XQEngine.hpp"
#include <vector>
#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/dataItem/ResultBuffer.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/VariableStore.hpp>
#include "XQVariableBinding.hpp"

class Collation;

class XQENGINE_API SortableItem
{
public:
	SortableItem();
	SortableItem(const SortableItem& other);

	bool operator==(const SortableItem& other) const;
	bool operator>(const SortableItem& other) const;

	int m_bAscending:1, m_bIsMax:1, m_bIsMin:1;
	DynamicContext* m_context;
	Collation* m_collation;
	AnyAtomicType::Ptr m_item;
};

class XQENGINE_API XQSort
{
public:
	class XQENGINE_API SortSpec
	{
	public:
		static const long ascending;
		static const long descending;
		static const long empty_greatest;
		static const long empty_least;

		typedef long sortModifier;

		SortSpec(DataItem* expr, sortModifier modifier, const XMLCh* collation);

		void staticResolution(StaticContext *context, StaticResolutionContext &src);
		SortableItem buildKey(DynamicContext* context);

    const DataItem *getExpression() const;
    sortModifier getModifier() const;
    const XMLCh *getCollation() const;

    void setExpression(DataItem *expr);

	protected:
		DataItem* _expr;
		sortModifier _modifier;
		const XMLCh* _collation;
	};

	typedef std::vector<SortSpec*, PathanAllocator<SortSpec*> > VectorOfSortSpec;
  typedef std::vector<SortableItem> SortItems;
	typedef enum { stable, unstable } sortType;

	XQSort(sortType type, VectorOfSortSpec* specList);
	void staticResolution(StaticContext *context, StaticResolutionContext &src);

	SortItems buildKeys(DynamicContext *context) const;

  sortType getSortType() const;
  const VectorOfSortSpec *getSortSpecs() const;

protected:
	sortType _type;
	VectorOfSortSpec* _specList;
};


class XQENGINE_API XQFLWOR : public DataItemImpl
{
public:
  XQFLWOR(VectorOfVariableBinding* bindings, DataItem* where, XQSort* orderBy, DataItem* returnExpr, XPath2MemoryManager* expr);

  virtual Result createResult(DynamicContext *context, int flags=0) const;
  virtual DataItem* staticResolution(StaticContext *context);

  const VectorOfVariableBinding *getBindings() const;
  const DataItem *getWhereExpr() const;
  const DataItem *getReturnExpr() const;
  const XQSort *getSort() const;

  void setWhereExpr(DataItem *where);
  void setReturnExpr(DataItem *ret);

  typedef std::pair<Sequence, XQSort::SortItems> ResultPair;

  class ProductFactor {
  public:
    ProductFactor(const XQVariableBinding *vb, DynamicContext *context);

    bool initialise(DynamicContext *context);
    bool next(DynamicContext *context);
    bool checkWhere(DynamicContext *context);

  private:
    const XQVariableBinding *_vb;
    Result _values;
    ResultBuffer _valuesBuffer;
    int _index;
    VariableStore::Entry *_refVar, *_posRefVar;
  };
  typedef std::vector<ProductFactor> ExecutionBindings;
  typedef std::vector<ResultPair> PreSortResult;

  bool nextState(ExecutionBindings &ebs, DynamicContext *context, bool initialisationState) const;

  VariableStore::Entry *getAccumulator(DynamicContext *context) const;
  void setAccumulator(VariableStore::Entry *refVar, const PreSortResult &toBeSorted, DynamicContext *context) const;

protected:

  bool checkWhere(DynamicContext *context) const;

  virtual Result createResultImpl(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                                  DynamicContext* context, int flags = 0) const;
  void staticResolutionImpl(StaticContext* context);
  DataItem *staticallyResolveWhere(DataItem *where, StaticContext* context);

  class SortingFLWORResult : public LazySequenceResult
  {
  public:
    SortingFLWORResult(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                       const XQFLWOR *flwor, int flags, DynamicContext *context);

    void getResult(Sequence &toFill, DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const XQFLWOR *_flwor;
    int _flags;
    ExecutionBindings _ebs; // mutable
  };

  class FLWORResult : public ResultImpl
  {
  public:
    FLWORResult(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                const XQFLWOR *flwor, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const XQFLWOR *_flwor;
    int _flags;
    ExecutionBindings _ebs;

    bool _toInit;
    Scope<Sequence> *_scope;
    Result _returnResult;
  };

	VectorOfVariableBinding* _bindings;
	DataItem* _return, * _where;
	XQSort* _sort;
};

#endif // !defined(AFX_XQFLWOR_H__6E34F36F_797C_4A12_AF0B_5538F1D650B1__INCLUDED_)
