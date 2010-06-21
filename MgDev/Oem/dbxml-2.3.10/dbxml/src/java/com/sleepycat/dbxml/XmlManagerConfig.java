//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlManagerConfig.java,v 1.8 2006/10/30 17:46:02 bostic Exp $
//

package com.sleepycat.dbxml;

public class XmlManagerConfig implements Cloneable {

    public static final XmlManagerConfig DEFAULT = new XmlManagerConfig();

    public boolean getAdoptEnvironment() {
	return adoptEnv;
    }

    public XmlManagerConfig setAdoptEnvironment(boolean value) {
	this.adoptEnv = value;
	return this;
    }

    public boolean getAllowExternalAccess() {
	return allowExternalAccess;
    }

    public XmlManagerConfig setAllowExternalAccess(boolean value) {
	this.allowExternalAccess = value;
	return this;
    }

    public boolean getAllowAutoOpen() {
	return allowAutoOpen;
    }

    public XmlManagerConfig setAllowAutoOpen(boolean value) {
	this.allowAutoOpen = value;
	return this;
    }

    /* package */
    int makeFlags() {
	int flags = 0;
	if (allowExternalAccess)
	    flags |= dbxml_javaConstants.DBXML_ALLOW_EXTERNAL_ACCESS;
	if (allowAutoOpen)
	    flags |= dbxml_javaConstants.DBXML_ALLOW_AUTO_OPEN;
	return flags;
    }

    /* Flags */
    private boolean adoptEnv = false;
    private boolean allowExternalAccess = false;
    private boolean allowAutoOpen = false;
}
