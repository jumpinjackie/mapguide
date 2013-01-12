<%--
  -Copyright (C) 2004-2006  Autodesk, Inc.
  -This library is free software; you can redistribute it and/or
  -modify it under the terms of version 2.1 of the GNU Lesser
  -General Public License as published by the Free Software Foundation.

  -This library is distributed in the hope that it will be useful,
  -but WITHOUT ANY WARRANTY; without even the implied warranty of
  -MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  -Lesser General Public License for more details.

  -You should have received a copy of the GNU Lesser General Public
  -License along with this library; if not, write to the Free Software
  -Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>

<%@ page import="org.osgeo.mapguide.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>

<%!
    public class Feature
    {
        public String displayValue = null;
        public double centerX = 0.0;
        public double centerY = 0.0;
        public Map idList = null;
        
        public Feature(String displayValue, MgPoint centerPoint, Map idList) throws MgException
        {
            this.displayValue = displayValue;
            if (centerPoint != null)
            {
                this.centerX = centerPoint.GetCoordinate().GetX();
                this.centerY = centerPoint.GetCoordinate().GetY();
            }
            this.idList = new HashMap(idList);
        }
    }
%>
