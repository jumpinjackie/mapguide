package org.osgeo.mapguide.test.common;

public class Assert
{
    public static void AreBytesEqual(byte[] expected, byte[] actual)
    {
        byte[] a = expected;
        byte[] b = actual;
        if (a.length != b.length)
            throw new AssertException("Expected byte array size of " + a.length + ", got size of: " + b.length);

        for (int i = 0; i < a.length; i++)
        {
            if (a[i] != b[i])
                throw new AssertException("Byte number " + (i + 1) + " in expected byte array does not match byte in actual byte array");
        }
    }

    public static void AreEqual(long expected, long actual) {
        if (expected != actual)
        {
            throw new AssertException("Expected: " + expected + ", got: " + actual);
        }
    }
    
    public static void AreEqual(float expected, float actual) {
        if (expected != actual)
        {
            throw new AssertException("Expected: " + expected + ", got: " + actual);
        }
    }

    public static void AreEqual(double expected, double actual) {
        if (expected != actual)
        {
            throw new AssertException("Expected: " + expected + ", got: " + actual);
        }
    }

    public static void AreEqual(Object expected, Object actual)
    {
        if (expected instanceof byte[] && actual instanceof byte[])
        {
            AreBytesEqual((byte[])expected, (byte[])actual);
        }
        else if (!expected.equals(actual))
        {
            throw new AssertException("Expected: " + expected + ", got: " + actual);
        }
    }

    public static void Fail(String message) {
        throw new AssertException(message);
    }

    public static void Greater(Comparable value, Comparable against)
    {
        if (value.compareTo(against) <= 0)
            throw new AssertException(value + " is not greater than " + against);
    }

    public static void IsNull(Object obj)
    {
        if (obj != null)
            throw new AssertException("Reference is not null");
    }

    public static void IsTrue(boolean condition)
    {
        if (!condition)
            throw new AssertException("Condition evaluated to false. Expected: true");
    }

    public static void IsTrue(boolean condition, String message)
    {
        if (!condition)
            throw new AssertException(message);
    }

    public static void IsFalse(boolean condition)
    {
        if (condition)
            throw new AssertException("Condition evaluated to true. Expected: false");
    }

    public static void IsNaN(double value)
    {
        if (!Double.isNaN(value))
            throw new AssertException("Double is a number. Expected: NaN");
    }
}