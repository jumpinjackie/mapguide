//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQDOMConstructor.hpp,v 1.13 2005/07/15 15:26:19 alby Exp $
//

//////////////////////////////////////////////////////////////////////
// XQDOMConstructor.hpp: interface for the XQDOMConstructor class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQDOMCONSTRUCTOR_H__56E97972_3896_49D3_B055_36CC3E9E550A__INCLUDED_)
#define AFX_XQDOMCONSTRUCTOR_H__56E97972_3896_49D3_B055_36CC3E9E550A__INCLUDED_

#include <vector>

#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/Node.hpp>

#include <xercesc/framework/XMLBuffer.hpp>

class XQENGINE_API XQDOMConstructor : public DataItemImpl
{
public:
	XQDOMConstructor(const XMLCh* nodeType, DataItem* name, VectorOfDataItems* attrList, VectorOfDataItems* children, XPath2MemoryManager* expr);

	virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
	virtual DataItem* staticResolution(StaticContext *context);

  const XMLCh* getNodeType() const;
  const DataItem *getName() const;
  const VectorOfDataItems *getAttributes() const;
  const VectorOfDataItems *getChildren() const;

  void setName(DataItem *name);

protected:
  bool isTextNode(const Node::Ptr &node) const;
    bool getStringValue(XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &value, DynamicContext *context) const;

protected:
	const XMLCh* m_nodeType;
	DataItem* m_name;
	VectorOfDataItems* m_attrList, *m_children;
};

#endif // !defined(AFX_XQDOMCONSTRUCTOR_H__56E97972_3896_49D3_B055_36CC3E9E550A__INCLUDED_)
