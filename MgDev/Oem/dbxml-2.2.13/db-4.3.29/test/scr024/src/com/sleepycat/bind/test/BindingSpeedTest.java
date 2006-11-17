/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002-2004
 *	Sleepycat Software.  All rights reserved.
 *
 * $Id: BindingSpeedTest.java,v 1.4 2004/08/02 18:53:08 mjc Exp $
 */

package com.sleepycat.bind.test;

import java.io.Externalizable;
import java.io.IOException;
import java.io.ObjectInput;
import java.io.ObjectInputStream;
import java.io.ObjectOutput;
import java.io.ObjectOutputStream;
import java.io.OutputStreamWriter;
import java.io.Serializable;
import java.io.Writer;

import javax.xml.parsers.SAXParserFactory;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;

import com.sleepycat.bind.serial.SerialInput;
import com.sleepycat.bind.serial.SerialOutput;
import com.sleepycat.bind.serial.test.TestClassCatalog;
import com.sleepycat.bind.tuple.TupleInput;
import com.sleepycat.bind.tuple.TupleOutput;
import com.sleepycat.collections.test.DbTestUtil;
import com.sleepycat.util.FastInputStream;
import com.sleepycat.util.FastOutputStream;

/**
 * @author Mark Hayes
 */
public class BindingSpeedTest extends TestCase {

    static final String JAVA_UNSHARED = "java-unshared".intern();
    static final String JAVA_SHARED = "java-shared".intern();
    static final String JAVA_EXTERNALIZABLE = "java-externalizable".intern();
    static final String XML_SAX = "xml-sax".intern();
    static final String TUPLE = "tuple".intern();

    static final int RUN_COUNT = 1000;
    static final boolean VERBOSE = false;

    public static void main(String[] args)
        throws Exception {

        junit.framework.TestResult tr =
            junit.textui.TestRunner.run(suite());
        if (tr.errorCount() > 0 ||
            tr.failureCount() > 0) {
            System.exit(1);
        } else {
            System.exit(0);
        }
    }

    public static Test suite() {

        TestSuite suite = new TestSuite();
        suite.addTest(new BindingSpeedTest(JAVA_UNSHARED));
        suite.addTest(new BindingSpeedTest(JAVA_SHARED));
        suite.addTest(new BindingSpeedTest(JAVA_EXTERNALIZABLE));
        suite.addTest(new BindingSpeedTest(XML_SAX));
        suite.addTest(new BindingSpeedTest(TUPLE));
        return suite;
    }

    private String command;
    private FastOutputStream fo;
    private TupleOutput to;
    private TestClassCatalog jtc;
    private byte[] buf;
    private XMLReader parser;

    public BindingSpeedTest(String name) {

        super("BindingSpeedTest." + name);
        command = name;
    }

    public void runTest()
        throws Exception {

        DbTestUtil.printTestName(getName());

        boolean isTuple = false;
        boolean isXmlSax = false;
        boolean isSerial = false;
        boolean isShared = false;
        boolean isExternalizable = false;

        if (command == TUPLE) {
            isTuple = true;
        } else if (command == XML_SAX) {
            isXmlSax = true;
        } else if (command == JAVA_UNSHARED) {
            isSerial = true;
        } else if (command == JAVA_SHARED) {
            isSerial = true;
            isShared = true;
        } else if (command == JAVA_EXTERNALIZABLE) {
            isSerial = true;
            isShared = true;
            isExternalizable = true;
        } else {
            throw new Exception("invalid command: " + command);
        }

        // Do initialization

        if (isTuple) {
            initTuple();
        } else if (isXmlSax) {
            initXmlSax();
        } else if (isSerial) {
            if (isShared) {
                initSerialShared();
            } else {
                initSerialUnshared();
            }
        }

        // Prime the Java compiler

        int size = 0;
        for (int i = 0; i < RUN_COUNT; i += 1) {

            if (isTuple) {
                size = runTuple();
            } else if (isXmlSax) {
                size = runXmlSax();
            } else if (isSerial) {
                if (isShared) {
                    if (isExternalizable) {
                        size = runSerialExternalizable();
                    } else {
                        size = runSerialShared();
                    }
                } else {
                    size = runSerialUnshared();
                }
            }
        }

        // Then run the timing tests

        long startTime = System.currentTimeMillis();

        for (int i = 0; i < RUN_COUNT; i += 1) {
            if (isTuple) {
                size = runTuple();
            } else if (isXmlSax) {
                size = runXmlSax();
            } else if (isSerial) {
                if (isShared) {
                    if (isExternalizable) {
                        size = runSerialExternalizable();
                    } else {
                        size = runSerialShared();
                    }
                } else {
                    size = runSerialUnshared();
                }
            }
        }

        long stopTime = System.currentTimeMillis();

	assertTrue("data size too big", size < 250);

        if (VERBOSE) {
            System.out.println(command);
            System.out.println("data size: " + size);
            System.out.println("run time:  " +
                ((stopTime - startTime) / (double) RUN_COUNT));
        }
    }

    public void tearDown() {

        /* Ensure that GC can cleanup. */
        command = null;
        fo = null;
        to = null;
        jtc = null;
        buf = null;
        parser = null;
    }

    void initSerialUnshared()
        throws Exception {

        fo = new FastOutputStream();
    }

    int runSerialUnshared()
        throws Exception {

        fo.reset();
        ObjectOutputStream oos = new ObjectOutputStream(fo);
        oos.writeObject(new Data());
        byte[] bytes = fo.toByteArray();
        FastInputStream fi = new FastInputStream(bytes);
        ObjectInputStream ois = new ObjectInputStream(fi);
        ois.readObject();
        return bytes.length;
    }

    void initSerialShared()
        throws Exception {

        jtc = new TestClassCatalog();
        fo = new FastOutputStream();
    }

    int runSerialShared()
        throws Exception {

        fo.reset();
        SerialOutput oos = new SerialOutput(fo, jtc);
        oos.writeObject(new Data());
        byte[] bytes = fo.toByteArray();
        FastInputStream fi = new FastInputStream(bytes);
        SerialInput ois = new SerialInput(fi, jtc);
        ois.readObject();
        return (bytes.length - SerialOutput.getStreamHeader().length);
    }

    int runSerialExternalizable()
        throws Exception {

        fo.reset();
        SerialOutput oos = new SerialOutput(fo, jtc);
        oos.writeObject(new Data2());
        byte[] bytes = fo.toByteArray();
        FastInputStream fi = new FastInputStream(bytes);
        SerialInput ois = new SerialInput(fi, jtc);
        ois.readObject();
        return (bytes.length - SerialOutput.getStreamHeader().length);
    }

    void initTuple()
        throws Exception {

        buf = new byte[500];
        to = new TupleOutput(buf);
    }

    int runTuple()
        throws Exception {

        to.reset();
        new Data().writeTuple(to);

        TupleInput ti = new TupleInput(
                          to.getBufferBytes(), to.getBufferOffset(),
                          to.getBufferLength());
        new Data().readTuple(ti);

        return to.getBufferLength();
    }

    void initXmlSax()
        throws Exception {

        buf = new byte[500];
        fo = new FastOutputStream();
        SAXParserFactory saxFactory = SAXParserFactory.newInstance();
        saxFactory.setNamespaceAware(true);
        parser = saxFactory.newSAXParser().getXMLReader();
    }

    int runXmlSax()
        throws Exception {

        fo.reset();
        OutputStreamWriter writer = new OutputStreamWriter(fo);
        new Data().writeXmlText(writer);

        byte[] bytes = fo.toByteArray();
        FastInputStream fi = new FastInputStream(bytes);
        InputSource input = new InputSource(fi);
        parser.parse(input);

        //InputStreamReader reader = new InputStreamReader(fi);
        //new Data().readXmlText(??);

        return bytes.length;
    }

    static class Data2 extends Data implements Externalizable {

        public Data2() {}

        public void readExternal(ObjectInput in)
            throws IOException, ClassNotFoundException {

            field1 = in.readUTF();
            field2 = in.readUTF();
            field3 = in.readInt();
            field4 = in.readInt();
            field5 = in.readUTF();
        }

        public void writeExternal(ObjectOutput out)
            throws IOException {

            out.writeUTF(field1);
            out.writeUTF(field2);
            out.writeInt(field3);
            out.writeInt(field4);
            out.writeUTF(field5);
        }
    }

    static class Data implements Serializable {

        String field1 = "field1";
        String field2 = "field2";
        int field3 = 333;
        int field4 = 444;
        String field5 = "field5";

        void readTuple(TupleInput _input) {

            field1 = _input.readString();
            field2 = _input.readString();
            field3 = _input.readInt();
            field4 = _input.readInt();
            field5 = _input.readString();
        }

        void writeTuple(TupleOutput _output) {

            _output.writeString(field1);
            _output.writeString(field2);
            _output.writeInt(field3);
            _output.writeInt(field4);
            _output.writeString(field5);
        }

        void writeXmlText(Writer writer) throws IOException {

            writer.write("<Data><Field1>");
            writer.write(field1);
            writer.write("</Field1><Field2>");
            writer.write(field2);
            writer.write("</Field2><Field3>");
            writer.write(String.valueOf(field3));
            writer.write("</Field3><Field4>");
            writer.write(String.valueOf(field4));
            writer.write("</Field4><Field5>");
            writer.write(field5);
            writer.write("</Field5></Data>");
            writer.flush();
        }
    }
}
