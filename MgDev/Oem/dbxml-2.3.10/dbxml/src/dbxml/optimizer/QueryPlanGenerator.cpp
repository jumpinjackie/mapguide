//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanGenerator.cpp,v 1.66.2.2 2007/01/16 18:48:41 jsnelson Exp $
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
#include "../dataItem/DbXmlStep.hpp"
#include "../dataItem/LookupIndexFunction.hpp"
#include "../UTF8.hpp"

#include <xqilla/context/DynamicContext.hpp>

#include <xqilla/operators/And.hpp>
#include <xqilla/operators/Or.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/Plus.hpp>
#include <xqilla/operators/Minus.hpp>
#include <xqilla/operators/Multiply.hpp>
#include <xqilla/operators/Divide.hpp>
#include <xqilla/operators/IntegerDivide.hpp>
#include <xqilla/operators/Mod.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/operators/UnaryMinus.hpp>
#include <xqilla/operators/NodeComparison.hpp>
#include <xqilla/operators/OrderComparison.hpp>
#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/Range.hpp>
#include <xqilla/operators/Intersect.hpp>
#include <xqilla/operators/Except.hpp>
#include <xqilla/operators/Union.hpp>

#include <xqilla/functions/FunctionAbs.hpp>
#include <xqilla/functions/FunctionAvg.hpp>
#include <xqilla/functions/FunctionBaseURI.hpp>
#include <xqilla/functions/FunctionBoolean.hpp>
#include <xqilla/functions/FunctionCeiling.hpp>
#include <xqilla/functions/FunctionCollection.hpp>
#include <xqilla/functions/FunctionCompare.hpp>
#include <xqilla/functions/FunctionConcat.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/functions/FunctionContains.hpp>
#include <xqilla/functions/FunctionCount.hpp>
#include <xqilla/functions/FunctionCurrentDate.hpp>
#include <xqilla/functions/FunctionCurrentDateTime.hpp>
#include <xqilla/functions/FunctionCurrentTime.hpp>
#include <xqilla/functions/FunctionData.hpp>
#include <xqilla/functions/FunctionDeepEqual.hpp>
#include <xqilla/functions/FunctionDefaultCollation.hpp>
#include <xqilla/functions/FunctionDistinctValues.hpp>
#include <xqilla/functions/FunctionDoc.hpp>
#include <xqilla/functions/FunctionDocumentURI.hpp>
#include <xqilla/functions/FunctionEmpty.hpp>
#include <xqilla/functions/FunctionEndsWith.hpp>
#include <xqilla/functions/FunctionError.hpp>
#include <xqilla/functions/FunctionExactlyOne.hpp>
#include <xqilla/functions/FunctionExists.hpp>
#include <xqilla/functions/FunctionFalse.hpp>
#include <xqilla/functions/FunctionFloor.hpp>
#include <xqilla/functions/FunctionInScopePrefixes.hpp>
#include <xqilla/functions/FunctionLocalNameFromQName.hpp>
#include <xqilla/functions/FunctionNamespaceURIFromQName.hpp>
#include <xqilla/functions/FunctionNamespaceURIForPrefix.hpp>
#include <xqilla/functions/FunctionNilled.hpp>
#include <xqilla/functions/FunctionId.hpp>
#include <xqilla/functions/FunctionIdref.hpp>
#include <xqilla/functions/FunctionInsertBefore.hpp>
#include <xqilla/functions/FunctionImplicitTimezone.hpp>
#include <xqilla/functions/FunctionIndexOf.hpp>
#include <xqilla/functions/FunctionLang.hpp>
#include <xqilla/functions/FunctionLast.hpp>
#include <xqilla/functions/FunctionLocalname.hpp>
#include <xqilla/functions/FunctionLowerCase.hpp>
#include <xqilla/functions/FunctionMatches.hpp>
#include <xqilla/functions/FunctionMax.hpp>
#include <xqilla/functions/FunctionMin.hpp>
#include <xqilla/functions/FunctionName.hpp>
#include <xqilla/functions/FunctionNamespaceUri.hpp>
#include <xqilla/functions/FunctionNodeName.hpp>
#include <xqilla/functions/FunctionNormalizeSpace.hpp>
#include <xqilla/functions/FunctionNormalizeUnicode.hpp>
#include <xqilla/functions/FunctionNot.hpp>
#include <xqilla/functions/FunctionNumber.hpp>
#include <xqilla/functions/FunctionOneOrMore.hpp>
#include <xqilla/functions/FunctionPosition.hpp>
#include <xqilla/functions/FunctionRemove.hpp>
#include <xqilla/functions/FunctionReplace.hpp>
#include <xqilla/functions/FunctionResolveQName.hpp>
#include <xqilla/functions/FunctionResolveURI.hpp>
#include <xqilla/functions/FunctionReverse.hpp>
#include <xqilla/functions/FunctionRoot.hpp>
#include <xqilla/functions/FunctionRound.hpp>
#include <xqilla/functions/FunctionRoundHalfToEven.hpp>
#include <xqilla/functions/FunctionStartsWith.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/functions/FunctionStringJoin.hpp>
#include <xqilla/functions/FunctionStringLength.hpp>
#include <xqilla/functions/FunctionStringToCodepoints.hpp>
#include <xqilla/functions/FunctionCodepointsToString.hpp>
#include <xqilla/functions/FunctionSubsequence.hpp>
#include <xqilla/functions/FunctionSubstring.hpp>
#include <xqilla/functions/FunctionSubstringAfter.hpp>
#include <xqilla/functions/FunctionSubstringBefore.hpp>
#include <xqilla/functions/FunctionSum.hpp>
#include <xqilla/functions/FunctionTokenize.hpp>
#include <xqilla/functions/FunctionTrace.hpp>
#include <xqilla/functions/FunctionTranslate.hpp>
#include <xqilla/functions/FunctionTrue.hpp>
#include <xqilla/functions/FunctionUnordered.hpp>
#include <xqilla/functions/FunctionUpperCase.hpp>
#include <xqilla/functions/FunctionZeroOrOne.hpp>
#include <xqilla/functions/FunctionYearsFromDuration.hpp>
#include <xqilla/functions/FunctionMonthsFromDuration.hpp>
#include <xqilla/functions/FunctionDaysFromDuration.hpp>
#include <xqilla/functions/FunctionHoursFromDuration.hpp>
#include <xqilla/functions/FunctionMinutesFromDuration.hpp>
#include <xqilla/functions/FunctionSecondsFromDuration.hpp>
#include <xqilla/functions/FunctionYearFromDateTime.hpp>
#include <xqilla/functions/FunctionMonthFromDateTime.hpp>
#include <xqilla/functions/FunctionDayFromDateTime.hpp>
#include <xqilla/functions/FunctionHoursFromDateTime.hpp>
#include <xqilla/functions/FunctionMinutesFromDateTime.hpp>
#include <xqilla/functions/FunctionSecondsFromDateTime.hpp>
#include <xqilla/functions/FunctionTimezoneFromDateTime.hpp>
#include <xqilla/functions/FunctionYearFromDate.hpp>
#include <xqilla/functions/FunctionMonthFromDate.hpp>
#include <xqilla/functions/FunctionDayFromDate.hpp>
#include <xqilla/functions/FunctionTimezoneFromDate.hpp>
#include <xqilla/functions/FunctionHoursFromTime.hpp>
#include <xqilla/functions/FunctionMinutesFromTime.hpp>
#include <xqilla/functions/FunctionSecondsFromTime.hpp>
#include <xqilla/functions/FunctionTimezoneFromTime.hpp>
#include <xqilla/functions/FunctionAdjustDateTimeToTimezone.hpp>
#include <xqilla/functions/FunctionAdjustDateToTimezone.hpp>
#include <xqilla/functions/FunctionAdjustTimeToTimezone.hpp>

#include <xqilla/axis/NodeTest.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

static const XMLCh XMLChDot[] = { chColon, chColon, chLatin_d, chLatin_o, chLatin_t, chNull };

QueryPlanGenerator::QueryPlanGenerator(DbXmlContext *xpc, Optimizer *parent)
	: Optimizer(parent),
	  xpc_(xpc),
	  unionOp_(new (&memMgr_) UnionQP(&memMgr_)),
	  varStore_(&memMgr_),
	  varId_(0)
{
	varStore_.setGlobalVar(XMLChDot, XMLChDot, varId_++);
}

void QueryPlanGenerator::resetInternal()
{
	unionOp_ = new (&memMgr_) UnionQP(&memMgr_),
	varStore_.clear();
	varId_ = 0;
	varStore_.setGlobalVar(XMLChDot, XMLChDot, varId_++);
}

void QueryPlanGenerator::optimize(XQQuery *query)
{
	varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);

	// Generate the imported module's global variables
	ImportedModules &modules = const_cast<ImportedModules&>(query->getImportedModules());
	for(ImportedModules::iterator it2 = modules.begin(); it2 != modules.end(); ++it2) {
		if ((*it2)->getVariables().size() != 0) {
			for(GlobalVariables::const_iterator it = (*it2)->getVariables().begin();
			    it != (*it2)->getVariables().end(); ++it) {
				PathResult ret = generateGlobalVar(*it, 0);
				setVariable((*it)->getVariableURI(),
					(*it)->getVariableLocalName(), ret);
				addSecondaryOp(ret);
			}
		}
	}

	if (query->getVariables().size() != 0) {
		for(GlobalVariables::const_iterator it = query->getVariables().begin();
		    it != query->getVariables().end(); ++it) {
			PathResult ret = generateGlobalVar(*it, 0);
			setVariable((*it)->getVariableURI(),
				    (*it)->getVariableLocalName(), ret);
			addSecondaryOp(ret);
		}
	}

	PathResult result = generate(query->getQueryBody(), 0);
	addSecondaryOpAndMark(result);

	setQueryPlans();

	delete varStore_.popScope();
}

ASTNode *QueryPlanGenerator::optimize(ASTNode *item)
{
	PathResult result = generate(item, 0);
	addSecondaryOpAndMark(result);

	setQueryPlans();

	return item;
}

void QueryPlanGenerator::setQueryPlans()
{
	QueryPlan *qp = unionOp_->compress();
	if(qp == 0) {
		qp = new (&memMgr_) UniverseQP(&memMgr_);
	}

	for(vector<QueryPlanFunction*>::iterator it = qpfs_.begin(); it != qpfs_.end(); ++it) {
		(*it)->setQueryPlan(qp->copy(xpc_->getMemoryManager())
			->rootFilter((*it)->getImpliedSchema()));
	}
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generate(ASTNode *item, VariableIDs *ids)
{
	PathResult result;
	switch(item->getType()) {
	case ASTNode::LITERAL: {
		result = generateLiteral((XQLiteral *)item, ids);
		break;
	}
	case ASTNode::SEQUENCE: {
		result = generateSequence((XQSequence *)item, ids);
		break;
	}
	case ASTNode::FUNCTION: {
		result = generateFunction((XQFunction *)item, ids);
		break;
	}
	case ASTNode::NAVIGATION: {
		result = generateNav((XQNav *)item, ids);
		break;
	}
	case ASTNode::VARIABLE: {
		result = generateVariable((XQVariable *)item, ids);
		break;
	}
	case ASTNode::STEP: {
		result = generateStep((XQStep *)item, ids);
		break;
	}
	case ASTNode::IF: {
		result = generateIf((XQIf *)item, ids);
		break;
	}
	case ASTNode::INSTANCE_OF: {
		result = generateInstanceOf((XQInstanceOf *)item, ids);
		break;
	}
	case ASTNode::CASTABLE_AS: {
		result = generateCastableAs((XQCastableAs *)item, ids);
		break;
	}
	case ASTNode::CAST_AS: {
		result = generateCastAs((XQCastAs *)item, ids);
		break;
	}
	case ASTNode::TREAT_AS: {
		result = generateTreatAs((XQTreatAs *)item, ids);
		break;
	}
	case ASTNode::PARENTHESIZED: {
		result = generateParenthesizedExpr((XQParenthesizedExpr *)item, ids);
		break;
	}
	case ASTNode::OPERATOR: {
		result = generateOperator((XQOperator *)item, ids);
		break;
	}
	case ASTNode::CONTEXT_ITEM: {
		result = generateContextItem((XQContextItem *)item, ids);
		break;
	}
	case ASTNode::DEBUG_HOOK: {
		result = generateDebugHook((XQDebugHook *)item, ids);
		break;
	}
	case ASTNode::FLWOR: {
		result = generateFLWOR((XQFLWOR *)item, ids);
		break;
	}
	case ASTNode::FLWOR_QUANTIFIED: {
		result = generateFLWORQuantified((XQQuantified *)item, ids);
		break;
	}
	case ASTNode::TYPESWITCH: {
		result = generateTypeswitch((XQTypeswitch *)item, ids);
		break;
	}
	case ASTNode::VALIDATE: {
		result = generateValidate((XQValidate *)item, ids);
		break;
	}
	case ASTNode::DOM_CONSTRUCTOR: {
		result = generateDOMConstructor((XQDOMConstructor *)item, ids);
		break;
	}
	case ASTNode::ORDERING_CHANGE: {
		result = generateOrderingChange((XQOrderingChange *)item, ids);
		break;
	}
	case ASTNode::ATOMIZE: {
		result = generateAtomize((XQAtomize *)item, ids);
		break;
	}
	case ASTNode::PROMOTE_UNTYPED: {
		result = generatePromoteUntyped((XQPromoteUntyped *)item, ids);
		break;
	}
	case ASTNode::PROMOTE_NUMERIC: {
		result = generatePromoteNumeric((XQPromoteNumeric *)item, ids);
		break;
	}
	case ASTNode::PROMOTE_ANY_URI: {
		result = generatePromoteAnyURI((XQPromoteAnyURI *)item, ids);
		break;
	}
	case ASTNode::DOCUMENT_ORDER: {
		result = generateDocumentOrder((XQDocumentOrder *)item, ids);
		break;
	}
	case ASTNode::PREDICATE: {
		result = generatePredicate((XQPredicate *)item, ids);
		break;
	}
	case ASTNode::USER_FUNCTION: {
		result = generateUserFunction((XQUserFunction::XQFunctionEvaluator *)item, ids);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE): {
		result = generateDbXmlCompare((DbXmlCompare*)item, ids);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_FILTER): {
		result = generateDbXmlFilter((DbXmlFilter*)item, ids);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS): {
		result = generateDbXmlContains((DbXmlContains*)item, ids);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::LOOKUP_INDEX): {
		result = generateLookupIndex((LookupIndex*)item, ids);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::QUERY_PLAN_FUNCTION): {
		result = generateQueryPlanFunction((QueryPlanFunction*)item, ids);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_STEP): {
		result = generateDbXmlStep((DbXmlStep*)item, ids);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_NAV): {
		result = generateDbXmlNav((DbXmlNav *)item, ids);
		break;
	}
	default: break;
	}
	return result;
}

static ImpliedSchemaNode::Type getTypeFromJoin(Join::Type join)
{
	ImpliedSchemaNode::Type result = (ImpliedSchemaNode::Type)-1;
	switch(join) {
	case Join::CHILD: {
		result = ImpliedSchemaNode::CHILD;
		break;
	}
	case Join::ATTRIBUTE: {
		result = ImpliedSchemaNode::ATTRIBUTE;
		break;
	}
	case Join::ATTRIBUTE_OR_CHILD: {
		result = ImpliedSchemaNode::CHILD;
		break;
	}
	case Join::DESCENDANT: {
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

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDbXmlStep(DbXmlStep *item, VariableIDs *ids)
{
	return generateStep(item->getJoinType(), item->getNodeTest(), item, ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateStep(XQStep *item, VariableIDs *ids)
{
	return generateStep((Join::Type)item->getAxis(), item->getNodeTest(), 0, ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateStep(Join::Type join, const NodeTest *step,
	QueryPlanHolder *qph, VariableIDs *ids)
{
	ImpliedSchemaNode::Type type = getTypeFromJoin(join);
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

		switch(join) {
		case Join::PARENT_A:
		case Join::PARENT_C:
		case Join::PARENT: {
			generateParentStep(*it, dummy, result);
			break;
		}
		case Join::DESCENDANT_OR_SELF: {
			generateDescendantOrSelfStep(*it, dummy, result);
			break;
		}
		case Join::SELF: {
			generateSelfStep(*it, dummy, result);
			break;
		}
		case Join::ANCESTOR: {
			generateAncestorStep(*it, dummy, result);
			break;
		}
		case Join::ANCESTOR_OR_SELF: {
			generateAncestorOrSelfStep(*it, dummy, result);
			break;
		}
		case Join::FOLLOWING: {
			generateFollowingStep(*it, dummy, result);
			break;
		}
		case Join::PRECEDING: {
			generatePrecedingStep(*it, dummy, result);
			break;
		}
		case Join::PRECEDING_SIBLING:
		case Join::FOLLOWING_SIBLING: {
			generateSiblingStep(*it, dummy, result);
			break;
		}
		case Join::ATTRIBUTE_OR_CHILD:
		case Join::ATTRIBUTE:
		case Join::CHILD:
		case Join::DESCENDANT: {
			generateBuiltInStep(*it, dummy, result);
			break;
		}
		case Join::NAMESPACE:
		default: {
			// Do nothing
			break;
		}
		}
	}

	PathsQP *pathsqp = new (&memMgr_) PathsQP(result.returnPaths, &memMgr_);
	result.operation = new (&memMgr_) IntersectQP(currentContext.operation, pathsqp, &memMgr_);

	if(qph != 0) qph->addToQueryPlan(pathsqp);

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

QueryPlanGenerator::PathResult QueryPlanGenerator::generateNav(XQNav *item, VariableIDs *ids)
{
	PathResult result = getCurrentContext();

	bool setContext = false;
	unsigned int varId = 0;
	XQNav::Steps &args = const_cast<XQNav::Steps &>(item->getSteps());
	for(XQNav::Steps::iterator i = args.begin(); i != args.end(); ++i) {
		if(setContext) {
			varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
			varId = setCurrentContext(result);
		}
		result.returnPaths.clear();

		PathResult ret = generate(i->step, ids);
		result.join(ret);
		result.operation = ret.operation;

		if(setContext) {
			delete varStore_.popScope();
			if(ids) ids->erase(varId);
		}
		setContext = true;
	}

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDbXmlNav(DbXmlNav *item, VariableIDs *ids)
{
	PathResult result = getCurrentContext();

	bool setContext = false;
	unsigned int varId = 0;
	DbXmlNav::Steps &args = const_cast<DbXmlNav::Steps &>(item->getSteps());
	for(DbXmlNav::Steps::iterator i = args.begin(); i != args.end(); ++i) {
		if(setContext) {
			varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
			varId = setCurrentContext(result);
		}
		result.returnPaths.clear();

		PathResult ret = generate(*i, ids);
		result.join(ret);
		result.operation = ret.operation;

		if(setContext) {
			delete varStore_.popScope();
			if(ids) ids->erase(varId);
		}
		setContext = true;
	}

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateQueryPlanFunction(QueryPlanFunction *item, VariableIDs *ids)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	PathResult result;

	if(item->getArgument() != 0) {
		PathResult ret = generate(const_cast<ASTNode*>(item->getArgument()), ids);
		addSecondaryOpAndMark(ret);
	}

	ImpliedSchemaNode *root = const_cast<ImpliedSchemaNode*>(item->getImpliedSchema());
	if(root == 0) {
		qpfs_.push_back(item);

		root = new (mm) ImpliedSchemaNode(ImpliedSchemaNode::ROOT, mm);
		item->setImpliedSchema(root);
	}
	storeInScopeVars(root);

	result.join(root);
	result.operation = new (&memMgr_) PathsQP(result.returnPaths, &memMgr_);
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateFunction(XQFunction *item, VariableIDs *ids)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	const XMLCh *uri = item->getFunctionURI();
	const XMLCh *name = item->getFunctionName();

	PathResult result;

	if(uri == XQFunction::XMLChFunctionURI) {

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
			if(!args.empty()) {
				PathResult ret = generate(args[0], ids);
				addSecondaryOpAndMark(ret);
			}

			ImpliedSchemaNode *root = new (xpc_->getMemoryManager()) ImpliedSchemaNode(ImpliedSchemaNode::ROOT, xpc_->getMemoryManager());
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
			result = generate(args[0], ids);
			addSecondaryOpAndMark(generate(args[1], ids));
		}
		else if(name == FunctionReverse::name ||
			name == FunctionUnordered::name) {
			result = generate(args[0], ids);
		}
		else if(name == FunctionSubsequence::name) {
			result = generate(args[0], ids);
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
			PathResult nodes;
			if(args.size() == 1) {
				const VarValue &currentContext = getCurrentContext();
				if(ids) ids->insert(currentContext.id);
				nodes = currentContext;
			}
			else {
				nodes = generate(args[1], ids);
				addSecondaryOpAndMark(nodes);
			}

			// We don't handle these, so we mark the trees as unoptimisable
			for(Paths::const_iterator i = nodes.returnPaths.begin(); i != nodes.returnPaths.end(); ++i) {
				(*i)->getRoot()->markSubtree();
			}

			addSecondaryOpAndMark(generate(args[0], ids));
		}
		else if(name == FunctionLang::name) {
			if(args.size() == 1) {
				const VarValue &currentContext = getCurrentContext();
				addSecondaryOpAndMark(currentContext);
				if(ids) ids->insert(currentContext.id);
				addSecondaryOpAndMark(generate(args[0], ids));
			}
			else {
				addSecondaryOpAndMark(generate(args[0], ids));
				addSecondaryOpAndMark(generate(args[1], ids));
			}
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
			result.operation = ret.operation;
		}

		// These use the presence of a node, not it's value (and hence sub tree)
		else if(name == FunctionNot::name ||
			name == FunctionName::name ||
			name == FunctionLocalname::name ||
			name == FunctionNamespaceUri::name ||
			name == FunctionEmpty::name ||
			name == FunctionCount::name ||
			name == FunctionNilled::name) {
			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
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
		else if(name == FunctionStartsWith::name) {
			generateSubstring(ImpliedSchemaNode::PREFIX, args, result, ids, 0);
		}
		else if(name == FunctionEndsWith::name) {
			generateSubstring(ImpliedSchemaNode::SUBSTRING, args, result, ids, 0);
		}
		else if(name == FunctionContains::name) {
			DbXmlFunContains *db = (DbXmlFunContains*)item;
			generateSubstring(ImpliedSchemaNode::SUBSTRING, args, result, ids,
				&db->getArg0QPH());
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
			addSecondaryOpAndMark(generate(args[0], ids));
		}

		else {
			// It's in the xquery function namespace, so assume it uses values from the returned paths
			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
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
			if(mdFunc->getConstantQNameArg(1, uri, name, xpc_)) {

				for(Paths::const_iterator it = targets.returnPaths.begin();
				    it != targets.returnPaths.end(); ++it) {
					ImpliedSchemaNode *newNode = new (xpc_->getMemoryManager())
						ImpliedSchemaNode(uri, false, name, false, false,
								  ImpliedSchemaNode::METADATA, xpc_->getMemoryManager());
					result.join((*it)->appendChild(newNode));
				}

				result.operation = new (&memMgr_) IntersectQP(targets.operation, new (&memMgr_)
					PathsQP(result.returnPaths, &memMgr_), &memMgr_);
			}
		}

		else if(name == LookupIndexFunction::name) {
			LookupIndexFunction *fun = (LookupIndexFunction*)item;

			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
				addSecondaryOpAndMark(generate(*i, ids));
			}

			generateLookup(fun, ImpliedSchemaNode::CHILD, fun->getChildURIName(),
				fun->getParentURIName(), result);
		}

		else if(name == LookupAttributeIndexFunction::name) {
			LookupAttributeIndexFunction *fun = (LookupAttributeIndexFunction*)item;

			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
				addSecondaryOpAndMark(generate(*i, ids));
			}

			generateLookup(fun, ImpliedSchemaNode::ATTRIBUTE, fun->getChildURIName(),
				fun->getParentURIName(), result);
		}

		else if(name == LookupMetaDataIndexFunction::name) {
			LookupMetaDataIndexFunction *fun = (LookupMetaDataIndexFunction*)item;

			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
				addSecondaryOpAndMark(generate(*i, ids));
			}

			generateLookup(fun, ImpliedSchemaNode::METADATA, fun->getChildURIName(),
				fun->getParentURIName(), result);
		}
	}

	else {
		// We don't know about this function, so mark the entire tree as being needed
		for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
			PathResult ret = generate(*i, ids);
			ret.markRoot();
			addSecondaryOp(ret);
		}
	}

	return result;
}

void QueryPlanGenerator::storeInScopeVars(ImpliedSchemaNode *root) {
	// List the in scope vars
	VariableIDs &vars = inScopeVars_[root];
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
}

class ArgHolder {
public:
	ArgHolder(const XMLCh *u, const XMLCh *n, const QueryPlanGenerator::PathResult &v)
		: uri(u), name(n), value(v) {}
	
	const XMLCh *uri, *name;
	QueryPlanGenerator::PathResult value;
};

QueryPlanGenerator::PathResult QueryPlanGenerator::generateUserFunction(XQUserFunction::XQFunctionEvaluator *item, VariableIDs *ids)
{
	PathResult result;

	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	const XQUserFunction *functionDef = item->getFunctionDefinition();

	// Evaluate the arguments in the current scope, declare them in the function's scope
	vector<ArgHolder> evaluatedArgs;
	if(functionDef->getParams()) {
		XQUserFunction::VectorOfFunctionParameters::const_iterator binding = functionDef->getParams()->begin();
		for(VectorOfASTNodes::iterator arg = args.begin(); arg != args.end() && binding != functionDef->getParams()->end(); ++arg, ++binding) {

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
	pair<set<const ASTNode*>::iterator, bool> ins = userFunctionStack_.insert(functionDef->getFunctionBody());
	if(ins.second) {
		VariableIDs myVars;
		varStore_.addScope(VarStore::MyScope::LOCAL_SCOPE);

		for(vector<ArgHolder>::iterator it = evaluatedArgs.begin(); it != evaluatedArgs.end(); ++it) {
			myVars.insert(setVariable(it->uri, it->name, it->value));
		}

		PathResult ret = generate(const_cast<ASTNode *>(functionDef->getFunctionBody()), ids);
		result.join(ret);
		result.operation = ret.operation;

		delete varStore_.popScope();
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

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDbXmlFilter(DbXmlFilter *item, VariableIDs *ids)
{
	PathResult result;

	IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);

	const VarValue &currentContext = getCurrentContext();
	intersectOp->addArg(currentContext.operation);

	if(ids) ids->insert(currentContext.id);

	PathResult ret = generate(const_cast<ASTNode*>(item->getArgument()), ids);
	intersectOp->addArg(ret.operation);

	result.operation = intersectOp;
	result.returnPaths = currentContext.returnPaths;

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDbXmlContains(DbXmlContains *item, VariableIDs *ids)
{
	PathResult result;

	IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);
	result.operation = intersectOp;

	ASTNode *arg = const_cast<ASTNode *>(item->getArgument());

	VariableIDs argIDs;
	PathResult ret = generate(arg, &argIDs);
	intersectOp->addArg(ret.operation);
	ret.markSubtree();
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
		ImpliedSchemaNode *newChild = (*it)->appendChild(new (mm)
			ImpliedSchemaNode(ImpliedSchemaNode::SUBSTRING, /*generalComp*/false, Syntax::STRING, arg, mm));
		nlsPaths.push_back(newChild);

		if(varsInScope(*it, argIDs)) result.join(newChild);
	}

	intersectOp->addArg(new (&memMgr_) PathsQP(result.returnPaths, &memMgr_));

	PathsQP *pathsqp = new (&memMgr_) PathsQP(nlsPaths, &memMgr_);
	item->addToQueryPlan(pathsqp);

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths = currentContext.returnPaths;

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateLookupIndex(LookupIndex *item, VariableIDs *ids)
{
	PathResult result;
	if(item->getQueryPlan() != 0 && item->getQueryPlan()->getType() == QueryPlan::PRESENCE) {
		PresenceQP *pqp = (PresenceQP*)item->getQueryPlan();
		generateLookup(item, pqp->getNodeType(), pqp->getChildName(), pqp->getParentName(), result);
	}
	return result;
}

void QueryPlanGenerator::generateLookup(QueryPlanHolder *qph, ImpliedSchemaNode::Type type,
	const char *child, const char *parent, PathResult &result)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

        ImpliedSchemaNode *root = new (mm) ImpliedSchemaNode(ImpliedSchemaNode::ROOT, mm);
        root->setQueryPlanHolder(qph);

	if(type == ImpliedSchemaNode::METADATA) {
		result.join(root);
	} else {
		ImpliedSchemaNode *pisn = 0;
		if(parent == 0 || *parent == 0) {
			pisn = new (mm) ImpliedSchemaNode(0, true, 0, true, false,
				ImpliedSchemaNode::DESCENDANT, mm);
		} else {
			Name pname(parent);
			const char *puri = pname.hasURI() ? pname.getURI() : 0;
			pisn = new (mm) ImpliedSchemaNode(mm->getPooledString(puri), false,
				mm->getPooledString(pname.getName()), false, false,
				ImpliedSchemaNode::DESCENDANT, mm);
		}
		pisn = root->appendChild(pisn);

		ImpliedSchemaNode *cisn = 0;
		if(child == 0 || *child == 0) {
			cisn = new (mm) ImpliedSchemaNode(0, true, 0, true, false, type, mm);
		} else {
			Name cname(child);
			const char *curi = cname.hasURI() ? cname.getURI() : 0;
			cisn = new (mm) ImpliedSchemaNode(mm->getPooledString(curi), false,
				mm->getPooledString(cname.getName()), false, false, type, mm);
		}
		result.join(pisn->appendChild(cisn));
	}

	result.operation = new (&memMgr_) PathsQP(result.returnPaths, &memMgr_);
}

static StaticType generalCompTypeConversion(const StaticType &arg0_type, const StaticType &arg1_type)
{
	StaticType result = arg0_type;
	result.flags &= ~(StaticType::UNTYPED_ATOMIC_TYPE | StaticType::NODE_TYPE);
	if(arg0_type.containsType(StaticType::UNTYPED_ATOMIC_TYPE | StaticType::NODE_TYPE)) {
		if(arg1_type.containsType(StaticType::NUMERIC_TYPE)) {
			result.flags |= StaticType::DOUBLE_TYPE;
		}
		if(arg1_type.containsType(StaticType::UNTYPED_ATOMIC_TYPE | StaticType::NODE_TYPE)) {
			result.flags |= StaticType::STRING_TYPE;
		}
		if(arg1_type.containsType(~(StaticType::NUMERIC_TYPE | StaticType::UNTYPED_ATOMIC_TYPE))) {
			result.flags |= arg1_type.flags & ~(StaticType::NUMERIC_TYPE | StaticType::UNTYPED_ATOMIC_TYPE);
		}
	}
	return result;
}

static StaticType valueCompTypeConversion(const StaticType &arg0_type)
{
	StaticType result = arg0_type;
	result.flags &= ~(StaticType::UNTYPED_ATOMIC_TYPE | StaticType::NODE_TYPE);
	if(arg0_type.containsType(StaticType::UNTYPED_ATOMIC_TYPE | StaticType::NODE_TYPE)) {
		result.flags |= StaticType::STRING_TYPE;
	}
	return result;
}

static XmlValue::Type staticTypeToValueType(unsigned int sType)
{
	switch(sType) {
	case StaticType::NODE_TYPE: return XmlValue::NODE;
	case StaticType::ANY_SIMPLE_TYPE: return XmlValue::ANY_SIMPLE_TYPE;
	case StaticType::ANY_URI_TYPE: return XmlValue::ANY_URI;
	case StaticType::BASE_64_BINARY_TYPE: return XmlValue::BASE_64_BINARY;
	case StaticType::BOOLEAN_TYPE: return XmlValue::BOOLEAN;
	case StaticType::DATE_TYPE: return XmlValue::DATE;
	case StaticType::DATE_TIME_TYPE: return XmlValue::DATE_TIME;
	case StaticType::DAY_TIME_DURATION_TYPE: return XmlValue::DAY_TIME_DURATION;
	case StaticType::DECIMAL_TYPE: return XmlValue::DECIMAL;
	case StaticType::DOUBLE_TYPE: return XmlValue::DOUBLE;
	case StaticType::DURATION_TYPE: return XmlValue::DURATION;
	case StaticType::FLOAT_TYPE: return XmlValue::FLOAT;
	case StaticType::G_DAY_TYPE: return XmlValue::G_DAY;
	case StaticType::G_MONTH_TYPE: return XmlValue::G_MONTH;
	case StaticType::G_MONTH_DAY_TYPE: return XmlValue::G_MONTH_DAY;
	case StaticType::G_YEAR_TYPE: return XmlValue::G_YEAR;
	case StaticType::G_YEAR_MONTH_TYPE: return XmlValue::G_YEAR_MONTH;
	case StaticType::HEX_BINARY_TYPE: return XmlValue::HEX_BINARY;
	case StaticType::NOTATION_TYPE: return XmlValue::NOTATION;
	case StaticType::QNAME_TYPE: return XmlValue::QNAME;
	case StaticType::STRING_TYPE: return XmlValue::STRING;
	case StaticType::TIME_TYPE: return XmlValue::TIME;
	case StaticType::UNTYPED_ATOMIC_TYPE: return XmlValue::UNTYPED_ATOMIC;
	case StaticType::YEAR_MONTH_DURATION_TYPE: return XmlValue::YEAR_MONTH_DURATION;
	default: break;
	}
	return XmlValue::NONE;
}

static Syntax::Type getComparisonType(const StaticType &arg0_type, const StaticType &arg1_type)
{
	XmlValue::Type result = staticTypeToValueType(arg0_type.flags);
	if(result == XmlValue::NONE) 
		result = staticTypeToValueType(arg1_type.flags);
	return AtomicTypeValue::convertToSyntaxType(result);
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

	ASTNode *arg = const_cast<ASTNode *>(item->getArgument());

	VariableIDs argIDs;
	PathResult ret = generate(arg, &argIDs);
	intersectOp->addArg(ret.operation);
	ret.markSubtree();
	if(ids) {
		VariableIDs::const_iterator i;
		for(i = argIDs.begin(); i != argIDs.end(); i++)
			ids->insert(*i);
	}

	StaticType arg0_type;
	arg0_type.flags = StaticType::NODE_TYPE;
	StaticType arg1_type = arg->getStaticResolutionContext().getStaticType();

	Syntax::Type syntaxType;
	if(item->isGeneralComp()) {
		syntaxType = getComparisonType(generalCompTypeConversion(arg0_type, arg1_type),
			generalCompTypeConversion(arg1_type, arg0_type));
	}
	else {
		syntaxType = getComparisonType(valueCompTypeConversion(arg0_type),
			valueCompTypeConversion(arg1_type));
	}

	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	const VarValue &currentContext = getCurrentContext();
	intersectOp->addArg(currentContext.operation);
	if(ids) ids->insert(currentContext.id);

	Paths nlsPaths;
	for(Paths::const_iterator it = currentContext.returnPaths.begin(); it != currentContext.returnPaths.end(); ++it) {
		if(type != (ImpliedSchemaNode::Type)-1) {
			ImpliedSchemaNode *newChild = (*it)->appendChild(new (mm)
				ImpliedSchemaNode(type, item->isGeneralComp(), syntaxType, arg, mm));
			nlsPaths.push_back(newChild);

			if(varsInScope(*it, argIDs)) result.join(newChild);
		} else {
			(*it)->markSubtree();
		}
	}

	intersectOp->addArg(new (&memMgr_) PathsQP(result.returnPaths, &memMgr_));

	PathsQP *pathsqp = new (&memMgr_) PathsQP(nlsPaths, &memMgr_);
	item->addToQueryPlan(pathsqp);

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths = currentContext.returnPaths;

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateOperator(XQOperator *item, VariableIDs *ids)
{
	PathResult result;

	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
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
		addSecondaryOp(generate(args[1], ids));
	}

	else if(name == Equals::name) {
		DbXmlEquals *db = (DbXmlEquals*)item;
		generateComparison(ImpliedSchemaNode::EQUALS, /*generateComp*/false, args, result, ids,
			db->getArg0QPH(), db->getArg1QPH());
	}

	else if(name == NotEquals::name) {
		DbXmlNotEquals *db = (DbXmlNotEquals*)item;
		generateComparison((ImpliedSchemaNode::Type)-1, /*generateComp*/false, args, result, ids,
			db->getArg0QPH(), db->getArg1QPH());
	}

	else if(name == LessThan::name) {
		DbXmlLessThan *db = (DbXmlLessThan*)item;
		generateComparison(ImpliedSchemaNode::LTX, /*generateComp*/false, args, result, ids,
			db->getArg0QPH(), db->getArg1QPH());
	}

	else if(name == LessThanEqual::name) {
		DbXmlLessThanEqual *db = (DbXmlLessThanEqual*)item;
		generateComparison(ImpliedSchemaNode::LTE, /*generateComp*/false, args, result, ids,
			db->getArg0QPH(), db->getArg1QPH());
	}

	else if(name == GreaterThan::name) {
		DbXmlGreaterThan *db = (DbXmlGreaterThan*)item;
		generateComparison(ImpliedSchemaNode::GTX, /*generateComp*/false, args, result, ids,
			db->getArg0QPH(), db->getArg1QPH());
	}

	else if(name == GreaterThanEqual::name) {
		DbXmlGreaterThanEqual *db = (DbXmlGreaterThanEqual*)item;
		generateComparison(ImpliedSchemaNode::GTE, /*generateComp*/false, args, result, ids,
			db->getArg0QPH(), db->getArg1QPH());
	}

	else if(name == GeneralComp::name) {
		DbXmlGeneralComp *db = (DbXmlGeneralComp*)item;

		switch(((const GeneralComp *)item)->getOperation()) {
		case GeneralComp::EQUAL:
			generateComparison(ImpliedSchemaNode::EQUALS, /*generateComp*/true, args, result, ids,
				db->getArg0QPH(), db->getArg1QPH());
			break;
		case GeneralComp::NOT_EQUAL:
			generateComparison((ImpliedSchemaNode::Type)-1, /*generateComp*/true, args, result, ids,
				db->getArg0QPH(), db->getArg1QPH());
			break;
		case GeneralComp::LESS_THAN:
			generateComparison(ImpliedSchemaNode::LTX, /*generateComp*/true, args, result, ids,
				db->getArg0QPH(), db->getArg1QPH());
			break;
		case GeneralComp::LESS_THAN_EQUAL:
			generateComparison(ImpliedSchemaNode::LTE, /*generateComp*/true, args, result, ids,
				db->getArg0QPH(), db->getArg1QPH());
			break;
		case GeneralComp::GREATER_THAN:
			generateComparison(ImpliedSchemaNode::GTX, /*generateComp*/true, args, result, ids,
				db->getArg0QPH(), db->getArg1QPH());
			break;
		case GeneralComp::GREATER_THAN_EQUAL:
			generateComparison(ImpliedSchemaNode::GTE, /*generateComp*/true, args, result, ids,
				db->getArg0QPH(), db->getArg1QPH());
			break;
		}
	}

	// These operators use the value of their arguments
	else if(name == Divide::name ||
		name == IntegerDivide::name ||
		name == Minus::name ||
		name == Mod::name ||
		name == Multiply::name ||
		name == Plus::name ||
		name == Range::name ||
		name == UnaryMinus::name) {
		for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
			addSecondaryOpAndMark(generate(*i, ids));
		}
	}

	else if(name == And::name) {
		IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);
		for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
			PathResult ret = generate(*i, ids);
			intersectOp->addArg(ret.operation);
		}
		result.operation = intersectOp;
	}

	else if(name == Or::name) {
		UnionQP *unionOp = new (&memMgr_) UnionQP(&memMgr_);
		result.operation = unionOp;
		for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
			PathResult ret = generate(*i, ids);
			unionOp->addArg(ret.operation);
			if(ret.operation == 0) result.operation = 0;
		}
	}

	// These operators use the presence of the node arguments, not their value
	else if(name == NodeComparison::name ||
		name == OrderComparison::name) {
		for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
			addSecondaryOp(generate(*i, ids));
		}
	}

	else {
		// Better to be safer than sorry
		for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
			PathResult ret = generate(*i, ids);
			ret.markRoot();
			addSecondaryOp(ret);
		}
	}

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

// static void outputStaticType(const string &label, const StaticType &sType)
// {
// 	cerr << label << ": ";

// 	if(sType.containsType(StaticType::NODE_TYPE)) cerr << "NODE_TYPE | ";
// 	if(sType.containsType(StaticType::ANY_SIMPLE_TYPE)) cerr << "ANY_SIMPLE_TYPE | ";
// 	if(sType.containsType(StaticType::ANY_URI_TYPE)) cerr << "ANY_URI_TYPE | ";
// 	if(sType.containsType(StaticType::BASE_64_BINARY_TYPE)) cerr << "BASE_64_BINARY_TYPE | ";
// 	if(sType.containsType(StaticType::BOOLEAN_TYPE)) cerr << "BOOLEAN_TYPE | ";
// 	if(sType.containsType(StaticType::DATE_TYPE)) cerr << "DATE_TYPE | ";
// 	if(sType.containsType(StaticType::DATE_TIME_TYPE)) cerr << "DATE_TIME_TYPE | ";
// 	if(sType.containsType(StaticType::DAY_TIME_DURATION_TYPE)) cerr << "DAY_TIME_DURATION_TYPE | ";
// 	if(sType.containsType(StaticType::DECIMAL_TYPE)) cerr << "DECIMAL_TYPE | ";
// 	if(sType.containsType(StaticType::DOUBLE_TYPE)) cerr << "DOUBLE_TYPE | ";
// 	if(sType.containsType(StaticType::DURATION_TYPE)) cerr << "DURATION_TYPE | ";
// 	if(sType.containsType(StaticType::FLOAT_TYPE)) cerr << "FLOAT_TYPE | ";
// 	if(sType.containsType(StaticType::G_DAY_TYPE)) cerr << "G_DAY_TYPE | ";
// 	if(sType.containsType(StaticType::G_MONTH_TYPE)) cerr << "G_MONTH_TYPE | ";
// 	if(sType.containsType(StaticType::G_MONTH_DAY_TYPE)) cerr << "G_MONTH_DAY_TYPE | ";
// 	if(sType.containsType(StaticType::G_YEAR_TYPE)) cerr << "G_YEAR_TYPE | ";
// 	if(sType.containsType(StaticType::G_YEAR_MONTH_TYPE)) cerr << "G_YEAR_MONTH_TYPE | ";
// 	if(sType.containsType(StaticType::HEX_BINARY_TYPE)) cerr << "HEX_BINARY_TYPE | ";
// 	if(sType.containsType(StaticType::NOTATION_TYPE)) cerr << "NOTATION_TYPE | ";
// 	if(sType.containsType(StaticType::QNAME_TYPE)) cerr << "QNAME_TYPE | ";
// 	if(sType.containsType(StaticType::STRING_TYPE)) cerr << "STRING_TYPE | ";
// 	if(sType.containsType(StaticType::TIME_TYPE)) cerr << "TIME_TYPE | ";
// 	if(sType.containsType(StaticType::UNTYPED_ATOMIC_TYPE)) cerr << "UNTYPED_ATOMIC_TYPE | ";
// 	if(sType.containsType(StaticType::YEAR_MONTH_DURATION_TYPE)) cerr << "YEAR_MONTH_DURATION_TYPE | ";

// 	cerr << endl;
// }

void QueryPlanGenerator::generateComparison(ImpliedSchemaNode::Type type, bool generalComp,
	VectorOfASTNodes &args, PathResult &result, VariableIDs *ids, QueryPlanHolder &arg0qph,
	QueryPlanHolder &arg1qph)
{
	IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);
	result.operation = intersectOp;

	ASTNode *argA = args[0];
	ASTNode *argB = args[1];

	VariableIDs argIDsA, argIDsB;
	PathResult retA = generate(argA, &argIDsA);
	PathResult retB = generate(argB, &argIDsB);
	intersectOp->addArg(retA.operation);
	intersectOp->addArg(retB.operation);
	if(ids) {
		VariableIDs::const_iterator i;
		for (i = argIDsA.begin(); i != argIDsA.end(); i++)
			ids->insert(*i);
		for (i = argIDsB.begin(); i != argIDsB.end(); i++)
			ids->insert(*i);
	}

	StaticType arg0_type = argA->getStaticResolutionContext().getStaticType();
	StaticType arg1_type = argB->getStaticResolutionContext().getStaticType();

	Syntax::Type syntaxType;
	if(generalComp) {
		syntaxType = getComparisonType(generalCompTypeConversion(arg0_type, arg1_type),
			generalCompTypeConversion(arg1_type, arg0_type));
	}
	else {
		syntaxType = getComparisonType(valueCompTypeConversion(arg0_type),
			valueCompTypeConversion(arg1_type));
	}

	XPath2MemoryManager *mm = xpc_->getMemoryManager();
	Paths nlsPaths;
	Paths::iterator it;
	for(it = retA.returnPaths.begin(); it != retA.returnPaths.end(); ++it) {
		if(type != (ImpliedSchemaNode::Type)-1) {
			ImpliedSchemaNode *newChild = (*it)->appendChild(new (mm)
				ImpliedSchemaNode(type, generalComp, syntaxType, argB, mm));
			nlsPaths.push_back(newChild);

			if(varsInScope(*it, argIDsB)) result.join(newChild);
		} else (*it)->markSubtree();
	}

	intersectOp->addArg(new (&memMgr_) PathsQP(result.returnPaths, &memMgr_));

	PathsQP *pathsqp = new (&memMgr_) PathsQP(nlsPaths, &memMgr_);
	arg0qph.addToQueryPlan(pathsqp);

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths.clear();

	nlsPaths.clear();
	for(it = retB.returnPaths.begin(); it != retB.returnPaths.end(); ++it) {
		if(type != (ImpliedSchemaNode::Type)-1) {
			ImpliedSchemaNode *newChild = (*it)->appendChild(new (mm)
				ImpliedSchemaNode(oppositeOperation(type), generalComp,
					syntaxType, argA, mm));
			nlsPaths.push_back(newChild);

			if(varsInScope(*it, argIDsA)) result.join(newChild);
		} else (*it)->markSubtree();
	}

	intersectOp->addArg(new (&memMgr_) PathsQP(result.returnPaths, &memMgr_));

	pathsqp = new (&memMgr_) PathsQP(nlsPaths, &memMgr_);
	arg1qph.addToQueryPlan(pathsqp);

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

void QueryPlanGenerator::generateSubstring(ImpliedSchemaNode::Type type, VectorOfASTNodes &args, PathResult &result,
	VariableIDs *ids, QueryPlanHolder *qph)
{
	if(args.size() > 2) {
		// Mark the collation argument
		addSecondaryOpAndMark(generate(args[2], ids));
	}

	VariableIDs argIDs;
	PathResult ret = generate(args[0], ids);
	addSecondaryOpAndMark(generate(args[1], &argIDs));

	XPath2MemoryManager *mm = xpc_->getMemoryManager();
	Paths nlsPaths;
	for(Paths::iterator it = ret.returnPaths.begin(); it != ret.returnPaths.end(); ++it) {
		ImpliedSchemaNode *newChild = (*it)->appendChild(new (mm)
			ImpliedSchemaNode(type, /*generalComp*/false, Syntax::STRING, args[1], mm));
		nlsPaths.push_back(newChild);

		if(varsInScope(*it, argIDs)) result.join(newChild);
	}
	result.operation =
		new (&memMgr_) IntersectQP(ret.operation, new (&memMgr_) PathsQP(result.returnPaths, &memMgr_), &memMgr_);

	if(qph != 0) {
		PathsQP *pathsqp = new (&memMgr_) PathsQP(nlsPaths, &memMgr_);
		qph->addToQueryPlan(pathsqp);
	}

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths.clear();
	if(ids) for(VariableIDs::const_iterator i = argIDs.begin(); i != argIDs.end(); ++i)
		ids->insert(*i);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateParenthesizedExpr(XQParenthesizedExpr *item, VariableIDs *ids)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getChildren());
	PathResult result;
	UnionQP *unionOp = new (&memMgr_) UnionQP(&memMgr_);
	for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
		PathResult ret = generate(*i, ids);
		result.join(ret);
		unionOp->addArg(ret.operation);
	}
	result.operation = unionOp;

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateVariable(XQVariable *item, VariableIDs *ids)
{
	VarStoreRef *ref = varStore_.getVar(item->getURI(), item->getName());

	PathResult result;
	if(ref != 0) {
		result = ref->getValue();
		if(ids) ids->insert(ref->getValue().id);
	}

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateIf(XQIf *item, VariableIDs *ids)
{
	// if(test_op) then op1 else op2
	//
	// We could use the formula u(n(test_op, op1), n(not(test_op), op2)) but the
	// not() may well not be worth it.
	//
	// Instead we use u(n(test_op, op1), op2) as an approxamation.

	PathResult result;
	UnionQP *unionOp = new (&memMgr_) UnionQP(&memMgr_);

	PathResult test = generate(const_cast<ASTNode *>(item->getTest()), ids);

	PathResult ret = generate(const_cast<ASTNode *>(item->getWhenTrue()), ids);
	result.join(ret);
	unionOp->addArg(new (&memMgr_) IntersectQP(test.operation, ret.operation, &memMgr_));

	ret = generate(const_cast<ASTNode *>(item->getWhenFalse()), ids);
	result.join(ret);
	unionOp->addArg(ret.operation);

	result.operation = unionOp;

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateInstanceOf(XQInstanceOf *item, VariableIDs *ids)
{
	PathResult result;
	addSecondaryOpAndMark(generate(const_cast<ASTNode *>(item->getExpression()), ids));

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateCastableAs(XQCastableAs *item, VariableIDs *ids)
{
	PathResult result;
	addSecondaryOpAndMark(generate(const_cast<ASTNode *>(item->getExpression()), ids));

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateFLWORBinding(XQFLWOR *item,
	VectorOfVariableBinding::iterator i, VectorOfVariableBinding::iterator end, VariableIDs *ids)
{
	PathResult result;
	IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);
	result.operation = intersectOp;

	if(i == end) {
		// sort specs
		const XQSort *sort = item->getSort();
		if(sort) {
			XQSort::VectorOfSortSpec *specs = const_cast<XQSort::VectorOfSortSpec *>(sort->getSortSpecs());
			for(XQSort::VectorOfSortSpec::iterator j = specs->begin(); j != specs->end(); ++j) {
				addSecondaryOpAndMark(generate(const_cast<ASTNode *>((*j)->getExpression()), ids));
				// Don't add sort expressions to the operation, as they don't have to exist
			}
		}

		// where and return
		PathResult returnpr = generate(const_cast<ASTNode *>(item->getReturnExpr()), ids);
		result.join(returnpr);
		intersectOp->addArg(returnpr.operation);

		if(item->getWhereExpr()) {
			PathResult ret = generate(const_cast<ASTNode *>(item->getWhereExpr()), ids);
			intersectOp->addArg(ret.operation);
		}

		return result;
	}

	PathResult r = generate((*i)->_allValues, ids);

	if((*i)->_bindingType == XQVariableBinding::forBinding) {
		intersectOp->addArg(r.operation);
	}

	VariableIDs myVars;
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
		PathResult ret = generate((*i)->_where, ids);
		intersectOp->addArg(ret.operation);

		if((*i)->_variable) {
			r.operation = new (&memMgr_) IntersectQP(r.operation, ret.operation, &memMgr_);
			VarStoreRef *ref = varStore_.getVar((*i)->_vURI, (*i)->_vName);
			const_cast<VarValue&>(ref->getValue()).operation = r.operation;
		}
	}

	PathResult returnpr = generateFLWORBinding(item, i + 1, end, ids);
	result.join(returnpr);
	intersectOp->addArg(returnpr.operation);

	// undo binding's scopes
	if((*i)->_variable || (*i)->_positionalVariable) {
		delete varStore_.popScope();
	}
	if(ids) for(VariableIDs::iterator it3 = myVars.begin(); it3 != myVars.end(); ++it3)
		ids->erase(*it3);

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateFLWOR(XQFLWOR *item, VariableIDs *ids)
{
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	return generateFLWORBinding(item, bindings->begin(), bindings->end(), ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateFLWORQuantified(XQQuantified *item, VariableIDs *ids)
{
	PathResult result;
	IntersectQP *intersectOp = new (&memMgr_) IntersectQP(&memMgr_);

	// bindings
	VariableIDs myVars;
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	for(VectorOfVariableBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
		PathResult r = generate((*i)->_allValues, ids);
		intersectOp->addArg(r.operation);

		if((*i)->_variable) {
			varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
			myVars.insert(setVariable((*i)->_vURI, (*i)->_vName, r));
		}
	}

	// no where or sort specs

	// return
	PathResult ret = generate(const_cast<ASTNode *>(item->getReturnExpr()), ids);
	intersectOp->addArg(ret.operation);
	result.operation = intersectOp;

	// undo binding's scopes
	for(VectorOfVariableBinding::iterator i2 = bindings->begin(); i2 != bindings->end(); ++i2) {
		if((*i2)->_variable) {
			delete varStore_.popScope();
		}
	}
	if(ids) for(VariableIDs::iterator it3 = myVars.begin(); it3 != myVars.end(); ++it3)
		ids->erase(*it3);

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateTypeswitch(XQTypeswitch *item, VariableIDs *ids)
{
	PathResult result;
	UnionQP *unionOp = new (&memMgr_) UnionQP(&memMgr_);

	PathResult expr = generate(const_cast<ASTNode *>(item->getExpression()), ids);

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
			delete varStore_.popScope();
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
		delete varStore_.popScope();
		if(ids) ids->erase(varId);
	}

	result.operation = new (&memMgr_) IntersectQP(expr.operation, unionOp, &memMgr_);

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateGlobalVar(XQGlobalVariable *item, VariableIDs *ids)
{
	PathResult result;
	if(item->getVariableExpr()) {
		result = generate(const_cast<ASTNode *>(item->getVariableExpr()), ids);
		varStore_.setGlobalVar(item->getVariableURI(), item->getVariableLocalName(),
				       VarValue(result, varId_++));
		result.returnPaths.clear();
		result.operation = 0;
	}
	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDOMConstructor(XQDOMConstructor *item, VariableIDs *ids)
{
	PathResult result;

	if(item->getName()) {
		addSecondaryOpAndMark(generate(const_cast<ASTNode *>(item->getName()), ids));
	}

	VectorOfASTNodes *attrs = const_cast<VectorOfASTNodes *>(item->getAttributes());
	if(attrs) {
		for(VectorOfASTNodes::iterator i = attrs->begin(); i != attrs->end(); ++i) {
			addSecondaryOpAndMark(generate(*i, ids));
		}
	}

	VectorOfASTNodes *children = const_cast<VectorOfASTNodes *>(item->getChildren());
	if(children) {
		for(VectorOfASTNodes::iterator j = children->begin(); j != children->end(); ++j) {
			addSecondaryOpAndMark(generate(*j, ids));
		}
	}

	if(item->getValue()) {
		addSecondaryOpAndMark(generate(const_cast<ASTNode *>(item->getValue()), ids));
	}

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateContextItem(XQContextItem *item, VariableIDs *ids)
{
	const VarValue &currentContext = getCurrentContext();
	PathResult result = currentContext;
	if(ids) ids->insert(currentContext.id);

	return result;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateLiteral(XQLiteral *item, VariableIDs *ids)
{
	return PathResult();
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateSequence(XQSequence *item, VariableIDs *ids)
{
	return PathResult();
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateCastAs(XQCastAs *item, VariableIDs *ids)
{
	return generate(const_cast<ASTNode *>(item->getExpression()), ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateTreatAs(XQTreatAs *item, VariableIDs *ids)
{
	return generate(const_cast<ASTNode *>(item->getExpression()), ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDebugHook(XQDebugHook *item, VariableIDs *ids)
{
	return generate(item->m_impl, ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateOrderingChange(XQOrderingChange *item, VariableIDs *ids)
{
	return generate(item->getExpr(), ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateAtomize(XQAtomize *item, VariableIDs *ids)
{
	PathResult ret = generate(const_cast<ASTNode *>(item->getExpression()), ids);
	ret.markSubtree();

	return ret;
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generatePromoteUntyped(XQPromoteUntyped *item, VariableIDs *ids)
{
	return generate(const_cast<ASTNode *>(item->getExpression()), ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generatePromoteNumeric(XQPromoteNumeric *item, VariableIDs *ids)
{
	return generate(const_cast<ASTNode *>(item->getExpression()), ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generatePromoteAnyURI(XQPromoteAnyURI *item, VariableIDs *ids)
{
	return generate(const_cast<ASTNode *>(item->getExpression()), ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateDocumentOrder(XQDocumentOrder *item, VariableIDs *ids)
{
	return generate(const_cast<ASTNode *>(item->getExpression()), ids);
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generateValidate(XQValidate *item, VariableIDs *ids)
{
	DBXML_ASSERT(false);
	return PathResult();
}

QueryPlanGenerator::PathResult QueryPlanGenerator::generatePredicate(XQPredicate *item, VariableIDs *ids)
{
	PathResult result = generate(const_cast<ASTNode *>(item->getExpression()), ids);

	IntersectQP *predOp = new (&memMgr_) IntersectQP(&memMgr_);
	predOp->addArg(result.operation);

	varStore_.addScope(VarStore::MyScope::LOGICAL_BLOCK_SCOPE);
	unsigned int varId = setCurrentContext(result);

	PathResult predResult = generate(const_cast<ASTNode *>(item->getPredicate()), ids);
	predOp->addArg(predResult.operation);

	delete varStore_.popScope();
	if(ids) ids->erase(varId);

	result.operation = predOp;
	return result;
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
