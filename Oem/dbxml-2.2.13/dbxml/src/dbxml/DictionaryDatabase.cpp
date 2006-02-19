//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DictionaryDatabase.cpp,v 1.16 2005/11/22 16:39:49 jsnelson Exp $
//

#include <sstream>

#include <dbxml/XmlException.hpp>
#include "DictionaryDatabase.hpp"
#include "Manager.hpp"
#include "Name.hpp"
#include "ScopedDbt.hpp"
#include "Container.hpp"
#include "OperationContext.hpp"
#include "db_utils.h"

using namespace DbXml;
using namespace std;

static const char *dictionary_name = "dictionary";

DictionaryDatabase::DictionaryDatabase(DbEnv *env, Transaction *txn, const std::string &name, u_int32_t pageSize, u_int32_t flags, int mode)
	: environment_(env),
	  containerName_(name),
	  primary_(new PrimaryDatabase(env, name, dictionary_name, pageSize, flags & DB_XA_CREATE)),
	  secondary_(new SecondaryDatabase(env, name, dictionary_name, pageSize, flags & DB_XA_CREATE)),
	  nidName_(0),
	  nidRoot_(0)
{
	flags &= ~DB_XA_CREATE;
	int err = 0;
	try {

		// Open the Db objects
		err = primary_->open(txn, flags, mode);
		if (err == 0)
			err = secondary_->open(txn, /*duplicates*/true, flags, mode);

		// Lookup/Define the dbxml namespace names (but don't define
		// if this is a read-only container)
		OperationContext oc(txn);
		bool rdonly = (flags & DB_RDONLY) != 0;
		if (err == 0)
			err = lookupName(oc, Name::dbxml_colon_name, nidName_, !rdonly);
		if (err == 0)
			err = lookupName(oc, Name::dbxml_colon_root, nidRoot_, !rdonly);

	} catch (DbException &e) {
		if(e.get_errno() == EEXIST) {
			throw XmlException(XmlException::CONTAINER_EXISTS, e.what());
		}
		else {
			throw XmlException(e);
		}
	}
	if(err == EEXIST) {
		throw XmlException(XmlException::CONTAINER_EXISTS, db_strerror(err));
	}
	else if(err != 0) {
		throw XmlException(err);
	}
}

DictionaryDatabase::~DictionaryDatabase()
{
	// nothing to do
}

int DictionaryDatabase::defineName(OperationContext &context, const Name &name, ID &id)
{
	// Primary { id -> uri, prefix, name }
	// Secondary { uri:name -> id }
	id.reset();
	name.setDbtFromThis_PrimaryValue(context.data());
	int err = primary_->appendPrimary(context, id, &context.data(), /*no flags*/0);
	if (err == 0) {
		id.setDbtFromThis(context.key());
		name.setDbtFromThis_SecondaryKey(context.data());
		err = secondary_->put(context.txn(), &context.data(), &context.key(), /*no flags*/0);
		if (err == 0 &&
			Log::isLogEnabled(Log::C_DICTIONARY, Log::L_INFO)) {
			ostringstream oss;
			oss << "Define new name " << id << " -> " << name;
			Log::log(environment_, Log::C_DICTIONARY, Log::L_INFO, containerName_.c_str(), oss.str().c_str());
		}
	}
	return err;
}

int DictionaryDatabase::defineName(OperationContext &context, const char *uriname, ID &id)
{
	Name name(uriname);
	return defineName(context, name, id);
}

int DictionaryDatabase::lookupName(OperationContext &context, const ID &id, Name &name) const
{
	int err = 0;
	if (id == nidName_) {
		name = Name::dbxml_colon_name;
	} else if (id == nidRoot_) {
		name = Name::dbxml_colon_root;
	} else {
		// primary key is the integer value of the id (database is
		// recno)
		id.setDbtFromThisAsId(context.key());
		err = primary_->get(context.txn(), &context.key(), &context.data(), /*no flags*/0);
		if (err == 0) {
			name.setThisFromDbt(context.data());
		} else {
			name.reset();
		}
	}
	return err;
}

int DictionaryDatabase::lookupName(OperationContext &context, const Name &name, ID &id, bool define)
{
	int err = 0;
	if (name == Name::dbxml_colon_name) {
		id = nidName_;
	} else if (name == Name::dbxml_colon_root) {
		id = nidRoot_;
	}
	if (id == 0) {
		name.setDbtFromThis_SecondaryKey(context.key());
		err = secondary_->get(context.txn(), &context.key(), &context.data(), /*no flags*/0);
		if (err == 0) {
			id.setThisFromDbt(context.data());
		} else if (err == DB_NOTFOUND && define) {
			err = defineName(context, name, id); // define from uri:name
		} else {
			id.reset();
		}
	}
	return err;
}

int DictionaryDatabase::lookupName(OperationContext &context, const char *uriname, ID &id, bool define)
{
	context.key().set((void*)uriname, strlen(uriname));
	int err = secondary_->get(context.txn(), &context.key(), &context.data(), /*no flags*/0);
	if (err == 0) {
		id.setThisFromDbt(context.data());
	} else if (err == DB_NOTFOUND && define) {
		err = defineName(context, uriname, id); // define from uri:name
	} else {
		id.reset();
	}
	return err;
}

int DictionaryDatabase::lookupName(OperationContext &context,
				   const std::string &uriname, ID &id) const
{
	return const_cast<DictionaryDatabase*>(this)->lookupName(context, uriname.c_str(), id, /*define=*/false);
}

int DictionaryDatabase::dump(DbEnv *env, const std::string &name,
			     std::ostream *out)
{
	PrimaryDatabase::Ptr primary(new PrimaryDatabase(env, name, dictionary_name, 0, 0));
	SecondaryDatabase::Ptr secondary(new SecondaryDatabase(env, name, dictionary_name, 0, 0));

	int err = Container::writeHeader(primary->getDatabaseName(), out);
	if(err == 0) err = primary->dump(out);

	if(err == 0) err = Container::writeHeader(secondary->getDatabaseName(), out);
	if(err == 0) err = secondary->dump(out);

	return err;
}

int DictionaryDatabase::load(DbEnv *env, const std::string &name, std::istream *in, unsigned long *lineno)
{
	PrimaryDatabase::Ptr primary(new PrimaryDatabase(env, name, dictionary_name, 0, 0));
	SecondaryDatabase::Ptr secondary(new SecondaryDatabase(env, name, dictionary_name, 0, 0));

	// Load primary
	int err = Container::verifyHeader(primary->getDatabaseName(), in);
	if(err != 0) {
		ostringstream oss;
		oss << "DictionaryDatabase::load() invalid database dump file loading '" << name << "'";
		Log::log(env, Log::C_DICTIONARY, Log::L_ERROR, oss.str().c_str());
	}
	else {
		err = primary->load(in, lineno);
	}

	// Load secondary
	if(err == 0) {
		err = Container::verifyHeader(secondary->getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "DictionaryDatabase::load() invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_DICTIONARY, Log::L_ERROR, oss.str().c_str());
		}
		else {
			err = secondary->load(in, lineno);
		}
	}

	return err;
}

int DictionaryDatabase::verify(DbEnv *env, const std::string &name,
			       std::ostream *out, u_int32_t flags)
{
	PrimaryDatabase::Ptr primary(new PrimaryDatabase(env, name, dictionary_name, 0, 0));
	SecondaryDatabase::Ptr secondary(new SecondaryDatabase(env, name, dictionary_name, 0, 0));

	int err = 0;
	if(flags & DB_SALVAGE) err = Container::writeHeader(primary->getDatabaseName(), out);
	if(err == 0) err = primary->verify(out, flags);

	if(err == 0 && (flags & DB_SALVAGE)) err = Container::writeHeader(secondary->getDatabaseName(), out);
	if(err == 0) err = secondary->verify(out, flags);

	return err;
}

// upgrades required:
//  from version 3 (2.0) to 4 (2.1) or higher
//  3->4 (2.0->2.1)
//   o byte-swap ids
//  4->5 (2.1->2.2)
//   o marshal all ids in secondary.  Keys are unchanged.
//     old value format: nameID as 32-bit int
//     new value format: nameID as packed/marshaled int

//static
void DictionaryDatabase::upgrade(const std::string &name, Manager &mgr,
				 int old_version, int current_version)
{
	DBXML_ASSERT(old_version < current_version);
	int err = 0;
	// could probably do 2-stage upgrade in one iteration, but
	// for now, it's simpler to do 2 separate stages
	if ((old_version == VERSION_20) && Manager::isBigendian()) {
		SecondaryDatabase secondary(mgr.getDbEnv(),
					    name, dictionary_name, 0, 0);
		err = secondary.open(0, /*duplicates*/true, 0, 0);
		// byte-swap dictionary ids
		// iterate through all records in secondary
		Dbt key;
		Dbt data;
		Cursor curs(secondary, (Transaction*)0, DbXml::CURSOR_WRITE);
		int ret = 0;
		while ((ret = curs.get(&key, &data, DB_NEXT)) == 0) {
			u_int32_t *p = reinterpret_cast<u_int32_t*>(data.get_data());
			curs.del(0);
			M_32_SWAP(*p);
			int putRet = curs.put(&key, &data, DB_NODUPDATA);
			if (putRet != 0)
				err = 1;
		}
	}
	if (err == 0 && current_version == VERSION_22) {
		SecondaryDatabase secondary(mgr.getDbEnv(),
					    name, dictionary_name, 0, 0);
		err = secondary.open(0, /*duplicates*/true, 0, 0);
		Dbt key;
		Dbt data;
		Cursor curs(secondary, (Transaction*)0, DbXml::CURSOR_WRITE);
		int ret = 0;
		while ((ret = curs.get(&key, &data, DB_NEXT)) == 0) {
			u_int32_t tid = *(reinterpret_cast<u_int32_t *>
					  (data.get_data()));
			if (Manager::isBigendian())
				M_32_SWAP(tid);
			// create and marshal ID, then put
			DbtOut dbt;
			ID id(tid);
			id.setDbtFromThis(dbt);
			curs.del(0);
			int putRet = curs.put(&key, &dbt, DB_NODUPDATA);
			if (putRet != 0)
				err = 1;
		}
	}
	if (err != 0)
		throw XmlException(
			XmlException::DATABASE_ERROR,
			"Unexpected error upgrading Dictionary DB");
}
