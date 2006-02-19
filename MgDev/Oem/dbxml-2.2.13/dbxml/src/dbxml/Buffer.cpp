//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Buffer.cpp,v 1.35 2005/08/16 13:29:24 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "Buffer.hpp"

#include <sstream>
#include <cassert>

using namespace std;
using namespace DbXml;

Buffer::Buffer()
	: bufferSize_(0),
	pBuffer_(0),
	pCursor_(0),
	pOccupancy_(0),
	ownsMemory_(true)
{}

Buffer::Buffer(const void *p, size_t size)
	: bufferSize_(0),
	pBuffer_(0),
	pCursor_(0),
	pOccupancy_(0),
	ownsMemory_(true)
{
	set(const_cast<void*>(p), size);
}

Buffer::Buffer (const Buffer& buffer, size_t extra)
	: bufferSize_(0),
	pBuffer_(0),
	pCursor_(0),
	pOccupancy_(0),
	ownsMemory_(true)
{
	if (extra > 0) {
		expandBuffer(buffer.getOccupancy() + extra);
		write(buffer.getBuffer(), buffer.getOccupancy());
	} else {
		*this = buffer;
	}
}

Buffer::Buffer(const void *p, size_t size, bool wrapper)
	: bufferSize_(0),
	pBuffer_(0),
	pCursor_(0),
	pOccupancy_(0),
	ownsMemory_(!wrapper)
{
	set(const_cast<void*>(p), size);
}

Buffer::~Buffer()
{
	deleteBuffer();
}

void Buffer::set(void *p, size_t nSize)
{
	// Deal with any previous buffer details.
	//
	deleteBuffer();

	bufferSize_ = nSize;
	if (nSize != 0) {
		if (ownsMemory_)
			pBuffer_ = new char [nSize];
		else
			pBuffer_ = p;
		if (p != 0) {
			if (ownsMemory_)
				::memcpy(pBuffer_, p, nSize);

			// Copied in something meaningful, therefore occupied.
			pOccupancy_ = static_cast<char*>(pBuffer_) + nSize;
		} else
			// Just allocated space, therefore no occupancy yet.
			pOccupancy_ = pBuffer_;
	} else {
		pBuffer_ = 0;
		pOccupancy_ = 0;
	}
	pCursor_ = pBuffer_;
}

size_t Buffer::readSeek(void* p, size_t n)
{
	// Tried to read something from a null pointer.
	// DBXML_ASSERT((n==0) || ((n>0) && (p!=0)));

	// Tried to read off the end of the data in the buffer.
	//REQUIRE( pCursor_+n <= pOccupancy_ );

	size_t toread = n;
	size_t remaining = static_cast<char*>(pOccupancy_) - static_cast<char*>(pCursor_);
	if (n > remaining) {
		toread = remaining;
	}
	if (toread > 0) {
		if (p)
			::memcpy(p, pCursor_, toread);
		pCursor_ = static_cast<char*>(pCursor_) + toread;
	}
	return toread;
}

size_t Buffer::read(size_t offset, void* p, size_t n)
{
	// Tried to read something from a null pointer.
	// DBXML_ASSERT((n==0) || ((n>0) && (p!=0)));

	// Tried to read off the end of the data in the buffer.
	//REQUIRE( pCursor_+n <= pOccupancy_ );

	char *startPos = static_cast<char*>(pBuffer_) + offset;

	size_t toread = n;
	size_t remaining = static_cast<char*>(pOccupancy_) - startPos;
	if (n > remaining) {
		toread = remaining;
	}
	if (toread > 0) {
		::memcpy(p, startPos, toread);
	}
	return toread;
}

size_t Buffer::write(const void* p, size_t n)
{
  size_t offset;
  size_t towrite = reserve(offset, n);
  return write(offset, p, towrite);
}

size_t Buffer::reserve(size_t &offset, size_t n)
{
	// Tried to write something to a null pointer.
	// DBXML_ASSERT((n==0) || ((n>0) && (p!=0))));

	// If we tried to write off the end then expand the buffer.
	//
	if (static_cast<char*>(pCursor_) + n >
	    static_cast<char*>(pBuffer_) + bufferSize_) {
		int need = static_cast<char*>(pCursor_) + n - static_cast<char*>(pBuffer_);
		expandBuffer(need);
	}

	size_t towrite = n;
	size_t remaining = (static_cast<char*>(pBuffer_) + bufferSize_) - static_cast<char*>(pCursor_);
	if (n>remaining) {
		towrite = remaining;
	}
	if (towrite>0) {
		offset = (size_t)((char*)pCursor_ - (char*)pBuffer_);
		pCursor_ = static_cast<char*>(pCursor_) + towrite;
		pOccupancy_ = (pOccupancy_ > pCursor_ ? pOccupancy_ : pCursor_);
	}

	return towrite;
}

size_t Buffer::write(size_t offset, const void *p, size_t n)
{
	DBXML_ASSERT(n == 0 || (void*)((char*)pBuffer_ + offset + n) <= pOccupancy_);

	if (n>0) {
		::memcpy((void*)((char*)pBuffer_ + offset), p, n);
	}

	return n;
}

size_t Buffer::getOccupancy() const
{
	ptrdiff_t n = 0;
	if (pOccupancy_ >= pBuffer_) {
		n = static_cast<char*>(pOccupancy_) - static_cast<char*>(pBuffer_);
	}
	return (size_t)n;
}

size_t Buffer::getCursorPosition() const
{
	ptrdiff_t n = 0;
	if (pCursor_ >= pBuffer_) {
		n = static_cast<char*>(pCursor_) - static_cast<char*>(pBuffer_);
	}
	return (size_t)n;
}

Buffer& Buffer::operator= (const Buffer& buffer)
{
	// Result is same sized buffer with same occupation.
	// b1 = b2; DBXML_ASSERT( b1 == b2 ); will work
	//
	if (this != &buffer) {
		set(buffer.getBuffer(), buffer.getSize());
		pOccupancy_ = static_cast<char*>(pBuffer_) + buffer.getOccupancy();
	}
	return *this;
}

int Buffer::operator==(const Buffer& buffer) const
{
	return (getOccupancy() == buffer.getOccupancy() && 0 == ::memcmp(pBuffer_, buffer.pBuffer_, getOccupancy()));
}

bool Buffer::operator<(const Buffer& buffer) const
{
  int i = ::memcmp(pBuffer_, buffer.pBuffer_,
                   getOccupancy() < buffer.getOccupancy() ?
                   getOccupancy() : buffer.getOccupancy());
  if(i == 0) {
    return getOccupancy() < buffer.getOccupancy();
  }
  else {
    return i < 0;
  }
}

static const unsigned long BUFFER_TOSTRING_BUFFERSIZE = 64;
string Buffer::asString() const
{
	size_t displayBytes = (getOccupancy() < BUFFER_TOSTRING_BUFFERSIZE ? getOccupancy() : BUFFER_TOSTRING_BUFFERSIZE);
	char p2[BUFFER_TOSTRING_BUFFERSIZE*2 + 4];
	char p3[BUFFER_TOSTRING_BUFFERSIZE + 4];
	unsigned char *b = static_cast<unsigned char*>(pBuffer_);
	char *p = p2;
	char *p1 = p3;
	for (size_t i = 0;i < displayBytes;++i) {
		int hn = (*b & 0xF0) >> 4;
		int ln = *b & 0x0F;
		*p++ = (hn < 10 ? '0' + hn : 'a' + hn - 10);
		*p++ = (ln < 10 ? '0' + ln : 'a' + ln - 10);
		*p1++ = ((*b >= 0x80 || iscntrl(*b)) ? '.' : *b);
		++b;
	}
	if(displayBytes < getOccupancy()) {
		*p++ = '.';
		*p++ = '.';
		*p++ = '.';
		*p1++ = '.';
		*p1++ = '.';
		*p1++ = '.';
	}
	*p++ = '\0';
	*p1++ = '\0';
	ostringstream os1;
	os1 << "Size=" << hex << bufferSize_;
	os1 << " Occ=" << hex << getOccupancy();
	os1 << " Hex=" << p2;
	os1 << " Text=" << p3;
	return os1.str();
}

string Buffer::asStringBrief() const
{
	size_t displayBytes = (getOccupancy() < BUFFER_TOSTRING_BUFFERSIZE ? getOccupancy() : BUFFER_TOSTRING_BUFFERSIZE);
	char p2[BUFFER_TOSTRING_BUFFERSIZE*2 + 4];
	unsigned char *b = static_cast<unsigned char*>(pBuffer_);
	char *p = p2;
	for (size_t i = 0;i < displayBytes;++i) {
		int hn = (*b & 0xF0) >> 4;
		int ln = *b & 0x0F;
		*p++ = (hn < 10 ? '0' + hn : 'a' + hn - 10);
		*p++ = (ln < 10 ? '0' + ln : 'a' + ln - 10);
		++b;
	}
	if(displayBytes < getOccupancy()) {
		*p++ = '.';
		*p++ = '.';
		*p++ = '.';
	}
	*p++ = '\0';
	ostringstream os1;
	os1 << "Size=" << hex << getOccupancy();
	os1 << " Hex=" << p2;
	return os1.str();
}

void Buffer::deleteBuffer()
{
	if (ownsMemory_) {
		delete [] static_cast<char*>(pBuffer_);
		pBuffer_ = 0;
	}
}

void Buffer::expandBuffer(size_t newSize)
{
	if (ownsMemory_) {
		size_t newBufferSize = bufferSize_ << 1;
		newBufferSize = (newSize > newBufferSize ? newSize : newBufferSize);
		void* newBuffer = new char [newBufferSize];
		::memcpy(newBuffer, pBuffer_, getOccupancy());
		pCursor_ = static_cast<char*>(newBuffer) + getCursorPosition();
		pOccupancy_ = static_cast<char*>(newBuffer) + getOccupancy();
		deleteBuffer();
		pBuffer_ = newBuffer;
		bufferSize_ = newBufferSize;
	} else {
		DBXML_ASSERT(newSize <= bufferSize_);
	}
}
