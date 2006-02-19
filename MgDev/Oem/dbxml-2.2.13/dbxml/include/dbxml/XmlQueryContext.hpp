//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlQueryContext.hpp,v 1.34 2005/08/26 11:50:01 jsnelson Exp $
//

#ifndef __XMLQUERYCONTEXT_HPP
#define __XMLQUERYCONTEXT_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include <string>

namespace DbXml
{

class QueryContext;

class DBXML_EXPORT XmlQueryContext
{
public:
	enum ReturnType {
		DeadValues,         ///< Return dead (copied) nodes from the documents that match the XPath expression.
		LiveValues          ///< Return live (non-copied) nodes that match the XPath expression, along with a reference to their document.
	};

	enum EvaluationType {
		Eager,  ///< Evaluate the whole query immediatly.
		Lazy    ///< Evaluate the query as the result set is iterated over.
	};

	/** @name For Reference Counting */
	XmlQueryContext();
	XmlQueryContext(const XmlQueryContext&);
	XmlQueryContext &operator=(const XmlQueryContext &);
	virtual ~XmlQueryContext();
	bool isNull() const { return queryContext_ == 0; }

	/** @name Namespace Methods */
	void setNamespace(const std::string &prefix, const std::string &uri);
	std::string getNamespace(const std::string &prefix);
	void removeNamespace(const std::string &prefix);
	void clearNamespaces(void);

	/** @name Variable Methods */
	void setVariableValue(const std::string &name, const XmlValue &value);
	void setVariableValue(const std::string &name, XmlResults &value);
	bool getVariableValue(const std::string &name, XmlValue &value) const;
	bool getVariableValue(const std::string &name, XmlResults &value) const;

	/** @name Base URI Methods */
	void setBaseURI(const std::string &baseURI);
	std::string getBaseURI() const;

	/** @name Return Type Methods */
	void setReturnType(ReturnType rt);
	ReturnType getReturnType() const;

	/** @name Evaluation Type Methods */
	void setEvaluationType(EvaluationType et);
	EvaluationType getEvaluationType() const;

	/** @name Default Collection Methods */
	void setDefaultCollection(const std::string &uri);
	std::string getDefaultCollection() const;

	/** @name Private Methods (for internal use) */
	// @{

	XmlQueryContext(QueryContext *context);
	operator const QueryContext *() const
	{
		return queryContext_;
	}
	operator QueryContext &() const
	{
		return *queryContext_;
	}
	operator QueryContext *()
	{
		return queryContext_;
	}

	//@}

private:
	QueryContext *queryContext_;
};

}

#endif

