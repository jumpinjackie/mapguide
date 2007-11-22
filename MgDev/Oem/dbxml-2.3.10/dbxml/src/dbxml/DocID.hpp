//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DocID.hpp,v 1.7 2006/11/22 20:05:39 gmf Exp $
//

#ifndef __DOCID_HPP
#define	__DOCID_HPP

#include <iosfwd>
#include <db_cxx.h>
/** DocID
 * A class to encapsulate Document IDs, which are
 * 64-bits, as of release 2.3.
 */

class Dbt;

typedef u_int64_t docId_t;

namespace DbXml
{

class DbtOut;
class Container;
class QueryContext;
class XmlDocument;
class ReferenceMinder;
class OperationContext;

class DocID
{
public:
	DocID() : id_(0)
	{}
	DocID(docId_t id) : id_(id)
	{ }
	void reset()
	{
		id_ = 0;
	}
	const void *rawPtr() const
	{
		return &id_;
	}
	void setThisFromDbt(const Dbt &dbt);
	void setDbtFromThis(DbtOut &dbt) const;
	u_int32_t unmarshal(const void *buf);
	u_int32_t marshal(void *buf) const;
	u_int32_t size() const { return sizeof(docId_t); }
	u_int32_t marshalSize() const;
	bool operator==(const DocID &o) const
	{
		return id_ == o.id_;
	}
	bool operator==(docId_t id) const
	{
		return id_ == id;
	}
	int operator!=(const DocID &o) const
	{
		return id_ != o.id_;
	}
	int operator!=(docId_t id) const
	{
		return id_ != id;
	}
	bool operator<(const DocID &o) const
	{
		return id_ < o.id_;
	}
	bool operator>(const DocID &o) const
	{
		return id_ > o.id_;
	}
	bool operator<=(const DocID &o) const
	{
		return id_ <= o.id_;
	}
	bool operator>=(const DocID &o) const
	{
		return id_ >= o.id_;
	}
	std::string asString() const;
	void asChars(char *buf, int buflen) const;
	void fetchDocument(const Container *container, QueryContext *qc,
		XmlDocument &doc, ReferenceMinder *minder) const;
	void fetchDocument(const Container *container, OperationContext &oc,
		u_int32_t flags, XmlDocument &doc, ReferenceMinder *minder) const;

	static int compareMarshaled(const unsigned char *&p1,
				    const unsigned char *&p2);

private:
	docId_t id_;
};

}

#endif

