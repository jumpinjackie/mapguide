package org.osgeo.mapguide.test.common;

import java.io.*;
import java.nio.file.*;

public class CommonUtility
{
    public static String GetTypeName(Object o) {
        Class cls = o.getClass();
        String name = cls.getName();
        if (name.lastIndexOf('.') > 0) {
            name = name.substring(name.lastIndexOf('.') + 1); // Map$Entry
            name = name.replace('$', '.');      // Map.Entry
        }
        return name;
    }

    public static void DeleteFile(String path) {
        try {
            Files.deleteIfExists(Paths.get(path));
        } catch (IOException ex) {

        }
    }

    public static byte[] ReadAllBytes(String path) {
        try {
            return Files.readAllBytes(Paths.get(path));
        } catch (IOException ex) {
            return null;
        }
    }

    public static String GetTempFileName() {
        try {
            Path p = Files.createTempFile(null, null);
            return p.toString();
        } catch (IOException ex) {
            return null;
        }
    }

    public static boolean IsNullOrEmpty(String str) {
        return str == null || str.length() == 0;
    }

    public static String ExceptionToString(Exception ex) {
        StringBuilder sb = new StringBuilder();
        sb.append(ex.toString() + "\n");
        sb.append(ex.getMessage() + "\n");
        StackTraceElement[] items = ex.getStackTrace();
        for (StackTraceElement item : items) {
            sb.append("\t" + item.toString() + "\n");
        }
        return sb.toString();
    }
}