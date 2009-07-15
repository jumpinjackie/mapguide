<%--
  -Copyright (C) 2004-2009 by Autodesk, Inc.
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
<%@ page import="java.net.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ include file="common.jsp" %>
<%@ include file="product.jsp" %>

<%!

String webLayoutDefinition = "";
String sessionId = "";
String orgSessionId = "";
String username = "";
String password = "";
String locale = "";
int curFlyout = 0;
Hashtable cmds = new Hashtable();
%>

<%!
void GenerateMainPage(int forDwf, HttpServletRequest request, HttpServletResponse response) throws IOException, MgException
{
try
{
    // Initialize web tier with the site configuration file.
    InitializeWebTier();

    //fetch the parameters for this request
    //
    GetRequestParameters(request);

    //Open a connection with the server
    //
    boolean createSession = true;

    MgUserInformation cred = new MgUserInformation();
    if(sessionId != null && sessionId != "")
    {
        cred.SetMgSessionId(sessionId);
        createSession = false;
    }
    else if(username != null)
        cred.SetMgUsernamePassword(username, password);
    else
    {
        RequestAuthentication(response);
        return;
    }

    MgSiteConnection site = new MgSiteConnection();
    cred.SetLocale(locale);

    cred.SetClientIp(GetClientIp(request));
    cred.SetClientAgent(GetClientAgent());

    site.Open(cred);

    if(createSession)
    {
        MgSite site1 = site.GetSite();
        sessionId = site1.CreateSession();
        if(forDwf == 0)
        {
            orgSessionId = sessionId;
        }
    }

    //Get a MgWebLayout object initialized with the specified web layout definition
    //
    MgWebLayout webLayout = null;
    MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);
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

    boolean showTaskPane = taskPane.IsVisible();
    boolean showTaskBar = taskBar.IsVisible();
    boolean showStatusbar = statusBar.IsVisible();
    boolean showToolbar = toolBar.IsVisible();

    int taskPaneWidth = taskPane.GetWidth();
    int toolbarHeight = 30;
    int statusbarHeight = 26;

    taskPaneWidth = showTaskPane? taskPaneWidth: 0;
    toolbarHeight = showToolbar? toolbarHeight: 0;
    statusbarHeight = showStatusbar? statusbarHeight: 0;
	
	//Encode the initial url so that it does not trip any sub-frames (especially if this url has parameters)
    String taskPaneUrl = URLEncoder.encode(taskPane.GetInitialTaskUrl(), "UTF-8");
    String vpath = GetSurroundVirtualPath(request);
    boolean defHome = false;
    if(taskPaneUrl == null || taskPaneUrl.length() == 0) {
        taskPaneUrl = "gettingstarted.jsp";
        defHome = true;
    }

    String mapDefinitionUrl = URLEncoder.encode(mapDef, "UTF-8");
    String title = webLayout.GetTitle();

    boolean showLegend = infoPane.IsLegendBandVisible();
    boolean showProperties = infoPane.IsPropertiesBandVisible();

    int infoWidth = 0;
    if(showLegend || showProperties)
    {
        if(infoPane.IsVisible())
        {
            infoWidth = infoPane.GetWidth();
            if(infoWidth < 5)
                infoWidth = 5;    //ensure visible
        }
        else
            showProperties = showLegend = false;
    }

    //calculate the url of the inner pages
    //
    String srcToolbar = showToolbar ? ( "src=\"" + vpath + "toolbar.jsp?LOCALE=" + locale + "\"" ) : "";
    String srcStatusbar = showStatusbar ? ( "src=\"" + vpath + "statusbar.jsp?LOCALE=" + locale + "\"" ) : "";
    String srcTaskFrame = showTaskPane? ("src=\"" + vpath + "taskframe.jsp?TASK=" + taskPaneUrl + "&WEBLAYOUT=" + URLEncoder.encode(webLayoutDefinition, "UTF-8") + "&DWF=" + (forDwf!=0? "1": "0") + "&SESSION=" + (sessionId != ""? sessionId: "") + "&LOCALE=" + locale + "\"") : "";
    String srcTaskBar = "src=\"" + vpath + "taskbar.jsp?LOCALE=" + locale + "\"";

    //view center
    //
    MgPoint ptCenter = webLayout.GetCenter();
    String center = "null";
    if(ptCenter != null)
    {
        MgCoordinate coord = ptCenter.GetCoordinate();
        Object[] formatArgs = { Double.toString(coord.GetX()), Double.toString(coord.GetY()) };
        center = MessageFormat.format("new Point({0}, {1})", formatArgs);
    }

    //Process commands and declare command objects
    //
    MgWebCommandCollection commands = webLayout.GetCommands();
    String cmdObjects = "";
    String cmdObject = "";
    int navCmdIndex = 0;
    int searchCmdIndex = 0;
    int measureCmdIndex = 0;
    int printCmdIndex = 0;
    int scriptCmdIndex = 0;
    String userCode = "";
    String userCodeCalls = "\nswitch(funcIndex)\n{\n";
    int selAwareCmdCount = 0;
    String selAwareCmds = "";

    for(int i = 0; i < commands.GetCount(); i++)
    {
        MgWebCommand cmd = commands.GetItem(i);
        if(!cmd.IsUsed())
            continue;
        int tgtViewer = cmd.GetTargetViewerType();
        if((tgtViewer == MgWebTargetViewerType.Dwf) != (forDwf == 1) && (tgtViewer != MgWebTargetViewerType.All))
            continue;
        String name = cmd.GetName();
        int action = cmd.GetAction();
        if (action == MgWebActions.Search)
        {
            MgWebSearchCommand searchCmd = (MgWebSearchCommand)cmd;

            // create the column objects
            String cols = "var resCols" + searchCmdIndex + " = new Array();\n";
            if(searchCmd.GetResultColumnCount() > 0)
            {
                for(int j = 0; j < searchCmd.GetResultColumnCount(); j++)
                {
                    Object[] formatArgs = { new Integer(searchCmdIndex), new Integer(j), StrEscape(searchCmd.GetColumnDisplayNameAt(j)), StrEscape(searchCmd.GetColumnPropertyNameAt(j)) };
                    String col = MessageFormat.format("resCols{0,number,integer}[{1,number,integer}] = new ResultColumn(\"{2}\", \"{3}\");\n", formatArgs);
                    cols += col;
                }
            }
            cmdObjects += cols;

            // declare a new search command object
            Object[] formatArgs = { new Integer(i),
                                    StrEscape(name),
                                    StrEscape(searchCmd.GetLabel()),
                                    new Integer(action),
                                    searchCmd.GetIconUrl(),
                                    searchCmd.GetDisabledIconUrl(),
                                    StrEscape(searchCmd.GetTooltip()),
                                    StrEscape(searchCmd.GetDescription()),
                                    searchCmd.GetLayer(),
                                    StrEscape(searchCmd.GetPrompt()),
                                    new Integer(searchCmdIndex),
                                    StrEscape(searchCmd.GetFilter()),
                                    new Integer(searchCmd.GetMatchLimit()),
                                    new Integer(searchCmd.GetTarget()),
                                    searchCmd.GetTargetName() };
            cmdObject = MessageFormat.format("commands[{0,number,integer}] = new SearchCommand(\"{1}\", \"{2}\", {3,number,integer}, \"{4}\", \"{5}\", \"{6}\", \"{7}\", \"{8}\", \"{9}\", resCols{10}, \"{11}\", {12,number,integer}, {13,number,integer}, \"{14}\");\n", formatArgs);

            searchCmdIndex++;
        }
        else if(action == MgWebActions.InvokeUrl)
        {
            MgWebInvokeUrlCommand invokeUrlCmd = (MgWebInvokeUrlCommand)cmd;

            // create the parameter objects
            String paramObjs = "var navParams" + navCmdIndex + " = new Array();\n";
            String layers = "var layers" + navCmdIndex + " = new Array();\n";
            if(invokeUrlCmd.GetParameterCount() > 0)
            {
                for(int j = 0; j < invokeUrlCmd.GetParameterCount(); j++)
                {
                    Object[] formatArgs = { new Integer(navCmdIndex), new Integer(j), invokeUrlCmd.GetParameterNameAt(j), invokeUrlCmd.GetParameterValueAt(j) };
                    String param = MessageFormat.format("navParams{0,number,integer}[{1,number,integer}] = new NavParam(\"{2}\", \"{3}\");\n", formatArgs);
                    paramObjs = paramObjs + param;
                }
                for( int j = 0;  j < invokeUrlCmd.GetLayerCount(); j++ )
                {
                    Object[] formatArgs = { new Integer(navCmdIndex), new Integer(j), invokeUrlCmd.GetLayerNameAt(j) };
                    String layer = MessageFormat.format("layers{0,number,integer}[{1,number,integer}] = \"{2}\";\n", formatArgs);
                    layers = layers + layer;
                }
            }
            cmdObjects = cmdObjects + paramObjs + layers;

            if(invokeUrlCmd.DisabledIfSelectionEmpty() || invokeUrlCmd.GetLayerCount() > 0)
            {
                Object[] formatArgs = { new Integer(selAwareCmdCount), new Integer(i) };
                selAwareCmds = selAwareCmds + MessageFormat.format("selectionAwareCmds[{0,number,integer}] = {1,number,integer};\n", formatArgs);
                selAwareCmdCount ++;
            }

            // declare a new invokeurl command object
            Object[] formatArgs = { new Integer(i),
                                    StrEscape(name),
                                    new Integer(action),
                                    invokeUrlCmd.GetIconUrl(),
                                    invokeUrlCmd.GetDisabledIconUrl(),
                                    StrEscape( invokeUrlCmd.GetTooltip()),
                                    StrEscape( invokeUrlCmd.GetDescription()),
                                    invokeUrlCmd.GetUrl(),
                                    new Integer(navCmdIndex),
                                    invokeUrlCmd.DisabledIfSelectionEmpty() ? "true" : "false",
                                    new Integer(navCmdIndex),
                                    new Integer(invokeUrlCmd.GetTarget()),
                                    invokeUrlCmd.GetTargetName() };
            cmdObject = MessageFormat.format("commands[{0,number,integer}] = new InvokeUrlCommand(\"{1}\", {2,number,integer}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", \"{7}\", navParams{8,number,integer}, {9}, layers{10,number,integer}, {11,number,integer}, \"{12}\");\n", formatArgs);
            navCmdIndex++;
        }
        else if(action == MgWebActions.Buffer || action == MgWebActions.SelectWithin ||
            action == MgWebActions.Measure || action == MgWebActions.ViewOptions || action == MgWebActions.GetPrintablePage)
        {
            MgWebUiTargetCommand targetCmd = (MgWebUiTargetCommand)cmd;

            if(action == MgWebActions.Measure)
            {
                if(measureCmdIndex != 0)
                    throw new Exception(MgLocalizer.GetString("ALREADYINMEASURE", locale));
                measureCmdIndex = i;
            }

            // declare a new ui target command object
            Object[] formatArgs = { new Integer(i),
                                    StrEscape( name ),
                                    new Integer( action ),
                                    targetCmd.GetIconUrl(),
                                    targetCmd.GetDisabledIconUrl(),
                                    StrEscape(targetCmd.GetTooltip()),
                                    StrEscape(targetCmd.GetDescription()),
                                    new Integer( targetCmd.GetTarget()),
                                    targetCmd.GetTargetName() };
            cmdObject = MessageFormat.format("commands[{0,number,integer}] = new UiTargetCommand(\"{1}\", {2,number,integer}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", {7,number,integer}, \"{8}\");\n", formatArgs);
        }
        else if(action == MgWebActions.Help)
        {
            MgWebHelpCommand helpCmd = (MgWebHelpCommand)cmd;

            // declare a new help  command object
            Object[] formatArgs = { new Integer(i),
                                    StrEscape(name),
                                    new Integer(action),
                                    helpCmd.GetIconUrl(),
                                    helpCmd.GetDisabledIconUrl(),
                                    StrEscape(helpCmd.GetTooltip()),
                                    StrEscape(helpCmd.GetDescription()),
                                    helpCmd.GetUrl(),
                                    new Integer(helpCmd.GetTarget()),
                                    helpCmd.GetTargetName() };
            cmdObject = MessageFormat.format("commands[{0,number,integer}] = new HelpCommand(\"{1}\", {2,number,integer}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", \"{7}\", {8,number,integer}, \"{9}\");\n", formatArgs);
        }
        else if(action == MgWebActions.PrintMap)
        {
            MgWebPrintCommand printCmd = (MgWebPrintCommand)cmd;

            // declare the print layouts
            String layouts = "var layouts" + printCmdIndex + " = new Array();\n";
            for(int j = 0; j < printCmd.GetPrintLayoutCount(); j++)
            {
                String layout = "";
                Object[] formatArgs = { new Integer(printCmdIndex), new Integer(j), printCmd.GetPrintLayoutAt(j) };
                layout = MessageFormat.format("layouts{0,number,integer}[{1,number,integer}] = \"{2}\";\n", formatArgs);
                layouts = layouts + layout;
            }
            cmdObjects = cmdObjects + layouts;

            // declare a new print command object
            Object[] formatArgs = { new Integer(i),
                                    StrEscape( name ),
                                    new Integer(action),
                                    printCmd.GetIconUrl(),
                                    printCmd.GetDisabledIconUrl(),
                                    StrEscape( printCmd.GetTooltip()),
                                    StrEscape( printCmd.GetDescription()),
                                    new Integer(printCmdIndex) };
            cmdObject = MessageFormat.format("commands[{0,number,integer}] = new PrintCommand(\"{1}\", {2,number,integer}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", layouts{7,number,integer});\n", formatArgs );
            printCmdIndex++;
        }
        else if(action == MgWebActions.InvokeScript)
        {
            MgWebInvokeScriptCommand invokeScriptCmd = (MgWebInvokeScriptCommand)cmd;

            // declare a new basic command object
            Object[] formatArgs = { new Integer(i),
                                    StrEscape(name),
                                    new Integer(action),
                                    invokeScriptCmd.GetIconUrl(),
                                    invokeScriptCmd.GetDisabledIconUrl(),
                                    StrEscape( invokeScriptCmd.GetTooltip()),
                                    StrEscape( invokeScriptCmd.GetDescription()),
                                    new Integer( scriptCmdIndex ) };
            cmdObject = MessageFormat.format("commands[{0,number,integer}] = new InvokeScriptCommand(\"{1}\", {2,number,integer}, \"{3}\", \"{4}\", \"{5}\", \"{6}\", {7,number,integer});\n", formatArgs);

            userCode = userCode + "\nfunction UserFunc" + scriptCmdIndex + "()\n{\n" + invokeScriptCmd.GetCode() + "\n}\n";
            Object[] formatArgs2 = { new Integer(scriptCmdIndex), new Integer(scriptCmdIndex) };
            userCodeCalls = userCodeCalls + MessageFormat.format("case {0,number,integer}: UserFunc{0,number,integer}(); break;\n", formatArgs2);

            scriptCmdIndex++;
        }
        else
        {
            // declare a new basic command object
            Object[] formatArgs = { new Integer(i),
                                    name,
                                    new Integer(action),
                                    cmd.GetIconUrl(),
                                    cmd.GetDisabledIconUrl(),
                                    StrEscape(cmd.GetTooltip()),
                                    StrEscape(cmd.GetDescription()) };
            cmdObject = MessageFormat.format("commands[{0,number,integer}] = new BasicCommand(\"{1}\", {2,number,integer}, \"{3}\", \"{4}\", \"{5}\", \"{6}\");\n", formatArgs);
        }
        cmdObjects = cmdObjects + cmdObject;
        cmds.put(name, new Integer(i));
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
    if(ctxMenu.IsVisible())
        ctxMenuDef = DeclareUiItems(ctxMenu, "ctxMenuItems");
    else
        ctxMenuDef = "";

    //task items texts
    String taskItemTexts = "";
    MgWebWidgetCollection taskButtons = taskBar.GetTaskButtons();
    for(int i = 0; i < 4; i ++)
    {
        MgWebTaskBarWidget btn = (MgWebTaskBarWidget)taskButtons.GetWidget(i);
        if(i > 0)
            taskItemTexts += ",";
        taskItemTexts += "\"" + StrEscape(btn.GetName()) + "\"," +
                            "\"" + StrEscape(btn.GetTooltip()) + "\"," +
                            "\"" + StrEscape(btn.GetDescription()) + "\"," +
                            "\"" + StrEscape(btn.GetIconUrl()) + "\"," +
                            "\"" + StrEscape(btn.GetDisabledIconUrl()) + "\"";
    }

    //transmit the session to the map pane if one was specified to this request
    String sessionParam = "";
    if(orgSessionId != "")
        sessionParam = "&SESSION=" + orgSessionId;

    //load the frameset template and format it
    String frameset = "";
    String viewerType = forDwf != 0? "DWF": "HTML";

    if(showTaskBar)
    {
        String frameSetTempl = LoadTemplate("/viewerfiles/framesettaskbar.templ");
        String[] vals = {
                        String.valueOf(statusbarHeight),
                        String.valueOf(taskPaneWidth),
                        String.valueOf(toolbarHeight),
                        srcToolbar,
                        vpath + "mapframe.jsp",
                        mapDefinitionUrl,
                        viewerType,
                        showLegend? "1": "0",
                        showProperties? "1": "0",
                        String.valueOf(infoWidth),
                        locale,
                        String.valueOf(webLayout.GetHyperlinkTarget()),
                        webLayout.GetHyperlinkTargetFrame(),
                        webLayout.IsZoomControlVisible()? "1": "0",
                        sessionParam,
                        vpath + "formframe.jsp",
                        String.valueOf(toolbarHeight),
                        srcTaskBar,
                        srcTaskFrame,
                        srcStatusbar
                        };

        frameset = Substitute(frameSetTempl, vals);
    }
    else
    {
        String frameSetTempl = LoadTemplate("/viewerfiles/framesetnotaskbar.templ");
        String[] vals = {
                        String.valueOf(toolbarHeight),
                        String.valueOf(statusbarHeight),
                        srcToolbar,
                        String.valueOf(taskPaneWidth),
                        vpath + "mapframe.jsp",
                        mapDefinitionUrl,
                        viewerType,
                        showLegend? "1": "0",
                        showProperties? "1": "0",
                        String.valueOf(infoWidth),
                        locale,
                        String.valueOf(webLayout.GetHyperlinkTarget()),
                        webLayout.GetHyperlinkTargetFrame(),
                        webLayout.IsZoomControlVisible()? "1": "0",
                        sessionParam,
                        srcTaskFrame,
                        vpath + "formframe.jsp",
                        srcStatusbar
                        };

        frameset = Substitute(frameSetTempl, vals);
    }

    String homePageUrl = taskPaneUrl;
    if(homePageUrl.substring(0, 7).compareToIgnoreCase("http://") != 0)
        homePageUrl = vpath + homePageUrl;

    //load the HTML template and format it
    //
    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/mainframe.templ"), locale, GetClientOS(request));

    String int0 = "0";
    String int1 = "1";
    String[] vals = {
                      webLayout.GetTitle(),
                      GetRootVirtualFolder(request) + "/mapagent/mapagent.fcgi",
                      webLayout.GetEnablePingServer()? int1 : int0,
                      locale,
                      showToolbar ? int1 : int0,
                      showStatusbar ? int1 : int0,
                      showTaskPane ? int1 : int0,
                      !showTaskPane ? int0 : (showTaskBar ? int1 : int0),
                      homePageUrl,
                      defHome? "1" : "0",
                      webLayoutDefinition,
                      mapDef,
                      String.valueOf(taskPaneWidth),
                      center,
                      String.valueOf(webLayout.GetScale()),
                      StrEscape(title),
                      (forDwf == 1)? "1" : "0",
                      cmdObjects,
                      toolbarDef,
                      taskListDef,
                      ctxMenuDef,
                      userCode,
                      taskItemTexts,
                      selAwareCmds,
                      vpath + "printablepageui.jsp",
                      vpath + "measureui.jsp",
                      vpath + "searchprompt.jsp",
                      vpath + "bufferui.jsp",
                      vpath + "selectwithinui.jsp",
                      userCodeCalls,
                      vpath + "viewoptions.jsp",
                      frameset
                     };

    templ = Substitute(templ, vals);

    response.getWriter().write(templ);
    response.setContentLength(templ.length());
}
catch(MgUserNotFoundException e)
{
    RequestAuthentication(response);
    return;
}
catch(MgUnauthorizedAccessException e)
{
    RequestAuthentication(response);
    return;
}
catch(MgAuthenticationFailedException e)
{
    RequestAuthentication(response);
    return;
}
catch(MgException e)
{
    String msg = e.GetMessage();
    response.getWriter().write(msg);
    response.setContentLength(msg.length());
}
catch(Exception ne)
{
    String msg = ne.getLocalizedMessage();
    response.getWriter().write(msg);
    response.setContentLength(msg.length());
}
}
%>

<%!
void RequestAuthentication(HttpServletResponse response) throws IOException
{
    response.addHeader("WWW-Authenticate", "Basic realm=\"" + product + "\"");
    response.setStatus( 401 );
    String msg = MgLocalizer.GetString("NEEDLOGIN", locale);
    response.getWriter().write(msg);
    response.setContentLength(msg.length());
    response.flushBuffer();
}

String DeclareUiItems(MgWebWidgetCollection coll, String varname) throws MgException
{
    String def = "";

    if(coll != null)
    {
        for(int i = 0, j = 0; i < coll.GetCount(); i++)
        {
            MgWebWidget item = coll.GetWidget(i);
            int it = item.GetType();
            if (it == MgWebWidgetType.Separator)
            {
                Object[] formatArgs = { varname, new Integer(j++) };
                def = def + MessageFormat.format("{0}[{1,number,integer}] = new UiItem(\"\");\n", formatArgs );
            }
            else if ( it == MgWebWidgetType.Command && item instanceof MgWebCommandWidget )
            {
                MgWebCommand cmd = ((MgWebCommandWidget)item).GetCommand();
                Integer cmdIndex = (Integer)cmds.get(cmd.GetName());
                if(cmdIndex == null)
                    continue;
                Object[] formatArgs = { varname, new Integer(j++), StrEscape(cmd.GetLabel()), cmdIndex };
                def = def + MessageFormat.format("{0}[{1,number,integer}] = new CommandItem(\"{2}\", {3,number,integer});\n", formatArgs);
            }
            else
            {
                curFlyout++;
                String subVarname = "flyoutDef" + curFlyout;
                String htmlName = "FlyoutDiv" + curFlyout;
                Object[] formatArgs1 = { subVarname };
                def = def + MessageFormat.format("var {0} = new Array()\n", formatArgs1);
                def = def + DeclareUiItems(((MgWebFlyoutWidget) item).GetSubItems(), subVarname);
                Object[] formatArgs2 = { varname, new Integer(j++), StrEscape( ((MgWebFlyoutWidget) item).GetLabel() ), subVarname, StrEscape( htmlName ), ((MgWebFlyoutWidget) item).GetIconUrl() };
                def = def + MessageFormat.format("{0}[{1,number,integer}] = new FlyoutItem(\"{2}\", {3}, \"{4}\", \"{5}\");\n", formatArgs2);
            }
        }
    }
    return def;
}

void GetRequestParameters(HttpServletRequest request)
{
    webLayoutDefinition = request.getParameter("WEBLAYOUT");
    if (webLayoutDefinition == null)
        webLayoutDefinition = "";

    String localeParam = request.getParameter("LOCALE");
    if (localeParam != null && localeParam.length() > 0)
    {
        locale = localeParam;
    }
    else
    {
        locale = GetDefaultLocale();
    }
    sessionId = request.getParameter("SESSION");
    if (sessionId != null && sessionId.length() > 0)
    {
        sessionId = request.getParameter("SESSION");
        orgSessionId = sessionId;
    }
    else
    {
        username = request.getParameter("USERNAME");
        if (username != null && username.length() > 0)
        {
            password = request.getParameter( "PASSWORD");
            if(password == null)
                password = "";
            return;
        }

        String auth = request.getHeader("authorization");
        try {
            String[] ss = auth.split(" ");
            String dec = new String(MgBase64.decode(ss[1]));
            String[] login = dec.split(":");
            username = login[0];
            password = login[1];
        }
        catch(Exception e) {
        }
    }
}

String StrEscape(String str)
{
    return StrEscape(str, false);
}

String StrEscape(String str, boolean single)
{
    String org = single ? "'" : "\"";
    String rep = single ? "\\\\'": "\\\\\"";

    return str.replaceAll(org, rep);
}

String str_replace(String[] args, Object[] vals, String format)
{
    String formattedString = new String(format);

    int numReplacements = args.length;
    for (int i = 0; i < numReplacements; i++)
        formattedString = formattedString.replaceFirst(args[i], vals[i].toString());

    return formattedString;
}
%>
