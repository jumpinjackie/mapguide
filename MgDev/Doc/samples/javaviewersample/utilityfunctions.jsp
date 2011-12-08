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

<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ page import="org.osgeo.mapguide.*" %>
<%!
void InitializeWebTier() throws MgException
{
    String realpath = getServletContext().getRealPath("/");
    // Initialize web tier with the site configuration file.  The config
    // file should be in the same folder as this script.
    String configPath =  realpath + "webconfig.ini";
    MapGuideJavaApi.MgInitializeWebTier( configPath );
}

String GetQueryXmlTemplatePath()
{
    String realpath = getServletContext().getRealPath("/");
    // Initialize web tier with the site configuration file.  The config
    // file should be in the same folder as this script.
    String xmlTemplatePath = realpath + "javaviewersample\\query\\templates\\filterlayerdefinition.xml";
    return xmlTemplatePath;
}

String GetThemeXmlTemplatePath()
{
    String realpath = getServletContext().getRealPath("/");
    // Initialize web tier with the site configuration file.  The config
    // file should be in the same folder as this script.
    String xmlTemplatePath = realpath + "javaviewersample\\theme\\templates\\arearuletemplate.xml";
    return xmlTemplatePath;
}

// Searches the map for a layer with the specified name.

MgLayer GetLayerByName(MgMap map, String layerName) throws MgException
{
    MgLayer layer = null;
    for (int i = 0; i < map.GetLayers().GetCount(); i++)
    {
        MgLayer nextLayer = (MgLayer) map.GetLayers().GetItem(i);
        if (nextLayer.GetName().equals(layerName))
        {
            layer = nextLayer;
            break;
        }
    }
    return layer;
}

void OutputReaderContent(ServletResponse response, MgByteReader byteReader) throws MgException, Exception
{
    ByteArrayOutputStream bos = new ByteArrayOutputStream();

    byte[] byteBuffer = new byte[1024];
    int numBytes = byteReader.Read(byteBuffer, 1024);
    while(numBytes > 0)
    {
        bos.write(byteBuffer, 0, numBytes);
        numBytes = byteReader.Read(byteBuffer, 1024);
    }
    response.setContentLength(bos.size());
    response.setContentType(byteReader.GetMimeType());
    response.getOutputStream().write(bos.toByteArray());
}

%>
