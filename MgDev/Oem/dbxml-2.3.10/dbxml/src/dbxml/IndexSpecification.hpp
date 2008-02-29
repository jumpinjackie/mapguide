//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: IndexSpecification.hpp,v 1.67.2.1 2006/12/19 15:22:03 gmf Exp $
//

#ifndef __INDEXSPECIFICATION_HPP
#define	__INDEXSPECIFICATION_HPP

#include <vector>
#include <string>
#include <map>
#include "Name.hpp"
#include "ScopedPtr.hpp"
#include "ReferenceCounted.hpp"
#include "Buffer.hpp"
#include <dbxml/XmlIndexSpecification.hpp>
#include "ConfigurationDatabase.hpp"
#include "Syntax.hpp"

namespace DbXml
{

extern const char *metaDataName_uri_name;
extern const char *metaDataName_uri_root;

class Transaction;
class SecondaryDatabase;
class Name;
class Cursor;

//
// Specifies an index.
//
class Index
{
public:
	typedef std::vector<Index> Vector;

	// Currently implemented indexing strategies...
	//
	// node-element-presence
	// node-attribute-presence
	// node-element-equality-string
	// node-element-equality-number
	// node-element-substring-string
	// node-attribute-equality-string
	// node-attribute-equality-number
	// node-attribute-substring-string
	// edge-element-presence
	// edge-attribute-presence
	// edge-element-equality-string
	// edge-element-equality-number
	// edge-element-substring-string
	// edge-attribute-equality-string
	// edge-attribute-equality-number
	// edge-attribute-substring-string
	//

	// These values should be kept the same as the ones in XmlIndexSpecification::Type
	/// Format = I--U--PP-----NNN-----KKKSSSSSSSS
	enum Type {
		INDEXER_ADD = 0x00000000,
		INDEXER_DELETE = 0x80000000,
		INDEXER_MASK = 0x80000000,

		UNIQUE_OFF = 0x00000000,
		UNIQUE_ON = 0x10000000,
		UNIQUE_MASK = 0x10000000,

		PATH_NONE = 0x00000000,  //
		PATH_NODE = 0x01000000,  // node-*-*-*
		PATH_EDGE = 0x02000000,  // edge-*-*-*
		PATH_MASK = 0x03000000,

		NODE_NONE = 0x00000000,  //
		NODE_ELEMENT = 0x00010000,  // *-element-*-*
		NODE_ATTRIBUTE = 0x00020000,  // *-attribute-*-*
		NODE_METADATA = 0x00030000,  // *-metadata-*-*
		NODE_MASK = 0x00070000,

		KEY_NONE = 0x00000000,  //
		KEY_PRESENCE = 0x00000100,  // *-*-presence-*
		KEY_EQUALITY = 0x00000200,  // *-*-equality-*
		KEY_SUBSTRING = 0x00000300,  // *-*-substring-*
		KEY_MASK = 0x00000700,

		SYNTAX_NONE = 0x00000000,  // *-*-*
		//		SYNTAX_STRING=  0x00000001, // *-*-*-string
		//		SYNTAX_NUMBER=  0x00000002, // *-*-*-number
		SYNTAX_MASK = 0x000000ff,

		NONE = (PATH_NONE | NODE_NONE | KEY_NONE | SYNTAX_NONE),

		NE = (PATH_NODE | NODE_ELEMENT),
		EE = (PATH_EDGE | NODE_ELEMENT),
		NA = (PATH_NODE | NODE_ATTRIBUTE),
		EA = (PATH_EDGE | NODE_ATTRIBUTE),
		NM = (PATH_NODE | NODE_METADATA),

		NEP = (PATH_NODE | NODE_ELEMENT | KEY_PRESENCE),
		NAP = (PATH_NODE | NODE_ATTRIBUTE | KEY_PRESENCE),
		EEP = (PATH_EDGE | NODE_ELEMENT | KEY_PRESENCE),
		EAP = (PATH_EDGE | NODE_ATTRIBUTE | KEY_PRESENCE),
		NMP = (PATH_NODE | NODE_METADATA | KEY_PRESENCE),

		NEE = (PATH_NODE | NODE_ELEMENT | KEY_EQUALITY),  // node-element-equality-*
		NAE = (PATH_NODE | NODE_ATTRIBUTE | KEY_EQUALITY),  // node-attribute-equality-*
		EEE = (PATH_EDGE | NODE_ELEMENT | KEY_EQUALITY),  // edge-element-equality-*
		EAE = (PATH_EDGE | NODE_ATTRIBUTE | KEY_EQUALITY),  // edge-attribute-equality-*
		NME = (PATH_NODE | NODE_METADATA | KEY_EQUALITY),  // metadata-equality-*
		XEE = (NODE_ELEMENT | KEY_EQUALITY),  // *-element-equality-*

		NES = (PATH_NODE | NODE_ELEMENT | KEY_SUBSTRING),  // node-element-substring-*
		NAS = (PATH_NODE | NODE_ATTRIBUTE | KEY_SUBSTRING),  // node-attribute-substring-*
		EES = (PATH_EDGE | NODE_ELEMENT | KEY_SUBSTRING),  // edge-element-substring-*
		EAS = (PATH_EDGE | NODE_ATTRIBUTE | KEY_SUBSTRING),  // edge-attribute-substring-*
		NMS = (PATH_NODE | NODE_METADATA | KEY_SUBSTRING),  // metadata-substring-*
		XES = (NODE_ELEMENT | KEY_SUBSTRING),  // *-element-substring-*

		PN_MASK = (PATH_MASK | NODE_MASK),
		NK_MASK = (NODE_MASK | KEY_MASK),
		PK_MASK = (PATH_MASK | KEY_MASK),
		PNK_MASK = (PATH_MASK | NODE_MASK | KEY_MASK),
		PKS_MASK = (PATH_MASK | KEY_MASK | SYNTAX_MASK),
		PNKS_MASK = (PATH_MASK | NODE_MASK | KEY_MASK | SYNTAX_MASK)
	};

	Index();
	Index(XmlIndexSpecification::Type type, XmlValue::Type syntax);
	Index(unsigned long i);
	Index(const std::string &s);

	//default is ok.
	// ~Index();
	// Index(const Index&);
	// Index &operator=(const Index &);

	void reset()
	{
		index_ = NONE;
	}
	bool set(const std::string &s);
	bool set(unsigned long index, unsigned long mask);
	bool set(unsigned long i);
	void setFromPrefix(unsigned char prefix);

	Index::Type getUnique() const
	{
		return (Type)(index_&UNIQUE_MASK);
	}
	Index::Type getPath() const
	{
		return (Type)(index_&PATH_MASK);
	}
	Index::Type getNode() const
	{
		return (Type)(index_&NODE_MASK);
	}
	Index::Type getKey() const
	{
		return (Type)(index_&KEY_MASK);
	}
	Syntax::Type getSyntax() const
	{
		return (Syntax::Type)(index_&SYNTAX_MASK);
	}
	Index::Type get() const
	{
		return (Type)index_;
	}

	unsigned char getKeyPrefix() const;

	bool indexerAdd() const;
	bool isNoneIndex() const;
	bool isValidIndex() const;
	bool equals(Index::Type index) const
	{
		return index_ == index;
	}
	bool equalsMask(Index::Type test, Index::Type mask) const;

	std::string asString() const;

	operator const Index::Type() const
	{
		return (Index::Type)index_;
	}
	bool operator==(const Index &index) const
	{
		return index.index_ == index_;
	}
	bool operator<(const Index &index) const
	{
		return index_ < index.index_;
	}

	static Index::Type getPathFromPrefix(unsigned char prefix);

private:
	std::string axisAsName(Type index) const;

	unsigned long index_;
};

std::ostream& operator<<(std::ostream& s, const Index &index);

inline Index::Type Index::getPathFromPrefix(unsigned char prefix)
{
	return (Type)((((unsigned long)prefix)<< 18) & PATH_MASK);
}

//
// Specifies the indexing for a node.
//
class IndexVector
{
public:
	IndexVector();
	IndexVector(const Name &name);
	IndexVector(const IndexVector &iv);
	~IndexVector();

	bool enableIndex(Index index);
	void enableIndex(const IndexVector &iv);

	bool disableIndex(const Index &index);
	void disableIndex(const IndexVector &iv);

	void set(unsigned long index);

	Index getIndex(const Index::Type &test, const Index::Type &mask) const;
	bool isEnabled(const Index::Type &test, const Index::Type &mask) const;
	bool isIndexed() const;
	const Syntax *getNextSyntax(int &i, Index::Type test, Index::Type mask, Index &index) const;

	std::string getURI() const
	{
		return name_.getURI();
	}
	std::string getName() const
	{
		return name_.getName();
	}
	const Name &getNameObject() const
	{
		return name_;
	}
	std::string asString() const;

	typedef Index::Vector::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	void clear() {
		iv_.clear();
	}

private:
	// no need for assignment
	IndexVector &operator=(const IndexVector &);

	void getNextIndex(int &i, Index::Type test, Index::Type mask, Index &index) const;

	Name name_;
	Index::Vector iv_;
};

class char_star_compare
{
public:
	bool operator()(const char *s1, const char *s2) const
	{
		return ::strcmp(s1, s2) < 0;
	}
};

//
// Specifies the indexing for a container.
//
class IndexSpecification : public ReferenceCounted
{
public:
	typedef std::map<const char*, IndexVector*, char_star_compare> IndexMap;
	typedef IndexMap::const_iterator const_iterator;
	typedef std::vector<Name> NameList;

	IndexSpecification();
	IndexSpecification(const IndexSpecification&);
	virtual ~IndexSpecification();

	void addIndex(const std::string &uri, const std::string &name, const Index &index);
	void addIndex(const std::string &uri, const std::string &name, const std::string &index);
	void deleteIndex(const std::string &uri, const std::string &name, const Index &index);
	void deleteIndex(const std::string &uri, const std::string &name, const std::string &index);

	void addDefaultIndex(const Index &index);
	void addDefaultIndex(const std::string &index);
	void deleteDefaultIndex(const Index &index);
	void deleteDefaultIndex(const std::string &index);

	void enableIndex(const IndexSpecification &is);
	void disableIndex(const IndexSpecification &is);

	void set(unsigned long index);

	const IndexVector *getIndexOrDefault(const char *uriname) const;

	bool isContentIndexed() const;
	bool isMetaDataDefaultIndex() const;
	
	bool isIndexed(Index::Type test, Index::Type mask) const;
	const NameList getIndexed(Index::Type test, Index::Type mask) const;

	IndexVector *getDefaultIndex()
	{
		return &defaultIndex_;
	}
	const IndexVector *getDefaultIndex() const
	{
		return &defaultIndex_;
	}
	const_iterator begin() const
	{
		return indexMap_.begin();
	}
	const_iterator end() const
	{
		return indexMap_.end();
	}
	bool find(const std::string &uri, const std::string &name, std::string &index);

	void clear();
	int read(const ConfigurationDatabase *config, Transaction *txn, bool lock);
	int write(ConfigurationDatabase *config, Transaction *txn) const;
	int upgrade(DbWrapper &config_db, int old_version, int current_version);
	std::string asString() const;

private:
	// no need for assignment
	IndexSpecification &operator=(const IndexSpecification &);

	void enableIndex(const char *uriname, const Index &index);
	void enableIndex(const char *uriname, const std::string &indexString);
	void enableIndex(const char *uriname, const IndexVector &iv);

	void disableIndex(const char *uriname, const Index &index);
	void disableIndex(const char *uriname, const std::string &indexString);
	void disableIndex(const char *uriname, const IndexVector &iv);

	void writeToBuffer() const;
	void upgradeEnableIndex(const char *uriname, const std::string &indexString);

	mutable Buffer buffer_;
	mutable Buffer tmpBuffer_;
	IndexMap indexMap_;
	IndexVector defaultIndex_;
};

class IndexSpecificationIterator : public ReferenceCounted
{
public:
	IndexSpecificationIterator(const IndexSpecification &is);

	bool next(std::string &uri, std::string &name, XmlIndexSpecification::Type &type, XmlValue::Type &syntax);
	bool next(std::string &uri, std::string &name, std::string &index);
	void reset();
private:
	void setVectorFromSpecIterator();

	const IndexSpecification &is_;
	IndexSpecification::const_iterator isi_;
	const IndexVector *iv_;
	IndexVector::const_iterator ivi_;
};

}

#endif

