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
    public class Feature : JsonObject
    {
        public String displayValue = null;
        public double centerX = 0.0;
        public double centerY = 0.0;
        public Hashtable idList = null;
        
        public Feature(String displayValue, MgPoint centerPoint, Hashtable idList)
    	{
    	    this.displayValue = displayValue;
    	    this.centerX = centerPoint.GetCoordinate().GetX();
    	    this.centerY = centerPoint.GetCoordinate().GetY();
    	    this.idList = new Hashtable(idList);
    	    
    	    //JSON Settings
    	    Put("displayValue", this.displayValue);
    	    Put("centerX", this.centerX);
    	    Put("centerY", this.centerY);
    	    Put("idList", this.idList);    	    
    	}
    }
</script>
