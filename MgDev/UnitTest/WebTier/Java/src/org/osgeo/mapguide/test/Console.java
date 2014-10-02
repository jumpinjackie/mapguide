package org.osgeo.mapguide.test;

// .net console shim
public class Console
{
    public static void Write(String message)
    {
        System.out.print(message);
    }

    public static void Write(String format, Object ... args)
    {
        Write(String.format(format, args));
    }

    public static void WriteLine(String message)
    {
        System.out.println(message);
    }

    public static void WriteLine(String format, Object ... args)
    {
        WriteLine(String.format(format, args));
    }
}