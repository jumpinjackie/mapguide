//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQVariableBinding.cpp,v 1.11 2005/08/04 15:48:10 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQVariableBinding.cpp: implementation of the XQVariableBinding class.
//////////////////////////////////////////////////////////////////////

#include "xquery/XQEngine.hpp"
#include "xquery/dataItem/XQVariableBinding.hpp"
#include <pathan/operators/And.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/XPath2Utils.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQVariableBinding::XQVariableBinding(XPath2MemoryManager* memMgr, 
                                     BindingType bindingType, 
                                     const XMLCh* variable, 
                                     DataItem* allValues, 
                                     const XMLCh* positionalVariable/*=NULL*/,
                                     SequenceType* seqType/*=NULL*/)
  : _bindingType(bindingType),
    _variable(memMgr->getPooledString(variable)),
    _vURI(0),
    _vName(0),
    _positionalVariable(positionalVariable ? memMgr->getPooledString(positionalVariable) : 0),
    _pURI(0),
    _pName(0),
    _src(memMgr),
    _pSrc(memMgr),
    _seqType(seqType),
    _allValues(allValues),
    _valuesResultMustBeRecalculated(false),
    _needsNewScope(false),
    _where(0),
    _line(0),
    _file(0),
    _memMgr(memMgr)
{
}

XQVariableBinding::XQVariableBinding(XPath2MemoryManager *memMgr, const XQVariableBinding &o)
  : _bindingType(o._bindingType),
    _variable(o._variable),
    _vURI(o._vURI),
    _vName(o._vName),
    _positionalVariable(o._positionalVariable),
    _pURI(o._pURI),
    _pName(o._pName),
    _src(o._src, memMgr),
    _pSrc(o._pSrc, memMgr),
    _seqType(o._seqType),
    _allValues(o._allValues),
    _valuesResultMustBeRecalculated(o._valuesResultMustBeRecalculated),
    _needsNewScope(o._needsNewScope),
    _where(o._where),
    _line(o._line),
    _file(o._file),
    _memMgr(o._memMgr)
{
}

void XQVariableBinding::addWhereCondition(DataItem *condition)
{
  if(!_where) {
    // Set the where condition
    _where = condition;
  }
  else if(_where->getType()==DataItem::OPERATOR &&
          XPath2Utils::equals(((DataItemOperator *)_where)->getOperatorName(), And::name)) {
    // Add an argument to the existing And
    And* fAnd = (And*)_where;
    fAnd->addArgument(condition);
  }
  else {
    // Change the where expression to an And, and add the two conditions to it as arguments
    VectorOfDataItems args(2, (DataItem*)0 ,PathanAllocator<DataItem*>(_memMgr));
    args[0] = _where;
    args[1] = condition;
    _where = new (_memMgr) And(args, _memMgr);
  }
}
