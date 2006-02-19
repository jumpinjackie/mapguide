//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQQuery.hpp,v 1.12 2005/07/14 15:21:08 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQQuery.h: interface for the XQQuery class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQQUERY_H__FAA9933A_2F10_49A4_93A2_857E2A8C48E3__INCLUDED_)
#define AFX_XQQUERY_H__FAA9933A_2F10_49A4_93A2_857E2A8C48E3__INCLUDED_

#include "XQEngine.hpp"
#include <vector>
#include <xercesc/util/XMemory.hpp>
#include <pathan/dataItem/Result.hpp>
#include <pathan/dataItem/ResultImpl.hpp>
#include <pathan/dataItem/LazySequenceResult.hpp>
#include <pathan/StaticContext.hpp>

class XQContext;
class XQFunction;
class XQGlobalVariable;
class DataItem;
class StaticResolutionContext;

class XQENGINE_API XQQuery : public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
{
public:
	XQQuery(const XMLCh* queryText, XPath2MemoryManager* memMgr);

  Result evaluate(XQContext* context) const;
  void staticResolution(XQContext *context);

	// Module stuff
	void setIsLibraryModule(bool bIsModule=true);
	bool getIsLibraryModule() const;
	void setModuleTargetNamespace(const XMLCh* uri);
	const XMLCh* getModuleTargetNamespace() const;
	void importModule(const XMLCh* szUri, VectorOfStrings* locations, StaticContext* context);

	DataItem* getQueryBody() const;
	void setQueryBody(DataItem* query);

	void addFunction(XQFunction* fnDef);
	void addVariable(XQGlobalVariable* varDef);

    const XMLCh* getFile() const;
    void setFile(const XMLCh* file);

    const XMLCh* getQueryText() const;

    std::vector<XQFunction*, PathanAllocator<XQFunction*> > m_userDefFns;
	std::vector<XQGlobalVariable*, PathanAllocator<XQGlobalVariable*> > m_userDefVars;

protected:
  class QueryResult : public ResultImpl
  {
  public:
    QueryResult(const XQQuery *query, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const XQQuery *_query;
    Result _parent;
    bool _toDo;
  };

  class DebugResult : public LazySequenceResult
  {
  public:
    DebugResult(const XQQuery *query, DynamicContext *context);

    void getResult(Sequence &toFill, DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const XQQuery *_query;
  };

	DataItem* m_query;
	bool m_bIsLibraryModule;
	const XMLCh* m_szTargetNamespace;
    const XMLCh* m_szQueryText;
    const XMLCh* m_szCurrentFile;
    XPath2MemoryManager* m_memMgr;
};

#endif // !defined(AFX_XQQUERY_H__FAA9933A_2F10_49A4_93A2_857E2A8C48E3__INCLUDED_)
