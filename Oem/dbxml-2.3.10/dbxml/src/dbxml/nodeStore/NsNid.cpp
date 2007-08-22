//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsNid.cpp,v 1.13 2006/10/31 23:39:52 jsnelson Exp $
//

/**
 * NsNid.cpp
 *
 * Node ID and accessors and nid generation
 *
 * This includes both transient and persistent formats of
 * this object, encapsulating its format, allowing for
 * possible changes
 */

#include "NsNid.hpp"
#include "NsConstants.hpp"
#include "NsUtil.hpp"
#include "NsDom.hpp" // for NsDomInsertType enum
#include <sstream>

XERCES_CPP_NAMESPACE_USE
using namespace DbXml;

/**
 * Node Id Format
 *
 * Node Ids are stored as null-terminated arrays of bytes,
 * with valid values as determined by the NS_ID_* constants
 * defined below.  Collation order is simple string collation.
 *
 * Each ID is of the form:
 *  byte 0:  number of bytes before the "decimal" point (n)
 *  bytes 1-(n-1): the primary node id
 *  bytes n-end_of_string: bytes after the "decimal" point,
 *    allocated/used when inserting new node ids between other
 *    node ids.  An unmodified document will not have any of
 *    these
 *
 * For example, let's say that the valid characters are only a-z.
 * In this case, a newly-created document will have ids like this:
 * 1 a 0
 * 1 b 0
 * ...
 * 1 z 0
 * 2 aa 0
 * 2 aa 0
 * ...
 *
 * This format is designed for these requirements:
 *  1.  Be compact when there are no node insertions after creation
 *  2.  Allow efficient comparison
 *  3.  Allow efficient insertion of new node ids between existing nodes
 *
 * Here's an example or two of inserting new nodes, using the
 * same a-z range as above:
 * 1.  Between "1a0" and "1b0" ==> 1am
 * 2.  Between "1a0" and "1am" ==> 1ag
 * 3.  Between "1a0" and "1ab" ==> 1aam (cannot use "1aa")
 */

const NsNid NsNid::docRootNid = NsNid();

#define NID_BETW_INITIAL_SIZE 4 // must be two or more
#define NID_INITIAL_DIGIT 0

//
// Node Id Comparison function
//
// A simple lexicographical comparison, stopping at the first null.
//
extern "C" {
int
nsCompareNodes(DB *db, const DBT *dbt1, const DBT *dbt2)
{
	// Assume null termination of node ids
	int res;
	const xmlbyte_t *p1 = (const xmlbyte_t *)dbt1->data;
	const xmlbyte_t *p2 = (const xmlbyte_t *)dbt2->data;
	while((res = (int)*p1 - (int)*p2) == 0 &&
	      *p1 != 0) {
		++p1; ++p2;
	}
	return res;
}
};

// NsNid methods

static inline void _nextId(unsigned char *id, uint32_t &digit)
{
	unsigned char *dest = id + 1;
	int32_t i = digit;
	while (i >= 0 && (dest[i] == NS_ID_LAST)) {
		dest[i--] = NS_ID_FIRST;
	}
	if (i < 0) {
		++digit;
		dest[digit] = NS_ID_FIRST;
	} else
		dest[i] += 1;

	DBXML_ASSERT(dest[digit+1] == 0);
	// assign number of digits to leading byte
	*id = (unsigned char)(digit + 1);
}

static inline xmlbyte_t idbyte2num(xmlbyte_t idbyte)
{
	return (idbyte == 0 || idbyte == NS_ID_ZERO) ? 0 :
		idbyte + 1 - NS_ID_FIRST;
}

static inline xmlbyte_t num2idbyte(xmlbyte_t num)
{
	return num == 0 ? NS_ID_ZERO : num + NS_ID_FIRST - 1;
}

static inline xmlbyte_t nextIdbyteAsNum(const xmlbyte_t *&p)
{
	if(*p == 0) return 0;
	++p;
	return idbyte2num(*p);
}

#define POW_OF_2 3
static inline int avgbyte(int pb, int nb, int itype)
{
	int avg = (pb + nb) >> 1;
	for (int i = 0; avg != pb && i < POW_OF_2; i++) {
		if (itype == nsDomInsertAfter)
			avg = ((int)nb + (int)avg) >> 1;
		else
			avg = ((int)pb + (int)avg) >> 1;
	}
	return avg;
}

// returns:
// 0 if equal
// < 0 if "this" is less than other
// > 0 if "this" is greater than other
int
NsNid::compareNids(const NsNid *other) const
{
	// Assume null termination of node ids
	int res = 0;
	const xmlbyte_t *p1 = getBytes();
	const xmlbyte_t *p2 = other->getBytes();
	while((res = (int)*p1 - (int)*p2) == 0 &&
	      *p1 != 0) {
		++p1; ++p2;
	}
	return res;
}

// generate id for this object bewteen prev and next
void
NsNid::getBetweenNid(MemoryManager *mmgr,
		     const NsNid *prev,
		     const NsNid *next,
		     int itype)
{
	DBXML_ASSERT(prev);

	freeNid(mmgr); // free any existing ID

	int plen = prev->getLen();
	int nlen = next ? next->getLen() : 0;
	if(plen == 0 && nlen == 0) {
		memset(this, 0, sizeof(NsNid));
		return;
	}

	// maximum length of the between id is one
	// greater than the maximum length of the
	// prev or next id
	int maxlen = plen > nlen ? plen : nlen;
	maxlen += 1;

	// Allocate memory to build the id in
	xmlbyte_t *dest;
	if (maxlen > NID_BYTES_SIZE) {
		dest = (xmlbyte_t*)mmgr->
			allocate(maxlen * sizeof(xmlbyte_t));
		nidStore.nidPtr = dest;
	} else {
		dest = nidStore.nidBytes;
	}

	if (nlen == 0) {
		// no next, so we must be appending to the end of
		// the document, just add a next nid to previous
		memcpy(dest, prev->getBytes(), plen);
		dest[maxlen] = 0;

		// eliminate everything after the decimal, and
		// allow for an extra digit
		dest[dest[0]+1] = 0;
		dest[dest[0]+2] = 0;

		// idDigit is 0-based, subtract one from num bytes
		uint32_t tdig = dest[0] - 1;
		_nextId(dest, tdig);
		nidLen = ::strlen((const char *)dest) + 1;
		if (nidLen > NID_BYTES_SIZE)
			setLenAlloc(nidLen);
		else if (maxlen > NID_BYTES_SIZE) {
			// maintain invariant that smaller NIDs are
			// in nidBytes array.
			memcpy(nidStore.nidBytes, dest, nidLen);
			mmgr->deallocate(dest);
		}
		return;
	}
	
	xmlbyte_t pb, nb;
	const xmlbyte_t *p = prev->getBytes();
	xmlbyte_t *d = dest;
	const xmlbyte_t *n = next->getBytes();
	
	// copy the indentical prefix of prev and next
	while((pb = *p) == (nb = *n)) {
		// This would mean next and prev were identical!
		DBXML_ASSERT(pb != 0);
		*d = pb;
		++d; ++p; ++n;
	}
	
	DBXML_ASSERT(nb > pb); // As next should be greater than prev
	
	pb = idbyte2num(pb);
	nb = idbyte2num(nb);
	// Special case where nb equals NS_ID_FIRST,
	// and pb equals 0 - in this case, idbyte2num
	// maps them both onto the number 0.
	while(pb == nb) {
		*(d++) = num2idbyte(nb);
		pb = nextIdbyteAsNum(p);
		++n;
		nb = idbyte2num(*n);
	}
	
	// Pick id so that it is half way between next and prev
	// NOTE: commented-out code will use the "old" algorithm
	// that doesn't consider append vs insertAfter vs insertBefore
	xmlbyte_t avg = avgbyte((int)pb, (int)nb, itype);
	//xmlbyte_t avg = ((int)pb + (int)nb) >> 1;
	*(d++) = num2idbyte(avg);
	while(avg == pb) {
		pb = nextIdbyteAsNum(p);
		avg = avgbyte((int)pb, idbyte2num(NS_ID_LAST) + 1, itype);
		//avg = ((int)pb + idbyte2num(NS_ID_LAST) + 1) >> 1;
		*(d++) = num2idbyte(avg);
	}
	*(d++) = 0; // null terminate
	
	// NOTE: this bypasses setLen*() -- see assertions
	// Use maxlen for comparison, as it was used to determine
	// allocation
	nidLen = (d - dest);
	if(nidLen > NID_BYTES_SIZE) {
		setLenAlloc(nidLen);
		DBXML_ASSERT(isAlloced());
	} else if (maxlen > NID_BYTES_SIZE) {
		// maintain invariant that smaller NIDs are
		// in nidBytes array.
		memcpy(nidStore.nidBytes, dest, nidLen);
		mmgr->deallocate(dest);
	}
#if 0
	std::cout << "Between, where is " << itype <<": ";
	displayNid(std::cout);
	std::cout << std::endl;
#endif
}

void
NsNid::setDocRootNid(MemoryManager *mmgr)
{
	if (mmgr)
		freeNid(mmgr);
	nidStore.nidBytes[0] = NID_ROOT_SIZE;
	nidStore.nidBytes[1] = NS_ROOT_ID;
	nidStore.nidBytes[2] = 0;
	nidLen = 3;
}

void
NsNid::setSpecialNid(XER_NS MemoryManager *mmgr, xmlbyte_t id)
{
	if (mmgr)
		freeNid(mmgr);
	nidStore.nidBytes[0] = NID_ROOT_SIZE;
	nidStore.nidBytes[1] = id;
	nidStore.nidBytes[2] = 0;
	nidLen = 3;
}

bool
NsNid::isDocRootNid() const
{
	return ((getBytes()[1] == NS_ROOT_ID) &&
		(getLen() == 3));
}

//static -- used only for one-byte ids -- metadata, etc
void
NsNid::initNid(xmlbyte_t *buf, xmlbyte_t id)
{
	buf[0] = 1;
	buf[1] = id;
	buf[2] = 0;
}

void
NsNid::copyNid(MemoryManager *mmgr, const NsNid *from)
{
	copyNid(mmgr, from->getBytes(), from->getLen());
}

void
NsNid::copyNid(XER_NS MemoryManager *mmgr, const xmlbyte_t *fromPtr, uint32_t newLen)
{
	// reuse existing memory if possible
	if (!newLen) {
		freeNid(mmgr);
		return;
	}
	if (newLen > NID_BYTES_SIZE) {
		if (getLen() < newLen) {
			// cannot reuse
			freeNid(mmgr);
			nidStore.nidPtr = (xmlbyte_t *)mmgr->allocate(newLen);
			if (!nidStore.nidPtr)
				NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
							 "copyNid failed to allocate memory",
							 __FILE__, __LINE__);
			setLenAlloc(newLen);
		} else {
			setLen(newLen); // maintains alloc'd status
		}
		memcpy(nidStore.nidPtr, fromPtr, newLen);
	} else {
		freeNid(mmgr);
		memcpy(nidStore.nidBytes, fromPtr, newLen);
		setLen(newLen);
	}
	DBXML_ASSERT(getLen() == newLen);
}

// static
void NsNid::displayNid(std::ostream &out, const char *buf, int len) {
	// find out where the decimal point is
	char n = buf[0] + 0x30;

	++buf;
	--len;
	for(int i = 0; i < len; ++i, ++buf) {
		if(i == n) out << ".";

		unsigned char b = *buf;
		int hn = (b & 0xF0) >> 4;
		int ln = b & 0x0F;
		out << (char)(hn < 10 ? '0' + hn : 'A' + hn - 10);
		out << (char)(ln < 10 ? '0' + ln : 'A' + ln - 10);
	}
}

// NsNidGen methods

NsNidGen::NsNidGen()
	: _idDigit(NID_INITIAL_DIGIT)
{
	memset(_idBuf, 0, NIDBUF_SIZE);
	
	_idBuf[1] = NS_ROOT_ID;
	_idBuf[0] = NID_ROOT_SIZE;
}

void
NsNidGen::nextId(MemoryManager *mmgr, NsNid *id)
{
	xmlbyte_t *dest;
	uint32_t len = _idDigit + 3; // incl leading byte and trailing null
	// ensure enough space for the id
	if (len > NID_BYTES_SIZE) {
		dest = id->nidStore.nidPtr =
			(xmlbyte_t *) mmgr->allocate(len);
		if (!dest)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				"nextId failed to allocate memory",
						 __FILE__, __LINE__);
		id->setLenAlloc(len);
	} else {
		dest = id->nidStore.nidBytes;
		id->setLen(len);
	}
	// copy the id
	memcpy(dest, _idBuf, len);
	// Increment the id, for next time
	_nextId(_idBuf, _idDigit); 
}
