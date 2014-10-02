package org.osgeo.mapguide.test.common;

import java.io.*;

public class TestLoggerFile implements ITestLogger
{
    private Writer sw;

    public TestLoggerFile(String file)
    {
        try {
            sw = new PrintWriter(file, "UTF-8");
        } catch (IOException ex) {

        }
    }

    public void Write(String format, Object ... args)
    {
        try {
            sw.write(String.format(format, args));
        } catch (IOException ex) {

        }
    }

    public void WriteLine(String format, Object ... args)
    {
        try {
            sw.write(String.format(format, args) + "\n");
        } catch (IOException ex) {

        }
    }

    public void close()
    {
        if (sw != null)
        {
            try {
                sw.close();
            } catch (IOException ex) {

            }
            sw = null;
        }
    }
}