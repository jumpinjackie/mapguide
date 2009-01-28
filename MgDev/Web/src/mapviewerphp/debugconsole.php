<head>

<!--
Copyright (C) 2004-2009 by Autodesk, Inc.

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
-->

<style type="text/css">
body,p
{
  font-family: Courier;
  font-size: 8pt;
}
</style>
<script language=javascript>
function Trace(msg)
{
    var msgs = msg.split("<p>");
    var bodyRef = document.getElementsByTagName("body").item(0);
    for(var i = 0; i < msgs.length; i++) {
        var newPara = document.createElement("p");
        var newText = document.createTextNode(msgs[i]);
        newPara.appendChild(newText);
        bodyRef.appendChild(newPara);
    }
    var scrollWidth = 0;
    var scrollHeight = 0;
    if(document.body.scrollHeight )
    {
        scrollWidth = document.body.scrollWidth;
        scrollHeight = document.body.scrollHeight;
    }

    window.self.scrollBy( scrollWidth, scrollHeight );
}
</script>
</head>
<body>
<h3>Debug traces</h3>
</body>
