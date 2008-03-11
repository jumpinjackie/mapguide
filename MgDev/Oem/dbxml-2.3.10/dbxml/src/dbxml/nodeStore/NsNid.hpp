//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsNid.hpp,v 1.12 2006/10/31 23:39:52 jsnelson Exp $
//

/**
 * NsNid and NsNidGen
 *
 * Node ID and accessors and node id generation.
 *
 * This includes both transient and persistent formats of
 * this object, encapsulating its format, allowing for
 * possible changes
 *
 * See NsNid.cpp for a description of the format
 */
#ifndef __DBXMLNSNID_HPP
#define __DBXMLNSNID_HPP

#include "NsTypes.hpp"
#include <cstring>

#define NIDBUF_SIZE 50
#define NID_BYTES_SIZE 5
#define NID_ROOT_SIZE 1
#define NS_ID_ZERO 0x01
#define NS_ID_FIRST 0x02
#define NS_ID_LAST 0xFF
#define NS_METADATA_ID (NS_ID_FIRST + 0) // document metadata
#define NS_ROOT_ID (NS_ID_FIRST + 2) // the document node

/*
 * Invariant:
 *  If length is > NID_BYTES_SIZE, nid is in nidStore.nidPtr,
 *  and if it is <= NID_BYTES_SIZE, it's in nidStore.nidBytes.
 *  Allocation (isAlloced) is distinct from this, because unmarshaled
 *  node ids may be > NID_BYTES_SIZE, but not require deallocation,
 *  because the storage is from the node buffer.
 */
extern "C" {
int nsCompareNodes(DB *db, const DBT *dbt1, const DBT *dbt2);
};

namespace DbXml
{
	
class NsNidGen;

#define NID_ALLOC_BIT 0x10000000
#define NID_ALLOC_MASK ~(0x10000000)
	
class NsNid {
private:
	// the state of a node id
	union nidStore_u {
		xmlbyte_t *nidPtr;
		xmlbyte_t nidBytes[NID_BYTES_SIZE];
	} nidStore;
	uint32_t nidLen;
	
	friend class NsNidGen;

public:
	// basic accessors
	bool isNull() const { return (getLen() == 0); }
	uint32_t getLen() const { return (nidLen & NID_ALLOC_MASK); }
	const xmlbyte_t *getBytes() const {
		if (getLen() > NID_BYTES_SIZE)
			return nidStore.nidPtr;
		return nidStore.nidBytes;
	}

	// next few are necessary for marshaling/unmarshaling
	xmlbyte_t *getBytesForCopy() {
		if (getLen() > NID_BYTES_SIZE)
			return nidStore.nidPtr;
		return nidStore.nidBytes;
	}
	void setLen(uint32_t len, bool alloced) {
		nidLen = len;
		if(alloced) nidLen |= NID_ALLOC_BIT;
	}
	void setLen(uint32_t len) {
		if (!isAlloced()) nidLen = len;
		else setLenAlloc(len);
	}
	void setLenAlloc(uint32_t len) { nidLen = (len | NID_ALLOC_BIT); }
	void setPtr(xmlbyte_t *ptr) { nidStore.nidPtr = ptr; }
	bool isAlloced() const { return ((nidLen&NID_ALLOC_BIT) != 0); }

	// instance methods
	int compareNids(const NsNid *other) const;
	// generate an id for this object that is between
	// prev and next. next is allowed to be null,
	// in which case this method calculates the next id after prev
	void getBetweenNid(XER_NS MemoryManager *mmgr, const NsNid *prev,
			   const NsNid *next, int itype);
	void freeNid(XER_NS MemoryManager *mmgr) {
		if (isAlloced())
			mmgr->deallocate(nidStore.nidPtr);
		nidLen = 0;
	}
	void copyNid(XER_NS MemoryManager *mmgr, const NsNid *from);
	void copyNid(XER_NS MemoryManager *mmgr, const xmlbyte_t *ptr, uint32_t len);
	void displayNid(std::ostream &out) const {
		displayNid(out, (const char *)getBytes(), getLen() - 1);
	}

	void setDocRootNid(XER_NS MemoryManager *mmgr);
	void setSpecialNid(XER_NS MemoryManager *mmgr, xmlbyte_t id);

	bool isDocRootNid() const;
	
	bool operator==(const NsNid &other) const {
		return (compareNids(&other) == 0);
	}
	bool operator!=(const NsNid &other) const {
		return (compareNids(&other) != 0);
	}
public:
	// static class methods
	static uint32_t nidStaticSize() { return NID_BYTES_SIZE; }
	static void displayNid(std::ostream &out, const char *buf, int len);
	static int compare(const unsigned char *n1, const unsigned char *n2) {
		return ::strcmp((const char *)n1, (const char *)n2);
	}
	static int getNidOverhead() {
		// protocol byte, 2 id bytes, null byte
		return 4;
	}
	static const NsNid *getRootNid() {
		return &docRootNid;
	}
	static void initDocRootNid() {
		const_cast<NsNid&>(docRootNid).setDocRootNid(0);
	}
	static void initNid(xmlbyte_t *buf, xmlbyte_t id);
private:
	static const NsNid docRootNid;

};

class NsNidGen {
public:
	NsNidGen();
	void nextId(XER_NS MemoryManager *mmgr, NsNid *id);
	
private:
	// id generation
	uint32_t _idDigit;
	uint8_t _idBuf[NIDBUF_SIZE];
};

}

#endif
