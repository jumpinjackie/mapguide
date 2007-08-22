//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DocID.cpp,v 1.5 2006/10/31 23:39:51 jsnelson Exp $
//

#include <sstream>

#include "DbXmlInternal.hpp"
#include "DocID.hpp"
#include "ScopedDbt.hpp"
#include "Manager.hpp"
#include "ReferenceMinder.hpp"
#include "QueryContext.hpp"
#include "Document.hpp"
#include "nodeStore/NsFormat.hpp"
#include "TypeConversions.hpp"

#include "db_utils.h"

using namespace DbXml;
using namespace std;

void DocID::setThisFromDbt(const Dbt &dbt)
{
	unmarshal(dbt.get_data());
}

void DocID::setDbtFromThis(DbtOut &dbt) const
{
	char buf[9];
	u_int32_t len = marshal(buf);
	dbt.set(buf, len); // copies the data
}

u_int32_t DocID::unmarshal(const void *buf)
{
	return NsFormat::unmarshalInt64((const xmlbyte_t *) buf, &id_);
}

u_int32_t DocID::marshal(void *buf) const
{
	return NsFormat::marshalInt64((xmlbyte_t *) buf, id_);
}

u_int32_t DocID::marshalSize() const
{
	return NsFormat::countInt64(id_);
}

std::string DocID::asString() const
{
	return DbXml::toString(id_);
}

void DocID::asChars(char *buf, int buflen) const
{
	DbXml::toChars(id_, buf, (size_t)buflen);
}

void DocID::fetchDocument(const Container *container, QueryContext *qc,
			  XmlDocument &doc, ReferenceMinder *minder) const
{
	fetchDocument(container, qc->getOperationContext(), qc->getFlags(), doc, minder);
}

void DocID::fetchDocument(const Container *container, OperationContext &oc,
	u_int32_t flags, XmlDocument &doc, ReferenceMinder *minder) const
{
	// Look up the document in the cache
	if(minder != 0) {
		doc = minder->findDocument(container, *this);
	}

	if(doc.isNull()) {
		// Fetch the document from the container
		int err = container->getDocument(oc, *this, doc, flags);
		// The document should be found
		if(err != 0) {
			container->log(Log::C_QUERY, Log::L_ERROR,
				"Invalid index values found during query (document not found)");
			throw XmlException(XmlException::INTERNAL_ERROR, "Invalid indexes");
		}
		if(minder != 0) {
			// Store the document in the cache
			minder->addDocument(doc);
		}
	}
}

int DocID::compareMarshaled(const unsigned char *&p1,
			    const unsigned char *&p2)
{
	// Sort based on marshaled integer length first
	// NOTE: countMarshaledInt knows about 64-bit encodings
	int count = NsFormat::countMarshaledInt(p1);
	int res = count - NsFormat::countMarshaledInt(p2);
	if(res != 0) return res;

	// Marshaled integers can be sorted using normal
	// lexicographical comparison if they are the same
	// length.
	while(count && (res = (int)*p1 - (int)*p2) == 0) {
		++p1; ++p2; --count;
	}
	return res;
}

