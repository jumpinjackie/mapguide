//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: PrintDataItemTree.hpp,v 1.3 2005/04/05 16:44:53 bostic Exp $
//

#ifndef AFX_XQUERYPRINTDATAITEMTREE_HPP__FAA9543A_2F10_49A4_93D2_857E2I8C48E3__INCLUDED_
#define	AFX_XQUERYPRINTDATAITEMTREE_HPP__FAA9543A_2F10_49A4_93D2_857E2I8C48E3__INCLUDED_

#include <pathan/internal/utils/PrintDataItemTree.hpp>

#include <xquery/dataItem/XQTypeswitch.hpp>
#include <xquery/functions/XQFunction.hpp>

class XQQuery;
class XQDebugHook;
class XQDOMConstructor;
class XQFLWOR;
class XQVariableBinding;
class XQSort;
class XQQuantified;
class XQFunctionCall;
class XQGlobalVariable;
class XQValidate;
class XQOrderingChange;

class XQENGINE_API XQueryPrintDataItemTree : public PrintDataItemTree
{
public:
	static std::string print(const PathanExpression *expr, const DynamicContext *context, int indent = 0);
	static std::string print(const XQQuery *query, const DynamicContext *context, int indent = 0);
	static std::string print(const DataItem *item, const DynamicContext *context, int indent = 0);

	virtual std::string printDataItem(const DataItem *item, const DynamicContext *context, int indent);
	virtual std::string printDebugHook(const XQDebugHook *item, const DynamicContext *context, int indent);
	virtual std::string printFLWOR(const XQFLWOR *item, const DynamicContext *context, int indent);
	virtual std::string printFLWORQuantified(const XQQuantified *item, const DynamicContext *context, int indent);
	virtual std::string printTypeswitch(const XQTypeswitch *item, const DynamicContext *context, int indent);
	virtual std::string printValidate(const XQValidate *item, const DynamicContext *context, int indent);
	virtual std::string printGlobal(const XQGlobalVariable *item, const DynamicContext *context, int indent);
	virtual std::string printFunctionCall(const XQFunctionCall *item, const DynamicContext *context, int indent);
	virtual std::string printDOMConstructor(const XQDOMConstructor *item, const DynamicContext *context, int indent);
	virtual std::string printOrderingChange(const XQOrderingChange *item, const DynamicContext *context, int indent);
	virtual std::string printUserFunction(const XQFunction::XQFunctionEvaluator *item, const DynamicContext *context, int indent);

	virtual std::string printXQVariableBinding(const XQVariableBinding *binding, const DynamicContext *context, int indent);
	virtual std::string printSort(const XQSort *sort, const DynamicContext *context, int indent);
	virtual std::string printClause(const XQTypeswitch::Clause *clause, const DynamicContext *context, int indent);
};

#endif
