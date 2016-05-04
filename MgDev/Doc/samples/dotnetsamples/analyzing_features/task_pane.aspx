<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
-->
<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<script runat="server">
String sessionId;
String mapName;
String fullPath;
</script>
<html>
  <head>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <script language=javascript>
      function submitBufferRequest(pageUrl)
      {
        xmlSel = parent.parent.mapFrame.GetSelectionXML();
        if (xmlSel.length > 0)
        {
          params = new Array("SESSION", parent.parent.mapFrame.GetSessionId(),
          "MAPNAME", parent.parent.mapFrame.GetMapName(),
          "SELECTION", encodeURIComponent(xmlSel));
          parent.parent.formFrame.Submit(pageUrl, params, "taskPaneFrame");
        }
        else {
          alert ("Please select a parcel");
        }
      }
      
     function OnPageLoad()
     {
         parent.parent.mapFrame.Refresh();
         parent.parent.mapFrame.ZoomToScale(9999);
     }
    </script>
  </head>
  <body onLoad="OnPageLoad()" class="AppFrame">
    <h1 class="AppHeading">Analyzing Features</h1>
    <%
        sessionId = Request.Params["SESSION"];
        mapName = Request.Params["MAPNAME"];
    %>
    <ul>
      <li>
        <a href="#" onClick="submitBufferRequest('../dotnetsamples/analyzing_features/createbuffer.aspx'); return false;">
        Create buffer</a>
        <br/>Create a buffer around a selected parcel.
        <br/>
        <%
            fullPath = Server.MapPath("createbuffer.aspx");
        %>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
      </li>

      <li>
        <a href="#" onClick="submitBufferRequest('../dotnetsamples/analyzing_features/selectfeaturesinbuffer.aspx'); return false;">
        Find features in buffer</a>
        <br/>Create a buffer around a selected parcel, then mark parcels inside the buffer that are
        of type "MFG".
        <br/>
        <%
            fullPath = Server.MapPath("selectfeaturesinbuffer.aspx");
        %>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
      </li>

    </ul>

    <p>View source for support files:</p>
    <% 
        fullPath = Server.MapPath("task_pane.aspx");
        String fullPath2 = Server.MapPath("bufferfunctions.aspx");
        String fullPath3 = Server.MapPath("bufferlayerdefinition.xml");
        String fullPath4 = Server.MapPath("parcelmarker.xml");
    %>
    <ul>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>">task_pane.aspx</a></li>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath2 %>">bufferfunctions.aspx</a></li>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath3 %>">bufferlayerdefinition.xml</a></li>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath4 %>">parcelmarker.xml</a></li>
    </ul>
  </body>
</html>
