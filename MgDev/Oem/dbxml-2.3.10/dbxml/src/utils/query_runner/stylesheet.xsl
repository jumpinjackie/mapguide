<!--
  See the file LICENSE for redistribution information.

  Copyright (c) 2002,2006 Oracle.  All rights reserved.
 
  $Id: stylesheet.xsl,v 1.6 2006/10/30 17:46:06 bostic Exp $
-->

<xsl:stylesheet version='1.0'
  xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
<xsl:output omit-xml-declaration='yes' output-type='text' />

<xsl:template match="/">

<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
    <title>Query Runner Results</title>
  </head>
  <body text="#000000" bgcolor="#ffffff">
    <h1>Summary</h1>
<xsl:apply-templates select="//error"  />
    <table border="1" width="75%">
      <tr>
        <th colspan="2">Group</th>
        <th colspan="1">Containers</th>
      </tr>
      <tr>
        <th>Name</th>
        <th>Time</th>
        <th>
          <table width="100%">
            <tr>
              <th width="50%">Type</th>
              <th width="25%">Queries</th>
              <th width="25%">Time</th>
            </tr>
          </table>
        </th>
      </tr>
<xsl:for-each select="/xqueryrunner/group">
      <tr valign="top">
        <td><a href="#{@name}"><xsl:value-of select="@name" /></a></td>
        <td><xsl:value-of select="@time" /></td>
        <td>
          <table width="100%">
<xsl:for-each select="./container">
            <tr>
              <td width="50%">
                <a href="#{@name}">
                  <xsl:value-of select="@size"/>
                  <xsl:text> / </xsl:text>
                  <xsl:value-of select="@type"/>
                </a>
              </td>
<!-- no. of queries executed / total no. of queries -->
              <td width="25%" align="centre">
                <xsl:value-of select="count(result)"/>
                <xsl:text> out of </xsl:text>
                <xsl:choose>
                  <xsl:when test="starts-with(@name, 'DC-MD')">
                    <xsl:text>15</xsl:text>
                  </xsl:when>
                  <xsl:when test="starts-with(@name, 'DC-SD')">
                    <xsl:text>16</xsl:text>
                  </xsl:when>
                  <xsl:when test="starts-with(@name, 'TC-MD')">
                    <xsl:text>19</xsl:text>
                  </xsl:when>
                  <xsl:when test="starts-with(@name, 'TC-SD')">
                    <xsl:text>17</xsl:text>
                  </xsl:when>
                </xsl:choose>
              </td>
              <td width="25%" align="right">
                <xsl:value-of select="@time"/>
              </td>
            </tr>
</xsl:for-each>
          </table>
        </td>
      </tr>
</xsl:for-each>
    </table>

    <p>
      <font size="+1">
        <ul>
          <xsl:text>Total time for workload: </xsl:text>
          <xsl:value-of select="/xqueryrunner/@time" />
          <xsl:text> seconds</xsl:text>
        </ul>
      </font>
    </p>
    <p>
      <b>NB</b> Tiny ~1MB, Small ~10MB, Medium ~50MB, Normal ~100MB, Large ~1GB, Huge ~10GB 
    </p>

    <hr/>

<xsl:apply-templates select="/xqueryrunner/group"  />

  </body>
</html>

</xsl:template>

<!-- 2004-10-12 ARW need to indicate source of error -->
<xsl:template match="error" > 
  <p>
    <font size="+2" color="red">
      <u><b><i>Errors in results!!!</i></b></u>
      (TODO extend stylesheet to deal with errors)
    </font>
  </p>
</xsl:template>

<xsl:template match="group" > 

<xsl:element name="a">
  <xsl:attribute name="id"><xsl:value-of select="@name"/></xsl:attribute>
</xsl:element>
    <h1><xsl:value-of select="@name" /></h1>
<xsl:apply-templates select="container" />
</xsl:template>


<xsl:template match="container" >
<xsl:element name="a">
  <xsl:attribute name="id"><xsl:value-of select="@name"/></xsl:attribute>
</xsl:element>
      <h2><xsl:value-of select="@name" /></h2>
        <p>Time for container: <xsl:value-of select="@time" /></p>
        <p>No. of queries executed: <xsl:value-of select="count(result)"/></p>
        <table>
          <tr>
            <th width="70%">Description</th>
            <th width="15%">Time (s)</th>
            <th width="15%">No. of Hits</th>
          </tr>
<xsl:apply-templates select="result"  />
        </table>
</xsl:template>

<xsl:template match="result" >
      <tr>
        <td><xsl:value-of select="description" /></td>
        <td align="right"><xsl:value-of select="@time" /></td>
        <td align="right"><xsl:value-of select="@count" /></td>
      </tr>
</xsl:template>

</xsl:stylesheet>
