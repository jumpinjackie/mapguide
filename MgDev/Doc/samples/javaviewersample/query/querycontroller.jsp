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
<%@ page import="org.json.*" %>
<%@ include file ="../utilityfunctions.jsp" %>
<%@ include file ="classes/query.jsp" %>
<%@ include file ="classes/command.jsp" %>

<%
    Map<String, String> args = new HashMap<String, String>();

    Enumeration paramNames = request.getParameterNames();
    while(paramNames.hasMoreElements())
    {
        String paramName = (String)paramNames.nextElement();
        String paramValue = request.getParameter(paramName);
        args.put(paramName, paramValue);
    }

    InitializeWebTier();

    Query query = new Query(args);
    JSONArray json = new JSONArray();
    JSONObject jsonObj = new JSONObject();
    String responseType = "";
    String responseString = "";

    if(args.get("COMMAND") != null)
    {
        int cmd = Integer.valueOf(args.get("COMMAND"));
        switch (cmd)
        {
            case Command.GETLAYERPROPERTIES:
                responseType = "text/plain";
                ArrayList<Property> arrayList = query.GetLayerProperties();
                for(int i=0;i<arrayList.size();i++)
                {
                    jsonObj.put("name", arrayList.get(i).name);
                    jsonObj.put("isString", arrayList.get(i).isString);
                    json.put(i, jsonObj.toString());
                }
                responseString = json.toString();
                break;
            case Command.TOGGLESPATIALFILTER:
                responseType = "text/plain";
                responseString = json.put(query.ToggleSpatialFilter()).toString();
                break;
            case Command.SHOWSPATIALFILTER:
                responseType = "text/plain";
                responseString = json.put(query.ShowSpatialFilter()).toString();
                break;
            case Command.EXECUTEQUERY:
                responseType = "text/plain";
                ArrayList<Feature> resultList = query.Execute();
                for(int i=0;i<resultList.size();i++)
                {
                    jsonObj.put("displayValue", resultList.get(i).displayValue);
                    jsonObj.put("centerX", resultList.get(i).centerX);
                    jsonObj.put("centerY", resultList.get(i).centerY);
                    jsonObj.put("idList", resultList.get(i).idList);
                    json.put(i, jsonObj.toString());
                }
                responseString = json.toString();
                break;
            case Command.GETSELECTIONXML:
                responseType = "text/xml";
                responseString = query.GetSelectionXML();
                break;
        }
    }
    
    response.setContentType(responseType);

    out.print(responseString);
    out.flush();
%>