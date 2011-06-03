<%--
Copyright (C) 2004-2011 by Autodesk, Inc.
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

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Collections" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->
<!-- #Include File="product.aspx -->

<script language="C#" runat="server">

String webLayoutDefinition = "";
String sessionId = "";
String orgSessionId = "";
String username = "";
String password = "";
String locale = "";
int curFlyout = 0;
NameValueCollection cmds = null;
</script>

<%
    try
    {
        Response.Charset = "utf-8";
        MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");

        // Initialize web tier with the site configuration file.
        InitializeWebTier();

        //Fetch request parameters for this request
        GetRequestParameters();

        //Open connection with the server
        bool createSession = true;

        MgUserInformation cred = new MgUserInformation();
        if (null != sessionId && "" != sessionId)
        {
            cred.SetMgSessionId(sessionId);
            createSession = false;
        }
        else if (null != username)
        {
            cred.SetMgUsernamePassword(username, password);
        }
        else
        {
            RequestAuthentication();
            return;
        }

        MgSiteConnection site = new MgSiteConnection();
        cred.SetLocale(locale);

        cred.SetClientIp(GetClientIp(Request));
        cred.SetClientAgent(GetClientAgent());

        site.Open(cred);

        if (createSession)
        {
            MgSite site1 = site.GetSite();
            sessionId = site1.CreateSession();
            if (forDwf == 0)
            {
                orgSessionId = sessionId;
            }
        }

        //Get an MgWebLayout object
        MgWebLayout webLayout = null;
        MgResourceService resourceSrvc = site.CreateService(MgServiceType.ResourceService) as MgResourceService;
        MgResourceIdentifier webLayoutId = new MgResourceIdentifier(webLayoutDefinition);
        webLayout = new MgWebLayout(resourceSrvc, webLayoutId);

        //calculate the size of the variable elements of the viewer
        //
        MgWebToolBar toolBar = webLayout.GetToolBar();
        MgWebUiPane statusBar = webLayout.GetStatusBar();
        MgWebTaskPane taskPane = webLayout.GetTaskPane();
        MgWebInformationPane infoPane = webLayout.GetInformationPane();
        MgWebTaskBar taskBar = taskPane.GetTaskBar();
        String mapDef = webLayout.GetMapDefinition();

        bool showTaskPane = taskPane.IsVisible();
        bool showTaskBar = taskBar.IsVisible();
        bool showStatusbar = statusBar.IsVisible();
        bool showToolbar = toolBar.IsVisible();

        int taskPaneWidth = taskPane.GetWidth();
        int toolbarHeight = 30;
        int statusbarHeight = 26;

        int taskWidth = showTaskPane ? taskPaneWidth : 0;
        toolbarHeight = showToolbar ? toolbarHeight : 0;
        statusbarHeight = showStatusbar ? statusbarHeight : 0;

        //Encode the initial url so that it does not trip any sub-frames (especially if this url has parameters)
        String taskPaneUrl = HttpUtility.UrlEncode(taskPane.GetInitialTaskUrl());
        String vpath = GetSurroundVirtualPath(Request);
        bool defHome = false;
        if (taskPaneUrl == null || taskPaneUrl.Length == 0)
        {
            taskPaneUrl = "gettingstarted.aspx";
            defHome = true;
        }
        String mapDefinitionUrl = HttpUtility.UrlEncode(mapDef);
        String title = webLayout.GetTitle();

        bool showLegend = infoPane.IsLegendBandVisible();
        bool showProperties = infoPane.IsPropertiesBandVisible();
        int infoWidth = 0;

        if (showLegend || showProperties)
        {
            if (infoPane.IsVisible())
            {
                infoWidth = infoPane.GetWidth();
                if (infoWidth < 5)
                {
                    infoWidth = 5;    //ensure visible
                }
            }
            else
            {
                showProperties = showLegend = false;
            }
        }

        //calculate the url of the inner pages
        //
        String srcToolbar = showToolbar ? ("src=\"" + vpath + "toolbar.aspx?LOCALE=" + locale + "\"") : "";
        String srcStatusbar = showStatusbar ? ("src=\"" + vpath + "statusbar.aspx?LOCALE=" + locale + "\"") : "";
        String srcTaskFrame = showTaskPane ? ("src=\"" + vpath + "taskframe.aspx?WEBLAYOUT=" + HttpUtility.UrlEncode(webLayoutDefinition) + "&DWF=" + (forDwf != 0 ? "1" : "0") + "&SESSION=" + (sessionId != "" ? sessionId : "") + "&LOCALE=" + locale + "\"") : "";
        String srcTaskBar = "src=\"" + vpath + "taskbar.aspx?LOCALE=" + locale + "\"";

        //view center
        //
        MgPoint ptCenter = webLayout.GetCenter();
        String center = "null";
        if (null != ptCenter)
        {
            MgCoordinate coord = ptCenter.GetCoordinate();
            center = String.Format("new Point({0}, {1})", coord.GetX().ToString(NumberFormatInfo.InvariantInfo), coord.GetY().ToString(NumberFormatInfo.InvariantInfo));
        }

        //Process commands and declare command objects
        //
        MgWebCommandCollection commands = webLayout.GetCommands();
        String cmdObjects = "";
        String cmdObject = "";
        int navCmdIndex = 0;
        int scriptCmdIndex = 0;
        int measureCmdIndex = 0;
        int printCmdIndex = 0;
        int searchCmdIndex = 0;
        String userCode = "";
        String userCodeCalls = "\nswitch(funcIndex)\n{\n";
        int selAwareCmdCount = 0;
        String selAwareCmds = "";

        cmds = new NameValueCollection();

        int i = 0;

        for (i = 0; i < commands.GetCount(); i++)
        {
            MgWebCommand cmd = commands.GetItem(i);
            if (!cmd.IsUsed())
                continue;
            int tgtViewer = cmd.GetTargetViewerType();
            if ((tgtViewer == MgWebTargetViewerType.Dwf) != (forDwf == 1) && (tgtViewer != MgWebTargetViewerType.All))
                continue;

            String name = cmd.GetName();
            int action = cmd.GetAction();
            if (action == MgWebActions.Search)
            {
                MgWebSearchCommand searchCmd = cmd as MgWebSearchCommand;

                // create the column objects
                String cols = "var resCols" + searchCmdIndex + " = new Array();\n";
                if (searchCmd.GetResultColumnCount() > 0)
                {
                    for (int j = 0; j < searchCmd.GetResultColumnCount(); j++)
                    {
                        String col = String.Format("resCols{0}[{1}] = new ResultColumn(\"{2}\", \"{3}\");\n", searchCmdIndex, j, StrEscape(searchCmd.GetColumnDisplayNameAt(j), true), StrEscape(searchCmd.GetColumnPropertyNameAt(j), true));
                        cols += col;
                    }
                }
                cmdObjects += cols;

                // declare a new search command object
                cmdObject = String.Format("commands[{0}] = new SearchCommand(\"{1}\", \"{2}\", {3}, \"{4}\", \"{5}\", \"{6}\", \"{7}\", \"{8}\", \"{9}\", resCols{10}, \"{11}\", {12}, {13}, \"{14}\");\n",
                    i, StrEscape(name, false),
                    StrEscape(searchCmd.GetLabel(), false),
                    searchCmd.GetAction(),
                    searchCmd.GetIconUrl(),
                    searchCmd.GetDisabledIconUrl(),
                    StrEscape(searchCmd.GetTooltip(), false),
                    StrEscape(searchCmd.GetDescription(), false),
                    searchCmd.GetLayer(),
                    StrEscape(searchCmd.GetPrompt(), false),
                    searchCmdIndex,
                    StrEscape(searchCmd.GetFilter(), false),
                    searchCmd.GetMatchLimit(),
                    searchCmd.GetTarget(),
                    searchCmd.GetTargetName());

                searchCmdIndex++;
            }
            else if (action == MgWebActions.InvokeUrl)
            {
                MgWebInvokeUrlCommand invokeUrlCmd = cmd as MgWebInvokeUrlCommand;
                // create the parameter objects
                String parameters = "var navParams" + navCmdIndex + " = new Array();\n";
                String layers = "var layers" + navCmdIndex + " = new Array();\n";
                if (invokeUrlCmd.GetParameterCount() > 0)
                {
                    for (int j = 0; j < invokeUrlCmd.GetParameterCount(); j++)
                    {
                        String param = String.Format("navParams{0}[{1}] = new NavParam(\"{2}\", \"{3}\");\n", navCmdIndex, j, invokeUrlCmd.GetParameterNameAt(j), invokeUrlCmd.GetParameterValueAt(j));
                        parameters = parameters + param;
                    }
                }
                for (int j = 0; j < invokeUrlCmd.GetLayerCount(); j++)
                {
                    String layer = String.Format("layers{0}[{1}] = \"{2}\";\n", navCmdIndex, j, invokeUrlCmd.GetLayerNameAt(j));
                    layers = layers + layer;
                }
                cmdObjects = cmdObjects + parameters + layers;

                if (invokeUrlCmd.DisabledIfSelectionEmpty() || invokeUrlCmd.GetLayerCount() > 0)
                {
                    selAwareCmds += String.Format("selectionAwareCmds[{0}] = {1};\n", selAwareCmdCount, i);
                    selAwareCmdCount++;
                }

                // declare a new invokeurl command object
                cmdObject = String.Format("commands[{0}] = new InvokeUrlCommand(\"{1}\", {2}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", \"{7}\", navParams{8}, {9}, layers{10}, {11}, \"{12}\");\n",
                    i,
                    StrEscape(name, false),
                    invokeUrlCmd.GetAction(),
                    invokeUrlCmd.GetIconUrl(),
                    invokeUrlCmd.GetDisabledIconUrl(),
                    StrEscape(invokeUrlCmd.GetTooltip(), false),
                    StrEscape(invokeUrlCmd.GetDescription(), false),
                    invokeUrlCmd.GetUrl(),
                    navCmdIndex,
                    invokeUrlCmd.DisabledIfSelectionEmpty() ? "true" : "false",
                    navCmdIndex,
                    invokeUrlCmd.GetTarget(),
                    invokeUrlCmd.GetTargetName());

                navCmdIndex++;
            }
            else if (action == MgWebActions.Buffer || action == MgWebActions.SelectWithin ||
                   action == MgWebActions.Measure || action == MgWebActions.ViewOptions || action == MgWebActions.GetPrintablePage)
            {
                MgWebUiTargetCommand targetCmd = cmd as MgWebUiTargetCommand;
                if (action == MgWebActions.Measure)
                {
                    if (measureCmdIndex != 0)
                    {
                        throw new Exception(MgLocalizer.GetString("ALREADYINMEASURE", locale));
                    }
                    measureCmdIndex = i;
                }

                // declare a new ui target command object
                cmdObject = String.Format("commands[{0}] = new UiTargetCommand(\"{1}\", {2}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", {7}, \"{8}\");\n",
                    i,
                    StrEscape(name, false),
                    targetCmd.GetAction(),
                    targetCmd.GetIconUrl(),
                    targetCmd.GetDisabledIconUrl(),
                    StrEscape(targetCmd.GetTooltip(), false),
                    StrEscape(targetCmd.GetDescription(), false),
                    targetCmd.GetTarget(),
                    targetCmd.GetTargetName());
            }

            else if (action == MgWebActions.Help)
            {
                MgWebHelpCommand helpCmd = cmd as MgWebHelpCommand;
                // declare a new help  command object
                cmdObject = String.Format("commands[{0}] = new HelpCommand(\"{1}\", {2}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", \"{7}\", {8}, \"{9}\");\n",
                    i,
                    StrEscape(name, false),
                    helpCmd.GetAction(),
                    helpCmd.GetIconUrl(),
                    helpCmd.GetDisabledIconUrl(),
                    StrEscape(helpCmd.GetTooltip(), false),
                    StrEscape(helpCmd.GetDescription(), false),
                    helpCmd.GetUrl(),
                    helpCmd.GetTarget(),
                    helpCmd.GetTargetName());
            }
            else if (action == MgWebActions.PrintMap)
            {
                MgWebPrintCommand printCmd = cmd as MgWebPrintCommand;
                // declare the print layouts
                String layouts = "var layouts" + printCmdIndex + " = new Array();\n";
                int j;
                for (j = 0; j < printCmd.GetPrintLayoutCount(); j++)
                {
                    String layout = "";
                    layout = String.Format("layouts{0}[{1}] = \"{2}\";\n", printCmdIndex, j, printCmd.GetPrintLayoutAt(j));
                    layouts = layouts + layout;
                }
                cmdObjects = cmdObjects + layouts;

                // declare a new print command object
                cmdObject = String.Format("commands[{0}] = new PrintCommand(\"{1}\", {2}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", layouts{7});\n",
                    i,
                    StrEscape(name, false),
                    printCmd.GetAction(),
                    printCmd.GetIconUrl(),
                    printCmd.GetDisabledIconUrl(),
                    StrEscape(printCmd.GetTooltip(), false),
                    StrEscape(printCmd.GetDescription(), false),
                    printCmdIndex);

                printCmdIndex++;
            }
            else if (action == MgWebActions.InvokeScript)
            {
                MgWebInvokeScriptCommand invokeScriptCmd = cmd as MgWebInvokeScriptCommand;
                // declare a new basic command object
                cmdObject = String.Format("commands[{0}] = new InvokeScriptCommand(\"{1}\", {2}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", {7});\n",
                    i,
                    StrEscape(name, false),
                    invokeScriptCmd.GetAction(),
                    invokeScriptCmd.GetIconUrl(),
                    invokeScriptCmd.GetDisabledIconUrl(),
                    StrEscape(invokeScriptCmd.GetTooltip(), false),
                    StrEscape(invokeScriptCmd.GetDescription(), false),
                    scriptCmdIndex);

                userCode = userCode + "\nfunction UserFunc" + scriptCmdIndex + "()\n{\n" + invokeScriptCmd.GetScriptCode() + "\n}\n";
                userCodeCalls = userCodeCalls + String.Format("case {0}: UserFunc{1}(); break;\n", scriptCmdIndex, scriptCmdIndex);

                scriptCmdIndex++;
            }
            else
            {
                // declare a new basic command object
                cmdObject = String.Format("commands[{0}] = new BasicCommand(\"{1}\", {2}, \"{3}\", \"{4}\", \"{5}\", \"{6}\");\n",
                    i,
                    name,
                    cmd.GetAction(),
                    cmd.GetIconUrl(),
                    cmd.GetDisabledIconUrl(),
                    StrEscape(cmd.GetTooltip(), false),
                    StrEscape(cmd.GetDescription(), false));
            }

            cmdObjects = cmdObjects + cmdObject;
            cmds[name] = i.ToString(NumberFormatInfo.InvariantInfo);
        }
        userCodeCalls = userCodeCalls + "\n}\n";

        //Declare toolbar items
        //
        String toolbarDef = DeclareUiItems(toolBar.GetWidgets(), "toolbarItems");

        //Declare task items
        String taskListDef = DeclareUiItems(taskBar.GetTaskList(), "taskItems");

        //Declare context menu items
        MgWebContextMenu ctxMenu = webLayout.GetContextMenu();
        String ctxMenuDef;
        if (ctxMenu.IsVisible())
            ctxMenuDef = DeclareUiItems(ctxMenu, "ctxMenuItems");
        else
            ctxMenuDef = "";

        //task items texts
        String taskItemTexts = "";
        MgWebWidgetCollection taskButtons = taskBar.GetTaskButtons();
        for (i = 0; i < 4; i++)
        {
            MgWebTaskBarWidget btn = (MgWebTaskBarWidget)taskButtons.GetWidget(i);
            if (i > 0)
                taskItemTexts += ",";
            taskItemTexts += "\"" + StrEscape(btn.GetName(), true) + "\"," +
                                "\"" + StrEscape(btn.GetTooltip(), true) + "\"," +
                                "\"" + StrEscape(btn.GetDescription(), true) + "\"," +
                                "\"" + StrEscape(btn.GetIconUrl(), true) + "\"," +
                                "\"" + StrEscape(btn.GetDisabledIconUrl(), true) + "\"";
        }

        //transmit the session to the map pane if one was specified to this request
        String sessionParam = "";
        if (orgSessionId != "")
        {
            sessionParam = "&SESSION=" + orgSessionId;
        }

        //load the frameset template and format it
        String frameset = "";
        String viewerType = (1 == forDwf) ? "DWF" : "HTML";

        if (showTaskBar)
        {
            frameset = LoadTemplate(Request, "../viewerfiles/framesettaskbar.templ");
            String[] vals = {
                        statusbarHeight.ToString(NumberFormatInfo.InvariantInfo),
                        taskWidth.ToString(NumberFormatInfo.InvariantInfo),
                        toolbarHeight.ToString(NumberFormatInfo.InvariantInfo),
                        srcToolbar,
                        vpath + "mapframe.aspx",
                        mapDefinitionUrl,
                        viewerType,
                        showLegend? "1" : "0",
                        showProperties? "1" : "0",
                        infoWidth.ToString(NumberFormatInfo.InvariantInfo),
                        locale,
                        webLayout.GetHyperlinkTarget().ToString(NumberFormatInfo.InvariantInfo),
                        webLayout.GetHyperlinkTargetFrame(),
                        webLayout.IsZoomControlVisible()? "1": "0",
                        sessionParam,
                        vpath + "formframe.aspx",
                        toolbarHeight.ToString(NumberFormatInfo.InvariantInfo),
                        srcTaskBar,
                        srcTaskFrame,
                        srcStatusbar
                        };
            frameset = Substitute(frameset, vals);
        }
        else
        {
            frameset = LoadTemplate(Request, "../viewerfiles/framesetnotaskbar.templ");
            String[] vals = {
                        toolbarHeight.ToString(NumberFormatInfo.InvariantInfo),
                        statusbarHeight.ToString(NumberFormatInfo.InvariantInfo),
                        srcToolbar,
                        taskWidth.ToString(NumberFormatInfo.InvariantInfo),
                        vpath + "mapframe.aspx",
                        mapDefinitionUrl,
                        viewerType,
                        showLegend? "1" : "0",
                        showProperties? "1" : "0",
                        infoWidth.ToString(NumberFormatInfo.InvariantInfo),
                        locale,
                        webLayout.GetHyperlinkTarget().ToString(NumberFormatInfo.InvariantInfo),
                        webLayout.GetHyperlinkTargetFrame(),
                        webLayout.IsZoomControlVisible()? "1": "0",
                        sessionParam,
                        srcTaskFrame,
                        vpath + "formframe.aspx",
                        srcStatusbar,
                        };
            frameset = Substitute(frameset, vals);
        }

        String homePageUrl = HttpUtility.UrlDecode(taskPaneUrl);
        if (homePageUrl.Length < 8 || String.Compare(homePageUrl.Substring(0, 7), "http://", true) != 0)
            homePageUrl = vpath + homePageUrl;

        //load the HTML template and format it
        //
        String template = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/mainframe.templ"), locale, GetClientOS(Request));
        String[] args = {
                    webLayout.GetTitle(),
                    GetRootVirtualFolder(Request) + "/mapagent/mapagent.fcgi",
                    webLayout.GetEnablePingServer()? "1" : "0",
                    site.GetSite().GetSessionTimeout().ToString(NumberFormatInfo.InvariantInfo),
                    locale,
                    showToolbar? "1" : "0",
                    showStatusbar? "1" : "0",
                    showTaskPane? "1" : "0",
                    showTaskPane? "0" : (showTaskBar? "1" : "0"),
                    homePageUrl,
                    defHome? "1" : "0",
                    webLayoutDefinition,
                    mapDef,
                    taskWidth.ToString(NumberFormatInfo.InvariantInfo),
                    center,
                    webLayout.GetScale().ToString(NumberFormatInfo.InvariantInfo),
                    StrEscape(title, false),
                    (forDwf == 1)? "1" : "0",
                    cmdObjects,
                    toolbarDef,
                    taskListDef,
                    ctxMenuDef,
                    userCode,
                    taskItemTexts,
                    selAwareCmds,
                    vpath + "quickplotpanel.aspx",
                    vpath + "measureui.aspx",
                    vpath + "searchprompt.aspx",
                    vpath + "bufferui.aspx",
                    vpath + "selectwithinui.aspx",
                    userCodeCalls,
                    vpath + "viewoptions.aspx",
                    frameset
                    };

        String templ = Substitute(template, args);
        Response.Write(templ);
    }
    catch (MgUnauthorizedAccessException)
    {
        RequestAuthentication();
        return;
    }
    catch (MgUserNotFoundException)
    {
        RequestAuthentication();
        return;
    }
    catch (MgAuthenticationFailedException e)
    {
        RequestAuthentication();
        return;
    }
    catch (MgException e)
    {
        String errorMsg = EscapeForHtml(e.GetDetails());
        Response.Write(errorMsg);
    }
    catch (Exception ne)
    {
        String errorMsg = EscapeForHtml(ne.ToString());
        Response.Write(errorMsg);
    }
%>

<script runat="server">
void RequestAuthentication()
{
    Response.AddHeader("WWW-Authenticate", "Basic realm=\"" + product + "\"");
    Response.StatusCode = 401;
    Response.StatusDescription = MgLocalizer.GetString("ACCESSDENIED", locale);
    Response.Write(MgLocalizer.GetString("NEEDLOGIN", locale));
    Response.Flush();
}

String DeclareUiItems(MgWebWidgetCollection coll, String varname)
{
    String def = "";
    int i, j;

    if(coll != null)
    {
        for(i = 0, j = 0; i < coll.GetCount(); i++)
        {
            MgWebWidget item = coll.GetWidget(i);
            int it = item.GetType();
            if(it == MgWebWidgetType.Separator)
            {
                def = def + String.Format("{0}[{1}] = new UiItem('');\n", varname, j++);
            }
            else if(it == MgWebWidgetType.Command)
            {
                MgWebCommandWidget cmdWidget = item as MgWebCommandWidget;
                MgWebCommand cmd = cmdWidget.GetCommand();
                if (cmds[cmd.GetName()] == null)
                    continue;
                int cmdIndex = Convert.ToInt32(cmds[cmd.GetName()]);
                def = def + String.Format("{0}[{1}] = new CommandItem(\"{2}\", {3});\n", varname, j++, cmd.GetLabel(), cmdIndex);
            }
            else
            {
                MgWebFlyoutWidget flyoutDef = item as MgWebFlyoutWidget;
                curFlyout++;
                String subVarname = "flyoutDef" + curFlyout;
                String htmlName = "FlyoutDiv" + curFlyout;
                def = def + String.Format("var {0} = new Array()\n", subVarname);
                def = def + DeclareUiItems(flyoutDef.GetSubItems(), subVarname);
                def = def + String.Format("{0}[{1}] = new FlyoutItem(\"{2}\", {3}, \"{4}\", \"{5}\");\n", varname, j++, flyoutDef.GetLabel(), subVarname, htmlName, flyoutDef.GetIconUrl());
            }
        }
    }
    return def;
}

void GetParameters(NameValueCollection parameters)
{
    locale = ValidateLocaleString(GetParameter(parameters, "LOCALE"));
    sessionId = ValidateSessionId(GetParameter(parameters, "SESSION"));
    webLayoutDefinition = ValidateResourceId(GetParameter(parameters, "WEBLAYOUT"));
    if (sessionId != null && sessionId.Length > 0)
    {
        orgSessionId = sessionId;
    }
    else
    {
        username = GetParameter(parameters, "USERNAME");
        if (null != username && username.Length > 0)
        {
            password = GetParameter(parameters, "PASSWORD");
            if (null == password)
            {
                password = "";
            }
            return;
        }

        //Check in server variables for username and password
        if (Request.ServerVariables["AUTH_USER"].Length>0)
        {
            username = Request.ServerVariables["AUTH_USER"];

            if (Request.ServerVariables["AUTH_PASSWORD"].Length>0)
            {
                password = Request.ServerVariables["AUTH_PASSWORD"];
            }
            return;
        }

        //Check the Http headers for Authorization header
        //If one exist use base64 decoding to get the username:password pair
        if(null != Request.Headers.Get("Authorization"))
        {
            String usernamePassword =Request.Headers["Authorization"];
            usernamePassword = base64Decode(usernamePassword.Substring(6));
            String [] authPair = usernamePassword.Split(':');
            username = authPair[0];
            password = authPair[1];
            return;
        }
    }
}

void GetRequestParameters()
{
    if (Request.HttpMethod == "POST")
    {
        GetParameters(Request.Form);
    }
    else
    {
        GetParameters(Request.QueryString);
    }
}

//Used to decode the String in the authorization header if necessary
String base64Decode(String data)
{
    try
    {
        System.Text.UTF8Encoding encoder = new System.Text.UTF8Encoding();
        System.Text.Decoder utf8Decode = encoder.GetDecoder();

        byte[] todecode_byte = Convert.FromBase64String(data);

        String result = System.Text.Encoding.UTF8.GetString(todecode_byte, 0, todecode_byte.Length);
        return result;
    }
    catch(Exception e)
    {
        throw new Exception("Error in base64Decode" + e.Message);
    }
}

String StrEscape(String str, bool single)
{
    String c = single? "'" : "\"";
    if (str.IndexOf(c)<0)
    {
        return str;
    }
    return (str.Replace(c, "\\"+c));
}

</script>
