/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.29
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.sleepycat.dbxml;

import com.sleepycat.db.*;
import com.sleepycat.db.internal.DbEnv;
import com.sleepycat.db.internal.DbConstants;

public class XmlUpdateContext {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected XmlUpdateContext(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(XmlUpdateContext obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public void delete() /* no exception */ {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      dbxml_javaJNI.delete_XmlUpdateContext(swigCPtr);
    }
    swigCPtr = 0;
  }

  public XmlUpdateContext(XmlUpdateContext o) throws XmlException {
    this(dbxml_javaJNI.new_XmlUpdateContext(XmlUpdateContext.getCPtr(o)), true);
  }

  public void setApplyChangesToContainers(boolean applyChanges) throws XmlException {
    dbxml_javaJNI.XmlUpdateContext_setApplyChangesToContainers(swigCPtr, applyChanges);
  }

  public boolean getApplyChangesToContainers() throws XmlException {
    return dbxml_javaJNI.XmlUpdateContext_getApplyChangesToContainers(swigCPtr);
  }

}