//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlResolver.hpp,v 1.7 2005/04/05 16:43:55 bostic Exp $
//

#ifndef __XMLRESOLVER_HPP
#define	__XMLRESOLVER_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include <string>

namespace DbXml
{

/**
 * Base class designed for the user to derive from, to implement
 * aspects of file resolution policy.
 */
class DBXML_EXPORT XmlResolver
{
public:
	/** @name Virtual Destructor */
	virtual ~XmlResolver() {};

	/** @name Document and Collection Resolution */

	/**
	 * Resolve the given uri to an XmlValue. If the uri cannot be resolved
	 * by this XmlResolver, then the method should return false, otherwise
	 * it should return true.
	 */
	virtual bool resolveDocument(XmlTransaction *txn, XmlManager &mgr,
				     const std::string &uri, XmlValue &result) const;
	/**
	 * Resolve the given uri to an XmlResults. If the uri cannot be resolved
	 * by this XmlResolver, then the method should return false, otherwise
	 * it should return true.
	 */
	virtual bool resolveCollection(XmlTransaction *txn, XmlManager &mgr,
				       const std::string &uri, XmlResults &result) const;

	/** @name Schema and DTD Resolution */
	// @{

	/**
	 * Resolve the given schema location and namespace to an XmlInputStream.If the uri
	 * cannot be resolved by this XmlResolver, then the method should return 0. The
	 * returned XmlInputStream is adopted by DbXml, and will be deleted by it.
	 */
	virtual XmlInputStream *resolveSchema(XmlTransaction *txn, XmlManager &mgr,
					      const std::string &schemaLocation,
					      const std::string &nameSpace) const;
	/**
	 * Resolve the given system id and public id to an XmlInputStream.If the uri
	 * cannot be resolved by this XmlResolver, then the method should return 0. The
	 * returned XmlInputStream is adopted by DbXml, and will be deleted by it.
	 */
	virtual XmlInputStream *resolveEntity(XmlTransaction *txn, XmlManager &mgr,
					      const std::string &systemId,
					      const std::string &publicId) const;

	// @}

protected:
	XmlResolver() {};
};

}

#endif
