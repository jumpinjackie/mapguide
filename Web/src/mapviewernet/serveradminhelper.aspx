<%--
Copyright (C) 2004-2010 by Autodesk, Inc.
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
--%>

<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections" %>
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script runat="server">
private void Page_Load(object sender, System.EventArgs e)
{
    try
    {
        Response.Charset = "utf-8";

        Response.Write("<b>MapGuide Server Admin Service Tests</b><p>");

        // Initialize web tier with the site configuration file.
        InitializeWebTier();

        MgUserInformation cred = new MgUserInformation();
        cred.SetMgUsernamePassword("Administrator", "admin");
        cred.SetLocale(GetDefaultLocale());
        cred.SetClientIp(GetClientIp(Request));
        cred.SetClientAgent(GetClientAgent());

        MgServerAdmin serverAdmin = new MgServerAdmin();
        serverAdmin.Open("localhost", cred);

        string operation = Request.QueryString["OPERATION"];

        //Get all information properties
        MgPropertyCollection infoProp = serverAdmin.GetInformationProperties();


        switch (operation)
        {
            case "ONLINE":
                serverAdmin.BringOnline();
                if (serverAdmin.IsOnline())
                {
                    Response.Write("Server online<p>");
                }
                else
                {
                    Response.Write("Server offline<p>");
                }
                break;
            case "OFFLINE":
                serverAdmin.TakeOffline();
                if (serverAdmin.IsOnline())
                {
                    Response.Write("Server online<p>");
                }
                else
                {
                    Response.Write("Server offline<p>");
                }
                break;

            case "GETTOTALPHYSICALMEMORY":
                MgInt64Property nMemory = infoProp.GetItem("TotalPhysicalMemory") as MgInt64Property;
                Response.Write("Total Physical Memory: " + nMemory.GetValue().ToString() + " bytes");
                break;
            case "GETAVAILABLEPHYSICALMEMORY":
                nMemory = infoProp.GetItem("AvailablePhysicalMemory") as MgInt64Property;
                Response.Write("Available Physical Memory: " + nMemory.GetValue().ToString() + " bytes");
                break;
            case "GETTOTALVIRTUALMEMORY":
                nMemory = infoProp.GetItem("TotalVirtualMemory") as MgInt64Property;
                Response.Write("Total Virtual Memory: " + nMemory.GetValue().ToString() + " bytes");
                break;
            case "GETAVAILABLEVIRTUALMEMORY":
                nMemory = infoProp.GetItem("AvailableVirtualMemory") as MgInt64Property;
                Response.Write("Total Available Virtual Memory: " + nMemory.GetValue() + " bytes");
                break;
            case "GETCPUUTILIZATION":
                MgInt32Property nCpuLoad = infoProp.GetItem("CpuUtilization") as MgInt32Property;
                Response.Write("Current CPU Utilization: " + nCpuLoad.GetValue().ToString() + "%");
                break;
            case "GETUPTIME":
                MgInt32Property nUptime = infoProp.GetItem("Uptime") as MgInt32Property;
                Response.Write("Server uptime: " + nUptime.GetValue().ToString() + " seconds");
                break;
            case "GETAVERAGEOPERATIONTIME":
                MgInt32Property nTime = infoProp.GetItem("AverageOperationTime") as MgInt32Property;
                Response.Write("Average Operation Time: " + nTime.GetValue().ToString() + " milliseconds");
                break;
            case "GETTOTALOPERATIONTIME":
                nTime = infoProp.GetItem("TotalOperationTime") as MgInt32Property;
                Response.Write("Total Operation Time: " + nTime.GetValue().ToString() + " seconds");
                break;
            case "GETADMINQUEUECOUNT":
                MgInt32Property nCount = infoProp.GetItem("AdminOperationsQueueCount") as MgInt32Property;
                Response.Write("Admin queue count: " + nCount.GetValue().ToString());
                break;
            case "GETCLIENTQUEUECOUNT":
                nCount = infoProp.GetItem("ClientOperationsQueueCount") as MgInt32Property;
                Response.Write("Client queue count: " + nCount.GetValue().ToString());
                break;
            case "GETSITEQUEUECOUNT":
                nCount = infoProp.GetItem("SiteOperationsQueueCount") as MgInt32Property;
                Response.Write("Site queue count: " + nCount.GetValue().ToString());
                break;
            case "SERVERVERSION":
                MgStringProperty version = infoProp.GetItem("ServerVersion") as MgStringProperty;
                Response.Write("Server version: " + version.GetValue());
                break;
            default:
                Response.Write("Error.  Unknown operation: operation");
                break;
        }
    }
    catch (MgException mge)
    {
        Response.Write(mge.GetMessage());
        Response.Write(mge.GetDetails());
    }
}
</script>
