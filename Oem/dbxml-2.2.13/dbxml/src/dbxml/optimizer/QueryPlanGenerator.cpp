//
//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryPlanGenerator.cpp,v 1.43 2005/12/07 15:58:18 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include "QueryPlanGenerator.hpp"
#include "../QueryContext.hpp"
#include "../Value.hpp"
#include "../dataItem/QueryPlanFunction.hpp"
#include "../dataItem/DbXmlDocAvailable.hpp"
#include "../dataItem/MetaDataFunction.hpp"
#include "../dataItem/DbXmlNodeTest.hpp"
#include "../dataItem/DbXmlCompare.hpp"
#include "../dataItem/DbXmlFilter.hpp"
#include "../dataItem/DbXmlContext.hpp"
#include "../dataItem/DbXmlContains.hpp"
#include "../dataItem/DbXmlNav.hpp"
#include "../UTF8.hpp"

#include <xquery/XQContext.hpp>

#include <pathan/operators/And.hpp>
#include <pathan/operators/Or.hpp>
#include <pathan/operators/Equals.hpp>
#include <pathan/operators/NotEquals.hpp>
#include <pathan/operators/Plus.hpp>
#include <pathan/operators/Minus.hpp>
#include <pathan/operators/Multiply.hpp>
#include <pathan/operators/Divide.hpp>
#include <pathan/operators/IntegerDivide.hpp>
#include <pathan/operators/Mod.hpp>
#include <pathan/operators/LessThan.hpp>
#include <pathan/operators/GreaterThan.hpp>
#include <pathan/operators/LessThanEqual.hpp>
#include <pathan/operators/GreaterThanEqual.hpp>
#include <pathan/operators/UnaryMinus.hpp>
#include <pathan/operators/NodeComparison.hpp>
#include <pathan/operators/OrderComparison.hpp>
#include <pathan/operators/GeneralComp.hpp>
#include <pathan/operators/Range.hpp>
#include <pathan/operators/Intersect.hpp>
#include <pathan/operators/Except.hpp>
#include <pathan/operators/Union.hpp>

#include <pathan/functions/FunctionAbs.hpp>
#include <pathan/functions/FunctionAvg.hpp>
#include <pathan/functions/FunctionBaseURI.hpp>
#include <pathan/functions/FunctionBoolean.hpp>
#include <pathan/functions/FunctionCeiling.hpp>
#include <pathan/functions/FunctionCollection.hpp>
#include <pathan/functions/FunctionCompare.hpp>
#include <pathan/functions/FunctionConcat.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/functions/FunctionContains.hpp>
#include <pathan/functions/FunctionCount.hpp>
#include <pathan/functions/FunctionCurrentDate.hpp>
#include <pathan/functions/FunctionCurrentDateTime.hpp>
#include <pathan/functions/FunctionCurrentTime.hpp>
#include <pathan/functions/FunctionData.hpp>
#include <pathan/functions/FunctionDeepEqual.hpp>
#include <pathan/functions/FunctionDefaultCollation.hpp>
#include <pathan/functions/FunctionDistinctValues.hpp>
#include <pathan/functions/FunctionDoc.hpp>
#include <pathan/functions/FunctionDocumentURI.hpp>
#include <pathan/functions/FunctionEmpty.hpp>
#include <pathan/functions/FunctionEndsWith.hpp>
#include <pathan/functions/FunctionError.hpp>
#include <pathan/functions/FunctionEscapeURI.hpp>
#include <pathan/functions/FunctionExactlyOne.hpp>
#include <pathan/functions/FunctionExists.hpp>
#include <pathan/functions/FunctionFalse.hpp>
#include <pathan/functions/FunctionFloor.hpp>
#include <pathan/functions/FunctionInScopePrefixes.hpp>
#include <pathan/functions/FunctionLocalNameFromQName.hpp>
#include <pathan/functions/FunctionNamespaceURIFromQName.hpp>
#include <pathan/functions/FunctionNamespaceURIForPrefix.hpp>
#include <pathan/functions/FunctionNilled.hpp>
#include <pathan/functions/FunctionId.hpp>
#include <pathan/functions/FunctionIdref.hpp>
#include <pathan/functions/FunctionInsertBefore.hpp>
#include <pathan/functions/FunctionImplicitTimezone.hpp>
#include <pathan/functions/FunctionIndexOf.hpp>
#include <pathan/functions/FunctionLang.hpp>
#include <pathan/functions/FunctionLast.hpp>
#include <pathan/functions/FunctionLocalname.hpp>
#include <pathan/functions/FunctionLowerCase.hpp>
#include <pathan/functions/FunctionMatches.hpp>
#include <pathan/functions/FunctionMax.hpp>
#include <pathan/functions/FunctionMin.hpp>
#include <pathan/functions/FunctionName.hpp>
#include <pathan/functions/FunctionNamespaceUri.hpp>
#include <pathan/functions/FunctionNodeName.hpp>
#include <pathan/functions/FunctionNormalizeSpace.hpp>
#include <pathan/functions/FunctionNormalizeUnicode.hpp>
#include <pathan/functions/FunctionNot.hpp>
#include <pathan/functions/FunctionNumber.hpp>
#include <pathan/functions/FunctionOneOrMore.hpp>
#include <pathan/functions/FunctionPosition.hpp>
#include <pathan/functions/FunctionRemove.hpp>
#include <pathan/functions/FunctionReplace.hpp>
#include <pathan/functions/FunctionResolveQName.hpp>
#include <pathan/functions/FunctionResolveURI.hpp>
#include <pathan/functions/FunctionReverse.hpp>
#include <pathan/functions/FunctionRoot.hpp>
#include <pathan/functions/FunctionRound.hpp>
#include <pathan/functions/FunctionRoundHalfToEven.hpp>
#include <pathan/functions/FunctionStartsWith.hpp>
#include <pathan/functions/FunctionString.hpp>
#include <pathan/functions/FunctionStringJoin.hpp>
#include <pathan/functions/FunctionStringLength.hpp>
#include <pathan/functions/FunctionStringToCodepoints.hpp>
#include <pathan/functions/FunctionCodepointsToString.hpp>
#include <pathan/functions/FunctionSubsequence.hpp>
#include <pathan/functions/FunctionSubstring.hpp>
#include <pathan/functions/FunctionSubstringAfter.hpp>
#include <pathan/functions/FunctionSubstringBefore.hpp>
#include <pathan/functions/FunctionSum.hpp>
#include <pathan/functions/FunctionTokenize.hpp>
#include <pathan/functions/FunctionTrace.hpp>
#include <pathan/functions/FunctionTranslate.hpp>
#include <pathan/functions/FunctionTrue.hpp>
#include <pathan/functions/FunctionUnordered.hpp>
#include <pathan/functions/FunctionUpperCase.hpp>
#include <pathan/functions/FunctionZeroOrOne.hpp>
#include <pathan/functions/FunctionYearsFromDuration.hpp>
#include <pathan/functions/FunctionMonthsFromDuration.hpp>
#include <pathan/functions/FunctionDaysFromDuration.hpp>
#include <pathan/functions/FunctionHoursFromDuration.hpp>
#include <pathan/functions/FunctionMinutesFromDuration.hpp>
#include <pathan/functions/FunctionSecondsFromDuration.hpp>
#include <pathan/functions/FunctionYearFromDateTime.hpp>
#include <pathan/functions/FunctionMonthFromDateTime.hpp>
#include <pathan/functions/FunctionDayFromDateTime.hpp>
#include <pathan/functions/FunctionHoursFromDateTime.hpp>
#include <pathan/functions/FunctionMinutesFromDateTime.hpp>
#include <pathan/functions/FunctionSecondsFromDateTime.hpp>
#include <pathan/functions/FunctionTimezoneFromDateTime.hpp>
#include <pathan/functions/FunctionYearFromDate.hpp>
#include <pathan/functions/FunctionMonthFromDate.hpp>
#include <pathan/functions/FunctionDayFromDate.hpp>
#include <pathan/functions/FunctionTimezoneFromDate.hpp>
#include <pathan/functions/FunctionHoursFromTime.hpp>
#include <pathan/functions/FunctionMinutesFromTime.hpp>
#include <pathan/functions/FunctionSecondsFromTime.hpp>
#include <pathan/functions/FunctionTimezoneFromTime.hpp>
#include <pathan/functions/FunctionAdjustDateTimeToTimezone.hpp>
#include <pathan/functions/FunctionAdjustDateToTimezone.hpp>
#include <pathan/functions/FunctionAdjustTimeToTimezone.hpp>

#include <pathan/internal/navigation/NodeTest.hpp>
#include <pathan/exceptions/DSLException.hpp>
#include <pathan/XPath2MemoryManager.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

static const XMLCh XMLChDot[] = { chColon, chColon, chLatin_d, chLatin_o, chLatin_t, chNull };

QueryPlanGenerator::QueryPlanGenerator(XmlManager &mgr, Transaction *txn, DynamicContext *xpc, Optimizer *parent)
	: Optimizer(parent),
	  mgr_(mgr),
	  txn_(txn),
	  xpc_(xpc),
	  predicateContext_(false),
	  unionOp_(new (&memMgr_) UnionQP(&memMgr_)),
	  varStore_(&memMgr_),
	  varId_(0)
{
	varStore_.setGlobalVar(XMLChDot, XMLChDot, varId_++);
}

void QueryPlanGenerator::resetInternal()
{
	unionOp_ = new (&memMgr_) UnionQP(&memMgr_),
	predicateContext_ = false;
	varStore_.clear();
	varId_ = 0;
	varStore_.setGlobalVar(XMLChDot, XMLChDot, varId_++);
}

void QueryPlanGenerator::optimize(XQQuery *query)
{
	if (query->m_userDefVars.size() != 0) {
		varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
		for(vector<XQGlobalVariable*,
			    PathanAllocator<XQGlobalVariable*> >::iterator it =
			    query->m_userDefVars.begin();
		    it != query->m_userDefVars.end(); ++it) {
			PathResult ret = generateGlobalVar(*it, 0);
			setVariable((*it)->getVariableURI(),
				    (*it)->getVariableLocalName(), ret);
			addSecondaryOp(ret);
		}
	}
	PathResult result = generate(query->getQueryBody(), 0);
	addSecondaryOpAndMark(result);

	generateRawQueryPlans(unionOp_->finalizeGlobalRawQueryPlan());
	if (query->m_userDefVars.size() != 0) {
		varStore_.popScope()->release();
	}
}

DataItem *QueryPlanGenerator::optimize(DataItem *item)
{
	PathResult result = generate(item, 0);
	addSecondaryOpAndMark(result);

	generateRawQueryPlans(unionOp_->finalizeGlobalRawQueryPlan());

	return item;
}

void QueryPlanGenerator::generateRawQueryPlans(QueryPlan *qp)
{
	if(qp == 0) {
		qp = new (&memMgr_) UniverseQP(&memMgr_);
	}

	try {
		for(vector<QueryPlanFunction*>::iterator it = qpfs_.begin(); it != qpfs_.end(); ++it) {
			(*it)->generateQueryPlan(mgr_, txn_, qp, xpc_);
		}

		for(vector<DbXmlDocAvailable*>::iterator it2 = docAvails_.begin(); it2 != docAvails_.end(); ++it2) {
			(*it2)->generateQueryPlan(mgr_, txn_, xpc_);
		}

		for(vector<QueryPlanHolder*>::iterator it3 = qpHolders_.begin(); it3 != qpHolders_.end(); ++it3) {
			(*it3)->generateQueryPlan(mgr_, txn_, xpc_);
		}
	}
	catch(const DSLException &e) {
		throw XmlException(XmlException::XPATH_PARSER_ERROR, XMLChToUTF8(e.getError()).str());
	}

	// Look for query plans in DataItemStep and DbXmlCompare that are subsets,
	// and remove the superset
	// TBD maybe move this to a post query plan generator optimisation step? - jpcs
	for(vector<DbXmlNav*>::reverse_iterator it4 = navs_.rbegin(); it4 != navs_.rend(); ++it4) {
		DataItemStep *prevStep = 0;
		DataItemNav::Steps &args = const_cast<DataItemNav::Steps &>((*it4)->getSteps());
		DataItemNav::Steps::iterator s = args.begin();
		for(; s != args.end(); ++s) {

			QueryPlanHolder *holder = 0;
			if(s->step->getType() == (DataItem::whichType)DbXmlContext::DBXML_CONTAINS) {
				holder = (DbXmlContains*)s->step;
			}
			else if(s->step->getType() == (DataItem::whichType)DbXmlContext::DBXML_COMPARE) {
				holder = (DbXmlCompare*)s->step;
			}
			// Also look for the first step inside a DbXmlFilter data item
			else if(s->step->getType() == (DataItem::whichType)DbXmlContext::DBXML_FILTER) {
				const DataItemNav *filterNav = ((DbXmlFilter*)s->step)->getFilterExpr();
				if(!filterNav->getGotoRootFirst() && !filterNav->getSteps().empty()) {
					const DataItem *filterStep = filterNav->getSteps().front().step;
					if(filterStep->getType() == (DataItem::whichType)DbXmlContext::DBXML_CONTAINS) {
						holder = (DbXmlContains*)filterStep;
					}
					else if(filterStep->getType() == (DataItem::whichType)DbXmlContext::DBXML_COMPARE) {
						holder = (DbXmlCompare*)filterStep;
					}
				}
			}

			if(holder != 0) {
				if(prevStep != 0 && holder->getQueryPlan() != 0) {
					DBXML_ASSERT(prevStep->getNodeTest()->getInterface(DbXmlNodeTest::gDbXml) != 0);

					DbXmlNodeTest *nodeTest = (DbXmlNodeTest*)prevStep->getNodeTest();
					if(nodeTest->getQueryPlan() == 0 ||
						holder->getQueryPlan()->isSubsetOf(nodeTest->getQueryPlan())) {

						if(prevStep->getAxis() == DataItemStep::SELF ||
							prevStep->getAxis() == DataItemStep::ATTRIBUTE ||
							prevStep->getAxis() == DataItemStep::CHILD ||
							prevStep->getAxis() == DataItemStep::DESCENDANT ||
							prevStep->getAxis() == DataItemStep::DESCENDANT_OR_SELF) {

							nodeTest->setQueryPlan(const_cast<QueryPlan*>(holder->getQueryPlan()));
							nodeTest->setQueryPlanExecutable(holder->isQueryPlanExecutable());
							nodeTest->setRunQueryPlanIfDocOnly(holder->runQueryPlanIfDocOnly());

							holder->setQueryPlan(0);
						}
					}
				}
			}

			if(s->step->getType() == DataItem::STEP) {
				prevStep = (DataItemStep*)s->step;
			} else if(s->step->getType() == (DataItem::whichType)DbXmlContext::DBXML_FILTER) {
				// prevStep stays the same
			} else {
				prevStep = 0;
			}
		}
	}

	// Look for query plans in SELF or ATTRIBUTE axis steps that can be pushed back to their
	// previous step
	// TBD maybe move this to a post query plan generator optimisation step? - jpcs
	for(vector<DbXmlNav*>::iterator it5 = navs_.begin(); it5 != navs_.end(); ++it5) {
		DataItemStep *prevStep = 0;
		DataItemNav::Steps &args = const_cast<DataItemNav::Steps &>((*it5)->getSteps());
		DataItemNav::Steps::iterator s = args.begin();
		for(; s != args.end(); ++s) {

			DataItemStep *thisStep = 0;
			if(s->step->getType() == DataItem::STEP) {
				thisStep = (DataItemStep*)s->step;
			}
			// Also look for the first step inside a DbXmlFilter data item
			else if(s->step->getType() == (DataItem::whichType)DbXmlContext::DBXML_FILTER) {
				const DataItemNav *filterNav = ((DbXmlFilter*)s->step)->getFilterExpr();
				if(!filterNav->getGotoRootFirst() && !filterNav->getSteps().empty()) {
					const DataItem *filterStep = filterNav->getSteps().front().step;
					if(filterStep->getType() == DataItem::STEP) {
						thisStep = (DataItemStep*)filterStep;
					}
				}
			}

			if(thisStep != 0 && prevStep != 0 && (thisStep->getAxis() == DataItemStep::SELF ||
				   thisStep->getAxis() == DataItemStep::ATTRIBUTE)) {

				DBXML_ASSERT(thisStep->getNodeTest()->getInterface(DbXmlNodeTest::gDbXml) != 0);
				DBXML_ASSERT(prevStep->getNodeTest()->getInterface(DbXmlNodeTest::gDbXml) != 0);

				DbXmlNodeTest *thisNodeTest = (DbXmlNodeTest*)thisStep->getNodeTest();
				DbXmlNodeTest *prevNodeTest = (DbXmlNodeTest*)prevStep->getNodeTest();

				if(thisNodeTest->getQueryPlan() != 0 && (prevNodeTest->getQueryPlan() == 0 ||
					   thisNodeTest->getQueryPlan()->isSubsetOf(prevNodeTest->getQueryPlan()))) {

					if(prevStep->getAxis() == DataItemStep::SELF ||
						prevStep->getAxis() == DataItemStep::ATTRIBUTE ||
						prevStep->getAxis() == DataItemStep::CHILD ||
						prevStep->getAxis() == DataItemStep::DESCENDANT ||
						prevStep->getAxis() == DataItemStep::DESCENDANT_OR_SELF) {

						prevNodeTest->setQueryPlan(const_cast<QueryPlan*>(thisNodeTest->getQueryPlan()));
						prevNodeTest->setQueryPlanExecutable(thisNodeTest->isQueryPlanExecutable());
						prevNodeTest->setRunQueryPlanIfDocOnly(thisNodeTest->runQueryPlanIfDocOnly());

						thisNodeTest->setQueryPlan(0);
					}
				}
			}

			if(s->step->getType() == DataItem::STEP) {
				prevStep = (DataItemStep*)s->step;
			} else if(s->step->getType() == (DataItem::whichType)DbXmlContext::DBXML_FILTER) {
				// prevStep stays the same
			} else {
				prevStep = 0;
			}
		}
	}
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generate(DataItem *item, VariableIDs *ids)
{
	PathResult result;
	switch(item->getType()) {
	case DataItem::LITERAL: {
		result = generateLiteral((DataItemLiteral *)item, ids);
		break;
	}
	case DataItem::SEQUENCE: {
		result = generateSequence((DataItemSequence *)item, ids);
		break;
	}
	case DataItem::FUNCTION: {
		result = generateFunction((DataItemFunction *)item, ids);
		break;
	}
	case DataItem::NAVIGATION: {
		result = generateNav((DataItemNav *)item, ids);
		break;
	}
	case DataItem::VARIABLE: {
		result = generateVariable((DataItemVariable *)item, ids);
		break;
	}
	case DataItem::STEP: {
		result = generateStep((DataItemStep *)item, ids);
		break;
	}
	case DataItem::IF: {
		result = generateIf((DataItemIf *)item, ids);
		break;
	}
	case DataItem::INSTANCE_OF: {
		result = generateInstanceOf((DataItemInstanceOf *)item, ids);
		break;
	}
	case DataItem::CASTABLE_AS: {
		result = generateCastableAs((DataItemCastableAs *)item, ids);
		break;
	}
	case DataItem::CAST_AS: {
		result = generateCastAs((DataItemCastAs *)item, ids);
		break;
	}
	case DataItem::TREAT_AS: {
		result = generateTreatAs((DataItemTreatAs *)item, ids);
		break;
	}
	case DataItem::PARENTHESIZED: {
		result = generateParenthesizedExpr((DataItemParenthesizedExpr *)item, ids);
		break;
	}
	case DataItem::OPERATOR: {
		result = generateOperator((DataItemOperator *)item, ids);
		break;
	}
	case DataItem::CONTEXT_ITEM: {
		result = generateContextItem((DataItemContextItem *)item, ids);
		break;
	}
	case ((DataItem::whichType)XQContext::DEBUG_HOOK): {
		result = generateDebugHook((XQDebugHook *)item, ids);
		break;
	}
	case ((DataItem::whichType)XQContext::FLWOR): {
		result = generateFLWOR((XQFLWOR *)item, ids);
		break;
	}
	case ((DataItem::whichType)XQContext::FLWOR_QUANTIFIED): {
		result = generateFLWORQuantified((XQQuantified *)item, ids);
		break;
	}
	case ((DataItem::whichType)XQContext::TYPESWITCH): {
		result = generateTypeswitch((XQTypeswitch *)item, ids);
		break;
	}
	case ((DataItem::whichType)XQContext::VALIDATE): {
		result = generateValidate((XQValidate *)item, ids);
		break;
	}
	case ((DataItem::whichType)XQContext::VARIABLE_DEFINITION): {
		result = generateGlobalVar((XQGlobalVariable *)item, ids);
		break;
	}
	case ((DataItem::whichType)XQContext::DOM_CONSTRUCTOR): {
		result = generateDOMConstructor((XQDOMConstructor *)item, ids);
		break;
	}
	case ((DataItem::whichType)XQContext::ORDERING_CHANGE): {
		result = generateOrderingChange((XQOrderingChange *)item, ids);
		break;
	}
	case ((DataItem::whichType)XQContext::USER_FUNCTION): {
		result = generateUserFunction((XQFunction::XQFunctionEvaluator *)item, ids);
		break;
	}
	case ((DataItem::whichType)DbXmlContext::DBXML_COMPARE): {
		result = generateDbXmlCompare((DbXmlCompare*)item, ids);
		break;
	}
	case ((DataItem::whichType)DbXmlContext::DBXML_FILTER): {
		result = generateDbXmlFilter((DbXmlFilter*)item, ids);
		break;
	}
	case ((DataItem::whichType)DbXmlContext::DBXML_CONTAINS): {
		result = generateDbXmlContains((DbXmlContains*)item, ids);
		break;
	}
// 	case DataItem::FOR: {
// 		result = generateFor((DataItemFor *)item, ids);
// 		break;
// 	}
// 	case DataItem::QUANTIFIED: {
// 		result = generateQuantifiedExpr((DataItemQuantifiedExpr *)item, ids);
// 		break;
// 	}
// 	case ((DataItem::whichType)XQContext::FUNCTION_CALL): {
// 		result = generateFunctionCall((XQFunctionCall *)item, ids);
// 		break;
// 	}
	default: break;
	}
	return result;
}

static ImpliedSchemaNode::Type getType(DataItemStep::Axis axis)
{
	ImpliedSchemaNode::Type result = (ImpliedSchemaNode::Type)-1;
	switch(axis) {
	case DataItemStep::CHILD: {
		result = ImpliedSchemaNode::CHILD;
		break;
	}
	case DataItemStep::ATTRIBUTE: {
		result = ImpliedSchemaNode::ATTRIBUTE;
		break;
	}
	case DataItemStep::DESCENDANT: {
		result = ImpliedSchemaNode::DESCENDANT;
		break;
	}
	default: {
		// Do nothing
		break;
	}
	}
	return result;
}

const QueryPlanGenerator::VarValue &QueryPlanGenerator::getCurrentContext() const
{
	return varStore_.getVar(XMLChDot, XMLChDot)->getValue();
}

unsigned int QueryPlanGenerator::setCurrentContext(const PathResult &value)
{
	return setVariable(XMLChDot, XMLChDot, value);
}

unsigned int QueryPlanGenerator::setVariable(const XMLCh *uri, const XMLCh *name, const PathResult &value)
{
	varStore_.declareVar(uri, name, VarValue(value, varId_));
	return varId_++;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateStep(DataItemStep *item, VariableIDs *ids)
{
	const NodeTest *step = item->getNodeTest();
	ImpliedSchemaNode::Type type = getType(item->getAxis());
	const XMLCh *uri = step->getNodeUri();
	const XMLCh *name = step->getNodeName();
	bool wildcardURI = step->getNamespaceWildcard();
	bool wildcardName = step->getNameWildcard();
	if(step->getItemType()) {
		wildcardURI = true;
		wildcardName = true;
	}

	bool wildcardNodeType;
	SequenceType::ItemType *itemtype = step->getItemType();
	if(itemtype) {
		wildcardNodeType = itemtype->getItemTestType() != SequenceType::ItemType::TEST_ELEMENT;
	}
	else {
		wildcardNodeType = step->getTypeWildcard() ||
			(step->isNodeTypeSet() && step->getNodeType() != Node::element_string);
	}

	PathResult result;

	ImpliedSchemaNode dummy(uri, wildcardURI, name, wildcardName, wildcardNodeType, type, xpc_->getMemoryManager());
	const VarValue &currentContext = getCurrentContext();
	if(ids) ids->insert(currentContext.id);
	for(Paths::const_iterator it = currentContext.returnPaths.begin(); it != currentContext.returnPaths.end(); ++it) {

		switch(item->getAxis()) {
		case DataItemStep::PARENT: {
			generateParentStep(*it, dummy, result);
			break;
		}
		case DataItemStep::DESCENDANT_OR_SELF: {
			generateDescendantOrSelfStep(*it, dummy, result);
			break;
		}
		case DataItemStep::SELF: {
			generateSelfStep(*it, dummy, result);
			break;
		}
		case DataItemStep::ANCESTOR: {
			generateAncestorStep(*it, dummy, result);
			break;
		}
		case DataItemStep::ANCESTOR_OR_SELF: {
			generateAncestorOrSelfStep(*it, dummy, result);
			break;
		}
		case DataItemStep::FOLLOWING: {
			generateFollowingStep(*it, dummy, result);
			break;
		}
		case DataItemStep::PRECEDING: {
			generatePrecedingStep(*it, dummy, result);
			break;
		}
		case DataItemStep::PRECEDING_SIBLING:
		case DataItemStep::FOLLOWING_SIBLING: {
			generateSiblingStep(*it, dummy, result);
			break;
		}
		case DataItemStep::ATTRIBUTE:
		case DataItemStep::CHILD:
		case DataItemStep::DESCENDANT: {
			generateBuiltInStep(*it, dummy, result);
			break;
		}
		case DataItemStep::NAMESPACE:
		default: {
			// Do nothing
			break;
		}
		}
	}

	PathsQP *pathsqp = new (&memMgr_) PathsQP(result.returnPaths, &memMgr_);
	result.operation = new (&memMgr_) IntersectQP(currentContext.operation, pathsqp, &memMgr_);

	DbXmlNodeTest *nodeTest = (DbXmlNodeTest*)item->getNodeTest()->getInterface(DbXmlNodeTest::gDbXml);
	if(nodeTest != 0) {
		nodeTest->addToQueryPlan(pathsqp);
	}
	else {
		// Replace the NodeTest object with our own version
		nodeTest = new (xpc_->getMemoryManager())
			DbXmlNodeTest(step, pathsqp, xpc_->getMemoryManager());
		qpHolders_.push_back(nodeTest);
		item->setNodeTest(nodeTest);
	}

	generatePredicates(item, result, ids);
	return result;
}

void QueryPlanGenerator::generateBuiltInStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node, PathResult &result)
{
	switch(target->getType()) {
	case ImpliedSchemaNode::ATTRIBUTE: {
		// Does nothing
		break;
	}
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::ROOT: {
		ImpliedSchemaNode *newNode = node.copy();
		result.join(target->appendChild(newNode));
		break;
	}
	default: break;
	}
}

void QueryPlanGenerator::generateParentStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node, PathResult &result)
{
	ImpliedSchemaNode *parent = target->getParent();

	switch(target->getType()) {
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD: {
		if(node.matches(parent)) {
			result.join(parent);
		}
		break;
	}
	case ImpliedSchemaNode::DESCENDANT: {
		if(node.matches(parent)) {
			result.join(parent);
		}

		ImpliedSchemaNode *newNode = node.copy();
		newNode->setType(ImpliedSchemaNode::DESCENDANT);
		result.join(parent->appendChild(newNode));
		break;
	}
	case ImpliedSchemaNode::ROOT: {
		// There is no parent
		break;
	}
	default: break;
	}
}

void QueryPlanGenerator::generateDescendantOrSelfStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node, PathResult &result)
{
	generateSelfStep(target, node, result);
	node.setType(ImpliedSchemaNode::DESCENDANT);
	generateBuiltInStep(target, node, result);
}

void QueryPlanGenerator::generateSelfStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node, PathResult &result)
{
	if(node.matches(target)) {
		result.join(target);
	}
}

void QueryPlanGenerator::generateAncestorStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node, PathResult &result)
{
	ImpliedSchemaNode *parent = target->getParent();

	switch(target->getType()) {
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD: {
		if(node.matches(parent)) {
			result.join(parent);
		}
		generateAncestorStep(parent, node, result);
		break;
	}
	case ImpliedSchemaNode::DESCENDANT: {
		if(node.matches(parent)) {
			result.join(parent);
		}

		ImpliedSchemaNode *newNode = node.copy();
		newNode->setType(ImpliedSchemaNode::DESCENDANT);
		result.join(parent->appendChild(newNode));

		generateAncestorStep(parent, node, result);
		break;
	}
	case ImpliedSchemaNode::ROOT: {
		// Has no ancestors
		break;
	}
	default: break;
	}
}

void QueryPlanGenerator::generateAncestorOrSelfStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node, PathResult &result)
{
	generateSelfStep(target, node, result);
	generateAncestorStep(target, node, result);
}

void QueryPlanGenerator::generateFollowingStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node, PathResult &result)
{
	ImpliedSchemaNode *newNode = node.copy();
	newNode->setType(ImpliedSchemaNode::DESCENDANT);
	result.join(target->getRoot()->appendChild(newNode));
}

void QueryPlanGenerator::generatePrecedingStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node, PathResult &result)
{
	ImpliedSchemaNode *root = target->getRoot();

	if(node.matches(root)) {
		result.join(root);
	}

	ImpliedSchemaNode *newNode = node.copy();
	newNode->setType(ImpliedSchemaNode::DESCENDANT);
	result.join(root->appendChild(newNode));
}

void QueryPlanGenerator::generateSiblingStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node, PathResult &result)
{
	ImpliedSchemaNode *parent = target->getParent();

	switch(target->getType()) {
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::DESCENDANT: {
		ImpliedSchemaNode *newNode = node.copy();
		newNode->setType(target->getType());
		result.join(parent->appendChild(newNode));
		break;
	}
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::ROOT: {
		// Have no siblings
		break;
	}
	default: break;
	}
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateNav(DataItemNav *item, VariableIDs *ids)
{
	navs_.push_back((DbXmlNav*)item);

	PathResult result;
	const VarValue &currentContext = getCurrentContext();

	bool setContext = false;
	if(item->getGotoRootFirst()) {
		if(ids) ids->insert(currentContext.id);

		for(Paths::const_iterator it = currentContext.returnPaths.begin(); it != currentContext.returnPaths.end(); ++it) {
			result.join((*it)->getRoot());
		}

		result.operation = new (&memMgr_) IntersectQP(currentContext.operation, new (&memMgr_) PathsQP(result.returnPaths, &memMgr_), &memMgr_);
		setContext = true;
	}
	else {
		result = currentContext;
	}

	unsigned int varId = 0;
	DataItemNav::Steps &args = const_cast<DataItemNav::Steps &>(item->getSteps());
	for(DataItemNav::Steps::iterator i = args.begin(); i != args.end(); ++i) {
		if(setContext) {
			varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
			varId = setCurrentContext(result);
		}
		result.returnPaths.clear();

		PathResult ret = generate(i->step, ids);
		result.join(ret);
		result.operation = ret.operation;

		if(setContext) {
			varStore_.popScope()->release();
			if(ids) ids->erase(varId);
		}
		setContext = true;
	}

	generatePredicates(item, result, ids);
	return result;
}

/*
  The rule for function parameters and operations:

  * If the parameter being an empty sequence implies that the
  function result will be an empty sequence, then the parameter
  operation needs to be *intersected* into the result.

  * If the parameter is otherwise included in the result of the
  function, then the parameter operation needs to be *unioned* into
  the result.

  * Remember, if you're unsure, the safest thing to do is not return
  the operation from the parameter. It will make less effective
  query plans, but it *will* return correct results!
*/
QueryPlanGenerator::PathResult QueryPlanGenerator::generateFunction(DataItemFunction *item, VariableIDs *ids)
{
	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getArguments());
	const XMLCh *uri = item->getFunctionURI();
	const XMLCh *name = item->getFunctionName();

	PathResult result;

	if(uri == DataItemFunction::XMLChFunctionURI) {

		// fn:root()
		if(name == FunctionRoot::name) {
			const VarValue &currentContext = getCurrentContext();

			// root() gets the root of it's argument, if it has one.
			// Otherwise it gets the root of the current context
			PathResult ret;
			if(args.empty()) {
				ret = currentContext;
				if(ids) ids->insert(currentContext.id);
			}
			else {
				ret = generate(args[0], ids);
			}

			for(Paths::iterator it = ret.returnPaths.begin(); it != ret.returnPaths.end(); ++it) {
				result.join((*it)->getRoot());
			}

			result.operation = new (&memMgr_) IntersectQP(new (&memMgr_) PathsQP(result.returnPaths, &memMgr_), ret.operation, &memMgr_);
		}

		// fn:doc() and fn:collection()
		else if(name == FunctionDoc::name ||
			name == FunctionCollection::name) {
			PathResult ret = generate(args[0], ids);
			addSecondaryOpAndMark(ret);

			ImpliedSchemaNode *root = new (xpc_->getMemoryManager()) ImpliedSchemaNode(ImpliedSchemaNode::ROOT, xpc_->getMemoryManager());
			result.join(root);

			result.operation = new (&memMgr_) PathsQP(result.returnPaths, &memMgr_);
		}

		// fn:QueryPlanFunction()
		else if(name == QueryPlanFunction::name) {
			QueryPlanFunction *qpFunc = (QueryPlanFunction*)item;

			if(!args.empty()) {
				PathResult ret = generate(args[0], ids);
				addSecondaryOpAndMark(ret);
			}

			ImpliedSchemaNode *root = const_cast<ImpliedSchemaNode*>(qpFunc->getImpliedSchema());
			if(root == 0) {
				qpfs_.push_back(qpFunc);

				root = new (xpc_->getMemoryManager())
					ImpliedSchemaNode(ImpliedSchemaNode::ROOT, xpc_->getMemoryManager());
				qpFunc->setImpliedSchema(root);
				storeInScopeVars(root);
			}
			result.join(root);

			result.operation = new (&memMgr_) PathsQP(result.returnPaths, &memMgr_);
		}

		else if(name == FunctionTrace::name) {
			// trace() returns it's first argument, and prints the string
			// value of it's second argument.

			result = generate(args[0], ids);
			addSecondaryOpAndMark(generate(args[1], ids));
		}
		else if(name == FunctionZeroOrOne::name ||
			name == FunctionOneOrMore::name ||
			name == FunctionExactlyOne::name) {
			// return their argument, or raise an exception

			result = generate(args[0], ids);
		}
		else if(name == FunctionInsertBefore::name) {
			// behaves a bit like union

			PathResult ret = generate(args[0], ids);
			result.join(ret);

			addSecondaryOpAndMark(generate(args[1], ids));

			PathResult ret2 = generate(args[2], ids);
			result.join(ret2);

			result.operation = new (&memMgr_) UnionQP(ret.operation, ret2.operation, &memMgr_);
		}
		else if(name == FunctionRemove::name) {
			PathResult ret = generate(args[0], ids);
			result.join(ret);
			result.operation = ret.operation;
			addSecondaryOpAndMark(generate(args[1], ids));
		}
		else if(name == FunctionReverse::name ||
			name == FunctionUnordered::name) {
			PathResult ret = generate(args[0], ids);
			result.join(ret);
			result.operation = ret.operation;
		}
		else if(name == FunctionSubsequence::name) {
			PathResult ret = generate(args[0], ids);
			result.join(ret);
			result.operation = ret.operation;
			addSecondaryOpAndMark(generate(args[1], ids));
			if(args.size() > 2) {
				addSecondaryOpAndMark(generate(args[2], ids));
			}
		}
		else if(name == FunctionNamespaceURIForPrefix::name) {
			addSecondaryOpAndMark(generate(args[0], ids));
			addSecondaryOp(generate(args[1], ids));
		}
		else if(name == FunctionInScopePrefixes::name) {
			addSecondaryOp(generate(args[0], ids));
		}
		else if(name == FunctionDeepEqual::name) {
			addSecondaryOp(generate(args[0], ids));
			addSecondaryOp(generate(args[1], ids));
			if(args.size() > 2) {
				addSecondaryOpAndMark(generate(args[2], ids));
			}
		}
		else if(name == FunctionId::name ||
			name == FunctionIdref::name) {
			// We don't handle these, so we mark the trees as unoptimisable
			const VarValue &currentContext = getCurrentContext();
			for(Paths::const_iterator i = currentContext.returnPaths.begin(); i != currentContext.returnPaths.end(); ++i) {
				(*i)->getRoot()->markSubtree();
			}
			result.operation = currentContext.operation;
			if(ids) ids->insert(currentContext.id);
		}
		else if(name == FunctionLang::name) {
			addSecondaryOpAndMark(generate(args[0], ids));
		}

		// These implicitly use the value of the current context if they have no parameters
		else if((name == FunctionStringLength::name ||
			 name == FunctionNormalizeSpace::name ||
			 name == FunctionNumber::name) &&
			args.empty()) {
			const VarValue &currentContext = getCurrentContext();
			addSecondaryOpAndMark(currentContext);
			if(ids) ids->insert(currentContext.id);
		}

		// These implicitly use the current context if they have no parameters
		else if((name == FunctionName::name ||
			 name == FunctionLocalname::name ||
			 name == FunctionNamespaceUri::name ||
			 name == FunctionLang::name) &&
			args.empty()) {
			const VarValue &currentContext = getCurrentContext();
			if(ids) ids->insert(currentContext.id);
		}

		// These use the presence of a node, not it's value (and hence sub tree)
		else if(name == FunctionBoolean::name ||
			name == FunctionExists::name) {
			PathResult ret = generate(args[0], ids);
			if(predicateContext_) {
				result.operation = ret.operation;
			}
		}

		// These use the presence of a node, not it's value (and hence sub tree)
		else if(name == FunctionNot::name ||
			name == FunctionName::name ||
			name == FunctionLocalname::name ||
			name == FunctionNamespaceUri::name ||
			name == FunctionEmpty::name ||
			name == FunctionCount::name ||
			name == FunctionNilled::name) {
			for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
				addSecondaryOp(generate(*i, ids));
			}
		}

		// These return the value of a node (which we treat as if it had returned the node itself)
		else if((name == FunctionString::name ||
			 name == FunctionNumber::name) &&
			args.empty()) {
			const VarValue &currentContext = getCurrentContext();
			if(ids) ids->insert(currentContext.id);
			addSecondaryOpAndMark(currentContext);
			result.operation = currentContext.operation;
		}
		else if(name == FunctionString::name ||
			name == FunctionNumber::name ||
			name == FunctionDistinctValues::name ||
			name == FunctionData::name ||
			name == FunctionMax::name ||
			name == FunctionMin::name) {
			PathResult ret = generate(args[0], ids);
			ret.markSubtree();
			result.operation = ret.operation;

			if(args.size() > 1) {
				addSecondaryOpAndMark(generate(args[1], ids));
			}
		}

		// These generate substring index lookups
		else if(predicateContext_ &&
			name == FunctionStartsWith::name) {
			generateSubstring(ImpliedSchemaNode::PREFIX, args, result, ids);
		}
		else if(predicateContext_ &&
			(name == FunctionEndsWith::name ||
			 name == FunctionContains::name)) {
			generateSubstring(ImpliedSchemaNode::SUBSTRING, args, result, ids);
		}

		else if(name == FunctionPosition::name ||
			name == FunctionLast::name) {
			// It fulfills our purposes to say that these functions
			// access the current context, since the context position
			// and size are always set at the same time.
			const VarValue &currentContext = getCurrentContext();
			if(ids) ids->insert(currentContext.id);
		}

		// fn:DbXmlDocAvailable()
		else if(name == DbXmlDocAvailable::name) {
			DbXmlDocAvailable *daFunc = (DbXmlDocAvailable*)item;
			docAvails_.push_back(daFunc);
			addSecondaryOpAndMark(generate(args[0], ids));
		}

		else {
			// It's in the xquery function namespace, so assume it uses values from the returned paths
			for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
				addSecondaryOpAndMark(generate(*i, ids));
			}
		}

	}

	else if(uri == MetaDataFunction::XMLChFunctionURI) {
		// dbxml:metadata()
		if(name == MetaDataFunction::name) {
			MetaDataFunction *mdFunc = (MetaDataFunction*)item;

			addSecondaryOpAndMark(generate(args[0], ids));

			PathResult targets;
			if(args.size() == 1) {
				const VarValue &currentContext = getCurrentContext();
				targets = currentContext;
				if(ids) ids->insert(currentContext.id);
			}
			else {
				targets = generate(args[1], ids);
			}

			const XMLCh *uri, *name;
			if(mdFunc->getConstantQNameArg(uri, name, xpc_)) {

				for(Paths::const_iterator it = targets.returnPaths.begin();
				    it != targets.returnPaths.end(); ++it) {
					ImpliedSchemaNode *newNode = new (xpc_->getMemoryManager())
						ImpliedSchemaNode(uri, false, name, false, false,
								  ImpliedSchemaNode::METADATA, xpc_->getMemoryManager());
					result.join((*it)->appendChild(newNode));
				}

				result.operation = new (&memMgr_) IntersectQP(targets.operation, new (&memMgr_) PathsQP(result.returnPaths, &memMgr_), &memMgr_);
			}
		}
	}

	else {
		// We don't know about this function, so mark the entire tree as being needed
		for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
			PathResult ret = generate(*i, ids);
			ret.markRoot();
			addSecondaryOp(ret);
		}
	}

	generatePredicates(item, result, ids);
	return result;
}

void QueryPlanGenerator::storeInScopeVars(ImpliedSchemaNode *root) {
	// List the in scope vars
	VariableIDs vars;
	VarStore::MyScope* index = const_cast<VarStore::MyScope*>(varStore_.getCurrentScope());
	while(index) {
		std::vector< std::pair<unsigned int, const XMLCh*> > tmp = index->getVars();
		for(std::vector< std::pair<unsigned int, const XMLCh*> >::iterator it = tmp.begin(); it != tmp.end(); ++it) {
			unsigned int varId = index->get(it->first, it->second)->getValue().id;
			vars.insert(varId);
		}
		// stop the search when we are going out of scope
		if(index->getType() == VarStore::MyScope::LOCAL_SCOPE)
			index = const_cast<VarStore::MyScope*>(varStore_.getGlobalScope());
		else index = index->getNext();
	}

	inScopeVars_[root] = vars;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateUserFunction(XQFunction::XQFunctionEvaluator *item, VariableIDs *ids)
{
	PathResult result;

	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getArguments());
	const XQFunction *functionDef = item->getFunctionDefinition();

	// Evaluate the arguments in the current scope, declare them in the function's scope
	vector<ArgHolder> evaluatedArgs;
	if(functionDef->getParams()) {
		XQFunction::VectorOfFunctionParameters::const_iterator binding = functionDef->getParams()->begin();
		for(VectorOfDataItems::iterator arg = args.begin(); arg != args.end() && binding != functionDef->getParams()->end(); ++arg, ++binding) {

			PathResult r = generate(*arg, ids);
			addSecondaryOp(r);

			if((*binding)->_qname) {
				evaluatedArgs.push_back(ArgHolder((*binding)->_uri, (*binding)->_name, r));

				if((*binding)->m_pType->getItemTestType() == SequenceType::ItemType::TEST_ATOMIC_TYPE) {
					// This will involve casting to values
					r.markSubtree();
				}
			}
		}
	}

	// Check to see if this user function is recursing into a user function
	// that has already been called.
	pair<set<const DataItem*>::iterator, bool> ins = userFunctionStack_.insert(functionDef->getFunctionBody());
	if(ins.second) {
		VariableIDs myVars;
		varStore_.addScope(VarStore::MyScope::LOCAL_SCOPE);

		for(vector<ArgHolder>::iterator it = evaluatedArgs.begin(); it != evaluatedArgs.end(); ++it) {
			myVars.insert(setVariable(it->uri, it->name, it->value));
		}

		PathResult ret = generate(const_cast<DataItem *>(functionDef->getFunctionBody()), ids);
		result.join(ret);
		result.operation = ret.operation;

		varStore_.popScope()->release();
		if(ids) for(VariableIDs::iterator it2 = myVars.begin(); it2 != myVars.end(); ++it2)
			ids->erase(*it2);

		userFunctionStack_.erase(ins.first);
	}
	else {
		// All we can really do if it is recursive is to mark the entire tree as needed
		// TBD see if we can handle recursive user functions better - jpcs
		for(vector<ArgHolder>::iterator it = evaluatedArgs.begin(); it != evaluatedArgs.end(); ++it) {
			it->value.markRoot();
			it->value.markSubtree();
		}
	}

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDbXmlFilter(DbXmlFilter *item, VariableIDs *ids)
{
	PathResult result;

	IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);

	const VarValue &currentContext = getCurrentContext();
	intersectOp->addArg(currentContext.operation);

	if(ids) ids->insert(currentContext.id);

	PathResult ret = generate(const_cast<DbXmlNav*>(item->getFilterExpr()), ids);
	intersectOp->addArg(ret.operation);

	result.operation = intersectOp;
	result.returnPaths = currentContext.returnPaths;

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDbXmlContains(DbXmlContains *item, VariableIDs *ids)
{
	PathResult result;

	IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);
	result.operation = intersectOp;

	DataItem *arg = const_cast<DataItem *>(item->getArgument());

	VariableIDs argIDs;
	PathResult ret = generate(arg, &argIDs);
	addSecondaryOpAndMark(ret);
	if(ids) {
		VariableIDs::const_iterator i;
		for(i = argIDs.begin(); i != argIDs.end(); i++)
			ids->insert(*i);
	}

	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	const VarValue &currentContext = getCurrentContext();
	intersectOp->addArg(currentContext.operation);
	if(ids) ids->insert(currentContext.id);

	Paths nlsPaths;
	for(Paths::const_iterator it = currentContext.returnPaths.begin(); it != currentContext.returnPaths.end(); ++it) {
		ImpliedSchemaNode *newChild = (*it)->appendChild(new (mm) ImpliedSchemaNode(ImpliedSchemaNode::SUBSTRING, arg, mm));
		nlsPaths.push_back(newChild);

		if(varsInScope(*it, argIDs)) result.join(newChild);
	}

	intersectOp->addArg(new (&memMgr_) PathsQP(result.returnPaths, &memMgr_));

	PathsQP *pathsqp = new (&memMgr_) PathsQP(nlsPaths, &memMgr_);
	if(item->getQueryPlan() == 0) {
		// It's the first time we've seen this item
		qpHolders_.push_back(item);
	}
	item->addToQueryPlan(pathsqp);

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths = currentContext.returnPaths;

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDbXmlCompare(DbXmlCompare *item, VariableIDs *ids)
{
	PathResult result;

	IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);
	result.operation = intersectOp;

	ImpliedSchemaNode::Type type;
	switch(item->getOperation()) {
	case GeneralComp::EQUAL: {
		type = ImpliedSchemaNode::EQUALS;
		break;
	}
	case GeneralComp::LESS_THAN: {
		type = ImpliedSchemaNode::LTX;
		break;
	}
	case GeneralComp::LESS_THAN_EQUAL: {
		type = ImpliedSchemaNode::LTE;
		break;
	}
	case GeneralComp::GREATER_THAN: {
		type = ImpliedSchemaNode::GTX;
		break;
	}
	case GeneralComp::GREATER_THAN_EQUAL: {
		type = ImpliedSchemaNode::GTE;
		break;
	}
	default: {
		type = (ImpliedSchemaNode::Type)-1;
		break;
	}
	}

	DataItem *arg = const_cast<DataItem *>(item->getArgument());

	VariableIDs argIDs;
	PathResult ret = generate(arg, &argIDs);
	addSecondaryOpAndMark(ret);
	if(ids) {
		VariableIDs::const_iterator i;
		for(i = argIDs.begin(); i != argIDs.end(); i++)
			ids->insert(*i);
	}

	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	const VarValue &currentContext = getCurrentContext();
	intersectOp->addArg(currentContext.operation);
	if(ids) ids->insert(currentContext.id);

	Paths nlsPaths;
	for(Paths::const_iterator it = currentContext.returnPaths.begin(); it != currentContext.returnPaths.end(); ++it) {
		if(type != (ImpliedSchemaNode::Type)-1) {
			ImpliedSchemaNode *newChild = (*it)->appendChild(new (mm) ImpliedSchemaNode(type, arg, mm));
			nlsPaths.push_back(newChild);

			if(varsInScope(*it, argIDs)) result.join(newChild);
		} else {
			(*it)->markSubtree();
		}
	}

	intersectOp->addArg(new (&memMgr_) PathsQP(result.returnPaths, &memMgr_));

	PathsQP *pathsqp = new (&memMgr_) PathsQP(nlsPaths, &memMgr_);
	if(item->getQueryPlan() == 0) {
		// It's the first time we've seen this item
		qpHolders_.push_back(item);
	}
	item->addToQueryPlan(pathsqp);

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths = currentContext.returnPaths;

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateOperator(DataItemOperator *item, VariableIDs *ids)
{
	PathResult result;

	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getArguments());
	const XMLCh *name = item->getOperatorName();

	if(name == Union::name) {
		UnionQP *unionOp = new (&memMgr_) UnionQP(&memMgr_);

		PathResult ret = generate(args[0], ids);
		result.join(ret);
		unionOp->addArg(ret.operation);

		ret = generate(args[1], ids);
		result.join(ret);
		unionOp->addArg(ret.operation);

		result.operation = unionOp;
	}

	else if(name == Intersect::name) {
		IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);

		PathResult ret = generate(args[0], ids);
		result.join(ret);
		intersectOp->addArg(ret.operation);

		ret = generate(args[1], ids);
		result.join(ret);
		intersectOp->addArg(ret.operation);

		result.operation = intersectOp;
	}

	else if(name == Except::name) {
		// We can't do except in terms of QueryPlan::EXCEPT
		// because operations work on documents, not nodes
		result = generate(args[0], ids);
		generate(args[1], ids);
	}

	// an equals operation in predicate context
	else if(predicateContext_ &&
		(name == Equals::name ||
		 (name == GeneralComp::name &&
		  ((const GeneralComp *)item)->getOperation() == GeneralComp::EQUAL))) {
		generateComparison(ImpliedSchemaNode::EQUALS, args, result, ids);
	}

	// a less than operation in predicate context
	else if(predicateContext_ &&
		(name == LessThan::name ||
		 (name == GeneralComp::name &&
		  ((const GeneralComp *)item)->getOperation() == GeneralComp::LESS_THAN))) {
		generateComparison(ImpliedSchemaNode::LTX, args, result, ids);
	}

	// a less than or equal operation in predicate context
	else if(predicateContext_ &&
		(name == LessThanEqual::name ||
		 (name == GeneralComp::name &&
		  ((const GeneralComp *)item)->getOperation() == GeneralComp::LESS_THAN_EQUAL))) {
		generateComparison(ImpliedSchemaNode::LTE, args, result, ids);
	}

	// a greater than operation in predicate context
	else if(predicateContext_ &&
		(name == GreaterThan::name ||
		 (name == GeneralComp::name &&
		  ((const GeneralComp *)item)->getOperation() == GeneralComp::GREATER_THAN))) {
		generateComparison(ImpliedSchemaNode::GTX, args, result, ids);
	}

	// a greater than or equal operation in predicate context
	else if(predicateContext_ &&
		(name == GreaterThanEqual::name ||
		 (name == GeneralComp::name &&
		  ((const GeneralComp *)item)->getOperation() == GeneralComp::GREATER_THAN_EQUAL))) {
		generateComparison(ImpliedSchemaNode::GTE, args, result, ids);
	}

	// These operators use the value of their arguments
	else if(name == Divide::name ||
		name == Equals::name ||
		name == GeneralComp::name ||
		name == GreaterThan::name ||
		name == GreaterThanEqual::name ||
		name == IntegerDivide::name ||
		name == LessThan::name ||
		name == LessThanEqual::name ||
		name == Minus::name ||
		name == Mod::name ||
		name == Multiply::name ||
		name == NotEquals::name ||
		name == Plus::name ||
		name == Range::name ||
		name == UnaryMinus::name) {
		for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
			addSecondaryOpAndMark(generate(*i, ids));
		}
	}

	else if(name == And::name && predicateContext_) {
		IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);
		for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
			PathResult ret = generate(*i, ids);
			intersectOp->addArg(ret.operation);
		}
		result.operation = intersectOp;
	}

	else if(name == Or::name && predicateContext_) {
		UnionQP *unionOp = new (&memMgr_) UnionQP(&memMgr_);
		for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
			PathResult ret = generate(*i, ids);
			unionOp->addArg(ret.operation);
		}
		result.operation = unionOp;
	}

	// These operators use the presence of the node arguments, not their value
	else if(name == NodeComparison::name ||
		name == OrderComparison::name ||
		name == And::name ||
		name == Or::name) {
		for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
			addSecondaryOp(generate(*i, ids));
		}
	}

	else {
		// Better to be safer than sorry
		for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
			generate(*i, ids).markRoot();
		}
	}

	generatePredicates(item, result, ids);
	return result;
}

static ImpliedSchemaNode::Type oppositeOperation(ImpliedSchemaNode::Type type)
{
	switch(type) {
	case ImpliedSchemaNode::LTX: {
		return ImpliedSchemaNode::GTX;
	}
	case ImpliedSchemaNode::LTE: {
		return ImpliedSchemaNode::GTE;
	}
	case ImpliedSchemaNode::GTX: {
		return ImpliedSchemaNode::LTX;
	}
	case ImpliedSchemaNode::GTE: {
		return ImpliedSchemaNode::LTE;
	}
	case ImpliedSchemaNode::EQUALS: {
		return ImpliedSchemaNode::EQUALS;
	}
	default: break;
	}
	return (ImpliedSchemaNode::Type)-1;
}

void QueryPlanGenerator::generateComparison(ImpliedSchemaNode::Type type, VectorOfDataItems &args, PathResult &result, VariableIDs *ids)
{
	IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);
	result.operation = intersectOp;

	DataItem *argA = args[0];
	DataItem *argB = args[1];

	VariableIDs argIDsA, argIDsB;
	PathResult retA = generate(argA, &argIDsA);
	intersectOp->addArg(retA.operation);
	PathResult retB = generate(argB, &argIDsB);
	intersectOp->addArg(retB.operation);
	if(ids) {
		VariableIDs::const_iterator i;
		for (i = argIDsA.begin(); i != argIDsA.end(); i++)
			ids->insert(*i);
		for (i = argIDsB.begin(); i != argIDsB.end(); i++)
			ids->insert(*i);
	}

	XPath2MemoryManager *mm = xpc_->getMemoryManager();
	Paths::iterator it;
	for(it = retA.returnPaths.begin(); it != retA.returnPaths.end(); ++it) {
		if(varsInScope(*it, argIDsB))
			result.join((*it)
				    ->appendChild(new (mm) ImpliedSchemaNode(type, argB, mm)));
		else (*it)->markSubtree();
	}

	intersectOp->addArg(new (&memMgr_) PathsQP(result.returnPaths, &memMgr_));

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths.clear();

	for(it = retB.returnPaths.begin(); it != retB.returnPaths.end(); ++it) {
		if(varsInScope(*it, argIDsA))
			result.join((*it)
				    ->appendChild(
					    new (mm) ImpliedSchemaNode(
						    oppositeOperation(type),
						    argA, mm)));
		else (*it)->markSubtree();
	}

	intersectOp->addArg(new (&memMgr_) PathsQP(result.returnPaths, &memMgr_));

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths.clear();
}

bool QueryPlanGenerator::varsInScope(ImpliedSchemaNode *node, const VariableIDs &argIDs) const {
	std::map<ImpliedSchemaNode*, VariableIDs>::const_iterator found =
		inScopeVars_.find(node->getRoot());
	if(found == inScopeVars_.end()) {
		return false;
	}

	const VariableIDs &rootIDs = found->second;
	for(VariableIDs::const_iterator i = argIDs.begin(); i != argIDs.end(); ++i) {
		if(rootIDs.find(*i) == rootIDs.end()) {
			return false;
		}
	}
	return true;
}

void QueryPlanGenerator::addSecondaryOp(const PathResult &result)
{
	unionOp_->addArg(result.operation);
}

void QueryPlanGenerator::addSecondaryOpAndMark(const PathResult &result)
{
	unionOp_->addArg(result.operation);
	result.markSubtree();
}

void QueryPlanGenerator::generateSubstring(ImpliedSchemaNode::Type type, VectorOfDataItems &args, PathResult &result, VariableIDs *ids)
{
	if(args.size() > 2) {
		// Mark the collation argument
		addSecondaryOpAndMark(generate(args[2], ids));
	}

	VariableIDs argIDs;
	PathResult ret = generate(args[0], &argIDs);
	generate(args[1], &argIDs).markRoot();

	XPath2MemoryManager *mm = xpc_->getMemoryManager();
	for(Paths::iterator it = ret.returnPaths.begin(); it != ret.returnPaths.end(); ++it) {
		result.join((*it)
			    ->appendChild(new (mm) ImpliedSchemaNode(type, args[1], mm)));
	}
	result.operation =
		new (&memMgr_) IntersectQP(ret.operation, new (&memMgr_) PathsQP(result.returnPaths, &memMgr_), &memMgr_);
	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths.clear();
	if(ids)
		for(VariableIDs::const_iterator i = argIDs.begin(); i != argIDs.end(); ++i)
			ids->insert(*i);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateParenthesizedExpr(DataItemParenthesizedExpr *item, VariableIDs *ids)
{
	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getChildren());
	PathResult result;
	UnionQP *unionOp = new (&memMgr_) UnionQP(&memMgr_);
	for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
		PathResult ret = generate(*i, ids);
		result.join(ret);
		unionOp->addArg(ret.operation);
	}
	result.operation = unionOp;

	generatePredicates(item,result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateVariable(DataItemVariable *item, VariableIDs *ids)
{
	VarStoreRef *ref = varStore_.getVar(item->getURI(), item->getName());

	PathResult result;
	if(ref != 0) {
		result = ref->getValue();
		if(ids) ids->insert(ref->getValue().id);
	}

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateIf(DataItemIf *item, VariableIDs *ids)
{
	// if(test_op) then op1 else op2
	//
	// We could use the formula u(n(test_op, op1), n(not(test_op), op2)) but the
	// not() may well not be worth it.
	//
	// Instead we use u(n(test_op, op1), op2) as an approxamation.

	PathResult result;
	UnionQP *unionOp = new (&memMgr_) UnionQP(&memMgr_);

	PathResult test = generate(const_cast<DataItem *>(item->getTest()), ids);

	PathResult ret = generate(const_cast<DataItem *>(item->getWhenTrue()), ids);
	result.join(ret);
	unionOp->addArg(new (&memMgr_) IntersectQP(test.operation, ret.operation, &memMgr_));

	ret = generate(const_cast<DataItem *>(item->getWhenFalse()), ids);
	result.join(ret);
	unionOp->addArg(ret.operation);

	result.operation = unionOp;

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateInstanceOf(DataItemInstanceOf *item, VariableIDs *ids)
{
	PathResult result;
	addSecondaryOpAndMark(generate(const_cast<DataItem *>(item->getExpression()), ids));

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateCastableAs(DataItemCastableAs *item, VariableIDs *ids)
{
	PathResult result;
	addSecondaryOpAndMark(generate(const_cast<DataItem *>(item->getExpression()), ids));

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateFLWOR(XQFLWOR *item, VariableIDs *ids)
{
	PathResult result;

	// bindings
	VariableIDs myVars;
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	for(VectorOfVariableBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
		PathResult r = generate((*i)->_allValues, ids);

		if((*i)->_variable || (*i)->_positionalVariable) {
			varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
		}
		if((*i)->_variable) {
			myVars.insert(setVariable((*i)->_vURI, (*i)->_vName, r));
		}
		if((*i)->_positionalVariable) {
			myVars.insert(setVariable((*i)->_pURI, (*i)->_pName, PathResult()));
		}

		if((*i)->_where) {
			bool oldPredicateContext = predicateContext_;
			predicateContext_ = true;

			PathResult ret = generate((*i)->_where, ids);
			r.operation = new (&memMgr_) IntersectQP(r.operation, ret.operation, &memMgr_);

			VarStoreRef *ref = varStore_.getVar((*i)->_vURI, (*i)->_vName);
			const_cast<VarValue&>(ref->getValue()).operation = r.operation;

			predicateContext_ = oldPredicateContext;
		}
		addSecondaryOp(r);
	}

	// sort specs
	const XQSort *sort = item->getSort();
	if(sort) {
		XQSort::VectorOfSortSpec *specs = const_cast<XQSort::VectorOfSortSpec *>(sort->getSortSpecs());
		for(XQSort::VectorOfSortSpec::iterator j = specs->begin(); j != specs->end(); ++j) {
			addSecondaryOpAndMark(generate(const_cast<DataItem *>((*j)->getExpression()), ids));
			// Don't add sort expressions to the operation, as they don't have to exist
		}
	}

	// where and return
	PathResult returnpr = generate(const_cast<DataItem *>(item->getReturnExpr()), ids);
	result.join(returnpr);

	if(item->getWhereExpr()) {
		bool oldPredicateContext = predicateContext_;
		predicateContext_ = true;

		PathResult ret = generate(const_cast<DataItem *>(item->getWhereExpr()), ids);
		returnpr.operation = new (&memMgr_) IntersectQP(returnpr.operation, ret.operation, &memMgr_);

		predicateContext_ = oldPredicateContext;
	}

	// undo binding's scopes
	for(VectorOfVariableBinding::iterator i2 = bindings->begin(); i2 != bindings->end(); ++i2) {
		if((*i2)->_variable || (*i2)->_positionalVariable) {
			varStore_.popScope()->release();
		}
	}
	if(ids) for(VariableIDs::iterator it3 = myVars.begin(); it3 != myVars.end(); ++it3)
		ids->erase(*it3);

	result.operation = returnpr.operation;

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateFLWORQuantified(XQQuantified *item, VariableIDs *ids)
{
	PathResult result;

	// bindings
	VariableIDs myVars;
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	for(VectorOfVariableBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
		PathResult r = generate((*i)->_allValues, ids);

		if((*i)->_variable) {
			varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
			myVars.insert(setVariable((*i)->_vURI, (*i)->_vName, r));
		}
	}

	// no where or sort specs

	// return
	PathResult ret = generate(const_cast<DataItem *>(item->getReturnExpr()), ids);
	if(predicateContext_) {
		result.operation = ret.operation;
	}

	// undo binding's scopes
	for(VectorOfVariableBinding::iterator i2 = bindings->begin(); i2 != bindings->end(); ++i2) {
		if((*i2)->_variable) {
			varStore_.popScope()->release();
		}
	}
	if(ids) for(VariableIDs::iterator it3 = myVars.begin(); it3 != myVars.end(); ++it3)
		ids->erase(*it3);

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateTypeswitch(XQTypeswitch *item, VariableIDs *ids)
{
	PathResult result;
	UnionQP *unionOp = new (&memMgr_) UnionQP(&memMgr_);

	PathResult expr = generate(const_cast<DataItem *>(item->getExpression()), ids);

	unsigned int varId = 0;
	XQTypeswitch::VectorOfClause *clauses = const_cast<XQTypeswitch::VectorOfClause *>(item->getClauses());
	for(XQTypeswitch::VectorOfClause::iterator i = clauses->begin(); i != clauses->end(); ++i) {

		if((*i)->_variable) {
			varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
			varId = setVariable((*i)->_uri, (*i)->_name, expr);
		}

		PathResult ret = generate((*i)->_expr, ids);
		result.join(ret);
		unionOp->addArg(ret.operation);

		if((*i)->_variable) {
			varStore_.popScope()->release();
			if(ids) ids->erase(varId);
		}
	}

	XQTypeswitch::Clause *def = const_cast<XQTypeswitch::Clause *>(item->getDefaultClause());
	if(def->_variable) {
		varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
		varId = setVariable(def->_uri, def->_name, expr);
	}

	PathResult ret = generate(def->_expr, ids);
	result.join(ret);
	unionOp->addArg(ret.operation);

	if(def->_variable) {
		varStore_.popScope()->release();
		if(ids) ids->erase(varId);
	}

	result.operation = unionOp;

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateGlobalVar(XQGlobalVariable *item, VariableIDs *ids)
{
	PathResult result;
	if(item->getVariableExpr()) {
		result = generate(const_cast<DataItem *>(item->getVariableExpr()), ids);
		varStore_.setGlobalVar(item->getVariableURI(), item->getVariableLocalName(),
				       VarValue(result, varId_++));
		result.returnPaths.clear();
		result.operation = 0;
	}
	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDOMConstructor(XQDOMConstructor *item, VariableIDs *ids)
{
	PathResult result;

	if(item->getName()) {
		addSecondaryOpAndMark(generate(const_cast<DataItem *>(item->getName()), ids));
	}
	VectorOfDataItems *attrs = const_cast<VectorOfDataItems *>(item->getAttributes());
	if(attrs) {
		for(VectorOfDataItems::iterator i = attrs->begin(); i != attrs->end(); ++i) {
			addSecondaryOpAndMark(generate(*i, ids));
		}
	}

	VectorOfDataItems *children = const_cast<VectorOfDataItems *>(item->getChildren());
	for(VectorOfDataItems::iterator j = children->begin(); j != children->end(); ++j) {
		addSecondaryOpAndMark(generate(*j, ids));
	}

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateContextItem(DataItemContextItem *item, VariableIDs *ids)
{
	const VarValue &currentContext = getCurrentContext();
	PathResult result = currentContext;
	if(ids) ids->insert(currentContext.id);

	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateLiteral(DataItemLiteral *item, VariableIDs *ids)
{
	PathResult result;
	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateSequence(DataItemSequence *item, VariableIDs *ids)
{
	PathResult result;
	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateCastAs(DataItemCastAs *item, VariableIDs *ids)
{
	PathResult result = generate(const_cast<DataItem *>(item->getExpression()), ids);
	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateTreatAs(DataItemTreatAs *item, VariableIDs *ids)
{
	PathResult result = generate(const_cast<DataItem *>(item->getExpression()), ids);
	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDebugHook(XQDebugHook *item, VariableIDs *ids)
{
	PathResult result = generate(item->m_impl, ids);
	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateOrderingChange(XQOrderingChange *item, VariableIDs *ids)
{
	PathResult result = generate(item->getExpr(), ids);
	generatePredicates(item, result, ids);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateValidate(XQValidate *item, VariableIDs *ids)
{
	PathResult result = generate(const_cast<DataItem *>(item->getExpression()), ids);
	generatePredicates(item, result, ids);
	return result;
}

void QueryPlanGenerator::generatePredicates(DataItemImpl *item, PathResult &result, VariableIDs *ids)
{
	DataItem::Predicates &preds = const_cast<DataItem::Predicates &>(item->getPredicates());

	if(!preds.empty()) {

		IntersectQP *predOp = new (&memMgr_) IntersectQP(&memMgr_);
		predOp->addArg(result.operation);

		bool oldPredicateContext = predicateContext_;
		predicateContext_ = true;
		varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
		unsigned int varId = setCurrentContext(result);

		for(DataItem::Predicates::iterator i = preds.begin(); i != preds.end(); ++i) {
			PathResult ret = generate(i->pred, ids);
			predOp->addArg(ret.operation);
		}

		varStore_.popScope()->release();
		if(ids) ids->erase(varId);
		predicateContext_ = oldPredicateContext;

		result.operation = predOp;
	}
}

////////////////////////////////////////////////////////////////////////////////////////

void QueryPlanGenerator::PathResult::join(const PathResult &o)
{
	returnPaths.insert(returnPaths.begin(), o.returnPaths.begin(), o.returnPaths.end());
}

void QueryPlanGenerator::PathResult::join(ImpliedSchemaNode *o)
{
	returnPaths.push_back(o);
}

void QueryPlanGenerator::PathResult::markSubtree() const
{
	// Mark the paths as needing the subtree
	for(Paths::const_iterator it = returnPaths.begin(); it != returnPaths.end(); ++it) {
		(*it)->markSubtree();
	}
}

void QueryPlanGenerator::PathResult::markRoot() const
{
	// Mark the paths as needing the entire tree
	for(Paths::const_iterator it = returnPaths.begin(); it != returnPaths.end(); ++it) {
		(*it)->getRoot()->markSubtree();
	}
}
