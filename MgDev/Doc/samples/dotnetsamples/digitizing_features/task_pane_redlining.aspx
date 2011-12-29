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
<%@ Page Language="c#" %>

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>

<script runat="server">
    String sessionId;
    String mapName;
</script>
<html>
  <head>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <title>Redlining</title>
    
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <meta http-equiv="content-script-type" content="text/javascript">
    <% 
    sessionId = Request.Params["SESSION"];
    mapName = Request.Params["MAPNAME"];
    %>
    <script language="javascript" type="text/javascript">       
      function DigitizeLine() {
        parent.parent.mapFrame.DigitizeLine(OnLineDigitized);
      }
      function OnLineDigitized(line) {          
        // Send the Javascript variables to 'draw_line.aspx', 
        // via the form frame
        var params = new Array("x0", line.Point(0).X,
        	"y0", line.Point(0).Y,
        	"x1", line.Point(1).X,
        	"y1", line.Point(1).Y,
        	"SESSION", "<%= sessionId %>",
        	"MAPNAME", "<%= mapName %>");
        parent.parent.formFrame.Submit("/mapguide/dotnetsamples/digitizing_features/draw_line.aspx", params, "scriptFrame");
      }
    </script>   
  </head>
  <body class="AppFrame">
    
    <h1 class="AppHeading">Redlining</h1>
    
    <!-- "Draw a Line" button --> 
    <p>Click the button, and then drag a line on the map.</p>
    <input type="button" value="Draw a Line" onclick="DigitizeLine();" /> 
    <p>To draw another line, click the button again and drag another line.</p>
    
    <!-- "Clear Lines" button -->
    <hr>
    <form target="scriptFrame" action="clear_lines.aspx" method="post">
      <input type="submit" value="Clear Lines" />
      <input type="hidden" name="SESSION" value="<%= sessionId %>" />
      <input type="hidden" name="MAPNAME" value="<%= mapName %>" />          
    </form>                     
          
  </body>
</html>
