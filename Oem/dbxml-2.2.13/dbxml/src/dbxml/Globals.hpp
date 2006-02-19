//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Globals.hpp,v 1.14 2005/09/14 16:54:01 jsnelson Exp $
//

#ifndef __GLOBALS_HPP
#define	__GLOBALS_HPP

#include <string>
#include <map>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/validators/datatype/DatatypeValidatorFactory.hpp>

#include "nodeStore/NsTypes.hpp"

class DbEnv;

namespace DbXml
{

class SimpleMemoryManager : public XER_NS MemoryManager
{
public:
	SimpleMemoryManager() {}
	void *allocate(size_t size);
	void deallocate(void *p);
};

class Globals
{
public:
	/// Called by Manager::initialize()
	static void initialize(DbEnv *env);
	/// Called by Manager::terminate()
	static void terminate();

	static XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidatorFactory
	&getDatatypeValidatorFactory() {
		return *validatorFactory_;
	}

	static bool isInitialized() { return (refCount_ != 0); }

	/// The map of index names to values
	typedef std::map<std::string, unsigned long> NameToNumber;
	static NameToNumber *indexMap;

	static XERCESC::DatatypeValidatorFactory *validatorFactory_;
	static XERCESC::MemoryManager *defaultMemoryManager;

	static const nid_t docRootNID;

	/// Mutex for the globals
	static void *mutex_;
	static int refCount_; // how many Manager objects are alive

	static const XMLCh XMLCh_UTF8[]; // = "UTF-8"
	static const XMLCh XMLCh_dbxml_scheme[]; // = "dbxml"

	static bool isBigendian_;
private:
	static void initializeXmlPlatform();
};

}

#endif
