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

</script>
