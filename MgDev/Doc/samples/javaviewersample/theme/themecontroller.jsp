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
<%@ include file ="classes/theme.jsp" %>
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

    Theme theme = new Theme(args);
    JSONArray jsonArr = new JSONArray();
    JSONArray jsonPropArr = new JSONArray();
    JSONArray jsonDistroArr = null;
    JSONArray jsonScaleArr = new JSONArray();
    JSONObject jsonObj = new JSONObject();
    JSONObject jsonPropObj = new JSONObject();
    String responseType = "";
    String responseString = "";

    if(args.get("COMMAND") != null)
    {
        int cmd = Integer.valueOf(args.get("COMMAND"));
        switch (cmd)
        {
            case Command.GETLAYERINFO:
                responseType = "text/plain";
                LayerInfo layerInfo = theme.GetLayerInfo();
                ArrayList<Property> propArrayList = layerInfo.properties;
                
                for(int i=0; i<propArrayList.size(); i++)
                {
                    jsonDistroArr = new JSONArray();
                    ArrayList<Integer> distroArrayList = propArrayList.get(i).distroTypes;
                    for(int j=0; j<distroArrayList.size(); j++)
                    {
                        jsonDistroArr.put(distroArrayList.get(j));
                    }
                    jsonPropObj.put("name", propArrayList.get(i).name);
                    jsonPropObj.put("dataType", propArrayList.get(i).dataType);
                    jsonPropObj.put("distroTypes", jsonDistroArr.toString());
                    jsonPropArr.put(i, jsonPropObj.toString());
                }
                ArrayList<String> scaleArrayList = layerInfo.scaleRanges;
                for(int k=0; k<scaleArrayList.size(); k++)
                {
                    jsonScaleArr.put(scaleArrayList.get(k));
                }
                    
                jsonObj.put("properties", jsonPropArr.toString());
                jsonObj.put("scaleRanges", jsonScaleArr.toString());
                responseString = jsonObj.toString();
                break;
            case Command.GETPROPERTYMINMAXCOUNT:
                responseType = "text/plain";
                ArrayList<String> arrayList = theme.GetPropertyMinMaxCount();
                for(int i=0;i<arrayList.size();i++)
                {
                    jsonArr.put(arrayList.get(i));
                }
                responseString = jsonArr.toString();
                break;
            case Command.APPLYTHEME:
                responseType = "text/plain";
		responseString = theme.ApplyTheme();
                break;
        }
    }
    
    response.setContentType(responseType);

    out.print(responseString);
    out.flush();
%>