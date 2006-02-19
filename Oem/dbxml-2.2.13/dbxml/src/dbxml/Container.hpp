//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Container.hpp,v 1.133 2005/12/19 19:51:24 gmf Exp $
//

#ifndef __CONTAINER_HPP
#define	__CONTAINER_HPP

#include "ConfigurationDatabase.hpp"
#include "DictionaryDatabase.hpp"
#include "DocumentDatabase.hpp"
#include "SyntaxDatabase.hpp"
#include <string>
#include <sstream>
#include <db_cxx.h>
#include <dbxml/XmlManager.hpp>
#include "scoped_ptr.hpp"
#include "ID.hpp"
#include "IndexSpecification.hpp"
#include "DbWrapper.hpp"
#include "ReferenceCounted.hpp"
#include "Syntax.hpp"
#include "Log.hpp"
#include "KeyStatistics.hpp"
#include "HighResTimer.hpp"

class XPathParser;

namespace DbXml
{

class DbtIn;
class UpdateContext;
class Results;
class Document;
class Transaction;
class Statistics;
class IndexDbNotify;

// container format versions
#define VERSION_121 2
#define VERSION_20 3
#define VERSION_21 4
#define VERSION_22 5
#define CURRENT_VERSION VERSION_22
	
/**
 * Container encapsulates all the Berkeley DB databases that constitute
 * an XmlContainer. Container provides methods that implement the
 * functionality provided through the XmlContainer interface.
 */
class Container : public Log, public ReferenceCountedProtected,
		  private Transaction::Notify
{
public:
	static const unsigned int version;   ///< format version of container

	virtual ~Container();

	/** @name Simple Accessors */
	const std::string &getName() const { return name_; }
	XmlManager &getManager() { return mgr_; }
	XmlContainer::ContainerType getContainerType() const {
		return contType_;
	}
	bool getIndexNodes() const {
		return indexNodes_;
	}

	void setIndexNodes(Transaction *txn, bool indexNodes);

	bool getDoValidation() const {
		return doValidation_;
	}
	u_int32_t getPageSize() const {
		return pageSize_;
	}

	void sync(); // flush database cache to disk
	
	/** @name Container Operations */
	int open(Transaction *txn, u_int32_t flags, int mode, bool doVersionCheck);
	int getDocument(OperationContext &context, const std::string &name,
			XmlDocument &document, u_int32_t flags) const;
	int getDocument(OperationContext &context, const ID &id,
			XmlDocument &document, u_int32_t flags) const;
	int addDocument(Transaction *txn, Document &document,
			UpdateContext &context, u_int32_t flags);
	int deleteDocument(Transaction *txn, const std::string &name,
			   UpdateContext &context);
	int deleteDocument(Transaction *txn, Document &document,
			   UpdateContext &context);
	int updateDocument(Transaction *txn, Document &document,
			   UpdateContext &context);
	int setIndexSpecification(Transaction *txn,
				  const IndexSpecification &index,
				  UpdateContext &context);

	Results *lookupIndex(Transaction *txn,
			     XmlQueryContext &context,
			     const IndexLookup &il,
			     u_int32_t flags);

	Statistics *lookupStatistics(Transaction *txn,
				     Name *child,
				     Name *parent,
				     const std::string &indexStr,
				     const XmlValue &value);
	
	/** @name Configuration Database Accessors */
	ConfigurationDatabase *getConfigurationDB() {
		return configuration_.get();
	}
	const ConfigurationDatabase *getConfigurationDB() const {
		return configuration_.get();
	}

	/** @name Dictionary Database Accessors */
	DictionaryDatabase *getDictionaryDB() { return dictionary_.get(); }
	const DictionaryDatabase *getDictionaryDB() const {
		return dictionary_.get();
	}

	/** @name Document Database Accessors */
	DocumentDatabase *getDocumentDB() { return document_.get(); }
	const DocumentDatabase *getDocumentDB() const {
		return document_.get();
	}

	/** @name Index Databases Accessors */
	SyntaxDatabase *getIndexDB(Syntax::Type type, Transaction *txn = 0,
				   bool toWrite = false);
	const SyntaxDatabase *getIndexDB(Syntax::Type type) const;

	/** @name Utility Methods */
	virtual void log(ImplLogCategory c, ImplLogLevel l,
			  const std::ostringstream &s) const;
	virtual void log(ImplLogCategory c, ImplLogLevel l,
			  const std::string &s) const;
	virtual void checkFlags(const FlagInfo *flag_info, const char *function,
				u_int32_t flags, u_int32_t mask) const;

	/** @name Administration methods */
	
	bool hasAlias() const { return hasAlias_; }
	bool addAlias(const std::string &alias);
	bool removeAlias(const std::string &alias);

	// close allows explicit shut down of databases, and not count
	// on lazy references or garbage collection.  It's only
	// supported for non-C++ languages.
	void close();
	
	// @{
	static void dump(Manager &mgr, const std::string &name,
			 std::ostream *out);
	static void load(Manager &mgr, const std::string &name,
			 std::istream *in, unsigned long *lineno,
			 UpdateContext &context);
	static void verify(Manager &mgr, const std::string &name,
			   std::ostream *out, u_int32_t flags);
	
	static int writeHeader(const std::string &name, std::ostream *out);
	static int verifyHeader(const std::string &name, std::istream *in);

	// upgrade containers
	static void upgradeContainer(const std::string &name,
				     Manager &mgr,
				     UpdateContext &context);
	// reindex container
	static void reindexContainer(Transaction *txn,
				     Manager &mgr,
				     const std::string &name,
				     UpdateContext &context,
				     u_int32_t flags);
	
	void upgrade(unsigned int saved_version, unsigned int current_version,
		     UpdateContext &uc);
	//@}

	static unsigned int checkContainer(const std::string &name, DbEnv &dbenv);
protected:
	Container(Manager &mgr, const std::string &name,
		  u_int32_t pagesize, u_int32_t seqIncr,
		  XmlContainer::ContainerType type);
	// VC6 needs this to be protected, rather than private
	// re-index entire container, based on IS.  Used for
	// addition of indexes, including container loading
	int reindex(Transaction *txn, const IndexSpecification &is,
		    UpdateContext &context);
private:
	// no need for copy and assignment
	Container(const Container&);
	void operator=(const Container &);
	
	int getUniqueDocumentName(OperationContext &context,
				  std::string &name, ID &id) const;
	int getDocumentID(OperationContext &context,
			  const std::string &name, ID &id) const;
	virtual void postNotify(bool commit);
	// remove the indexes in the IS
	int removeIndexes(Transaction *txn, const IndexSpecification &is,
			  UpdateContext &context);
	int removeIndexes(OperationContext &oc, const IndexVector &iv,
			  UpdateContext &context, bool isDefault);
	
	// removes, then re-creates indexes; used for upgrade.
	void reloadIndexes(Transaction *txn, UpdateContext &uc);
	void closeIndexes(int idx = -1);
	void openIndexDbs(Transaction *txn, u_int32_t flags, int mode);
protected:
	XmlManager mgr_;
private:
	DbEnv *environment_;
	Transaction *openingTransaction_;

	std::string name_;
	u_int32_t pageSize_;
	u_int32_t seqIncr_;
	XmlContainer::ContainerType contType_;
	bool indexNodes_;
	bool doValidation_;
	bool hasAlias_;
	IndexDbNotify *indexDbNotify_;

	ConfigurationDatabase::Ptr configuration_;
	DictionaryDatabase::Ptr dictionary_;
	DocumentDatabase::Ptr document_;
	SyntaxDatabase::Vector indexes_;

	friend class XmlManager;
	friend class IndexDbNotify;
};

}

#endif

