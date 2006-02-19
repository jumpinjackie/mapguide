//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQVariableBinding.hpp,v 1.11 2005/08/04 15:48:05 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQVariableBinding.hpp: interface for the XQVariableBinding class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQVARIABLEBINDING_H__4EDA7D8A_78E2_476F_80DA_029C3E4B9AC8__INCLUDED_)
#define AFX_XQVARIABLEBINDING_H__4EDA7D8A_78E2_476F_80DA_029C3E4B9AC8__INCLUDED_

#include "../XQEngine.hpp"
#include <pathan/dataItem/DataItem.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

class XQENGINE_API XQVariableBinding
{
public:
	typedef enum
	{
		letBinding,
		forBinding
	} BindingType;

  XQVariableBinding(XPath2MemoryManager* memMgr, BindingType bindingType, const XMLCh* variable, DataItem* allValues, const XMLCh* positionalVariable=NULL, SequenceType* seqType=NULL);
  XQVariableBinding(XPath2MemoryManager* memMgr, const XQVariableBinding &o);

  void addWhereCondition(DataItem *clause);

	BindingType _bindingType;

	const XMLCh *_variable, *_vURI, *_vName;
  const XMLCh *_positionalVariable, *_pURI, *_pName;

  StaticResolutionContext _src, _pSrc;

  SequenceType* _seqType;

	DataItem* _allValues;

  bool _valuesResultMustBeRecalculated;
  bool _needsNewScope;

  DataItem *_where;

	unsigned long _line;
	const XMLCh* _file;

  XPath2MemoryManager *_memMgr;

private:
  XQVariableBinding(const XQVariableBinding &o);
};

typedef std::vector<XQVariableBinding*, PathanAllocator<XQVariableBinding*> > VectorOfVariableBinding;

#endif // !defined(AFX_XQVARIABLEBINDING_H__4EDA7D8A_78E2_476F_80DA_029C3E4B9AC8__INCLUDED_)
