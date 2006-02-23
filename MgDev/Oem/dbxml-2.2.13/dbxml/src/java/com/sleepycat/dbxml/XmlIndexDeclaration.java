//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlIndexDeclaration.java,v 1.7 2005/04/05 16:44:20 bostic Exp $
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
