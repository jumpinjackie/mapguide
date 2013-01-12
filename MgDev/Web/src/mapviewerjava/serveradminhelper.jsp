<%--
  -Copyright (C) 2004-2011 by Autodesk, Inc.
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
<%@ page import="java.text.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ include file="common.jsp" %>
<%@ page isThreadSafe="false" %>

<%

    PrintWriter outStream = response.getWriter();

    try
    {
        response.setContentType("text/html; charset=UTF-8");
        request.setCharacterEncoding("UTF-8");

        outStream.write("<b>MapGuide Server Admin Service Tests</b><p>");

        InitializeWebTier();

        MgUserInformation cred = new MgUserInformation();
        cred.SetMgUsernamePassword("Administrator", "admin");
        cred.SetLocale(GetDefaultLocale());
        cred.SetClientIp(GetClientIp(request));
        cred.SetClientAgent(GetClientAgent());

        MgServerAdmin serverAdmin = new MgServerAdmin();
        serverAdmin.Open("localhost", cred);

        String operation = request.getParameter("OPERATION");

        // Get all information properties
        MgPropertyCollection infoProp = serverAdmin.GetInformationProperties();

        if (operation.equals("ONLINE"))
        {
            serverAdmin.BringOnline();
            if (serverAdmin.IsOnline())
            {
                outStream.write("Server online<p>");
            }
            else
            {
                outStream.write("Server offline<p>");
            }
        }
        else if (operation.equals("OFFLINE"))
        {
            serverAdmin.TakeOffline();
            if (serverAdmin.IsOnline())
            {
                outStream.write("Server online<p>");
            }
            else
            {
                outStream.write("Server offline<p>");
            }
        }
        else if (operation.equals("GETTOTALPHYSICALMEMORY"))
        {
            MgInt64Property nMemory = (MgInt64Property)infoProp.GetItem("TotalPhysicalMemory");
            outStream.write("Total Physical Memory: " + nMemory.GetValue() + " bytes");
        }
        else if (operation.equals("GETAVAILABLEPHYSICALMEMORY"))
        {
            MgInt64Property nMemory = (MgInt64Property)infoProp.GetItem("AvailablePhysicalMemory");
            outStream.write("Available Physical Memory: " + nMemory.GetValue() + " bytes");
        }
        else if (operation.equals("GETTOTALVIRTUALMEMORY"))
        {
            MgInt64Property nMemory = (MgInt64Property)infoProp.GetItem("TotalVirtualMemory");
            outStream.write("Total Virtual Memory: " + nMemory.GetValue() + " bytes");
        }
        else if (operation.equals("GETAVAILABLEVIRTUALMEMORY"))
        {
            MgInt64Property nMemory = (MgInt64Property)infoProp.GetItem("AvailableVirtualMemory");
            outStream.write("Available Virtual Memory: " + nMemory.GetValue() + " bytes");
        }
        else if (operation.equals("GETCPUUTILIZATION"))
        {
            MgInt32Property nCpuLoad = (MgInt32Property)infoProp.GetItem("CpuUtilization");
            outStream.write("Current CPU Utilization: " + nCpuLoad.GetValue() + "%");
        }
        else if (operation.equals("GETUPTIME"))
        {
            MgInt32Property nUptime = (MgInt32Property)infoProp.GetItem("Uptime");
            outStream.write("Server uptime: " + nUptime.GetValue() + " seconds");
        }
        else if (operation.equals("GETAVERAGEOPERATIONTIME"))
        {
            MgInt32Property nTime = (MgInt32Property)infoProp.GetItem("AverageOperationTime");
            outStream.write("Average Operation Time: " + nTime.GetValue() + " milliseconds");
        }
        else if (operation.equals("GETTOTALOPERATIONTIME"))
        {
            MgInt32Property nTime = (MgInt32Property)infoProp.GetItem("TotalOperationTime");
            outStream.write("Total Operation Time: " + nTime.GetValue() + " seconds");
        }
        else if (operation.equals("GETADMINQUEUECOUNT"))
        {
            MgInt32Property nCount = (MgInt32Property)infoProp.GetItem("AdminOperationsQueueCount");
            outStream.write("Admin queue count: " + nCount.GetValue());
        }
        else if (operation.equals("GETCLIENTQUEUECOUNT"))
        {
            MgInt32Property nCount = (MgInt32Property)infoProp.GetItem("ClientOperationsQueueCount");
            outStream.write("Client queue count: " + nCount.GetValue());
        }
        else if (operation.equals("GETSITEQUEUECOUNT"))
        {
            MgInt32Property nCount = (MgInt32Property)infoProp.GetItem("SiteOperationsQueueCount");
            outStream.write("Site queue count: " + nCount.GetValue());
        }
        else if (operation.equals("SERVERVERSION"))
        {
            MgStringProperty version = (MgStringProperty)infoProp.GetItem("ServerVersion");
            outStream.write("Server version: " + version.GetValue());
        }
        else
        {
            outStream.write("Error.  Unknown operation: " + operation);
        }
    }
    catch (MgException mge)
    {
        outStream.write(mge.GetExceptionMessage());
        outStream.write(mge.GetDetails());
    }

%>
