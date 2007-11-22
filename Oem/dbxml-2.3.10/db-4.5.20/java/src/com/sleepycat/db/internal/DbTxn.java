/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.29
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.sleepycat.db.internal;

import com.sleepycat.db.*;
import java.util.Comparator;

public class DbTxn {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected DbTxn(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(DbTxn obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  /* package */ void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      throw new UnsupportedOperationException("C++ destructor does not have public access");
    }
    swigCPtr = 0;
  }

	public void abort() throws DatabaseException {
		try {
			abort0();
		} finally {
			swigCPtr = 0;
		}
	}

	public void commit(int flags) throws DatabaseException {
		try {
			commit0(flags);
		} finally {
			swigCPtr = 0;
		}
	}

	public void discard(int flags) throws DatabaseException {
		try {
			discard0(flags);
		} finally {
			swigCPtr = 0;
		}
	}

	/*
	 * We override Object.equals because it is possible for the Java API to
	 * create multiple DbTxns that reference the same underlying object.
	 * This can happen for example during DbEnv.txn_recover().
	 */
	public boolean equals(Object obj)
	{
		if (this == obj)
			return true;

		if (obj != null && (obj instanceof DbTxn)) {
			DbTxn that = (DbTxn)obj;
			return (this.swigCPtr == that.swigCPtr);
		}
		return false;
	}

	/*
	 * We must override Object.hashCode whenever we override
	 * Object.equals() to enforce the maxim that equal objects have the
	 * same hashcode.
	 */
	public int hashCode()
	{
		return ((int)swigCPtr ^ (int)(swigCPtr >> 32));
	}

  /* package */ void abort0() { db_javaJNI.DbTxn_abort0(swigCPtr); }

  /* package */ void commit0(int flags) { db_javaJNI.DbTxn_commit0(swigCPtr, flags); }

  /* package */ void discard0(int flags) { db_javaJNI.DbTxn_discard0(swigCPtr, flags); }

  public String get_name() throws com.sleepycat.db.DatabaseException {
    return db_javaJNI.DbTxn_get_name(swigCPtr);
  }

  public int id() throws com.sleepycat.db.DatabaseException { return db_javaJNI.DbTxn_id(swigCPtr); }

  public void prepare(byte[] gid) throws com.sleepycat.db.DatabaseException { db_javaJNI.DbTxn_prepare(swigCPtr, gid); }

  public void set_timeout(long timeout, int flags) throws com.sleepycat.db.DatabaseException { db_javaJNI.DbTxn_set_timeout(swigCPtr, timeout, flags); }

  public void set_name(String name) throws com.sleepycat.db.DatabaseException { db_javaJNI.DbTxn_set_name(swigCPtr, name); }

}
