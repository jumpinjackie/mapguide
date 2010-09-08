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

public class XmlResults {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected XmlResults(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(XmlResults obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public void delete() /* no exception */ {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      dbxml_javaJNI.delete_XmlResults(swigCPtr);
    }
    swigCPtr = 0;
  }

  public XmlResults(XmlResults results) throws XmlException {
    this(dbxml_javaJNI.new_XmlResults(XmlResults.getCPtr(results)), true);
  }

  public boolean hasNext() throws XmlException {
    return dbxml_javaJNI.XmlResults_hasNext(swigCPtr);
  }

  public boolean hasPrevious() throws XmlException {
    return dbxml_javaJNI.XmlResults_hasPrevious(swigCPtr);
  }

  public XmlValue next() throws XmlException {
    long cPtr = dbxml_javaJNI.XmlResults_next__SWIG_0(swigCPtr);
    return (cPtr == 0) ? null : new XmlValue(cPtr, true);
  }

  public XmlValue previous() throws XmlException {
    long cPtr = dbxml_javaJNI.XmlResults_previous__SWIG_0(swigCPtr);
    return (cPtr == 0) ? null : new XmlValue(cPtr, true);
  }

  public XmlValue peek() throws XmlException {
    long cPtr = dbxml_javaJNI.XmlResults_peek__SWIG_0(swigCPtr);
    return (cPtr == 0) ? null : new XmlValue(cPtr, true);
  }

  public int getEvaluationType() throws XmlException {
    return dbxml_javaJNI.XmlResults_getEvaluationType(swigCPtr);
  }

  public boolean next(XmlDocument document) throws XmlException {
    return dbxml_javaJNI.XmlResults_next__SWIG_1(swigCPtr, XmlDocument.getCPtr(document));
  }

  public boolean previous(XmlDocument document) throws XmlException {
    return dbxml_javaJNI.XmlResults_previous__SWIG_1(swigCPtr, XmlDocument.getCPtr(document));
  }

  public boolean peek(XmlDocument document) throws XmlException {
    return dbxml_javaJNI.XmlResults_peek__SWIG_1(swigCPtr, XmlDocument.getCPtr(document));
  }

  public void reset() throws XmlException {
    dbxml_javaJNI.XmlResults_reset(swigCPtr);
  }

  public int size() throws XmlException { return dbxml_javaJNI.XmlResults_size(swigCPtr); }

  public void add(XmlValue value) throws XmlException {
    dbxml_javaJNI.XmlResults_add(swigCPtr, XmlValue.getCPtr(value));
  }

}