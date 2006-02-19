//
// $Id: DbXmlNodeTest.hpp,v 1.3 2005/08/09 15:16:08 jsnelson Exp $
//

#ifndef __DBXMLNODETEST_HPP
#define	__DBXMLNODETEST_HPP

#include "QueryPlanHolder.hpp"
#include "../QueryPlan.hpp"
#include "../nodeStore/NsTypes.hpp"

#include <pathan/internal/navigation/NodeTest.hpp>

namespace DbXml
{

class QueryPlan;

class DbXmlNodeTest : public NodeTest, public QueryPlanHolder
{
public:
	/** The DB XML NodeTest interface */
	static const XMLCh gDbXml[];

	DbXmlNodeTest(const NodeTest *other, QueryPlan *qp, XPath2MemoryManager *memMgr);

	virtual void *getInterface(const XMLCh *name) const;

	const xmlbyte_t* getNodeName8() const { return name8_; }
	const xmlbyte_t* getNodeUri8() const { return uri8_; }

protected:
	const xmlbyte_t *name8_;
	const xmlbyte_t *uri8_;
};

}

#endif
