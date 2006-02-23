//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlInputStreamWrap.java,v 1.1 2005/12/12 17:01:41 gmf Exp $
//

package com.sleepycat.dbxml;

import java.io.InputStream;
import java.io.IOException;

import com.sleepycat.dbxml.XmlInputStream;

/**
 * An implementation of InputStream that uses XmlInputStream
 */ 
/* package */ class XmlInputStreamWrap extends java.io.InputStream {
    private XmlInputStream xis_;

    /* package */ XmlInputStreamWrap(XmlInputStream xis) {
	xis_ = xis;
    }

    /* InputStream methods */
    public void close() throws IOException {
	xis_.delete();
	xis_ = null;
    }

    public boolean markSupported() {
	return false;
    }

    public int read() throws IOException {
	try {
	    byte b[] = new byte[1];
	    long n = xis_.readBytes(b, 1);
	    if (n == 0)
		return -1;
	    return b[0];
	} catch (XmlException e) {
	    throw new IOException("XmlException in XmlInputStreamWrap.read()");
	}
    }

    public int read(byte[] b) throws IOException {
	try {
	    return (int) xis_.readBytes(b, b.length);
	} catch (XmlException e) {
	    throw new IOException("XmlException in XmlInputStreamWrap.read()");
	}
    }

    public int read(byte[] b, int off, int len) throws IOException {
	try {
	    /* this is a bit ugly.  If off is 0, readBytes() works, but
	     * if not, there is no way to pass an offset into b, so
	     * allocate a temporary array, read, and use System.arraycopy()
	     * to move the bytes into the right place.
	     */
	    if (off == 0)
		return (int) xis_.readBytes(b, len);
	    byte tb[] = new byte[len];
	    int tlen = (int) xis_.readBytes(tb, len);
	    if (tlen != 0) 
		System.arraycopy(tb, 0, b, off, tlen);
	    return tlen;
	} catch (XmlException e) {
	    throw new IOException("XmlException in XmlInputStreamWrap.read()");
	}
    }

    /** use default implementations of:
     * available()  -- returns 0
     * skip -- reads into dummy buffer
     */
}
