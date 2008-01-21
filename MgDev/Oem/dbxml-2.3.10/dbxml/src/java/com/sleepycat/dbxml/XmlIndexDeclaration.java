//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlIndexDeclaration.java,v 1.9 2006/10/30 17:46:02 bostic Exp $
//

package com.sleepycat.dbxml;

public class XmlIndexDeclaration {
    public String index;
    public String name;
    public String uri;


    public XmlIndexDeclaration(String uri, String name, String index) {
        this.uri = uri;
        this.name = name;
        this.index = index;
    }
}
