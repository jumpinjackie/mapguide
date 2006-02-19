//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// Some of the transcoding code is from Xerces, and is
// under the Apache license:
//
// The Apache Software License, Version 1.1
//
// Copyright (c) 1999-2004 The Apache Software Foundation.  All rights
//  reserved.
//
// $Id: NsUtil.cpp,v 1.57 2005/12/14 17:57:07 jsnelson Exp $
//

#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include "Manager.hpp"
#include <db_cxx.h>
#include <xercesc/util/XMLUTF8Transcoder.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#ifdef NS_DEBUG
#include "UTF8.hpp"
#include "NsDom.hpp"
#endif

using namespace DbXml;

XERCES_CPP_NAMESPACE_USE

/* macros */
#define nsFlags(node) (node)->nd_header.nh_flags
#define nsSetFlag(node, flag) nsFlags(node) |= (flag)
#define nsFlagSet(node, flag) (nsFlags(node) & (flag))

extern "C" {

typedef struct {
	union idStore_u {
		xmlbyte_t *idPtr;
		xmlbyte_t idBytes[4];
	} idStore;
	uint32_t idLen;
} old_nid_t;

typedef struct {
	uint32_t ce_textIndex;
	old_nid_t ce_id;
} old_nsChildEntry_t;

};

//
// Transcoding
//

/*
 * Transcoding globals (from Xerces)
 *
 *  gUTFBytes
 *      A list of counts of trailing bytes for each initial byte in the input.
 *
 *  gUTFByteIndicator
 *      For a UTF8 sequence of n bytes, n>=2, the first byte of the
 *      sequence must contain n 1's followed by precisely 1 0 with the
 *      rest of the byte containing arbitrary bits.  This array stores
 *      the required bit pattern for validity checking.
 *
 *  gUTFByteIndicatorTest
 *      When bitwise and'd with the observed value, if the observed
 *      value is correct then a result matching gUTFByteIndicator will
 *      be produced.
 *
 *  gUTFOffsets
 *      A list of values to offset each result char type, according to how
 *      many source bytes when into making it.
 *
 *  gFirstByteMark
 *      A list of values to mask onto the first byte of an encoded sequence,
 *      indexed by the number of bytes used to create the sequence.
 */

static const xmlbyte_t gUTFBytes[256] =
{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    ,   3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

int
NsUtil::nsCharSizeUTF8(xmlbyte_t firstByte)
{
	return gUTFBytes[firstByte] + 1;
}

static const xmlbyte_t gUTFByteIndicator[6] =
{
    0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

static const xmlbyte_t gUTFByteIndicatorTest[6] =
{
    0x80, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE
};

static const uint32_t gUTFOffsets[6] =
{
    0, 0x3080, 0xE2080, 0x3C82080, 0xFA082080, 0x82082080
};

static const xmlbyte_t gFirstByteMark[7] =
{
	0x00, 0x00, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc
};

/*
 * The only issues in text and attribute values are
 * for the characters: & and <
 * >, ', and " can be parsed in most contexts in XML.
 * The only issue is " vs ' in attribute values.  DB XML
 * uses the convention of attr="value" so it will escape
 * any double-quote (") in attribute values, on output.
 */
#define CHECK_ENT(c) \
if (hasEntity && ((c) < chLatin_A)) {  \
	if (((c) == chAmpersand) || ((c) == chOpenAngle)) \
		*hasEntity = true; \
	else if ((c) == chDoubleQuote && type == isAttrVal) \
		*hasEntity = true; \
}

/*
 * transcode from UTF-16 to UTF-8
 *  o will (re)allocate destination buffer if not present.
 *  start out with 3x the number of characters.  This memory
 *  should not hang around long.
 *  o returns number of *bytes* in the transcoded buffer.
 *  o code is derived from the Xerces class, XMLUTF8Transcoder
 *  o returns true or false in hasEntity if a special entity is
 *  encountered: &, <, >, ", '
 *  No error checking -- the UTF-16 input was created by Xerces, and
 *  is trusted.
 */

int
NsUtil::nsToUTF8(MemoryManager *mmgr, xmlbyte_t **dest, const xmlch_t *src,
		 uint32_t nchars, uint32_t maxbytes, bool *hasEntity,
		 enum checkType type)
{
	uint32_t bufsize;
	xmlbyte_t *outPtr = *dest;
	if (hasEntity)
		*hasEntity = false;
	if (!outPtr) {
		bufsize = nchars * 3;
		// caller needs to know if using manager or new for allocation
		if (mmgr)
			outPtr = (xmlbyte_t *) mmgr->allocate(bufsize);
		else
			outPtr = new xmlbyte_t[bufsize];
		if (!outPtr)
			nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "Unable to allocate memory for transcoding",
					 __FILE__, __LINE__);
		*dest = outPtr;
	} else {
		bufsize = maxbytes;
	}
	const xmlch_t *srcEnd = src + nchars;
	const xmlbyte_t *outEnd = outPtr + bufsize;

	while (src < srcEnd) {
		uint32_t curVal = *src;
		// deal with surrogates
		if ((curVal >= 0xd800) && (curVal <= 0xdbff)) {
			curVal =  ((curVal - 0xd800) << 10)
				+ ((*(src + 1) - 0xdc00) + 0x10000);
			src++;
		}
		src++;
		CHECK_ENT(curVal);
		// how many bytes
		uint32_t nbytes;
		if (curVal < 0x80)
			nbytes = 1;
		else if (curVal < 0x800)
			nbytes = 2;
		else if (curVal < 0x10000)
			nbytes = 3;
		else if (curVal < 0x200000)
			nbytes = 4;
		else if (curVal < 0x4000000)
			nbytes = 5;
		else if (curVal <= 0x7FFFFFFF)
			nbytes = 6;

		if (outPtr + nbytes > outEnd) {
			// reallocate (double size), or fail
			if (!mmgr)
				nsThrowException(XmlException::INTERNAL_ERROR,
						 "nsToUTF8: no memory manager",
						 __FILE__, __LINE__);
			uint32_t count = (outPtr - *dest);
			xmlbyte_t *newmem =
				(xmlbyte_t *) mmgr->allocate(bufsize << 1);
			if (!newmem)
				nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "nsToUTF8 failed to allocate",
						 __FILE__, __LINE__);
			memcpy(newmem, *dest, bufsize);
			bufsize <<= 1;
			mmgr->deallocate(*dest);
			*dest = newmem;
			outEnd = newmem + bufsize;
			outPtr = newmem + count;
		}
		/* optimize 1-byte case */
		if (nbytes == 1) {
			*outPtr++ = (xmlbyte_t) curVal;
		} else {
			outPtr += nbytes; // work backwards
			switch(nbytes) {
			case 6 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 5 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 4 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 3 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 2 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 1 : *--outPtr = xmlbyte_t
					 (curVal | gFirstByteMark[nbytes]);
			}
			outPtr += nbytes; // add bytes back again
		}
	} // while
	return (outPtr - *dest);
}
static inline void checkTrailingBytes(const xmlbyte_t toCheck)
{
	if ((toCheck & 0xC0) != 0x80)
		NsUtil::nsThrowException(XmlException::INVALID_VALUE,
				 "checkTrailingBytes: bad utf-8 encoding",
				 __FILE__, __LINE__);
}

/*
 * _nsFromUTF8 -- internal method for transcoding from utf-8 to
 *  utf-16
 *
 * This method returns how much of the original
 * utf-8 string is consumed in the eaten paramter.
 * This allows long strings to be transcoded in chunks.
 *
 * The code is basically structured to allow multiple chunk calls.
 * This is derived from Xerces transcoding code.
 *
 * The string is not entirely trusted as a valid utf-8 encoding.
 */
static int
_nsFromUTF8(xmlch_t *dest, const unsigned char *src,
	    uint32_t nbytes, uint32_t maxchars, uint32_t &eaten)
{
	DBXML_ASSERT(nbytes);

	const xmlbyte_t*  srcP = (const xmlbyte_t *) src;
	const xmlbyte_t*  endP = srcP + nbytes;
	xmlch_t *outP = dest;
	xmlch_t *outEnd = outP + maxchars;

	// loop until done with input, or out of space
	while ((srcP < endP) && (outP < outEnd)) {
		// ascii subset is simple
		if (*srcP <= 127) {
			*outP++ = *srcP++;
			continue;
		}

		// how many trailing src bytes will this sequence require
		const uint32_t trailingBytes = gUTFBytes[*srcP];

		// if not enough source to do an entire character, break out
		// before processing the current one
		if (srcP + trailingBytes >= endP)
			break;

		// start constructing the value.
		// Don't entirely trust encoding -- do validation checks
		// test first byte
		if((gUTFByteIndicatorTest[trailingBytes] & *srcP) !=
		   gUTFByteIndicator[trailingBytes]) {
			NsUtil::nsThrowException(XmlException::INVALID_VALUE,
					 "nsFromUTF8: bad utf-8 encoding",
					 __FILE__, __LINE__);
		}

		/***
		 * See http://www.unicode.org/reports/tr27/
		 *
		 * Table 3.1B. lists all of the byte sequences that
		 * are legal in UTF-8. A range of byte values such as
		 * A0..BF indicates that any byte from A0 to BF (inclusive)
		 * is legal in that position.
		 * Any byte value outside of the ranges listed is illegal.
		 *
		 * For example,the byte sequence <C0 AF> is illegal
		 * since C0 is not legal in the 1st Byte column.
		 *
		 * The byte sequence <E0 9F 80> is illegal since in the row
		 * where E0 is legal as a first byte, 9F is not legal
		 * as a second byte.
		 *
		 * The byte sequence <F4 80 83 92> is legal,
		 * since every byte in that sequence matches
		 * a byte range in a row of the table (the last row).
		 *
		 * The table is reproduced here for reference
		 *
		 * Table 3.1B. Legal UTF-8 Byte Sequences
		 * Code Points       1st Byte 2nd Byte 3rd Byte 4th Byte
		 * ==========================================================
		 * U+0000..U+007F      00..7F
		 * ----------------------------------------------------------
		 * U+0080..U+07FF      C2..DF  80..BF
		 *
		 * ----------------------------------------------------------
		 * U+0800..U+0FFF      E0      A0..BF   80..BF
		 *                             --
		 *
		 * U+1000..U+FFFF      E1..EF  80..BF   80..BF
		 *
		 * ----------------------------------------------------------
		 * U+10000..U+3FFFF    F0      90..BF   80..BF   80..BF
		 *                             --
		 * U+40000..U+FFFFF    F1..F3  80..BF   80..BF   80..BF
		 * U+100000..U+10FFFF  F4      80..8F   80..BF   80..BF
		 *                                 --
		 * ==========================================================
		 *
		 * Cases where a trailing byte range is not 80..BF are
		 * underlined in the table to draw attention to them.
		 * These occur only in the second byte of a sequence.
		 */
		uint32_t tmpVal = 0;

		switch(trailingBytes) {
		case 1 :
			// UTF-8:   [110y yyyy] [10xx xxxx]
			// Unicode: [0000 0yyy] [yyxx xxxx]
			//
			// 0xC0, 0xC1 has been filtered out
			checkTrailingBytes(*(srcP+1));

			tmpVal = *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;
			break;
		case 2 :
			// UTF-8:   [1110 zzzz] [10yy yyyy] [10xx xxxx]
			// Unicode: [zzzz yyyy] [yyxx xxxx]
			//
			if (( *srcP == 0xE0) && ( *(srcP+1) < 0xA0)) {
				NsUtil::nsThrowException(
					XmlException::INVALID_VALUE,
					"nsFromUTF8: bad utf-8 encoding",
					__FILE__, __LINE__);
			}

			checkTrailingBytes(*(srcP+1));
			checkTrailingBytes(*(srcP+2));

			//
			// See D36 (a)-(c) of
			//  http://www.unicode.org/reports/tr27/
			//
			//irregular three bytes sequence
			// that is zzzzyy matches leading surrogate tag
			// 110110 or trailing surrogate tag 110111
			//
			// *srcP=1110 1101
			// *(srcP+1)=1010 yyyy or
			// *(srcP+1)=1011 yyyy
			//
			// 0xED 1110 1101
			// 0xA0 1010 0000

			if ((*srcP == 0xED) && (*(srcP+1) >= 0xA0)) {
				NsUtil::nsThrowException(
					XmlException::INVALID_VALUE,
					"nsFromUTF8: bad utf-8 encoding",
					__FILE__, __LINE__);
			}

			tmpVal = *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;

			break;
		case 3 :
			// UTF-8:[1111 0uuu] [10uu zzzz] [10yy yyyy] [10xx xxxx]*
			// Unicode: [1101 10ww] [wwzz zzyy] (high surrogate)
			//          [1101 11yy] [yyxx xxxx] (low surrogate)
			//          * uuuuu = wwww + 1
			//
			if (((*srcP == 0xF0) && (*(srcP+1) < 0x90)) ||
			    ((*srcP == 0xF4) && (*(srcP+1) > 0x8F))) {
				NsUtil::nsThrowException(
					XmlException::INVALID_VALUE,
					"nsFromUTF8: bad utf-8 encoding",
					__FILE__, __LINE__);
			}

			checkTrailingBytes(*(srcP+1));
			checkTrailingBytes(*(srcP+2));
			checkTrailingBytes(*(srcP+3));

			tmpVal = *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;

			break;
		default: // trailingBytes > 3

			/***
			 * The definition of UTF-8 in Annex D of ISO/IEC
			 * 10646-1:2000 also allows for the use of five- and
			 * six-byte sequences to encode characters that are
			 * outside the range of the Unicode character set;
			 * those five- and six-byte sequences are illegal
			 * for the use of UTF-8 as a transformation of Unicode
			 * characters. ISO/IEC 10646 does not allow mapping
			 * of unpaired surrogates, nor U+FFFE and
			 * U+FFFF (but it does allow other noncharacters).
			 ***/
			NsUtil::nsThrowException(
				XmlException::INVALID_VALUE,
				"nsFromUTF8: bad utf-8 encoding",
				__FILE__, __LINE__);
			break;
		}

		tmpVal -= gUTFOffsets[trailingBytes];

		//
		//  If it will fit into a single char, then put it in. Otherwise
		//  encode it as a surrogate pair. If its not valid, use the
		//  replacement char.
		//
		if (!(tmpVal & 0xFFFF0000)) {
			*outP++ = (xmlch_t) tmpVal;
		} else if (tmpVal > 0x10FFFF) {
			NsUtil::nsThrowException(
				XmlException::INVALID_VALUE,
				"nsFromUTF8: bad utf-8 encoding",
				__FILE__, __LINE__);
		} else {
			// if there's not enough room for all chars,
			// break out.
			if (outP + 1 >= outEnd)
				break; // this char never happened

			// Store the leading surrogate char
			tmpVal -= 0x10000;
			*outP++ = (xmlch_t)((tmpVal >> 10) + 0xD800);

			// the trailing char
			*outP++ = (xmlch_t)((tmpVal & 0x3FF) + 0xDC00);
		}
	}

	// Update the bytes eaten
	eaten = srcP - src;

	// Return the characters read
	return outP - dest;
}

/*
 * transcode from UTF-8 to UTF-16.  Return number of UTF-16
 * characters that resulted, including NULLs, if any are included
 * in nbytes.
 */
int
NsUtil::nsFromUTF8(MemoryManager *mmgr, xmlch_t **dest, const xmlbyte_t *src,
		  uint32_t nbytes, uint32_t maxchars)
{
	uint32_t bufsize;
	xmlch_t *outPtr = *dest;
	uint32_t eaten;
	if (!outPtr) {
		bufsize = nbytes << 1; // multiply by 2
		// caller needs to know if using manager or new for allocation
		if (mmgr)
			outPtr = (xmlch_t *) mmgr->allocate(bufsize);
		else
			outPtr = new xmlch_t[nbytes];
		if (!outPtr)
			nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "nsFromUTF8: allocation failed",
					 __FILE__, __LINE__);
		*dest = outPtr;
	}
	if (!maxchars)
		maxchars = nbytes;

	return _nsFromUTF8(outPtr, src, nbytes, maxchars, eaten);
}

// Parse utilities to create nsNode_t structures

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

int
NsUtil::nsCompareIds(const nid_t *id1, const nid_t *id2)
{
	// Assume null termination of node ids
	int res = 0;
	const xmlbyte_t *p1 = (const xmlbyte_t *)nsNode(id1);
	const xmlbyte_t *p2 = (const xmlbyte_t *)nsNode(id2);
	while((res = (int)*p1 - (int)*p2) == 0 &&
	      *p1 != 0) {
		++p1; ++p2;
	}
	return res;
}

void
NsUtil::nsBetweenId(MemoryManager *mmgr, nid_t *id,
		    const nid_t *prev, const nid_t *next)
{
	DBXML_ASSERT(prev);

	nsFreeId(mmgr, id);

	int plen = prev->idLen;
	int nlen = next ? next->idLen : 0;
	if(plen == 0 && nlen == 0) {
		memset(id, 0, sizeof(nid_t));
		return;
	}

	// maximum length of the between id is one
	// greater than the maximum length of the
	// prev or next id
	int maxlen = plen > nlen ? plen : nlen;
	maxlen += 1;

	// Allocate memory to build the id in
	xmlbyte_t *dest = (xmlbyte_t*)mmgr->
		allocate(maxlen * sizeof(xmlbyte_t));

	xmlbyte_t pb, nb;
	const xmlbyte_t *p = nsNode(prev);
	xmlbyte_t *d = dest;
	if(nlen == 0) {
		DBXML_ASSERT(*p);

		// copy first digit (ids with one digit are reserved
		// for "special" nodes)
		*(d++) = *(p++);

		// copy prev, until we find a digit that can be increased
		while((pb = *p) != 0 && pb == NS_ID_LAST) {
			*d = pb;
			++d; ++p;
		}

		if(pb == 0) {
			// Add an extra digit
			*(d++) = (NS_ID_FIRST + NS_ID_LAST) >> 1;
		}
		else {
			// Increment the next digit
			*(d++) = num2idbyte(idbyte2num(pb) + 1);
		}
	}
	else {
		const xmlbyte_t *n = nsNode(next);

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
		xmlbyte_t avg = ((int)pb + (int)nb) >> 1;
		*(d++) = num2idbyte(avg);
		while(avg == pb) {
			pb = nextIdbyteAsNum(p);
			avg = ((int)pb + idbyte2num(NS_ID_LAST) + 1) >> 1;
			*(d++) = num2idbyte(avg);
		}
	}

	// copy the buffer to the destination id
	*(d++) = 0; // null terminate
	id->idLen = (d - dest);
	if(id->idLen > NID_BYTES_SIZE) {
		id->idStore.idPtr = dest;
	}
	else {
		memcpy(id->idStore.idBytes, dest, id->idLen * sizeof(xmlbyte_t));
		mmgr->deallocate(dest);
	}
}

void
NsUtil::nsFreeId(MemoryManager *mmgr, nid_t *id)
{
	if (id->idLen > NID_BYTES_SIZE)
		mmgr->deallocate(id->idStore.idPtr);
}

void
NsUtil::nsCopyId(MemoryManager *mmgr, nid_t *to, const nid_t *from)
{
	nsFreeId(mmgr, to);
	uint32_t len = from->idLen;
	if (len > NID_BYTES_SIZE) {
		to->idStore.idPtr = (xmlbyte_t *)mmgr->allocate(len);
		if (!to->idStore.idPtr)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				"copyId failed to allocate memory",
				__FILE__, __LINE__);
		memcpy(to->idStore.idPtr, from->idStore.idPtr, len);
	} else
		memcpy(to->idStore.idBytes, from->idStore.idBytes, len);
	to->idLen = len;
}

void
NsUtil::nsCopyText(MemoryManager *mmgr, nsText_t *to, const nsText_t *from,
		   bool isUTF8)
{
	if(to->t_chars != 0) mmgr->deallocate(to->t_chars);

	if(from->t_chars == 0) {
		to->t_chars = 0;
		to->t_len = 0;
	}
	else {
		uint32_t len = from->t_len + 1;
		if(isUTF8) len *= sizeof(xmlbyte_t);
		else len *= sizeof(xmlch_t);

		to->t_chars = mmgr->allocate(len);
		if (!to->t_chars)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "copyId failed to allocate memory",
						 __FILE__, __LINE__);
		memcpy(to->t_chars, from->t_chars, len);
		to->t_len = from->t_len;
	}
}

void
NsUtil::nsCopyName(MemoryManager *mmgr, nsName_t *to, const nsName_t *from,
		   bool isUTF8)
{
	to->n_prefix = from->n_prefix;
	to->n_type = from->n_type;
	nsCopyText(mmgr, &to->n_text, &from->n_text, isUTF8);
}

nsAttrList_t *
NsUtil::nsAllocAttrList(MemoryManager *mmgr, uint32_t attrCount)
{
	if (attrCount != 0) {
		size_t msize = sizeof(nsAttrList_t) +
			((attrCount - 1) * sizeof(nsAttr_t));
		nsAttrList_t *attrlist =
			(nsAttrList_t *)mmgr->allocate(msize);
		if (!attrlist)
			nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "nsAllocAttrList: allocation failed",
					 __FILE__, __LINE__);
		memset(attrlist, 0, msize);
		attrlist->al_max = attrCount;
		return attrlist;
	}
	return 0;
}

/*
 * reallocate list, copying array of nsAttr_t structs
 */
static nsAttrList_t *
_nsReallocAttrList(MemoryManager *mmgr, nsAttrList_t *list)
{
	if (list == 0)
		return NsUtil::nsAllocAttrList(mmgr, NS_NTEXT_DEFAULT);

	/* realloc -- double size */
	uint32_t oldsize = sizeof(nsAttrList_t) +
		((list->al_max - 1) * sizeof(nsAttr_t));

	list->al_max <<= 1;
	nsAttrList_t *result = NsUtil::nsAllocAttrList(mmgr, list->al_max);

	memcpy(result, list, oldsize);
	mmgr->deallocate(list);
	return result;
}

nsAttrList_t *
NsUtil::nsCopyAttrList(MemoryManager *mmgr, const nsAttrList_t *attrs, bool isUTF8)
{
	uint32_t attrCount = attrs->al_nattrs;
	uint32_t amax = attrCount << 1; // leave some space for more attr nodes...
	size_t msize = sizeof(nsAttrList_t) +
		((amax - 1) * sizeof(nsAttr_t));
	nsAttrList_t *result =
		(nsAttrList_t *)mmgr->allocate(msize);
	if (!result)
		nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "nsCopyAttrList: allocation failed",
				 __FILE__, __LINE__);

	memset(result, 0, msize);
	result->al_len = attrs->al_len;
	result->al_nattrs = attrCount;
	result->al_max = amax;

	if(isUTF8) {
		for(uint32_t i = 0; i < attrCount; ++i) {
			nsCopyName(mmgr, &result->al_attrs[i].a_name, &attrs->al_attrs[i].a_name, isUTF8);
			xmlbyte_t *value = (xmlbyte_t*)result->al_attrs[i].a_name.n_text.t_chars;
			while(*value++); // Get past the first null, to the value
			result->al_attrs[i].a_value = value;
			result->al_attrs[i].a_flags = attrs->al_attrs[i].a_flags;
			result->al_attrs[i].a_uri = attrs->al_attrs[i].a_uri;
		}
	} else {
		for(uint32_t i = 0; i < attrCount; ++i) {
			nsCopyName(mmgr, &result->al_attrs[i].a_name, &attrs->al_attrs[i].a_name, isUTF8);
			xmlch_t *value = (xmlch_t*)result->al_attrs[i].a_name.n_text.t_chars;
			while(*value++); // Get past the first null, to the value
			result->al_attrs[i].a_value = value;
			result->al_attrs[i].a_flags = attrs->al_attrs[i].a_flags;
			result->al_attrs[i].a_uri = attrs->al_attrs[i].a_uri;
		}
	}

	return result;
}

/*
 * allocate a node.  Since attribute count is always known,
 * allocate space for it, as well.
 * Alignment node: since nsNode_t is all pointers, assume
 * that memory after it is sufficiently aligned to allow
 * placing nsAttrList_t there.
 */
nsNode_t *
NsUtil::nsAllocNode(MemoryManager *mmgr, uint32_t attrCount,
		    uint32_t flags)
{
	/* node + [attrList + array of text for attrs] */
	size_t msize = sizeof(nsNode_t);
	nsNode_t *node = (nsNode_t *)mmgr->allocate(msize);
	if (!node)
		nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "nsAllocNode: allocation failed",
				 __FILE__, __LINE__);

	memset(node, 0, msize);
	// NS_ALLOCATED must be set, or the node won't be deallocated properly
	nsSetFlag(node, NS_ALLOCATED|flags);
	if (attrCount != 0) {
		node->nd_attrs = nsAllocAttrList(mmgr, attrCount);
		nsSetFlag(node, NS_HASATTR);
	}
	node->nd_header.nh_name.n_prefix = NS_NOPREFIX;
	return node;
}

/*
 * If needed, reallocates the given node so that it's structures
 * are allocated separatly. This is so that they can individually
 * be resized and changed as needed. Deallocates the original node.
 */
nsNode_t *
NsUtil::nsMakeTransient(MemoryManager *mmgr, nsNode_t *node)
{
	if(nsIsAllocated(node) != 0) return node;
	nsNode_t *result = nsCopyNode(mmgr, node);
	nsFreeNode(mmgr, node);
	return result;
}

nsNode_t *
NsUtil::nsCopyNode(MemoryManager *mmgr, const nsNode_t *node)
{
	uint32_t flags = nsFlags(node);
	bool isUTF8 = !nsIsUTF16(node);

	nsNode_t *result = nsAllocNode(mmgr, 0, flags);
	nsCopyId(mmgr, &result->nd_header.nh_id, &node->nd_header.nh_id);
	nsCopyId(mmgr, &result->nd_header.nh_parent, &node->nd_header.nh_parent);
	result->nd_header.nh_uriIndex = node->nd_header.nh_uriIndex;
	nsCopyName(mmgr, &result->nd_header.nh_name, &node->nd_header.nh_name,
		   isUTF8);
	result->nd_level = node->nd_level;

	if (flags & NS_HASCHILD) {
		result->nd_child = nsCopyChildList(mmgr, node->nd_child);
	}
	if (flags & NS_HASATTR) {
		result->nd_attrs = nsCopyAttrList(mmgr, node->nd_attrs, isUTF8);
	}
	if (flags & NS_HASTEXT) {
		result->nd_text = nsCopyTextList(mmgr, node->nd_text, isUTF8);
	}
	return result;
}

/*
 * Free all memory associated with node.
 * Memory management issue: even for memory managers that can
 *  do all-in-one cleanup, this function should be used (assuming
 *  deallocate() does something meaningful).  The reason is to
 *  make it possible to parse/load huge documents, that would otherwise
 *  use all of memory.
 */
void
NsUtil::nsFreeNode(MemoryManager *mmgr, nsNode_t *node)
{
	if(nsIsAllocated(node)) {
		// Each part is allocated separatly, including all lists and strings
		uint32_t flags = nsFlags(node);

		nsFreeId(mmgr, &(node->nd_header.nh_id));
		nsFreeId(mmgr, &(node->nd_header.nh_parent));
		mmgr->deallocate(nsName(node)->n_text.t_chars);

		if (flags & NS_HASCHILD) {
			nsFreeChildList(mmgr, node->nd_child);
		}
		if (flags & NS_HASATTR) {
			nsFreeAttrList(mmgr, node->nd_attrs);
		}
		if (flags & NS_HASTEXT) {
			nsFreeTextList(mmgr, node->nd_text);
		}
		mmgr->deallocate(node);
	}
	else {
		// Free any extra memory attached to the nsNode_t
		if(node->nd_memory) {
			// We call free, because this memory comes from a Dbt
			// with DB_DBT_MALLOC set
			::free(node->nd_memory);
		}

		// All nsNode_t memory allocated in one large chunk
		mmgr->deallocate(node);
	}
}

nsChildList_t *
NsUtil::nsCopyChildList(MemoryManager *mmgr, const nsChildList_t *clist)
{
	uint32_t nchild = clist->cl_numChild;
	uint32_t max = nchild << 1; // leave some space for more child nodes...

	uint32_t size = sizeof(nsChildList_t) +
		((max - 1) * sizeof(nsChildEntry_t));
	nsChildList_t *result = (nsChildList_t*)mmgr->allocate(size);
	memset(result, 0, size);

	result->cl_numChild = nchild;
	result->cl_maxChild = max;
	for(uint32_t i = 0; i < nchild; ++i) {
		result->cl_child[i].ce_textIndex = clist->cl_child[i].ce_textIndex;
		nsCopyId(mmgr, &result->cl_child[i].ce_id, &clist->cl_child[i].ce_id);
	}

	return result;
}

void
NsUtil::nsFreeChildList(MemoryManager *mmgr, nsChildList_t *clist)
{
	uint32_t nchild = clist->cl_numChild;
	for(uint32_t i = 0; i < nchild; ++i) {
		nsFreeId(mmgr, &clist->cl_child[i].ce_id);
	}
	mmgr->deallocate(clist);
}

void
NsUtil::nsFreeAttrList(MemoryManager *mmgr, nsAttrList_t *attrs)
{
	int32_t nattrs = attrs->al_nattrs;
	for (int i = 0; i < nattrs; i++)
		mmgr->deallocate(attrs->al_attrs[i].a_name.n_text.t_chars);
	mmgr->deallocate(attrs);
}

nsTextList_t *
NsUtil::nsCopyTextList(MemoryManager *mmgr, const nsTextList_t *text, bool isUTF8)
{
	uint32_t ntext = text->tl_ntext;
	uint32_t nmax = ntext << 1; // leave some space for more text nodes...

	uint32_t size = sizeof(nsTextList_t) +
		((nmax - 1) * sizeof(nsTextEntry_t));
	nsTextList_t *result = (nsTextList_t*)mmgr->allocate(size);

	if (!result)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "nsCopyTextList: allocation failed",
					 __FILE__, __LINE__);

	memset(result, 0, size);
	result->tl_len = text->tl_len;
	result->tl_ntext = ntext;
	result->tl_max = nmax;

	for(uint32_t i = 0; i < ntext; ++i) {
		result->tl_text[i].te_type = text->tl_text[i].te_type;
		nsCopyText(mmgr, &result->tl_text[i].te_text, &text->tl_text[i].te_text, isUTF8);
	}

	return result;
}

void
NsUtil::nsFreeTextList(MemoryManager *mmgr, nsTextList_t *text)
{
	int32_t ntext = text->tl_ntext;
	for (int i = 0; i < ntext; i++)
		mmgr->deallocate(text->tl_text[i].te_text.t_chars);
	mmgr->deallocate(text);
}

nsChildList_t *
NsUtil::nsAllocChildList(MemoryManager *mmgr, uint32_t nchild)
{
	if (nchild != 0) {
		size_t msize = sizeof(nsChildList_t) +
			((nchild - 1) * sizeof(nsChildEntry_t));
		nsChildList_t *childlist =
			(nsChildList_t *)mmgr->allocate(msize);
		if (!childlist)
			nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "_nsAllocChildList: allocation failed",
					 __FILE__, __LINE__);
		memset(childlist, 0, msize);
		childlist->cl_maxChild = nchild;
		return childlist;
	}
	return 0;
}

/*
 * reallocate list, copying array of uint32_t
 */
static nsChildList_t *
_nsReallocChildList(MemoryManager *mmgr, nsChildList_t *list)
{
	if (list == 0)
		return NsUtil::nsAllocChildList(mmgr, NS_NTEXT_DEFAULT);

	/* realloc -- double size */
	uint32_t oldsize = sizeof(nsChildList_t) +
		((list->cl_numChild - 1) * sizeof(nsChildEntry_t));

	list->cl_maxChild <<= 1;
	nsChildList_t *result = NsUtil::nsAllocChildList(mmgr, list->cl_maxChild);

	memcpy(result, list, oldsize);
	mmgr->deallocate(list);
	return result;
}

int
NsUtil::nsAddChild(MemoryManager *mmgr, nsNode_t *node, uint32_t textIndex)
{
	nsChildList_t *list = node->nd_child;
	if ((list == 0) || (list->cl_numChild == list->cl_maxChild)) {
		list = _nsReallocChildList(mmgr, list);
		DBXML_ASSERT(list);
		node->nd_header.nh_flags |= NS_HASCHILD;
		node->nd_child = list;
	}
        /* add to list */
	int index = list->cl_numChild++;
	list->cl_child[index].ce_textIndex = textIndex;
	memset(&list->cl_child[index].ce_id, 0, sizeof(nid_t));
	return index;
}

void
NsUtil::nsInsertChild(MemoryManager *mmgr, nsNode_t *node, uint32_t index,
		      uint32_t textIndex)
{
	nsChildList_t *childList = node->nd_child;
	if ((childList == 0) || (childList->cl_numChild == childList->cl_maxChild)) {
		childList = _nsReallocChildList(mmgr, childList);
		DBXML_ASSERT(childList);
		node->nd_header.nh_flags |= NS_HASCHILD;
		node->nd_child = childList;
	}

	// Copy higher children down to fill space of removed child
	nsChildEntry_t *child =  &(childList->cl_child[index]);
	memmove(child + 1, child, (childList->cl_numChild - index) * sizeof(nsChildEntry_t));

        /* add to list */
	childList->cl_numChild += 1;
	child->ce_textIndex = textIndex;
	memset(&child->ce_id, 0, sizeof(nid_t));
}

void
NsUtil::nsRemoveChild(MemoryManager *mmgr, nsNode_t *node, uint32_t index)
{
	nsChildList_t *childList = node->nd_child;

	if (childList == 0 || childList->cl_numChild <= index) return;

	if(childList->cl_numChild == 1) {
		// There are no children left, so remove the nsChild_t
		nsFreeChildList(mmgr, childList);
		node->nd_child = 0;
		node->nd_header.nh_flags &= ~NS_HASCHILD;
	}
	else {
		nsChildEntry_t *child =  &(childList->cl_child[index]);
		nsFreeId(mmgr, &child->ce_id);

		// Reduce total number of children
		childList->cl_numChild -= 1;

		// Copy higher children down to fill space of removed child
		memcpy(child, child + 1, (childList->cl_numChild - index) * sizeof(nsChildEntry_t));
	}
}

bool nsMemcpyCheck8(xmlbyte_t *dest, const xmlbyte_t *src, int len,
		    enum checkType type)
{
	bool hasEnt = false;
	bool *hasEntity = &hasEnt;
	xmlbyte_t c;
	while (len--) {
		c = *src++;
		CHECK_ENT(c);
		*dest++ = c;
	}
	return hasEnt;
}

bool nsMemcpyCheck16(xmlch_t *dest, const xmlch_t *src, int len,
		     enum checkType type)
{
	bool hasEnt = false;
	bool *hasEntity = &hasEnt;
	xmlch_t c;
	while (len--) {
		c = *src++;
		CHECK_ENT(c);
		*dest++ = c;
	}
	return hasEnt;
}

// Input is one of:
//   utf-16 -- always copied
//   utf-8 -- donated
//   utf-8 -- must be copied.
// In the last instance, the string is scanned during the copy
// to check for entities, and the result returned in hasEntity
//
int
NsUtil::nsCreateText(MemoryManager *mmgr, nsText_t *dest,
		     const void *text, uint32_t length, bool isUTF8,
		     bool donate, bool *hasEntity, enum checkType type)
{
	uint32_t allocLen;
	if (length == 0 && text == 0) {
		// handle empty text
		static const char emptyText[4] = { 0, 0, 0, 0};
		if (!donate)
			text = (void*) emptyText;
		else {
			text = mmgr->allocate(4);
			if (!text)
				nsThrowException(
					XmlException::NO_MEMORY_ERROR,
					"nsCreateText: allocation failed",
					__FILE__, __LINE__);
			*((char*)text) = 0;
		}
	}
	length += 1; // trailing null
	if (!donate) {
		allocLen = length;
		if (!isUTF8)
			allocLen <<= 1;
		dest->t_chars = mmgr->allocate(allocLen);
		if (!dest->t_chars)
			nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "nsCreateText: allocation failed",
					 __FILE__, __LINE__);
		if (hasEntity) {
			if (isUTF8)
				*hasEntity = nsMemcpyCheck8(
					(xmlbyte_t *)dest->t_chars,
					(const xmlbyte_t *)text,
					allocLen, type);
			else
				*hasEntity = nsMemcpyCheck16(
					(xmlch_t *)dest->t_chars,
					(const xmlch_t *)text,
					(allocLen>>1), type);
		} else
			memcpy(dest->t_chars, text, allocLen);
	} else
		dest->t_chars = (void *) text; // cast is safe on donate
	dest->t_len = length - 1; /* don't include trailing null */
	return length;
}

/*
 * Allocate default size for text list
 */
static nsTextList_t *
_nsCreateTextList(MemoryManager *mmgr)
{
	size_t msize = sizeof(nsTextList_t) +
		((NS_NTEXT_DEFAULT - 1) * sizeof(nsTextEntry_t));
	nsTextList_t *tlist = (nsTextList_t *) mmgr->allocate(msize);
	if (tlist != 0) {
		memset(tlist, 0, msize);
		tlist->tl_max = NS_NTEXT_DEFAULT;
	}
	return tlist;
}

/*
 * reallocate list, copying array of nsTextEntry_t structs
 */
static nsTextList_t *
_nsReallocTextList(MemoryManager *mmgr, nsTextList_t *list)
{
	if (list == 0)
		return _nsCreateTextList(mmgr);
	/* realloc -- double size */
	uint32_t oldsize = sizeof(nsTextList_t) +
		((list->tl_max - 1) * sizeof(nsTextEntry_t));
	list->tl_max <<= 1;
	size_t msize = sizeof(nsTextList_t) +
		((list->tl_max-1) * sizeof(nsTextEntry_t));

	// reallocate
	void *newlist = mmgr->allocate(msize);
	if (!newlist)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "_nsReallocTextList: allocation failed",
					 __FILE__, __LINE__);
	memcpy(newlist, list, oldsize);
	mmgr->deallocate(list);
	return (nsTextList_t *) newlist;
}

int
NsUtil::nsAddText(MemoryManager *mmgr, nsNode_t *node,
		  const xmlch_t *text, uint32_t type)
{
	bool utf8 = !nsIsUTF16(node);

	if(utf8) {
		enum checkType ttype = (type == NS_TEXT) ? isCharacters : ignore;
		NsDonator chars(mmgr, text, NsUtil::nsStringLen(text),
				ttype);
		if (chars.getHasEntity())
			type |= NS_ENTITY_CHK;
		return nsAddText(mmgr, node, chars.getStr(), chars.getLen(),
				 type, true, true);
	}
	else {
		return nsAddText(mmgr, node, text, NsUtil::nsStringLen(text),
				 type, false, false);
	}
	return 0;
}

/*
 * Add a text node to the list.  Length is
 * available in this case.  Return index used.
 * Create text list if necessary.
 */
int
NsUtil::nsAddText(MemoryManager *mmgr, nsNode_t *node, const void *text,
		  uint32_t length, uint32_t type, bool isUTF8, bool donate)
{
	nsTextList_t *list = node->nd_text;
	if ((list == 0) || (list->tl_ntext == list->tl_max)) {
		list = _nsReallocTextList(mmgr, list);
		DBXML_ASSERT(list);
		node->nd_header.nh_flags |= NS_HASTEXT;
		node->nd_text = list;
	}
        /* add to list */
	int index = list->tl_ntext++;
	bool hasEntity = false;
	nsTextEntry_t *entry = &list->tl_text[index];
	entry->te_type = type;
	// set isCharacters -- it's ignored if the hasEntity arg is 0
	list->tl_len +=
		nsCreateText(mmgr, &entry->te_text,
			     text, length, isUTF8, donate,
			     (nsTextType(type) == NS_TEXT ? &hasEntity : 0),
			     isCharacters);
	if (hasEntity)
		entry->te_type |= NS_ENTITY_CHK;
	return index;
}

static void
_nsInsertText(MemoryManager *mmgr, nsNode_t *node, uint32_t index,
	      const void *text, uint32_t length, uint32_t type,
	      bool isUTF8, bool donate, int elemIndex)
{
	nsTextList_t *textList = node->nd_text;
	if ((textList == 0) || (textList->tl_ntext == textList->tl_max)) {
		textList = _nsReallocTextList(mmgr, textList);
		DBXML_ASSERT(textList);
		node->nd_header.nh_flags |= NS_HASTEXT;
		node->nd_text = textList;
	}

	nsTextEntry_t *entry =  &(textList->tl_text[index]);

	if(textList->tl_ntext > index) {
		// Copy texts up to leave a gap at index
		memmove(entry + 1, entry, (textList->tl_ntext - index) * sizeof(nsTextEntry_t));
	}

	// Renumber the child element's text indices
	nsChildList_t *childList = node->nd_child;
	if(childList) {
		int numElem = childList->cl_numChild;
		while(elemIndex < numElem)
			++childList->cl_child[elemIndex++].ce_textIndex;
	}

	// Create the new text entry
	bool hasEntity = false;
	entry->te_type = type;
	// set isCharacters -- ignored if hasEntity is null
	textList->tl_len +=
		NsUtil::nsCreateText(mmgr, &entry->te_text,
				     text, length, isUTF8, donate,
				     (nsTextType(type) == NS_TEXT ?
				      &hasEntity : 0), isCharacters);
	if (hasEntity)
		entry->te_type |= NS_ENTITY_CHK;

	// Increase the number of text
	textList->tl_ntext += 1;
}

void
NsUtil::nsInsertText(MemoryManager *mmgr, nsNode_t *node, uint32_t index,
		     const xmlch_t *text, uint32_t type, int elemIndex)
{
	bool utf8 = !nsIsUTF16(node);

	if(utf8) {
		enum checkType ttype = (type == NS_TEXT) ? isCharacters : ignore;
		NsDonator chars(mmgr, text, NsUtil::nsStringLen(text),
				ttype);
		if (chars.getHasEntity())
			type |= NS_ENTITY_CHK;
		_nsInsertText(mmgr, node, index, chars.getStr(), chars.getLen(),
			      type, true, true, elemIndex);
	}
	else {
		_nsInsertText(mmgr, node, index, text, NsUtil::nsStringLen(text),
			      type, false, false, elemIndex);
	}
}

void
NsUtil::nsRemoveText(MemoryManager *mmgr, nsNode_t *node, uint32_t index)
{
	nsTextList_t *textList = node->nd_text;
	nsChildList_t *childList = node->nd_child;

	if (textList == 0 || textList->tl_ntext <= index) return;

	if(textList->tl_ntext == 1) {
		// There are no text nodes left, so remove the nsTextList_t
		nsFreeTextList(mmgr, textList);
		node->nd_text = 0;
		node->nd_header.nh_flags &= ~NS_HASTEXT;
		if(childList) {
			for(uint32_t i = 0; i < childList->cl_numChild; ++i) {
				childList->cl_child[i].ce_textIndex = 0;
			}
		}
	}
	else {
		nsTextEntry_t *text =  &(textList->tl_text[index]);

		// Reduce total length of text, and number of text
		textList->tl_len -= text->te_text.t_len + 1;
		textList->tl_ntext -= 1;

		// Free the text's values
		if(text->te_text.t_chars != 0)
			mmgr->deallocate(text->te_text.t_chars);

		// Copy higher texts down to fill space of removed text
		memcpy(text, text + 1, (textList->tl_ntext - index) * sizeof(nsTextEntry_t));

		// Renumber the child element's text indices
		if(childList) {
			nsChildEntry_t *child;
			int numElem = childList->cl_numChild;
			for(int i = 0; i < numElem; ++i) {
				child = &childList->cl_child[i];
				if(child->ce_textIndex > index)
					--child->ce_textIndex;
			}
		}
	}
}

int
NsUtil::nsCoalesceText(MemoryManager *mmgr, nsTextList_t *list, uint32_t index,
		       const void *text, uint32_t len, bool isUTF8, bool checkEnt)
{
	nsTextEntry_t *entry = &list->tl_text[index];
	uint32_t allocLen, copyLen, newLen, oldLen = entry->te_text.t_len;
	newLen = oldLen + len + 1;

	// do copy operations in bytes
	if (isUTF8) {
		allocLen = newLen;
		copyLen = len + 1; // add trailing null
	} else {
		allocLen = newLen << 1;
		oldLen <<= 1;
		copyLen = (len + 1) << 1;
	}

	xmlbyte_t *newText = (xmlbyte_t *)mmgr->allocate(allocLen);

	if (!newText)
		nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "nsCoalesceText: allocation failed",
				 __FILE__, __LINE__);
	// copy original string, but don't copy null
	memcpy(newText, entry->te_text.t_chars, oldLen);
	// only do entity check copy if 1) not already set, and
	// 2) checkEnt is true
	// this function is only called for character text
	if (checkEnt && !(entry->te_type & NS_ENTITY_CHK)) {
		DBXML_ASSERT(isUTF8);
		checkEnt = nsMemcpyCheck8(
			(xmlbyte_t*)(newText + oldLen),
			(xmlbyte_t*) text, copyLen,
			isCharacters);
		if (checkEnt)
			entry->te_type |= NS_ENTITY_CHK;
	} else {
		// add trailing null for this copy
		memcpy(newText + oldLen, text, copyLen);
	}
	mmgr->deallocate(entry->te_text.t_chars);
	entry->te_text.t_chars = newText;
	entry->te_text.t_len = newLen - 1; // subtract null
	// NOTE: tl_len is ONLY used for marshaling, which means
	// it's a *byte* character count, and is irrelevant for the
	// non-UTF8 case, which is never marshaled.
	list->tl_len += len; // trailing null already accounted for
	return allocLen;
}

/*
 * Add text of the form, name0value0 to a text node.
 * Return total number of characters used, including all
 * nulls.
 */
static int
_nsAddMultiText(MemoryManager *mmgr, const void *name, const void *value,
		nsText_t *text, bool isUTF8, bool *hasEntity)
{
	int nlen, vlen, ncopyLen, vcopyLen, allocLen;
	// operate in byte lengths
	if (isUTF8) {
		nlen = NsUtil::nsStringLen((const xmlbyte_t*)name);
		vlen = NsUtil::nsStringLen((const xmlbyte_t *)value);
		allocLen = nlen + vlen + 2;
		ncopyLen = nlen + 1;
		vcopyLen = vlen + 1;
	} else {
		nlen = NsUtil::nsStringLen((const xmlch_t*)name);
		vlen = NsUtil::nsStringLen((const xmlch_t *)value);
		allocLen = (nlen + vlen + 2) << 1;
		ncopyLen = (nlen + 1) << 1;
		vcopyLen = (vlen + 1) << 1;
	}

	text->t_chars =	mmgr->allocate(allocLen);
	if (!text->t_chars)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "_nsAddMultiText: allocation failed",
					 __FILE__, __LINE__);
	text->t_len = nlen + vlen + 1; // don't count final NULL

	// copy to format, name0value0, only do entity check on value
	// hasEntity is ONLY non-zero for attributes...
	memcpy(text->t_chars, name, ncopyLen);
	if (hasEntity) {
		if (isUTF8)
			*hasEntity = nsMemcpyCheck8(
				((xmlbyte_t*)text->t_chars) +
				ncopyLen, (const xmlbyte_t*)value,
				vcopyLen, isAttrVal);
		else
			*hasEntity = nsMemcpyCheck16(
				((xmlch_t*)text->t_chars) +
				(ncopyLen>>1), (const xmlch_t*)value,
				(vcopyLen>>1), isAttrVal);
	} else {
		memcpy(((xmlbyte_t*)text->t_chars) + ncopyLen,
		       value, vcopyLen);
	}
	return (nlen + vlen + 2);
}

/*
 * Set attribute in the list.
 * If donate is true:
 *   o isUTF8 must be true
 *   o the string, starting at name, must be of the form, "name0value0"
 *   o value must point to the correct string
 *   o len must be set to indicate the length of the string, including both nulls
 */
static nsAttr_t *
_nsSetAttr(MemoryManager *mmgr, nsNode_t *node, uint32_t index,
	   const void *name, const void *value, bool isUTF8,
	   bool donate, uint32_t len, bool specified)
{
	nsAttrList_t *attrList = node->nd_attrs;
	nsAttr_t *attr =  &(attrList->al_attrs[index]);

	if(attr->a_name.n_text.t_chars != 0)
		mmgr->deallocate(attr->a_name.n_text.t_chars);

	// add both name/value to a_name.n_text
	attr->a_name.n_prefix = NS_NOPREFIX;
	attr->a_name.n_type = NS_NOTYPE;
	attr->a_uri = NS_NOURI;
	attr->a_flags = 0;
	if (donate) {
		DBXML_ASSERT(isUTF8 && len);
		attr->a_name.n_text.t_chars = (void *) name;
		attr->a_name.n_text.t_len = len - 1; // len includes trailing null
		attr->a_value = (void *)value;
	} else {
		// do real work
		bool hasEntity = false;
		len = _nsAddMultiText(mmgr, name, value,
				      &(attr->a_name.n_text),
				      isUTF8, &hasEntity);
		if (isUTF8) {
			xmlbyte_t *tptr8 =
				(xmlbyte_t *) attr->a_name.n_text.t_chars;
			while (*tptr8++); // get past name
			attr->a_value = tptr8;
		} else {
			xmlch_t *tptr =
				(xmlch_t *) attr->a_name.n_text.t_chars;
			while (*tptr++); // get past name
			attr->a_value = tptr;
		}
		if (hasEntity)
			attr->a_flags |= NS_ATTR_ENT;
	}
	if (!specified)
		attr->a_flags |= NS_ATTR_NOT_SPECIFIED;
	attrList->al_len += len;
	return attr;
}

nsAttr_t *
NsUtil::nsSetAttr(NsDocument *doc, nsNode_t *node, uint32_t index,
		  const xmlch_t *prefix, const xmlch_t *uri,
		  const xmlch_t *localname, const xmlch_t *value,
		  int32_t parseruri, bool specified)
{
	MemoryManager *mmgr = doc->getMemoryManager();

	bool utf8 = !nsIsUTF16(node);

	int32_t uriIndex = NS_NOURI;
	int32_t prefixIndex = NS_NOPREFIX;
	nsAttr_t *attr = 0;
	if(utf8) {
		NsDonator tattr(mmgr, localname, value, isAttrVal);
		attr = _nsSetAttr(mmgr, node, index,
				  tattr.getStr(),
				  tattr.getStr2(),
				  true, true,
				  tattr.getLen(), specified);
		if (tattr.getHasEntity())
			attr->a_flags |= NS_ATTR_ENT;
	} else {
		attr = _nsSetAttr(mmgr, node, index,
				  localname, value,
				  false, false, 0, specified);
	}

	if (doc->getDoNamespaces()) {
		if (prefix == 0 &&
		    nsStringEqual(localname,
				  _xmlnsPrefix16)) {
			/* default ns -- xmlns="uri" */
			nsSetFlag(node, NS_HASNSINFO);
		}
		if (nsStringEqual(prefix,
				  _xmlnsPrefix16)) {
			/* prefix mapping -- xmlns:xxx */
			nsSetFlag(node, NS_HASNSINFO);
		}
		if (uri) {
			if(utf8) {
				NsDonator auri(mmgr, uri, nsStringLen(uri));
				uriIndex = doc->
					addUri(auri.getStr(),
					       parseruri, true, true,
					       auri.getLen());
			}
			else {
				uriIndex = doc->
					addUri(uri, parseruri,
					       false, false, 0);
			}
			// xmlns="" has uri, but no prefix
			if (prefix) {
				if(utf8) {
					NsDonator apref(
						mmgr, prefix,
						NsUtil::nsStringLen(prefix));
					prefixIndex = doc->
						addPrefix(apref.getStr(),
							  uriIndex, true, true,
							  apref.getLen());
				}
				else {
					prefixIndex = doc->
						addPrefix(prefix, uriIndex,
							  false, false, 0);
				}
			}
		}
	}
	nsSetAttrPrefix(attr, prefixIndex);
	nsSetAttrUri(attr, uriIndex);
	// TBD.  If the attribute has type info,
	// add it to namespace prefix,
	// and put prefix in a_name.n_type.

	return attr;
}

nsAttr_t *
NsUtil::nsAddAttr(NsDocument *doc, nsNode_t *node, const xmlch_t *prefix,
		  const xmlch_t *uri, const xmlch_t *localname,
		  const xmlch_t *value, int32_t parseruri, bool specified)
{
	nsAttrList_t *attrList = node->nd_attrs;

	if ((attrList == 0) || (attrList->al_nattrs == attrList->al_max)) {
		attrList = _nsReallocAttrList(doc->getMemoryManager(), attrList);
		DBXML_ASSERT(attrList);
		node->nd_header.nh_flags |= NS_HASATTR;
		node->nd_attrs = attrList;
	}

	uint32_t index = attrList->al_nattrs++;
	nsAttr_t *attr =  &(attrList->al_attrs[index]);
	memset(attr, 0, sizeof(attr));

	return nsSetAttr(doc, node, index, prefix, uri, localname, value, parseruri, specified);
}

void
NsUtil::nsRemoveAttr(MemoryManager *mmgr, nsNode_t *node, uint32_t index)
{
	nsAttrList_t *attrList = node->nd_attrs;

	if (attrList == 0 || attrList->al_nattrs <= index) return;

	if(attrList->al_nattrs == 1) {
		// There are no attributes left, so remove the nsAttrList_t
		nsFreeAttrList(mmgr, attrList);
		node->nd_attrs = 0;
		node->nd_header.nh_flags &= ~NS_HASATTR;
	}
	else {
		nsAttr_t *attr =  &(attrList->al_attrs[index]);

		// Reduce total length of attrs, and number of attrs
		attrList->al_len -= attr->a_name.n_text.t_len + 1;
		attrList->al_nattrs -= 1;

		// Free the attr's values
		if(attr->a_name.n_text.t_chars != 0)
			mmgr->deallocate(attr->a_name.n_text.t_chars);

		// Copy higher attrs down to fill space of removed attr
		memcpy(attr, attr + 1, (attrList->al_nattrs - index) * sizeof(nsAttr_t));
	}
}

int
NsUtil::nsAddPI(MemoryManager *mmgr, nsNode_t *node,
		const xmlch_t *target, const xmlch_t *data)
{
	bool utf8 = !nsIsUTF16(node);

	if(utf8) {
		NsDonator chars(mmgr, target, data);
		return nsAddPI(mmgr, node, chars.getStr(), chars.getStr2(),
			       true, true, chars.getLen());
	}
	else {
		return nsAddPI(mmgr, node, target, data,
			       false, false, 0);
	}
	return 0;
}

int
NsUtil::nsCreatePI(MemoryManager *mmgr, nsText_t *dest,
		   const void *target, const void *data, bool isUTF8,
		   bool donate, uint32_t len)
{
	if (donate) {
		DBXML_ASSERT(isUTF8 && len);
		dest->t_chars = (void *) target;
		dest->t_len = len - 1; // subtract trailing null
		return len;
	} else {
		return _nsAddMultiText(mmgr, target, data,
				       dest, isUTF8, 0);
	}
}

//
// if donate is true:
//  o isUTF8 must be true
//  o string, starting at target, is of the form, target0data0
//  o len is the byte length of the entire string, including both nulls
//  o data is ignored, and can be null
//
int
NsUtil::nsAddPI(MemoryManager *mmgr, nsNode_t *node, const void *target,
		const void *data, bool isUTF8, bool donate, uint32_t len)
{
	/* all this is duplicated from nsAddText() */
	nsTextList_t *list = node->nd_text;
	if ((list == 0) || (list->tl_ntext == list->tl_max)) {
		list = _nsReallocTextList(mmgr, list);
		if (list == 0)
			return -1;
		node->nd_header.nh_flags |= NS_HASTEXT;
		node->nd_text = list;
	}
        /* add to list */
	int index = list->tl_ntext++;
	nsTextEntry_t *entry = &list->tl_text[index];
	entry->te_type = NS_PINST;

	len = nsCreatePI(mmgr, &entry->te_text, target,
			 data, isUTF8, donate, len);
	if (len <= 0) return -1;

	list->tl_len += len;
	return index;
}

static void
_nsInsertPI(MemoryManager *mmgr, nsNode_t *node, uint32_t index, const void *target,
	    const void *data, bool isUTF8, bool donate, uint32_t len, int elemIndex)
{
	nsTextList_t *textList = node->nd_text;
	if ((textList == 0) || (textList->tl_ntext == textList->tl_max)) {
		textList = _nsReallocTextList(mmgr, textList);
		DBXML_ASSERT(textList);
		node->nd_header.nh_flags |= NS_HASTEXT;
		node->nd_text = textList;
	}

	nsTextEntry_t *entry =  &(textList->tl_text[index]);

	if(textList->tl_ntext > index) {
		// Copy texts up to leave a gap at index
		memmove(entry + 1, entry, (textList->tl_ntext - index) * sizeof(nsTextEntry_t));
	}

	// Renumber the child element's text indices
	nsChildList_t *childList = node->nd_child;
	if(childList) {
		int numElem = childList->cl_numChild;
		while(elemIndex < numElem)
			++childList->cl_child[elemIndex++].ce_textIndex;
	}

	// Create the new text entry
	entry->te_type = NS_PINST;
	textList->tl_len += NsUtil::nsCreatePI(mmgr, &entry->te_text, target,
					       data, isUTF8, donate, len);

	// Increase the number of text
	textList->tl_ntext += 1;
}

void
NsUtil::nsInsertPI(MemoryManager *mmgr, nsNode_t *node, uint32_t index,
		   const xmlch_t *target, const xmlch_t *data, int elemIndex)
{
	bool utf8 = !nsIsUTF16(node);

	if(utf8) {
		NsDonator chars(mmgr, target, data);
		_nsInsertPI(mmgr, node, index, chars.getStr(), chars.getStr2(),
			    true, true, chars.getLen(), elemIndex);
	}
	else {
		_nsInsertPI(mmgr, node, index, target, data,
			    false, false, 0, elemIndex);
	}
}

/*
 * Marshaling/unmarshaling related functions
 */

/* fixed overhead, in bytes:
 *  o version (1)
 */
#define NS_MARSH_OVERHEAD 1
#define NS_UNMARSH_MIN sizeof(nsNode_t) + \
		sizeof(nsTextList_t) + \
		sizeof(nsAttrList_t) + \
		sizeof(nsChildList_t)

/*
 * internal functions/macros to count bytes for marshaling
 */

/* t_len is in UTF-8 bytes */
#define _nsTextSize(t) ((t).t_len + 1) /* include null */

/*
 * marshal UTF-8 text, including trailing null.
 * The text has already been transcoded to UTF-8.
 *
 * An advantage of using UTF-8 here is that there is no
 * chance of buffer overflow.
 *
 */
static inline int _nsMarshText(unsigned char *ptr, const nsText_t *text)
{
	memcpy(ptr, text->t_chars, text->t_len + 1);
	return (text->t_len + 1);
}

/*
 * marshal an attribute.  Format is:
 *  flags [prefixIndex] [uriIndex] [typeIndex] [unionTypeIndex] name0value0
 * where the various indexes (ints) are based on their related flags fields.
 * unionTypeIndex can only exist if typeIndex is set, also.
 */
static inline int _nsMarshAttr(unsigned char *ptr, const nsAttr_t *attr)
{
	// nsText_t field of the attribute name has entire
	// name/value string, contiguous.
	unsigned char *tptr = ptr;
	tptr += NsUtil::nsMarshInt(tptr, attr->a_flags);
	if (attr->a_flags & NS_ATTR_PREFIX)
		tptr += NsUtil::nsMarshInt(tptr, attr->a_name.n_prefix);
	if (attr->a_flags & NS_ATTR_URI)
		tptr += NsUtil::nsMarshInt(tptr, attr->a_uri);
	if (attr->a_flags & NS_ATTR_TYPE) {
		tptr += NsUtil::nsMarshInt(tptr, attr->a_name.n_type);
		if (attr->a_flags & NS_ATTR_UNION_TYPE)
			tptr += NsUtil::nsMarshInt(tptr, attr->a_name.n_utype);
	}
	tptr += _nsMarshText(tptr, &attr->a_name.n_text);

	return (tptr-ptr);
}

/*
 * marshal a node into a buffer (DBT, most likely).
 * If the "count" paramter is true, just the size required
 * is passed back, buf is not touched (it's probably null).
 *
 * This function is generally called twice -- once to count bytes,
 * once to do the marshal.
 *
 * The order of marshaling, obviously, is important.  Also
 * very important is putting the version byte and node id in front,
 * so the id can be used to discriminate among duplicate keys in DB.
 *
 * Returns >=0 on success, which is size required
 * for marshaling.  If not counting, return value is
 * undefined.  The function cannot fail.
 */
int
NsUtil::nsMarshalNode(const nsNode_t *node, unsigned char *buf, bool count)
{
	unsigned char *ptr = buf;
	int size = NS_MARSH_OVERHEAD;
	uint32_t tflags = node->nd_header.nh_flags;
	DBXML_ASSERT((tflags & NS_UTF16) == 0);

	// TBD - remove unmarshSize, since it stores sizeof(objects) in the
	// database, which make databases cross-platform incompatible, and
	// cause problems when the structs are altered. - jpcs
	uint32_t unmarshSize = 0;
	if (nsFlagSet(node, NS_HASATTR))
		unmarshSize += (node->nd_attrs->al_nattrs * sizeof(nsAttr_t));
	if (nsFlagSet(node, NS_HASTEXT))
		unmarshSize += (node->nd_text->tl_ntext *
				sizeof(nsTextEntry_t));
	// Store the size of the *old* nsChildList_t, to be compatible
	// with 2.2.8, and not waste memory in 2.2.11.
	if (nsFlagSet(node, NS_HASCHILD))
		unmarshSize += (node->nd_child->cl_numChild *
				sizeof(old_nsChildEntry_t));
	/* rely on the basic buffer size to hold enough raw text */

	if (count) {
		size += nsCountInt(unmarshSize);
		size += nsCountId(&node->nd_header.nh_id);
		if (!(tflags & NS_ISDOCUMENT)) {
			// document node has no parent, and no name
			size += nsCountId(&node->nd_header.nh_parent);
			size += _nsTextSize(node->nd_header.nh_name.n_text);
			if (tflags & NS_NAMEPREFIX)
				size += nsCountInt(
					node->nd_header.nh_name.n_prefix);
			if (tflags & NS_HASURI)
				size += nsCountInt(
					node->nd_header.nh_uriIndex);
			if (tflags & NS_NAMETYPE) {
				size += nsCountInt(
					node->nd_header.nh_name.n_type);
				if (tflags & NS_UNIONTYPE)
					size += nsCountInt(
						node->nd_header.
						nh_name.n_utype);
			}
		}
		size += nsCountInt(node->nd_level);
		size += nsCountInt(tflags & NS_DBFLAGMASK);
	} else {
		*ptr++ = NS_PROTOCOL_VERSION;
		ptr += nsMarshId(ptr, &node->nd_header.nh_id);
		ptr += nsMarshInt(ptr, unmarshSize);
		// Remove flags that shouldn't go into the db
		ptr += nsMarshInt(ptr, tflags & NS_DBFLAGMASK);
		ptr += nsMarshInt(ptr, node->nd_level);
		if (!(tflags & NS_ISDOCUMENT)) {
			ptr += nsMarshId(ptr,
					  &node->nd_header.nh_parent);
			if (tflags & NS_NAMEPREFIX)
				ptr += nsMarshInt(ptr,
					node->nd_header.nh_name.n_prefix);
			if (tflags & NS_HASURI)
				ptr += nsMarshInt(ptr,
					  node->nd_header.nh_uriIndex);
			if (tflags & NS_NAMETYPE) {
				ptr += nsMarshInt(
					ptr,node->nd_header.nh_name.n_type);
				if (tflags & NS_UNIONTYPE)
					ptr += nsMarshInt(
						ptr, node->nd_header.
						nh_name.n_utype);
			}
			ptr += _nsMarshText(ptr,
					    &(node->nd_header.nh_name.n_text));
		}
	}

	/* child list; <nchild, ([textIndex] child_nid)+ > */
	if (nsFlagSet(node, NS_HASCHILD)) {
		nsChildList_t *child = node->nd_child;
		uint32_t nchild = child->cl_numChild;
		if (count) {
			size += nsCountInt(nchild);
			for (unsigned int i = 0; i < nchild; ++i) {
				if (nsFlagSet(node, NS_HASTEXT))
					size += nsCountInt(child->cl_child[i].ce_textIndex);
				size += nsCountId(&child->cl_child[i].ce_id);
			}
		} else {
			ptr += nsMarshInt(ptr, nchild);
			for (unsigned int i = 0; i < nchild; ++i) {
				if (nsFlagSet(node, NS_HASTEXT))
					ptr += nsMarshInt(ptr, child->cl_child[i].ce_textIndex);
				ptr += nsMarshId(ptr, &child->cl_child[i].ce_id);
			}
		}
	}

	/* attributes.  <nattrs, nm10val10 [nm20val20]> */
	if (nsFlagSet(node, NS_HASATTR)) {
		nsAttrList_t *attrs = node->nd_attrs;
		int32_t nattrs = attrs->al_nattrs;
		if (count) {
			size += node->nd_attrs->al_len;
			size += nsCountInt(nattrs);
			// need to count optional:
			//  prefixes, uris, and types
			for (int i = 0; i < nattrs; i++) {
				nsAttr_t *attr = &(attrs->al_attrs[i]);
				size += 1; // a_flags are ALWAYS 1 byte
				if (attr->a_flags & NS_ATTR_PREFIX)
					size += nsCountInt(attr->a_name.n_prefix);
				if (attr->a_flags & NS_ATTR_URI)
					size += nsCountInt(attr->a_uri);
				if (attr->a_flags & NS_ATTR_TYPE) {
					size += nsCountInt(attr->a_name.n_type);
					if (attr->a_flags &
					    NS_ATTR_UNION_TYPE)
						size += nsCountInt(attr->a_name.n_utype);
				}
			}
		} else {
			ptr += nsMarshInt(ptr, nattrs);
			for (int i = 0; i < nattrs; i++) {
				ptr += _nsMarshAttr(ptr,
						    &(attrs->al_attrs[i]));
			}
		}
	}

	/* text.  <ntext, type1string1[type,string]> */
	if (nsFlagSet(node, NS_HASTEXT)) {
		nsTextList_t *list = node->nd_text;
		int32_t ntext = list->tl_ntext;
		if (count) {
			size += nsCountInt(ntext);
			size += list->tl_len +
				list->tl_ntext; /* for type bytes */
		} else {
			ptr += nsMarshInt(ptr, ntext);
			for (int i = 0; i < ntext; i++) {
				*ptr++ = (unsigned char)list->tl_text[i].te_type;
				ptr += _nsMarshText(ptr,
					    &(list->tl_text[i].te_text));
			}
		}
	}
	return size;
}

/*
 * allocate "space" for xmlbyte_t from end of array.
 * endPP points to last *available* memory.
 */
static inline void *_allocChars(xmlbyte_t **endPP, int len) {
	*endPP -= (len);
	xmlbyte_t *ret = *endPP + 1;
	return (void*) ret;
}

#define _nsAllocChars(dest, endPP, len) (dest) = _allocChars((endPP), (len))

/*
 * Unmarshal ID.
 * Eventually, this will use space off of endPP, which is
 * why it's passed.
 */
uint32_t
NsUtil::nsUnmarshId(const xmlbyte_t *ptr, nid_t *id, xmlbyte_t ** endPP, bool copyStrings)
{
	id->idLen = NsUtil::nsStringLen(ptr) + 1;
	if (id->idLen > NID_BYTES_SIZE) {
		if(copyStrings) {
			id->idStore.idPtr = (xmlbyte_t*)_allocChars(endPP, id->idLen);
			memcpy(id->idStore.idPtr, ptr, id->idLen);
		}
		else {
			id->idStore.idPtr = (xmlbyte_t*)ptr;
		}
	} else
		memcpy(id->idStore.idBytes, ptr, id->idLen);
	return id->idLen;
}

/*
 * Unmarshal text.  Text comes from a null-terminated, UTF-8 string,
 * pointed to by the ptr argument.  Return all characters moved, including
 * the NULL.
 */

static uint32_t
_nsUnmarshText(unsigned char *ptr, nsText_t *text, xmlbyte_t **endPP, bool copyStrings)
{
	uint32_t len = NsUtil::nsStringLen(ptr) + 1;

	if(copyStrings) {
		/* allocate UTF-16 memory */
		_nsAllocChars(text->t_chars, endPP, len);
		memcpy(text->t_chars, ptr, len);
	}
	else {
		text->t_chars = (void*)ptr;
	}
	text->t_len = len - 1;
	return len;
}

/*
 * unmarshal a header.
 *  args:
 *    node -- obvious
 *    endPP -- return -- updated end pointer; used for string allocs.
 *    ptr -- current pointer into the packed buffer.
 *  return number of bytes consumed from packed buffer (add to ptr).
 *  o initialize, allocating strings from end of buffer, as necessary
 */
static unsigned char *
_nsUnmarshHeader(nsNode_t *node, xmlbyte_t **endPP, unsigned char *ptr,
		 bool copyStrings)
{
	uint32_t dummy;
	ptr += NsUtil::nsUnmarshId(ptr, &node->nd_header.nh_id, endPP, copyStrings);
	/* already used unmarsh size */
	ptr += NsUtil::nsUnmarshInt(ptr, &dummy);
	ptr += NsUtil::nsUnmarshInt(ptr, &(node->nd_header.nh_flags));
	ptr += NsUtil::nsUnmarshInt(ptr, &(node->nd_level));
	if (!(node->nd_header.nh_flags & NS_ISDOCUMENT)) {
		ptr += NsUtil::nsUnmarshId(ptr, &node->nd_header.nh_parent,
					   endPP, copyStrings);

		if (node->nd_header.nh_flags & NS_NAMEPREFIX)
			ptr += NsUtil::nsUnmarshInt(ptr,
		            (uint32_t*)&(node->nd_header.nh_name.n_prefix));
		else
			node->nd_header.nh_name.n_prefix = NS_NOPREFIX;
		if (node->nd_header.nh_flags & NS_HASURI)
			ptr += NsUtil::nsUnmarshInt(ptr,
				  (uint32_t*)&(node->nd_header.nh_uriIndex));
		else
			node->nd_header.nh_uriIndex = 0;
		if (node->nd_header.nh_flags & NS_NAMETYPE) {
			ptr += NsUtil::nsUnmarshInt(ptr,
		            (uint32_t*)&(node->nd_header.nh_name.n_type));
			if (node->nd_header.nh_flags & NS_UNIONTYPE)
				ptr += NsUtil::nsUnmarshInt(
					ptr, (uint32_t*)&(node->nd_header.
							  nh_name.n_utype));
		} else
			node->nd_header.nh_name.n_type = NS_NOTYPE;
		ptr += _nsUnmarshText(ptr,
				      &(node->nd_header.nh_name.n_text),
				      endPP, copyStrings);
	}

	return ptr;
}

/*
 * unmarshal attribute list
 * ptr points to packed attribute list of the form:
 *   name10value10name20value20...
 *
 */
static unsigned char *
_nsUnmarshAttrs(unsigned char *ptr, nsAttrList_t *attrs, xmlbyte_t **endPP,
		bool copyStrings)
{
	for (uint32_t i = 0; i < attrs->al_nattrs; i++) {
		nsAttr_t *attr = &attrs->al_attrs[i];
		nsName_t *name = &attr->a_name;

		ptr += NsUtil::nsUnmarshInt(ptr,
					    (uint32_t*)&(attr->a_flags));
		if (attr->a_flags & NS_ATTR_PREFIX)
			ptr += NsUtil::nsUnmarshInt(
				ptr, (uint32_t*)&(name->n_prefix));
		else
			name->n_prefix = NS_NOPREFIX;
		if (attr->a_flags & NS_ATTR_URI)
			ptr += NsUtil::nsUnmarshInt(ptr,
					     (uint32_t*)&(attr->a_uri));
		else
			attr->a_uri = NS_NOURI;
		if (attr->a_flags & NS_ATTR_TYPE) {
			ptr += NsUtil::nsUnmarshInt(
				ptr, (uint32_t*)&(name->n_type));
			if (attr->a_flags & NS_ATTR_UNION_TYPE)
				ptr += NsUtil::nsUnmarshInt(
					ptr, (uint32_t*)&(name->n_utype));
		} else
			name->n_type = NS_NOTYPE;

		int name_len = NsUtil::nsStringLen(ptr) + 1; /* add null */
		int alen = name_len + NsUtil::nsStringLen(ptr + name_len) + 1;

		if(copyStrings) {
			_nsAllocChars(name->n_text.t_chars, endPP, alen);
			memcpy(name->n_text.t_chars, ptr, alen);
		}
		else {
			name->n_text.t_chars = (void*)ptr;
		}
		name->n_text.t_len = alen - 1;

		// set value
		attr->a_value = ((xmlbyte_t*)name->n_text.t_chars) + name_len;

		/* al_len includes all nulls */
		attrs->al_len += alen;
		ptr += alen;
	}
	return ptr;
}

/*
 * unmarshal text list
 * ptr points to packed text list of the form:
 *   [type]text10[type]text20...
 * except for PI, which is [type]target0data0
 *
 */
static unsigned char *
_nsUnmarshTextList(unsigned char *ptr, nsTextList_t *text, xmlbyte_t **endPP,
		   bool copyStrings)
{
	for (uint32_t i = 0; i < text->tl_ntext; i++) {
		nsTextEntry_t *entry = &text->tl_text[i];
		entry->te_type = (uint32_t) *ptr++;
		int tlen = NsUtil::nsStringLen(ptr) + 1; /* add null */
		if (nsTextType(entry->te_type) == NS_PINST)
			tlen += NsUtil::nsStringLen(ptr + tlen) + 1;
		if(copyStrings) {
			_nsAllocChars(entry->te_text.t_chars, endPP, tlen);
			memcpy(entry->te_text.t_chars, ptr, tlen);
		}
		else {
			entry->te_text.t_chars = (void*)ptr;
		}
		entry->te_text.t_len = tlen - 1;
		/* tl_len includes all nulls */
		text->tl_len += tlen;
		ptr += tlen;
	}
	return ptr;
}

/*
 * unmarshal a buffer into a node.
 *
 * reverse the process from marshaling.
 *
 * Text transcoding to UTF-16 is done lazily.  UTF-8
 * text from the buffer is copied directly.
 * As the text strings are
 * used by (DOM) operations that require UTF-16, they
 * are transcoded to new buffers.  Doing the transcode
 * in-place was considered, to minimize allocations,
 * but the existing transcoding code doesn't handle that.
 * It could if it worked backwards.
 *
 * UTF-8 characters are allocated from the end of the
 * buffer, backwards, in order to keep structure alignment
 * in the front end.
 *
 * Return node pointer upon success, NULL otherwise.
 *
 * The allocation size is based on:
 *   NS_UNMARSH_MIN -- a skeleton nsNode_t, with all related objects
 *   unmarshSize -- extra objects, based on number of attribute and
 *     text nodes.
 *   bufsize -- covers raw text, which does not expand during unmarshal
 */

nsNode_t *
NsUtil::nsUnmarshalNode(MemoryManager *mmgr, unsigned char *buf,
	uint32_t bufsize, bool adoptBuffer)
{
	uint32_t allocSize;
	nsNode_t *node;
	char *curP;
	unsigned char *ptr = buf;
	xmlbyte_t *endP;

	if (*ptr++ != NS_PROTOCOL_VERSION)
		nsThrowException(XmlException::INTERNAL_ERROR,
				 "protocal mismatch in node",
				 __FILE__, __LINE__);
	/*
	 * use unmarshal size, which follows the node id,
	 * to determine allocation size.  This means skipping into
	 * the buffer, past the node id.
	 */
	const xmlbyte_t *tptr = nsSkipId(ptr);

	nsUnmarshInt(tptr, &allocSize);

	// We don't trust the unmarshal size, as it may have stored
	// old sizeof() information in the database.
	allocSize *= sizeof(nsChildEntry_t);
	allocSize /= sizeof(old_nsChildEntry_t);

	allocSize += NS_UNMARSH_MIN;

	// Now allocate, using the adjusted allocSize
	DBXML_ASSERT(mmgr);
	curP = (char *)mmgr->allocate(allocSize);
	if (!curP)
		nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "nsUnmarshalNode failed to allocate memory",
				 __FILE__, __LINE__);
	memset(curP, 0, allocSize);
	node = (nsNode_t *) curP;
	endP = (xmlbyte_t *) (curP + allocSize - 1);
	curP = (char *) (node + 1);

	if(adoptBuffer) node->nd_memory = buf;

	ptr = _nsUnmarshHeader(node, &endP, ptr, /*copyStrings*/false);

	/*
	 * Continue with header bits, and easy structures,
	 * carving them off from curP, as necessary.
	 */
	if (nsFlagSet(node, NS_HASCHILD)) {
		nsChildList_t *child = (nsChildList_t *)curP;
		node->nd_child = child;
		ptr += nsUnmarshInt(ptr, &child->cl_numChild);
		child->cl_maxChild = child->cl_numChild;
		curP = (char *)(child + 1);
		curP += (child->cl_numChild - 1) * sizeof(nsChildEntry_t);
		if (nsFlagSet(node, (NS_HASTEXT))) {
			for (unsigned int i = 0; i < child->cl_numChild; ++i) {
				ptr += nsUnmarshInt(ptr, &(child->cl_child[i].ce_textIndex));
				ptr += nsUnmarshId(ptr,
					&child->cl_child[i].ce_id,
					&endP, /*copyStrings*/false);
			}
		}
		else {
			for (unsigned int i = 0; i < child->cl_numChild; ++i) {
				ptr += nsUnmarshId(ptr,
					&child->cl_child[i].ce_id,
					&endP, /*copyStrings*/false);
			}
		}
	}

	if (nsFlagSet(node, (NS_HASATTR))) {
		nsAttrList_t *attrs = (nsAttrList_t *)curP;
		node->nd_attrs = attrs;
		/* need to know number before can carve structure */
		ptr += nsUnmarshInt(ptr, &(attrs->al_nattrs));
		attrs->al_max = attrs->al_nattrs;
		curP = (char *)(attrs + 1);
		/* add nattrs-1 worth of nsText_t */
		curP += ((attrs->al_nattrs - 1) * sizeof(nsAttr_t));
		/* unmarshal the attributes */
		ptr = _nsUnmarshAttrs(ptr, attrs, &endP, /*copyStrings*/false);
	}

	if (nsFlagSet(node, (NS_HASTEXT))) {
		nsTextList_t *text = (nsTextList_t *)curP;
		node->nd_text = text;
		ptr += nsUnmarshInt(ptr, &(text->tl_ntext));
		text->tl_max = text->tl_ntext;
		curP = (char *)(text + 1);
		curP += ((text->tl_ntext - 1) * sizeof(nsTextEntry_t));
		ptr = _nsUnmarshTextList(ptr, text, &endP, /*copyStrings*/false);
	}

	/* debug */
	if (curP >= (char *)endP) {
		nsThrowException(XmlException::INTERNAL_ERROR,
				 "nsUnmarshalNode overlap",
				 __FILE__, __LINE__);
	}
	return node;
}

// String routines

xmlbyte_t *
NsUtil::nsStringDup(MemoryManager *mmgr, const xmlbyte_t *str, int *lenP)
{
	if(str == 0) return 0;
	int len = nsStringLen(str) + 1;
	xmlbyte_t *copy = (xmlbyte_t *) mmgr->allocate(len);
	if (copy)
		memcpy(copy, str, len);
	else
		nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "nsStringDup failed to allocate memory",
				 __FILE__, __LINE__);
	if (lenP)
		*lenP = len;
	return copy;
}

xmlch_t *
NsUtil::nsStringDup(MemoryManager *mmgr, const xmlch_t *str, int *lenP)
{
	if(str == 0) return 0;
	int len = (nsStringLen(str) + 1) << 1;
	xmlch_t *copy = (xmlch_t *) mmgr->allocate(len);
	if (copy)
		memcpy(copy, str, len);
	else
		nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "nsStringDup failed to allocate memory",
				 __FILE__, __LINE__);
	if (lenP)
		*lenP = (len >> 1); // divide bytes by 2
	return copy;
}

bool
NsUtil::nsStringEqual(const xmlch_t *str1, const xmlch_t *str2)
{
	if (str1 == 0 || str2 == 0)
		return (str1 == str2);

	while (*str1 == *str2) {
		if (*str1 == 0)
			return true;
		++str1;
		++str2;
	}
	return false;
}

/*
 * Escape < & "
 */
static uint32_t _amp = '&';
static uint32_t _lt = '<';
static uint32_t _gt = '>';
static uint32_t _dq = '"';
static uint32_t _sp = ' ';
static const char *_ampStr = "&amp;";
static const char *_ltStr = "&lt;";
static const char *_dquotStr = "&quot;";
#define _isNum(ch) (((ch) >= '0') && ((ch) <= '9'))
//
// only escape &, <, and "
//
uint32_t
NsUtil::nsEscape(char *dest, const xmlbyte_t *src, int len, bool isAttr)
{
	char *start = dest;
	uint32_t ch;
	while (--len != 0 && (ch = *src++) != 0) {
		// reduce odds on common characters.  adds instructions
		// for real escapes, and some other characters,
		// but they are presumed uncommon
		if (ch > _gt || ch == _sp || _isNum(ch)) {
			*dest++ = ch;
		} else {
			if (ch == _amp) {
				memcpy(dest, _ampStr, 5);
				dest += 5;
			} else if (ch == _lt) {
				memcpy(dest, _ltStr, 4);
				dest += 4;
			} else if ((ch == _dq) && isAttr) {
				memcpy(dest, _dquotStr, 6);
				dest += 6;
			} else {
				*dest++ = ch;
			}
		}
	}
	*dest = '\0';
	return (dest-start);
}

//
// error handling utilities
//

// TBD: change code to XmlException::ExceptionCode
// Implement XmlException calls.
void
NsUtil::nsThrowException(XmlException::ExceptionCode code, const char *desc,
			 const char *file, int line)
{
	throw XmlException(code, desc, file, line);
}

void
NsUtil::nsThrowDbException(const DbException &e, const char *file, int line)
{
	throw XmlException(e, file, line);
}

void
NsUtil::nsThrowParseException(const char *message)
{
	throw XmlException(XmlException::INDEXER_PARSER_ERROR,
			   message);
}

NsDonator::NsDonator(MemoryManager *mmgr, const xmlch_t *src, uint32_t len,
		     enum checkType type)
	: _str(0), _str2(0), _len(0),
	  _hasEntity(false)
{
	// handle empty strings -- this can happen for comments
	if (len == 0)
		return;
	bool checkEntity = (type == ignore ? false : true);
	if (src && *src)
		_len = NsUtil::nsToUTF8(mmgr, &_str, src,
					len+1, 0, (checkEntity ?
						   &_hasEntity : 0), type) - 1;
	// if no src, parts will be uninitialized, and _str is null
}

NsDonator::NsDonator(MemoryManager *mmgr, const xmlch_t *src1,
		     const xmlch_t *src2, enum checkType type)
	:_str(0), _str2(0), _len(0), _hasEntity(false)
{
	if(src1 != 0 || src2 != 0) {
		bool checkEntity = (type == ignore ? false : true);
		int nlen = src1 ? NsUtil::nsStringLen(src1) : 0;
		int vlen = src2 ? NsUtil::nsStringLen(src2) : 0;

		int sz = (nlen + vlen);
		// make plenty of space for transcoding (3x) and 2 nulls.
		sz = (sz * 3) + 4;
		xmlbyte_t *str = (xmlbyte_t *) mmgr->allocate(sz);
		if (!str)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				"NsDonator: allocation failed",
				__FILE__, __LINE__);

		uint32_t tlen = 0;
		if(src1 != 0) {
			_str = str;
			// never check for entities on first string
			tlen = NsUtil::nsToUTF8(NULL, &str, src1, nlen + 1, sz, 0);
			str += tlen;
		}
		if(src2 != 0) {
			_str2 = str;
			tlen += NsUtil::nsToUTF8(NULL, &str, src2, vlen + 1, sz - tlen,
				(checkEntity ? &_hasEntity : 0), type);
		}
		_len = tlen;
	}
}

void
NsUtil::displayNID(std::ostringstream &out, const nid_t *nid) {
	displayNID(out, (const char*)nsNode(nid), nid->idLen - 1);
}

void
NsUtil::displayNID(std::ostringstream &out, const char *buf, int len) {
	for (int i = 0; i < len; ++i, ++buf) {
		unsigned char b = *buf;
		int hn = (b & 0xF0) >> 4;
		int ln = b & 0x0F;
		out << (char)(hn < 10 ? '0' + hn : 'A' + hn - 10);
		out << (char)(ln < 10 ? '0' + ln : 'A' + ln - 10);
	}
}

// utilities and debug routines
#ifdef NS_DEBUG

#define NS_CH_CAST xmlbyte_t
static const char crText[2] = {'\\','r'};
static const char lfText[2] = {'\\','n'};
static const char tabText[2] = {'\\','t'};
static const char spText[2] = {'s','p'};
static const unsigned char ch_CR = '\015';
static const unsigned char ch_LF = '\012';
static const unsigned char ch_Tab = '\027';
static const unsigned char ch_Space = ' ';

void
NsUtil::nsPrintId(const char *label, const nid_t *id)
{
	if(id != 0) {
		printf("nid(%s): 0x", label);

		const xmlbyte_t *nid = NsUtil::nsNode(id);
		while(*nid != 0) {
			xmlbyte_t hi = *nid >> 4;
			xmlbyte_t lo = *nid & 0xf;
			printf("%c%c", (hi > 9 ? (char)(hi - 9 + 'A') : (char)(hi + '0')),
				(lo > 9 ? (char)(lo - 9 + 'A') : (char)(lo + '0')));
			++nid;
		}

		printf("\n");
	}
	else {
		printf("nid(%s): null\n", label);
	}
}

void
NsUtil::nsTestBetweenId(MemoryManager *mmgr)
{
	nid_t a, b, c;

	a.idStore.idBytes[0] = NS_ID_FIRST;
	a.idStore.idBytes[1] = NS_ID_ZERO;
	a.idStore.idBytes[2] = NS_ID_LAST;
	a.idStore.idBytes[3] = 0;
	a.idLen = 4;

	b.idStore.idBytes[0] = NS_ID_FIRST;
	b.idStore.idBytes[1] = NS_ID_FIRST;
	b.idStore.idBytes[2] = 0;
	b.idLen = 3;

	memset(&c, 0, sizeof(nid_t));

	NsUtil::nsBetweenId(mmgr, &c, &a, &b);
	NsUtil::nsPrintId("a", &a);
	NsUtil::nsPrintId("b", &b);
	NsUtil::nsPrintId("c", &c);
	NsUtil::nsFreeId(mmgr, &c);

	printf("%s\n", "-----");

	memset(&b, 0, sizeof(nid_t));
	memset(&c, 0, sizeof(nid_t));

	NsUtil::nsBetweenId(mmgr, &c, &a, &b);
	NsUtil::nsPrintId("a", &a);
	NsUtil::nsPrintId("b", &b);
	NsUtil::nsPrintId("c", &c);
	NsUtil::nsFreeId(mmgr, &c);

	printf("%s\n", "-----");

	a.idStore.idBytes[0] = NS_ID_ZERO;
	a.idStore.idBytes[1] = NS_ID_LAST;
	a.idStore.idBytes[2] = NS_ID_LAST;
	a.idStore.idBytes[3] = 0;
	a.idLen = 4;

	b.idStore.idBytes[0] = NS_ID_FIRST;
	b.idStore.idBytes[1] = 0;
	b.idLen = 2;

	memset(&c, 0, sizeof(nid_t));

	NsUtil::nsBetweenId(mmgr, &c, &a, &b);
	NsUtil::nsPrintId("a", &a);
	NsUtil::nsPrintId("b", &b);
	NsUtil::nsPrintId("c", &c);
	NsUtil::nsFreeId(mmgr, &c);

	printf("%s\n", "-----");

	a.idStore.idBytes[0] = NS_ID_FIRST + 6;
	a.idStore.idBytes[1] = NS_ID_FIRST;
	a.idStore.idBytes[2] = 0;
	a.idLen = 3;

	b.idStore.idBytes[0] = NS_ID_FIRST + 7;
	b.idStore.idBytes[1] = 0;
	b.idLen = 2;

	memset(&c, 0, sizeof(nid_t));

	NsUtil::nsBetweenId(mmgr, &c, &a, &b);
	NsUtil::nsPrintId("a", &a);
	NsUtil::nsPrintId("b", &b);
	NsUtil::nsPrintId("c", &c);
	NsUtil::nsFreeId(mmgr, &c);

	printf("%s\n", "-----");

	a.idStore.idBytes[0] = NS_ID_ZERO;
	a.idStore.idBytes[1] = NS_ID_LAST;
	a.idStore.idBytes[2] = NS_ID_LAST - 1;
	a.idStore.idBytes[3] = 0;
	a.idLen = 4;

	b.idStore.idBytes[0] = NS_ID_FIRST;
	b.idStore.idBytes[1] = 0;
	b.idLen = 2;

	memset(&c, 0, sizeof(nid_t));

	NsUtil::nsBetweenId(mmgr, &c, &a, &b);
	NsUtil::nsPrintId("a", &a);
	NsUtil::nsPrintId("b", &b);
	NsUtil::nsPrintId("c", &c);
	NsUtil::nsFreeId(mmgr, &c);

	printf("%s\n", "-----");

	a.idStore.idBytes[0] = NS_ID_FIRST;
	a.idStore.idBytes[1] = NS_ID_FIRST;
	a.idStore.idBytes[2] = NS_ID_FIRST + 2;
	a.idStore.idBytes[3] = 0;
	a.idLen = 4;

	b.idStore.idBytes[0] = NS_ID_FIRST;
	b.idStore.idBytes[1] = NS_ID_FIRST + 1;
	b.idStore.idBytes[2] = 0;
	b.idLen = 3;

	memset(&c, 0, sizeof(nid_t));

	NsUtil::nsBetweenId(mmgr, &c, &a, &b);
	NsUtil::nsPrintId("a", &a);
	NsUtil::nsPrintId("b", &b);
	NsUtil::nsPrintId("c", &c);
	NsUtil::nsFreeId(mmgr, &c);

	printf("%s\n", "-----");

	a.idStore.idBytes[0] = NS_ID_FIRST;
	a.idStore.idBytes[1] = NS_ID_FIRST;
	a.idStore.idBytes[2] = NS_ID_FIRST + 2;
	a.idStore.idBytes[3] = 0;
	a.idLen = 4;

	b.idStore.idBytes[0] = NS_ID_FIRST;
	b.idStore.idBytes[1] = NS_ID_FIRST + 2;
	b.idStore.idBytes[2] = 0;
	b.idLen = 3;

	memset(&c, 0, sizeof(nid_t));

	NsUtil::nsBetweenId(mmgr, &c, &a, &b);
	NsUtil::nsPrintId("a", &a);
	NsUtil::nsPrintId("b", &b);
	NsUtil::nsPrintId("c", &c);
	NsUtil::nsFreeId(mmgr, &c);

	printf("%s\n", "-----");

}

static const char *
_asWhiteSpace(const char *chars, char *buf)
{
	const char* ptr = chars;
	char *toptr = buf;
	bool isAllWS = true;
	while (*ptr) {
		if (*ptr == ch_CR) {
			memcpy(toptr, crText, 2);
			toptr += 2;
		} else if (*ptr == ch_LF) {
			memcpy(toptr, lfText, 2);
			toptr += 2;
		} else if (*ptr == ch_Tab) {
			memcpy(toptr, tabText, 2);
			toptr += 2;
		} else if (*ptr == ch_Space) {
			memcpy(toptr, spText, 2);
			toptr += 2;
		} else {
			*toptr++ = *ptr;
			isAllWS = false;
		}
		ptr++;
	}
	*toptr = 0;
	return (isAllWS ? buf : chars);
}

static const char *
_printFlags(uint32_t flags, char *buf)
{
	*buf = 0;
	if (flags  & NS_ALLOCATED)
		strcat(buf, "ALLOCATED|");
	if (flags & NS_HASCHILD)
		strcat(buf, "CHILD|");
	if (flags & NS_HASATTR)
		strcat(buf, "ATTR|");
	if (flags & NS_HASTEXT)
		strcat(buf, "TEXT|");
	if (flags & NS_ISDOCUMENT)
		strcat(buf, "ISDOCUMENT|");
	if (flags & NS_ISROOT)
		strcat(buf, "ISROOT|");
	if (flags & NS_HASNSINFO)
		strcat(buf, "NSINFO|");
	if (flags & NS_STANDALONE)
		strcat(buf, "STANDALONE|");
	if (flags & NS_UTF16)
		strcat(buf, "UTF16|");
	int l = strlen(buf);
	if (buf[l-1] == '|')
		buf[l-1] = '\0';
	return buf;
}

//
// assume text is UTF-8
//
static void
_printText8(nsTextEntry_t *entry, int index)
{
	const char *type = "text";
	const char *text = (char *)entry->te_text.t_chars;
	char *buf = (char *)malloc((entry->te_text.t_len + 1) << 1);
	if (!buf)
		return;

	printf("\t%d, len %d ", index, entry->te_text.t_len);
	if (nsTextType(entry->te_type) == NS_COMMENT)
		type = "comment";
	else if (nsTextType(entry->te_type) == NS_CDATA)
		type = "cdata";
	else  if (nsTextType(entry->te_type) == NS_PINST) {
		/* PI is stored as target0data0 */
		const char *datap = text + NsUtil::nsStringLen((const unsigned char *)text) + 1;
		type = "pinst";
		printf("(%s): ", type);
		printf("%s=", _asWhiteSpace(text, buf));
		printf("%s\n", _asWhiteSpace(datap, buf));
		free(buf);
		return;
	}
	printf("(%s): %s\n", type, _asWhiteSpace((const char *)text, buf));
	free(buf);
}

void
NsUtil::nsPrintNode8(nsNode_t *node, NsDocument *doc)
{
	char fbuf[512];
	const char *name = "#DOCUMENT";
	int32_t prefix = NS_NOPREFIX;
	int32_t typeIndex = NS_NOTYPE;
	printf("Node: ");
	if (!(node->nd_header.nh_flags & NS_ISDOCUMENT)) {
		name = (const char *)
			node->nd_header.nh_name.n_text.t_chars;
		prefix = node->nd_header.nh_name.n_prefix;
		typeIndex = node->nd_header.nh_name.n_type;
	}

	if (prefix != NS_NOPREFIX)
		if(doc == 0)
			printf(" %d:", prefix);
		else
			printf(" %s:", doc->getPrefix8(prefix));
	printf("%s, %s, level %d, %s, NS prefix %d",
	       name,
	       nsNode(&node->nd_header.nh_id),
	       node->nd_level,
	       _printFlags(node->nd_header.nh_flags, fbuf),
		prefix);
	if (typeIndex != NS_NOTYPE)
		if(doc == 0)
			printf(", type: %d", typeIndex);
		else
			printf(", type: %s", doc->getPrefix8(typeIndex));
	printf("\n");
	if (node->nd_attrs) {
		nsAttrList_t *attrs = node->nd_attrs;
		printf("    Attrs (num, total len, max):  %d, %d, %d\n",
		       attrs->al_nattrs, attrs->al_len, attrs->al_max);
		for (uint32_t i = 0; i < attrs->al_nattrs; i++) {
			nsText_t *text = &attrs->al_attrs[i].a_name.n_text;
			printf("\t");
			if (attrs->al_attrs[i].a_name.n_prefix != NS_NOPREFIX)
				if(doc == 0)
					printf("%d:", attrs->al_attrs[i].
						a_name.n_prefix);
				else
					printf("%s:", doc->getPrefix8(
						       attrs->al_attrs[i].
						       a_name.n_prefix));
			const xmlbyte_t *value = (const xmlbyte_t *)attrs->
				al_attrs[i].a_value;
			printf("%s = %s", (const xmlbyte_t *)
			       text->t_chars, value);
			if (attrs->al_attrs[i].a_name.n_type != NS_NOTYPE) {
				if(doc == 0)
					printf(", attrtype: %d", attrs->al_attrs[i].
						a_name.n_type);
				else
					printf(", attrtype: %s",
						doc->getPrefix8(
							attrs->al_attrs[i].
							a_name.n_type));
			}
			printf("\n");
		}
	}
	if (node->nd_text) {
		nsTextEntry_t *entries = node->nd_text->tl_text;
		printf("    Text nodes (num, total len, max):  %d, %d, %d\n",
		       node->nd_text->tl_ntext,
		       node->nd_text->tl_len, node->nd_text->tl_max);
		for (uint32_t i = 0; i < node->nd_text->tl_ntext; i++) {
			_printText8(&entries[i], i);
		}
	}
	if (node->nd_child) {
		printf("    Children: num: %d\n",
		       node->nd_child->cl_numChild);

		printf("      textIndex, child_id:\n");
		for (uint32_t i = 0; i < node->nd_child->cl_numChild; ++i) {
			printf("        %d, %s\n",
			       node->nd_child->cl_child[i].ce_textIndex,
			       nsNode(&node->nd_child->cl_child[i].ce_id));
		}
	}
}

//
// assume text is UTF-16
//
static void
_printText16(nsTextEntry_t *entry, int index)
{
	const char *type = "text";
	xmlch_t *text = (xmlch_t *)entry->te_text.t_chars;
	char *buf = (char *)malloc((entry->te_text.t_len + 1) << 1);
	if (!buf)
		return;

	printf("\t%d, len %d ", index, entry->te_text.t_len);
	if (nsTextType(entry->te_type) == NS_COMMENT)
		type = "comment";
	else if (nsTextType(entry->te_type) == NS_CDATA)
		type = "cdata";
	else  if (nsTextType(entry->te_type) == NS_PINST) {
		/* PI is stored as target0data0 */
		const xmlch_t *datap = text + NsUtil::nsStringLen(text) + 1;
		type = "pinst";
		printf("(%s): ", type);
		printf("%s=", _asWhiteSpace(XMLChToUTF8(text).str(), buf));
		printf("%s\n", _asWhiteSpace(XMLChToUTF8(datap).str(), buf));
		free(buf);
		return;
	}
	printf("(%s): %s\n", type, _asWhiteSpace(XMLChToUTF8(text).str(), buf));
	free(buf);
}

void
NsUtil::nsPrintNode16(nsNode_t *node, NsDocument *doc)
{
	char fbuf[512];
	const char *docName = "#DOCUMENT";
	const xmlch_t *name = 0;
	int32_t prefix = NS_NOPREFIX;
	int32_t typeIndex = NS_NOTYPE;
	printf("Node: ");
	if (!(node->nd_header.nh_flags & NS_ISDOCUMENT)) {
		name = (const xmlch_t *)node->nd_header.nh_name.n_text.t_chars;
		prefix = node->nd_header.nh_name.n_prefix;
		typeIndex = node->nd_header.nh_name.n_type;
	}

	if (prefix != NS_NOPREFIX)
		printf(" %s:", doc->getPrefix8(prefix));
	printf("%s, %s, level %d, %s",
	       (name ? XMLChToUTF8(name).str() : docName),
	       nsNode(&node->nd_header.nh_id),
	       node->nd_level,
	       _printFlags(node->nd_header.nh_flags, fbuf));
	if (typeIndex != NS_NOTYPE)
		printf(", type: %s", doc->getPrefix8(typeIndex));
	printf("\n");

	if (node->nd_attrs) {
		nsAttrList_t *attrs = node->nd_attrs;
		printf("    Attrs (num, total len, max):  %d, %d, %d\n",
		       attrs->al_nattrs, attrs->al_len, attrs->al_max);
		for (uint32_t i = 0; i < attrs->al_nattrs; i++) {
			nsText_t *text = &attrs->al_attrs[i].a_name.n_text;
			printf("\t");
			if (attrs->al_attrs[i].a_name.n_prefix != NS_NOPREFIX)
				printf("%s:", doc->getPrefix8(
					      attrs->al_attrs[i].
					      a_name.n_prefix));
			const xmlch_t *value = (const xmlch_t *)attrs->
				al_attrs[i].a_value;
			printf("%s = %s", XMLChToUTF8((const xmlch_t *)
					       text->t_chars).str(),
			       XMLChToUTF8(value).str());
			if (attrs->al_attrs[i].a_name.n_type != NS_NOTYPE) {
				printf(", attrtype: %s",
				       doc->getPrefix8(
					       attrs->al_attrs[i].
					       a_name.n_type));
			}
			printf("\n");
		}
	}
	if (node->nd_text) {
		nsTextEntry_t *entries = node->nd_text->tl_text;
		printf("    Text nodes (num, total len, max):  %d, %d, %d\n",
		       node->nd_text->tl_ntext,
		       node->nd_text->tl_len, node->nd_text->tl_max);
		for (uint32_t i = 0; i < node->nd_text->tl_ntext; i++) {
			_printText16(&entries[i], i);
		}
	}
	if (node->nd_child) {
		printf("    Children: num: %d\n",
		       node->nd_child->cl_numChild);

		printf("      textIndex, child_id:\n");
		for (uint32_t i = 0; i < node->nd_child->cl_numChild; ++i) {
			printf("        %d, %s\n",
			       node->nd_child->cl_child[i].ce_textIndex,
			       nsNode(&node->nd_child->cl_child[i].ce_id));
		}
	}
}

static int
_nsCompareNodes(nsNode_t *node1, nsNode_t *node2)
{
	if (NsUtil::nsCompareIds(&node1->nd_header.nh_id,
			  &node2->nd_header.nh_id) != 0)
		return 1;
	if (node1->nd_level != node2->nd_level)
		return 2;
	if ((node1->nd_header.nh_flags & NS_DBFLAGMASK) != (node2->nd_header.nh_flags & NS_DBFLAGMASK)) {
		return 3;
	}
	if (!(node1->nd_header.nh_flags & NS_ISDOCUMENT)) {
		if (NsUtil::nsCompareIds(&node1->nd_header.nh_parent,
				  &node2->nd_header.nh_parent) != 0)
			return 4;
		if (node1->nd_header.nh_uriIndex !=
		    node2->nd_header.nh_uriIndex)
			return 45;
		if (node1->nd_header.nh_name.n_prefix !=
		    node2->nd_header.nh_name.n_prefix)
			return 5;
		if (node1->nd_header.nh_name.n_type !=
		    node2->nd_header.nh_name.n_type)
			return 55;
		if (!NsUtil::nsStringEqual((NS_CH_CAST *)
				   node1->nd_header.nh_name.n_text.t_chars,
				   (NS_CH_CAST *)
				   node2->nd_header.nh_name.n_text.t_chars))
			return 6;
	}
	if (node1->nd_attrs) {
		if (!node2->nd_attrs)
			return 7;
		nsAttrList_t *attrs = node1->nd_attrs;
		nsAttrList_t *attrs1 = node2->nd_attrs;
		if (attrs->al_nattrs != attrs1->al_nattrs)
			return 8;
		for (uint32_t i = 0; i < attrs->al_nattrs; i++) {
			nsText_t *text = &attrs->al_attrs[i].a_name.n_text;
			nsText_t *text1 = &attrs1->al_attrs[i].a_name.n_text;
			int nlen = NsUtil::nsStringLen(
				(NS_CH_CAST*)text->t_chars);
			if (!NsUtil::nsStringEqual((NS_CH_CAST*)text->t_chars,
					   (NS_CH_CAST*)text1->t_chars))
				return 9;
			if (!NsUtil::nsStringEqual(
				    ((NS_CH_CAST*)text->t_chars)+nlen+1,
				    ((NS_CH_CAST*)text1->t_chars)+nlen+1))
				return 10;

			if (attrs->al_attrs[i].a_name.n_prefix !=
			    attrs1->al_attrs[i].a_name.n_prefix)
				return 100;
			if (attrs->al_attrs[i].a_name.n_type !=
			    attrs1->al_attrs[i].a_name.n_type)
				return 110;
		}
	}
	if (node1->nd_text) {
		if (!node2->nd_text)
			return 11;
		nsTextEntry_t *entries = node1->nd_text->tl_text;
		nsTextEntry_t *entries1 = node2->nd_text->tl_text;
		if (node1->nd_text->tl_ntext != node2->nd_text->tl_ntext)
			return 12;
		if (node1->nd_text->tl_len != node2->nd_text->tl_len) {
			return 13;
		}
		for (uint32_t i = 0; i < node1->nd_text->tl_ntext; i++) {
			if ((entries[i].te_type)
			    != (entries1[i].te_type))
				return 14;
			if (entries[i].te_text.t_len !=
			    entries1[i].te_text.t_len)
				return 15;
			if (!NsUtil::nsStringEqual(
				    (NS_CH_CAST*)entries[i].te_text.t_chars,
				    (NS_CH_CAST*)entries1[i].te_text.t_chars))
				return 16;
		}
	}
	if (node1->nd_child) {
		if (!node2->nd_child)
			return 17;
		if (node1->nd_child->cl_numChild !=
		    node2->nd_child->cl_numChild)
			return 18;
		for (uint32_t i = 0; i < node1->nd_child->cl_numChild; ++i) {
			if(node1->nd_child->cl_child[i].ce_textIndex !=
			   node2->nd_child->cl_child[i].ce_textIndex)
				return 19;
			if(NsUtil::nsCompareIds(&node1->nd_child->cl_child[i].ce_id,
						&node2->nd_child->cl_child[i].ce_id) != 0)
				return 20;
		}
	}
	return 0;
}

void
NsUtil::nsTestMarshal(MemoryManager *mmgr, nsNode_t *node)
{
	unsigned char *buf = 0;
	int bufsize = nsMarshalNode(node, NULL, true);
	buf = (unsigned char *) malloc(bufsize);

	nsMarshalNode(node, buf, false);
	nsNode_t *node2 = nsUnmarshalNode(mmgr, buf, bufsize, /*adoptBuffer*/false);

	printf("nsTestMarshal: compare results is %d\n",
	       _nsCompareNodes(node, node2));
	free(buf);
	nsFreeNode(mmgr, node2);
}

void
NsUtil::nsTestCopy(MemoryManager *mmgr, nsNode_t *node)
{
	nsNode_t *node2 = nsCopyNode(mmgr, node);

	printf("nsTestCopy: compare results is %d\n",
	       _nsCompareNodes(node, node2));

	nsFreeNode(mmgr, node2);
}

static const char *
getNsType(NsNodeType_t type)
{
	switch (type) {
	case nsNodeElement:
		return "Element";
	case nsNodeText:
		return "Text";
	case nsNodeAttr:
		return "Attribute";
	case nsNodeDocument:
		return "Element";
	default:
		return "Unknown";
	}
}

//
// Test DOM navigation methods...
//
void
NsUtil::dumpNsDomNode(NsDomNode *node, const char *indent)
{
	NsNodeType_t type = node->getNsNodeType();
	if (type == nsNodeEntStart ||
	    type == nsNodeEntEnd)
		return;
	const xmlch_t *name = node->getNsNodeName();
	const xmlch_t *value = node->getNsNodeValue();
	XMLChToUTF8 n(name);
	XMLChToUTF8 v(value);
	std::cout << indent << "Type: " << getNsType(type);
	std::cout << ", level: " << node->getNsLevel();
	// TBD: node id printer that handles non-printable numbers
//	std::cout << ", ID: " << (char*)nsNode(node->getNodeId());
	if (name)
		std::cout << ", name: " << n.str();
	if (value) {
		std::cout << ", value: ";
		std::cout << v.str() << std::endl;
	} else
		std::cout << std::endl;
	if (node->hasNsAttributes()) {
		NsDomNamedNodeMap *attrs =
			((NsDomElement*)node)->getNsAttributes();
		int length = attrs->getNsLength();
		std::cout << "\tHas Attributes (" << length << "):\n";
		for (int i = 0; i < length; i++) {
			NsDomAttr *attr = attrs->getNsItem(i);
			dumpNsDomNode(attr, "\t  ");
		}
	}
}

#endif // NS_DEBUG
