//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ReferenceCounted.cpp,v 1.17 2006/10/30 17:45:53 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "ReferenceCounted.hpp"
#include "Globals.hpp"
#include <xercesc/util/PlatformUtils.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;

ReferenceCountedProtected::ReferenceCountedProtected()
	: count_(0),
	  mutex_(XMLPlatformUtils::makeMutex(Globals::defaultMemoryManager))
{}

ReferenceCountedProtected::~ReferenceCountedProtected()
{
	XMLPlatformUtils::closeMutex(mutex_);
}

void ReferenceCountedProtected::acquire()
{
	MutexLock lock(mutex_);
	++count_;
}

void ReferenceCountedProtected::lock()
{
	XMLPlatformUtils::lockMutex(mutex_);
}

void ReferenceCountedProtected::unlock()
{
	XMLPlatformUtils::unlockMutex(mutex_);
}

void ReferenceCountedProtected::release()
{
	int newValue;
	
	// Musn't hold the mutex lock when this class is deleted
	{
		MutexLock lock(mutex_);
		newValue = --count_;
	}
	
	if(newValue == 0) {
		delete this;
	}
}

MutexLock::MutexLock(void *mutex)
	: toLock_(mutex)
{
	XMLPlatformUtils::lockMutex(toLock_);
}

MutexLock::~MutexLock()
{
	XMLPlatformUtils::unlockMutex(toLock_);
}
