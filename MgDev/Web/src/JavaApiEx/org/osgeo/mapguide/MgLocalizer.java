/*
  Copyright (C) 2004-2011 by Autodesk, Inc.

  This library is free software; you can redistribute it and/or
  modify it under the terms of version 2.1 of the GNU Lesser
  General Public License as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

package org.osgeo.mapguide;

import java.util.*;
import java.io.*;

public class MgLocalizer
{
    protected static String english = "en";
    protected static String localizationPath = "";
    protected static Hashtable languages = new Hashtable();

    public static void SetLocalizedFilesPath(String path)
    {
        localizationPath = path;
    }

    public static String Localize(String text, String locale, int os)
    {
        String fontSuffix = (os==0? "Windows": (os==1? "Macintosh": "Linux"));
        Hashtable sb = null;
        try
        {
            sb = GetStringBundle(locale);
        }
        catch (Exception e)
        {
            return "";
        }
        int len = text.length();

        for (int i = 0; i < len; )
        {
            int pos1 = text.indexOf("__#", i);
            if (pos1 != -1)
            {
                int pos2 = text.indexOf("#__", pos1 + 3);
                if (pos2 != -1)
                {
                    String id = text.substring(pos1 + 3, pos2);
                    String locStr;
                    locStr = (String)sb.get(id.equals("@font") || id.equals("@fontsize") ? id + fontSuffix : id);
                    if (locStr == null)
                        locStr = "";
                    int locLen = locStr.length();

                    String begin, end;
                    if (pos1 > 0)
                        begin = text.substring(0, pos1);
                    else
                        begin = "";
                    end = text.substring(pos2 + 3);
                    text = begin + locStr + end;

                    len = len - 6 - id.length() + locLen;
                    i = pos1 + locLen;
                }
                else
                    i = len;
            }
            else
                i = len;
        }
        return text;
    }

    public static String GetString(String id, String locale)
    {
        Hashtable sb = null;
        try
        {
            sb = GetStringBundle(locale);
        }
        catch (Exception e)
        {
            return "";
        }
        String s = (String)sb.get(id);
        if (s == null)
            return "";
        return s;
    }

    protected static Hashtable GetStringBundle(String locale)
    {
        if (locale.equals(""))
            locale = english;
        else
            locale = locale.toLowerCase();

        if (!languages.containsKey(locale))
        {
            BufferedReader in = null;
            try
            {
                File f = new File(localizationPath + locale);
                if (!f.exists())
                {
                    // requested locale is not supported, default to English
                    if (languages.containsKey(english))
                        return (Hashtable)languages.get(english);
                    f = new File(localizationPath + english);
                }
                in = new BufferedReader(new InputStreamReader(new FileInputStream(f), "UTF-8"));
                String line;
                Hashtable sb = new Hashtable();
                while ((line = in.readLine()) != null)
                {
                    line = line.trim();
                    if (line.equals("") || line.charAt(0) == '#')
                        continue;
                    int sep = line.indexOf('=');
                    if (sep == -1)
                        continue;
                    String key = line.substring(0, sep).trim();
                    if (key.equals(""))
                        continue;
                    sb.put(key, line.substring(sep + 1).trim());
                }
                languages.put(locale, sb);
            }
            catch (Exception e)
            {
                return null;
            }
            finally
            {
                if (in != null)
                {
                    try { in.close(); }
                    catch (Exception e) { }
                }
            }
        }
        return (Hashtable)languages.get(locale);
    }

}
