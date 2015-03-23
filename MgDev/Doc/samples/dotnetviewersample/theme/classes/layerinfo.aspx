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

<script language="c#" runat="server">
    public class LayerInfo : JsonObject
    {
        public ArrayList properties = null;
        public ArrayList scaleRanges = null;
        
        public LayerInfo(ArrayList properties, ArrayList scaleRanges)
    	{
    	    this.properties = new ArrayList(properties);
    	    this.scaleRanges = new ArrayList(scaleRanges);
    	    
    	    //JSON Settings
    	    Put("properties", this.properties);
    	    Put("scaleRanges", this.scaleRanges);
    	}
    }
</script>
