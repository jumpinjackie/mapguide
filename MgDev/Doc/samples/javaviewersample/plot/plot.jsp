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
<%@ include file ="../utilityfunctions.jsp" %>

<%
    // This script plots the current view of the map, optionally
    // using the Scale parameter if Scale > 0.

    String mgSessionId = request.getParameter("SESSION");

    try
    {
        // Initialize the Web Extensions and connect to the Site using
        // the session identifier stored in JSP session state.

        InitializeWebTier();

        MgUserInformation userInfo = new MgUserInformation(mgSessionId);
        MgSiteConnection siteConnection = new MgSiteConnection();
        siteConnection.Open(userInfo);

        // Create an instance of ResourceService and use that to open the
        // current map instance stored in session state.

        MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
        MgMap map = new MgMap();
        map.Open(resourceService, "Sheboygan");

        // Now create an instance of MappingService and use it to plot the
        // current view of the map.

        MgDwfVersion dwfVersion = new MgDwfVersion("6.01", "1.2");
        MgPlotSpecification plotSpec = new MgPlotSpecification(8.5f, 11.0f, MgPageUnitsType.Inches, 0f, 0f, 0f, 0f);
        plotSpec.SetMargins(0.5f, 0.5f, 0.5f, 0.5f);

        MgMappingService mappingService = (MgMappingService)siteConnection.CreateService(MgServiceType.MappingService);

        MgLayout layout = null;
        String useLayout = request.getParameter("UseLayout");
        if (useLayout != null && useLayout.equals("true"))
        {
            MgResourceIdentifier layoutRes = new MgResourceIdentifier("Library://Samples/Sheboygan/Layouts/SheboyganMap.PrintLayout");
            layout = new MgLayout(layoutRes, "City of Sheboygan", MgPageUnitsType.Inches);
        }

        double scale = Double.parseDouble(request.getParameter("Scale"));
        MgByteReader byteReader = null;
        if (scale <= 0)
        {
            byteReader = mappingService.GeneratePlot(map, plotSpec, layout, dwfVersion);
        }
        else
        {
            MgCoordinate mapCenter = map.GetViewCenter().GetCoordinate();
            byteReader = mappingService.GeneratePlot(map, mapCenter, scale, plotSpec, layout, dwfVersion);
        }

        // Now output the resulting DWF.

        OutputReaderContent(response, byteReader);
    }
    catch (MgException e)
    {
        String result = e.GetExceptionMessage() + "<br><br>" + e.GetDetails();

        response.setContentType("text/html");
        response.setContentLength(result.length());
        out.write(result);
    }
%>
