/*
  Copyright (C) 2004-2010 by Autodesk, Inc.
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

using System;
using System.Text;
using System.Collections;
using System.IO;

namespace OSGeo.MapGuide
{

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
        String fontSuffix = (os == 0 ? "Windows" : (os == 1 ? "Macintosh" : "Linux"));
        Hashtable sb = null;
        try
        {
            sb = GetStringBundle(locale);
        }
        catch (Exception)
        {
            return "";
        }
        int len = text.Length;

        for (int i = 0; i < len; )
        {
            int pos1 = text.IndexOf("__#", i);
            if (pos1 != -1)
            {
                int pos2 = text.IndexOf("#__", pos1 + 3);
                if (pos2 != -1)
                {
                    String id = text.Substring(pos1 + 3, pos2 - pos1 - 3);
                    String locStr;
                    locStr = (String)sb[id == "@font" || id == "@fontsize" ? id + fontSuffix : id];
                    if (locStr == null)
                        locStr = "";
                    int locLen = locStr.Length;

                    String begin, end;
                    if (pos1 > 0)
                        begin = text.Substring(0, pos1);
                    else
                        begin = "";
                    end = text.Substring(pos2 + 3);
                    text = begin + locStr + end;

                    len = len - 6 - id.Length + locLen;
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
        catch (Exception)
        {
            return "";
        }
        String s = (String)sb[id];
        if (s == null)
            return "";
        return s;
    }

    protected static Hashtable GetStringBundle(String locale)
    {
        if (locale.Equals(""))
            locale = english;
        else
            locale = locale.ToLower();

        if (!languages.ContainsKey(locale))
        {
            StreamReader sr = null;
            String fname = localizationPath + locale;
            try
            {
                if (!File.Exists(fname))
                {
                    // requested locale is not supported, default to English
                    if (languages.ContainsKey(english))
                        return (Hashtable)languages[english];
                    fname = localizationPath + english;
                }
                sr = new StreamReader(fname, Encoding.UTF8);
                string line;
                Hashtable sb = new Hashtable();
                while ((line = sr.ReadLine()) != null)
                {
                    line = line.Trim();
                    if (line.Equals("") || line[0] == '#')
                        continue;
                    int sep = line.IndexOf('=');
                    if (sep == -1)
                        continue;
                    String key = line.Substring(0, sep).Trim();
                    if (key.Equals(""))
                        continue;
                    sb[key] = line.Substring(sep + 1).Trim();
                }
                languages[locale] = sb;
            }
            catch (Exception)
            {
                return null;
            }
            finally
            {
                if (sr != null)
                {
                    try { sr.Close(); }
                    catch (Exception) { }
                }
            }
        }
        return (Hashtable)languages[locale];
    }

}

}
