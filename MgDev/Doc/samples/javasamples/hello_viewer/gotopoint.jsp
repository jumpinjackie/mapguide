<?php

//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
//

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
    <title>Viewer Sample Application- Goto Point</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
</head>
<%
  String x = request.getParameter("X");
  String y = request.getParameter("Y");
  String scale = request.getParameter("Scale");
%>
<!-- Emit this function and assocate it with the onLoad event for the -->
<!-- page so that it gets executed when this page loads in the        -->
<!-- browser. The function calls the ZoomToView method on the         -->
<!-- Viewer frame forcing the Viewer to navigate to the specified     -->
<!-- point and scale.                                                 -->

<script language="javascript">

function OnPageLoad()
{
    parent.ZoomToView(<%= x %>, <%= y %>, <%= scale %>, true);
}

</script>

<body onLoad="OnPageLoad()">

</body>

</html>
