package org.osgeo.mapguide.test.common;

public class MutableInteger
{
    private int _value;

    public MutableInteger(int value) {
        _value = value;
    }

    public int increment() {
        _value++;
        return _value;
    }

    public int decrement() {
        _value--;
        return _value;
    }

    public int getValue() { return _value; }
}