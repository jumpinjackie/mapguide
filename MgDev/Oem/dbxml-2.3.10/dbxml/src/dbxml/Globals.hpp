//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Globals.hpp,v 1.19 2006/10/30 17:45:52 bostic Exp $
//

#ifndef __GLOBALS_HPP
#define	__GLOBALS_HPP

#include <string>
#include <map>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemoryManager.hpp>
#include "nodeStore/NsTypes.hpp"

class DbEnv;
class DatatypeLookup;
class DocumentCache;

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

	static const DatatypeLookup *getDatatypeLookup() {
		return datatypeLookup_;
	}

	static bool isInitialized() { return (refCount_ != 0); }

	/// The map of index names to values
	typedef std::map<std::string, unsigned long> NameToNumber;
	static NameToNumber *indexMap;

	static XERCESC::MemoryManager *defaultMemoryManager;

	/// Mutex for the globals
	static void *mutex_;
	static int refCount_; // how many Manager objects are alive

	static const XMLCh XMLCh_UTF8[]; // = "UTF-8"
	static const XMLCh XMLCh_dbxml_scheme[]; // = "dbxml"

	static bool isBigendian_;
private:
	static void initializeXmlPlatform();

	static DocumentCache *documentCache_;
	static DatatypeLookup *datatypeLookup_;
};

}

#endif
