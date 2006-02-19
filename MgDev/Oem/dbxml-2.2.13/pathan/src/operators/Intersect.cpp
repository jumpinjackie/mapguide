/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/operators/Intersect.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Node.hpp>

/*static*/ const XMLCh Intersect::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Intersect::Intersect(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemOperator(name, args, memMgr)
{
  _src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED);
  _src.getStaticType().flags = StaticType::NODE_TYPE;
}

Sequence Intersect::collapseTreeInternal(DynamicContext* context, int flags) const
{
	Sequence param1 = _args[0]->collapseTree(context, flags & DataItem::UNORDERED).toSequence(context);
	if(!checkSequenceIsNodes(param1)) {
		DSLthrow(XPath2ErrorException,X("Intersect::collapseTreeInternal"), X("The operator 'intersect' has been called with a parameter that is not just nodes"));
	}
	Sequence param2 = _args[1]->collapseTree(context, flags & DataItem::UNORDERED).toSequence(context);
	if(!checkSequenceIsNodes(param2)) {
		DSLthrow(XPath2ErrorException,X("Intersect::collapseTreeInternal"), X("The operator 'intersect' has been called with a parameter that is not just nodes"));
	}

	XPath2MemoryManager* memMgr = context->getMemoryManager();
	Sequence result(param1.getLength(),memMgr);

	Sequence::const_iterator p1It = param1.begin();
	Sequence::const_iterator p2It;
	Sequence::const_iterator end1 = param1.end();
	Sequence::const_iterator end2 = param2.end();

	for(;p1It != end1; ++p1It) {
		for(p2It = param2.begin();p2It != end2; ++p2It) {
			if(((Node*)(const Item*)(*p1It))->equals((const Node::Ptr )*p2It)) {
				result.addItem(*p2It);
			}
		}
	}

    //Sort the nodes into document order
    if(!(context->getNodeSetOrdering()==StaticContext::ORDERING_UNORDERED || flags & DataItem::UNORDERED))
        result.sortIntoDocumentOrder(context);
    return result;
}
