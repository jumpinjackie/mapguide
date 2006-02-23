//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQGlobalVariable.hpp,v 1.10 2005/07/14 15:21:09 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQGlobalVariable.hpp: interface for the XQGlobalVariable class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQGLOBALVARIABLE_H__A97FA5AD_5355_45CD_9092_ED94F5CD035B__INCLUDED_)
#define AFX_XQGLOBALVARIABLE_H__A97FA5AD_5355_45CD_9092_ED94F5CD035B__INCLUDED_

#include <pathan/dataItem/DataItemImpl.hpp>

class SequenceType;

class XQENGINE_API XQGlobalVariable : public DataItemImpl
{
public:
	XQGlobalVariable(const XMLCh* varQName, SequenceType* seqType, DataItem* value, XPath2MemoryManager* expr);

	virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
	virtual DataItem* staticResolution(StaticContext *context);

	const XMLCh* getVariableName() const;
	bool isExternal() const;
	void setVariableExpr(DataItem* value);

  const XMLCh *getVariableURI() const;
  const XMLCh *getVariableLocalName() const;
  const SequenceType *getSequenceType() const;
  const DataItem *getVariableExpr() const;

protected:
	const XMLCh* m_szQName;
	const XMLCh* m_szURI;
	const XMLCh* m_szLocalName;
	SequenceType* m_Type;
	DataItem* m_Value;
};

#endif // !defined(AFX_XQGLOBALVARIABLE_H__A97FA5AD_5355_45CD_9092_ED94F5CD035B__INCLUDED_)
