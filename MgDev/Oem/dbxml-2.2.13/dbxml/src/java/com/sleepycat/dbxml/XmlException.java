//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlException.java,v 1.19 2005/09/28 14:39:20 jsnelson Exp $
//
// IMPORTANT: the enumerations here must match those in 
// include/dbxml/XmlException.hpp
//

package com.sleepycat.dbxml;
import com.sleepycat.db.DatabaseException;

/**
 *  The XmlException class represents an error condition that has
 *  occurred within the Berkeley DB XML system. The system throws an
 *  XmlException if an API method call results in an error condition.
 *  The XmlException class exposes the following methods:</p> <hr
 *  size=1 noshade> <h3>Description: XmlException.toString</h3> <hr
 *  size=1 noshade> <h3>Description: XmlException.getErrorCode</h3>
 *  <hr size=1 noshade> <h3>Description: XmlException.getDbError</h3>
 */
public class XmlException extends com.sleepycat.db.DatabaseException {
    /**
     *
     *</ul>
     *
     */
    public final static int CONTAINER_CLOSED = 2;
    /**
     *
     *</ul>
     *
     */
    public final static int CONTAINER_EXISTS = 12;
    /**
     *
     *</ul>
     *
     */
    public final static int CONTAINER_OPEN = 1;
    /**
     *
     *</ul>
     *
     */
    public final static int DATABASE_ERROR = 5;
    /**
     *
     *</ul>
     *
     */
    public final static int DOCUMENT_NOT_FOUND = 11;
    /**
     *
     *</ul>
     *
     */
    public final static int DOM_PARSER_ERROR = 7;
    /**
     *
     *</ul>
     *
     */
    public final static int INDEXER_PARSER_ERROR = 4;
    /**
     *
     *</ul>
     *
     */
    public final static int INTERNAL_ERROR = 0;
    /**
     *
     *</ul>
     *
     */
    public final static int INVALID_VALUE = 14;
    /**
     *
     *</ul>
     *
     */
    public final static int VERSION_MISMATCH = 15;
    /**
     *
     *</ul>
     *
     */
    public final static int LAZY_EVALUATION = 10;
    /**
     *
     *</ul>
     *
     */
    public final static int NO_VARIABLE_BINDING = 9;
    /**
     *
     *</ul>
     *
     */
    public final static int UNKNOWN_INDEX = 13;
    /**
     *
     *</ul>
     *
     */
    public final static int XPATH_EVALUATION_ERROR = 8;
    /**
     *
     *</ul>
     *
     */
    public final static int XPATH_PARSER_ERROR = 6;
    /**
     *
     *</ul>
     *
     */
    public final static int CONTAINER_NOT_FOUND = 17;
    /**
     *
     *</ul>
     *
     */
    public final static int TRANSACTION_ERROR = 18;
    /**
     *
     *</ul>
     *
     */
    public final static int UNIQUE_ERROR = 19;
    /**
     *
     *</ul>
     *
     */
    public final static int NO_MEMORY_ERROR = 20;

    final static String[] xml_strerror = {
            "INTERNAL_ERROR",
            "CONTAINER_OPEN",
            "CONTAINER_CLOSED",
            "NULL_POINTER",
            "INDEXER_PARSER_ERROR",
            "DATABASE_ERROR",
            "XPATH_PARSER_ERROR",
            "DOM_PARSER_ERROR",
            "XPATH_EVALUATION_ERROR",
            "NO_VARIABLE_BINDING",
            "LAZY_EVALUATION",
            "DOCUMENT_NOT_FOUND",
            "CONTAINER_EXISTS",
            "UNKNOWN_INDEX",
            "INVALID_VALUE",
            "VERSION_MISMATCH",
            "UNUSED_EXCEPTION_2",
            "CONTAINER_NOT_FOUND",
            "TRANSACTION_ERROR",
            "UNIQUE_ERROR",
            "NO_MEMORY_ERROR"
    };
    private DatabaseException dbexc;

    private int errcode;


    public XmlException(int errcode, String description) {
        super(null, 0);
        this.errcode = errcode;
		this.dbexc = dbexc;
    }

    public XmlException(int errcode, String description, 
			DatabaseException dbexc, int dberr) {
        super(description, dberr);
        this.errcode = errcode;
		this.dbexc = dbexc;
    }

    /**
     * @return    The getDatabaseException() method retrieves a Berkeley DB
     *  DatabaseException object associated with this exception if
     *  one is present.  If this is a pure BDB XML exception, it returns
     *  null.</p>
     */
    public DatabaseException getDatabaseException() {
        return dbexc;
    }

    /**
     * @return    The getErrorCode() method returns the exception
     *      code.</p>
     */
    public int getErrorCode() {
        return errcode;
    }


    public String toString() {
        return super.toString() + ", errcode = " + xml_strerror[errcode];
    }
 
   /**
     * @return    The getDbError() method is deprecated.
     *  It returns the Berkeley DB error number assoicated with the
     *  exception if the ExceptionCode is DATABASE_ERROR.  If
     *  returns 0 if the ExceptionCode is not DATABASE_ERROR.</p>
     */
    public int getDbError() {
	if (dbexc != null)
	    return dbexc.getErrno();
        return 0;
    }

}
