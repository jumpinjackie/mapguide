<%--
Copyright (C) 2004-2007  Autodesk, Inc.
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

<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<%@ Import Namespace="Jayrock.Json" %>

<script language="c#" runat="server">
    public class Property : JsonObject
    {
        public String name = null;
        public bool isString = false;
        
        public Property(String name, bool isString)
    	{
    	    this.name = name;
    	    this.isString = isString;
    	    
    	    //JSON Settings
    	    Put("name", this.name);
    	    Put("isString", this.isString);
    	}
    }
</script>
