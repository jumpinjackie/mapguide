//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Globals.cpp,v 1.23 2005/12/07 15:58:16 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "Globals.hpp"
#include "Value.hpp"
#include "IndexSpecification.hpp"
#include "SyntaxManager.hpp"
#include "nodeStore/NsConstants.hpp"

#include <xercesc/util/PlatformUtils.hpp>
#include <pathan/PathanPlatformUtils.hpp>
#include <xercesc/internal/MemoryManagerImpl.hpp>
#include <pathan/internal/AnyAtomicTypeDatatypeValidator.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/PathanException.hpp>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <math.h>
#define srandom srand
#endif

#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

Globals::NameToNumber *Globals::indexMap = 0;
const nid_t Globals::docRootNID = nid_t();

bool Globals::isBigendian_;
int Globals::refCount_ = 0;
void *Globals::mutex_ = 0;
DatatypeValidatorFactory *Globals::validatorFactory_ = 0;

static SimpleMemoryManager staticMemoryManager;
MemoryManager *Globals::defaultMemoryManager = &staticMemoryManager;


void *SimpleMemoryManager::allocate(size_t size)
{
	return malloc(size);
}

void SimpleMemoryManager::deallocate(void *p)
{
	free(p);
}

void Globals::initialize(DbEnv *env)
{
	if (mutex_ == 0) {
		mutex_ = XMLPlatformUtils::makeMutex(defaultMemoryManager);
	}
	MutexLock lock(mutex_);

	if (refCount_++ == 0) {

		SyntaxManager::initSyntaxManager();

		// seed the random number function (used by name generator)
		srandom((unsigned int)time(0));

		// init Xerces and Pathan, and related structures
		initializeXmlPlatform();

		// initialise the map of index names to values
		indexMap = new NameToNumber;
		(*indexMap)["none"] = Index::NONE;
		(*indexMap)["unique"] = Index::UNIQUE_ON;
		(*indexMap)["node"] = Index::PATH_NODE;
		(*indexMap)["edge"] = Index::PATH_EDGE;
		(*indexMap)["element"] = Index::NODE_ELEMENT;
		(*indexMap)["attribute"] = Index::NODE_ATTRIBUTE;
		(*indexMap)["metadata"] = Index::NODE_METADATA;
		(*indexMap)["equality"] = Index::KEY_EQUALITY;
		(*indexMap)["presence"] = Index::KEY_PRESENCE;
		(*indexMap)["substring"] = Index::KEY_SUBSTRING;
		isBigendian_ = (env->get_DB_ENV()->is_bigendian()) ? true : false;
		initTypeConversions();

		// initialise the document root NID
		DBXML_ASSERT(NID_BYTES_SIZE >= 2);
		const_cast<nid_t&>(docRootNID).idLen = 2;
		const_cast<nid_t&>(docRootNID).idStore.idBytes[0] = NS_ROOT_ID;
		const_cast<nid_t&>(docRootNID).idStore.idBytes[1] = 0;
	}
}

void Globals::initializeXmlPlatform()
{
	try {
		// initialise Pathan and Xerces
		PathanPlatformUtils::initialize(defaultMemoryManager);

		// create the datatype validator factory
		validatorFactory_ = new DatatypeValidatorFactory(
			defaultMemoryManager);

		// set up the Xerces data type validators (built-in registry)
		validatorFactory_->expandRegistryToFullSchemaSet();

		// set up data type validators for types supported by
		// Pathan (xs:duration subtypes and untyped atomic)
		// (user-defined registry) xdt:dayTimeDuration
		RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>
			*facets = new (defaultMemoryManager)
			RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER
			KVStringPair>(1,defaultMemoryManager);

		facets->put(
			(void*) SchemaSymbols::fgELT_PATTERN,
			new (defaultMemoryManager)
			XERCES_CPP_NAMESPACE_QUALIFIER
			KVStringPair(SchemaSymbols::fgELT_PATTERN,
			      ATDurationOrDerived::pattern_DT_DAYTIMEDURATION,
			      defaultMemoryManager));

		validatorFactory_->
			createDatatypeValidator(
			ATDurationOrDerived::fgDT_DAYTIMEDURATION_XERCESHASH,
			validatorFactory_->
			getDatatypeValidator(SchemaSymbols::fgDT_DURATION),
			facets, 0, false, 0, true, defaultMemoryManager);

		// xdt:yearMonthDuration
		facets = new (defaultMemoryManager) RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>(1,defaultMemoryManager);
		facets->put((void*) SchemaSymbols::fgELT_PATTERN, new (defaultMemoryManager) XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair(SchemaSymbols::fgELT_PATTERN, ATDurationOrDerived::pattern_DT_YEARMONTHDURATION, defaultMemoryManager));
		validatorFactory_->createDatatypeValidator(ATDurationOrDerived::fgDT_YEARMONTHDURATION_XERCESHASH, validatorFactory_->getDatatypeValidator(SchemaSymbols::fgDT_DURATION), facets, 0, false, 0, true, defaultMemoryManager);

		// xdt:anyAtomicType (copy the Pathan design of putting
		// into the built-in registry so that it can be used
		// as a base validator of untypedAtomic)
		DVHashTable *defRegistry =
			validatorFactory_->getBuiltInRegistry();

		if(!defRegistry->
		   containsKey((void*)
			       AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH)) {
			// This validator belongs to Xerces, so use its
			// memory manager, not ours.
			DatatypeValidator *dv =
				new (XMLPlatformUtils::fgMemoryManager)
				AnyAtomicTypeDatatypeValidator();
			dv->setTypeName(
				AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH);
			defRegistry->put(
				(void*) AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH, dv);
		}

		// xdt:untypedAtomic
		validatorFactory_->createDatatypeValidator(ATUntypedAtomic::fgDT_UNTYPEDATOMIC_XERCESHASH, validatorFactory_->getDatatypeValidator(AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH), 0, 0, false, 0, true, defaultMemoryManager);
	}
	catch (const XMLException &e) {
		char *desc = XMLString::transcode(e.getMessage());
		throw XmlException(
			XmlException::INTERNAL_ERROR,
			"Error during Xerces-C initialization: " +
			std::string(desc), __FILE__, __LINE__);
		XMLString::release(&desc);
	}
	catch (const PathanException &e) {
		char *desc = XMLString::transcode(e.getString());
		throw XmlException(
			XmlException::INTERNAL_ERROR,
			"Error during Pathan initialization: " +
			std::string(desc), __FILE__, __LINE__);
		XMLString::release(&desc);
	}
}

void Globals::terminate()
{
	{
		MutexLock lock(mutex_);
		if (--refCount_ == 0) {
			// delete the map of index names to values
			delete indexMap;
			indexMap = 0;

			delete validatorFactory_;
			validatorFactory_ = 0;
			terminateTypeConversions();
			// terminate Pathan and Xerces
			PathanPlatformUtils::terminate();
			SyntaxManager::uninitSyntaxManager();
		}
	}
	// Don't close the mutex, as it can't be done in a thread safe fashion
}

const XMLCh Globals::XMLCh_UTF8[] = {
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_U, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F, XERCES_CPP_NAMESPACE_QUALIFIER chDash,
	XERCES_CPP_NAMESPACE_QUALIFIER chDigit_8, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh Globals::XMLCh_dbxml_scheme[] = {
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
	XERCES_CPP_NAMESPACE_QUALIFIER chNull
};
