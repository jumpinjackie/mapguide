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
 * $Id: XQFLWOR.hpp,v 1.13 2006/11/01 16:37:11 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQFLWOR.hpp: interface for the XQFLWOR class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQFLWOR_H__6E34F36F_797C_4A12_AF0B_5538F1D650B1__INCLUDED_)
#define AFXQ_XQFLWOR_H__6E34F36F_797C_4A12_AF0B_5538F1D650B1__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <vector>
#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/ResultBuffer.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/ast/XQVariableBinding.hpp>

class Collation;

class XQILLA_API SortableItem
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
  const LocationInfo *m_info;
};

class XQILLA_API XQSort
{
public:
	class XQILLA_API SortSpec : public LocationInfo
	{
	public:
		static const long ascending;
		static const long descending;
		static const long empty_greatest;
		static const long empty_least;

		typedef long sortModifier;

		SortSpec(ASTNode* expr, sortModifier modifier, const XMLCh* collation);

		void staticResolution(StaticContext *context, StaticResolutionContext &src);
		void staticTyping(StaticContext *context, StaticResolutionContext &src);
		SortableItem buildKey(DynamicContext* context);

    const ASTNode *getExpression() const;
    sortModifier getModifier() const;
    const XMLCh *getCollation() const;
    const Collation *getCollationObject() const;

    void setExpression(ASTNode *expr);

	protected:
		ASTNode* _expr;
		sortModifier _modifier;
		const XMLCh* _collationURI;
    Collation *_collation;
	};

	typedef std::vector<SortSpec*, XQillaAllocator<SortSpec*> > VectorOfSortSpec;
  typedef std::vector<SortableItem> SortItems;
  typedef enum { stable, unstable } sortType;

	XQSort(sortType type, VectorOfSortSpec* specList);
	void staticResolution(StaticContext *context, StaticResolutionContext &src);
	void staticTyping(StaticContext *context, StaticResolutionContext &src);

	SortItems buildKeys(DynamicContext *context) const;

  sortType getSortType() const;
  const VectorOfSortSpec *getSortSpecs() const;

protected:
	sortType _type;
	VectorOfSortSpec* _specList;
};


class XQILLA_API XQFLWOR : public ASTNodeImpl
{
public:
  XQFLWOR(VectorOfVariableBinding* bindings, ASTNode* where, XQSort* orderBy, ASTNode* returnExpr, XPath2MemoryManager* expr);

  virtual Result createResult(DynamicContext *context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const VectorOfVariableBinding *getBindings() const;
  const ASTNode *getWhereExpr() const;
  const ASTNode *getReturnExpr() const;
  const XQSort *getSort() const;

  void setWhereExpr(ASTNode *where);
  void setReturnExpr(ASTNode *ret);

  typedef std::pair<Sequence, XQSort::SortItems> ResultPair;

  class ProductFactor {
  public:
    ProductFactor(const XQVariableBinding *vb);

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
  void staticTypingImpl(StaticContext* context);
  ASTNode *calculateSRCForWhere(ASTNode *where, StaticContext *context);

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
                const XQFLWOR *flwor, int flags);
    virtual ~FLWORResult();

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
	ASTNode* _return, * _where;
	XQSort* _sort;
};

#endif // !defined(AFXQ_XQFLWOR_H__6E34F36F_797C_4A12_AF0B_5538F1D650B1__INCLUDED_)
