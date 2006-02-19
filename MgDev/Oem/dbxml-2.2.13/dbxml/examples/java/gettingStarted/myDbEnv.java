//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: myDbEnv.java,v 1.9 2005/06/21 21:56:12 sarette Exp $
//

package dbxml.gettingStarted;

import java.io.*;
import java.util.*;
import com.sleepycat.db.*;
import com.sleepycat.dbxml.*;

//Class used to open and close a Berkeley DB environment
public class myDbEnv
{
    private Environment dbEnv_ = null;
    private XmlManager mgr_ = null;
    private boolean dbEnvIsOpen_ = false;
    private File path2DbEnv_ = null;

    public myDbEnv(File path2DbEnv) 
	throws Throwable {
	if (! path2DbEnv.isDirectory()) {
	    throw new Exception(path2DbEnv.getPath() + 
				" does not exist or is not a directory.");
	}

        EnvironmentConfig config = new EnvironmentConfig();
        config.setCacheSize(50 * 1024 * 1024);
        config.setAllowCreate(true);
        config.setInitializeCache(true);
        config.setTransactional(true);
        config.setInitializeLocking(true);
	config.setInitializeLogging(true);
	config.setErrorStream(System.err);
        dbEnv_ = new Environment(path2DbEnv, config);

	//Boolean used to know whether to close the environment
	// when the cleanup() method is called.
	dbEnvIsOpen_ = true;
	path2DbEnv_ = path2DbEnv;
	mgr_ = new XmlManager(dbEnv_, null);
    }

    //Returns the path to the database environment
    public File getDbEnvPath() { return path2DbEnv_; }

    //Returns the database environment encapsulated by this class.
    public Environment getEnvironment() { return dbEnv_; }

    //Returns the XmlManager encapsulated by this class.
    public XmlManager getManager() { return mgr_; }

    //Used to close the environment
    public void cleanup() throws DatabaseException
    {
       if (dbEnvIsOpen_) {
            dbEnv_.close();
            dbEnvIsOpen_ = false;
       }
    }
}
