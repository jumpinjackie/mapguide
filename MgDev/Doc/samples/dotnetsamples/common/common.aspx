<%--
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
--%>

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<script language="C#" runat="server">

public class Constants
{
    public const string RootDirectory = "C:\\Program Files\\OSGeo\\MapGuide\\";
    
    public static string MapGuideServerDirectory 
    {
        get { return Path.Combine(RootDirectory, "Server"); }
    }
    
    public static string SchemaDirectory 
    {
        get { return Path.Combine(MapGuideServerDirectory, "Schema"); }
    }
    
    public static string WebExtensionsDirectory
    {
        get { return Path.Combine(RootDirectory, "Web"); }
    }
    
    public static string ViewerFilesDirectory
    {
        get { return Path.Combine(WebExtensionsDirectory, "www\\viewerfiles"); } 
    }
    
    public static string WebConfigPath
    {
        get { return Path.Combine(WebExtensionsDirectory, "www\\webconfig.ini"); }
    }
}

public class TemplateUtil
{
    public static String Substitute(String templ, params String[] vals)
    {
        StringBuilder res = new StringBuilder();
        int index = 0, val = 0;
        bool found;
        do
        {
            found = false;
            int i = templ.IndexOf('%', index);
            if(i != -1)
            {
                found = true;
                res.Append(templ.Substring(index, i - index));
                if(i < templ.Length - 1)
                {
                    if(templ[i+1] == '%')
                        res.Append('%');
                    else if(templ[i+1] == 's')
                        res.Append(vals[val ++]);
                    else
                        res.Append('@');    //add a character illegal in jscript so we know the template was incorrect
                    index = i + 2;
                }
            }
        } while(found);
        res.Append(templ.Substring(index));
        return res.ToString();
    }
}

public class Utility
{
    public static void OutputReaderContent(MgByteReader byteReader, HttpResponse response)
    {
        MemoryStream memBuf = new MemoryStream();
        byte[] byteBuffer = new byte[1024];
        int numBytes = byteReader.Read(byteBuffer, 1024);
        while (numBytes > 0)
        {
            memBuf.Write(byteBuffer, 0, numBytes);
            numBytes = byteReader.Read(byteBuffer, 1024);
        }
        response.ContentType = byteReader.GetMimeType();
        byte[] content = memBuf.ToArray();
        response.OutputStream.Write(content, 0, content.Length);
    }
}

</script>
