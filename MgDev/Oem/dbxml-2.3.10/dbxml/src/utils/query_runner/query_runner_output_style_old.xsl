<!--
  See the file LICENSE for redistribution information.

  Copyright (c) 2002,2006 Oracle.  All rights reserved.
 
  $Id: query_runner_output_style_old.xsl,v 1.4 2006/10/30 17:46:06 bostic Exp $
-->

<xsl:stylesheet version='1.0'
  xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
<xsl:output omit-xml-declaration='yes' output-type='text' />


<xsl:template match="/">

<html>
  <head>
    <title>Query Runner Results</title>
  </head>
  <body text="#000000" bgcolor="#ffffff">
    <html>

<table>
<tr><td>Total time:</td><td> <xsl:value-of select="/xqueryrunner/@time" /></td></tr>

<xsl:for-each select="/xqueryrunner/group">
<tr><td><a href="#{@name}">Group <xsl:value-of select="@name" /></a></td><td><xsl:value-of select="@time" /></td></tr>
</xsl:for-each>

<br/>

<xsl:apply-templates select="/xqueryrunner/group"  />


</table>


    </html>
  </body>
</html>

</xsl:template>


<xsl:template match="group" >
<br/>
<h2>
<tr><td>  <a name="{@name}" /> Group <xsl:value-of select="@name" /> results - </td>
<td>Total time <xsl:value-of select="@time" /></td>
</tr>
</h2>
<br/>

<xsl:apply-templates select="container"  />
</xsl:template>


<xsl:template match="container" >
<br/>
<h3>
<tr><td>  Container <xsl:value-of select="@name" /> results - </td>
<td>Total time <xsl:value-of select="@time" /></td>
</tr>
</h3>
<br/>

<xsl:apply-templates select="result"  />
</xsl:template>

<xsl:template match="result" >
<br/>
<tr><td>  Query </td>
<td>Time <xsl:value-of select="@time" /></td>
</tr>
<tr><td> Description: <xsl:value-of select="description" /> </td></tr>
<tr><td> Action: <xsl:value-of select="action" /> </td></tr>




</xsl:template>

</xsl:stylesheet>
