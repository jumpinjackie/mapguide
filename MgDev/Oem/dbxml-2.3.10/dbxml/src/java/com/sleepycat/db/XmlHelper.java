//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlHelper.java,v 1.5 2006/10/30 17:46:01 bostic Exp $
//

// This class allows dipping into package-protected state
// in Berkeley DB.  Ideally, there would be a better way.
// That is TBD.

package com.sleepycat.db;
import com.sleepycat.db.internal.DbEnv;
import com.sleepycat.db.internal.DbTxn;
import com.sleepycat.db.LockMode;

public class XmlHelper {

    public static DbEnv getDbEnv(Environment env) {
	return env.unwrap();
    }
    public static DbTxn getDbTxn(Transaction txn) {
	return txn.txn;
    }
    public static Environment makeNewEnvironment(DbEnv env) 
    throws DatabaseException {
	return new Environment(env);
    }
    public static int getFlag(LockMode mode)
    throws DatabaseException {
	return LockMode.getFlag(mode);
    }
};
