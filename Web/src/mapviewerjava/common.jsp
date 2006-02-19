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

<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ page import="org.osgeo.mapguide.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>

<%!
void InitializeWebTier() throws MgException
{
    // Initialize web tier with the site configuration file.  The config
    // file should be in the same folder as this script.
    //
    String realpath = getServletContext().getRealPath("/");
    String configPath =  realpath + "webconfig.ini";
    MapGuideJavaApi.MgInitializeWebTier(configPath);
}

String GetParameter(HttpServletRequest request, String name)
{
    String strval = request.getParameter(name);
    if(strval == null)
        return "";
    return strval;
}

int GetIntParameter(HttpServletRequest request, String name)
{
    String strval = GetParameter(request, name);
    if(strval.equals(""))
        return 0;
    return Integer.parseInt(strval);
}

double GetDoubleParameter(HttpServletRequest request, String name)
{
    String strval = GetParameter(request, name);
    if(strval.equals(""))
        return 0;
    return Double.parseDouble(strval);
}

boolean IsParameter(HttpServletRequest request, String name)
{
    return request.getParameter(name) != null;
}

String LoadTemplate(String filename) throws FileNotFoundException, IOException
{
    String realpath = getServletContext().getRealPath("/");
    String templPath =  realpath + File.separator + filename;

    File theFile = new File(templPath);
    int size = (int)theFile.length();
    FileInputStream is = new FileInputStream(theFile);
    byte[] buffer = new byte[size];
    is.read(buffer);
    return new String(buffer);
}

String Substitute(String templ, String[] vals)
{
    StringBuffer res = new StringBuffer();
    int index = 0, val = 0;
    boolean found;
    do
    {
        found = false;
        int i = templ.indexOf('%', index);
        if(i != -1)
        {
            found = true;
            res.append(templ.substring(index, i));
            if(i < templ.length() - 1)
            {
                if(templ.charAt(i+1) == '%')
                    res.append('%');
                else if(templ.charAt(i+1) == 's')
                {
                    res.append(vals[val ++]);
                }
                else
                    res.append('@');    //add a character illegal in jscript so we know the template was incorrect
                index = i + 2;
            }
        }
    } while(found);
    res.append(templ.substring(index));
    return res.toString();
}

String GetSurroundVirtualPath(HttpServletRequest request) {
    String uri = request.getRequestURI();
    return uri.substring(0, uri.lastIndexOf('/') + 1);
}

String GetRootVirtualFolder(HttpServletRequest request) {
    return request.getContextPath();
}

int GetClientOS(HttpServletRequest request) {
    String agent = request.getHeader("User-Agent");
    if(agent != null) {
        if(agent.indexOf("Windows") != -1)
            return 0;
        else if(agent.indexOf("Macintosh") != -1)
            return 1;
        else
            return 2;
    }
    return 0;
}

%>
