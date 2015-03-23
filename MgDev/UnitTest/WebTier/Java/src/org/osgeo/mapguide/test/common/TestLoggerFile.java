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
            if (args.length > 0)
                sw.write(String.format(format, args));
            else
                sw.write(format);
        } catch (IOException ex) {

        }
    }

    public void WriteLine(String format, Object ... args)
    {
        try {
            if (args.length > 0)
                sw.write(String.format(format, args) + "\n");
            else
                sw.write(format + "\n");
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