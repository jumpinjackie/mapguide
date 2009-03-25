<?php

//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

include 'common.php';
include 'constants.php';
include 'product.php';

$webLayoutDefinition = '';
$locale = '';
$sessionId = '';
$orgSessionId = '';
$username = '';
$password = '';
$curFlyout = 0;
$cmds = array();

function BuildViewer($forDwf = true)
{
    global $debug, $webLayoutDefinition, $cmds, $locale;
    global $sessionId, $username, $password, $orgSessionId;
    global $product;

    SetLocalizedFilesPath(GetLocalizationPath());

    try
    {
        // Initialize web tier with the site configuration file.
        InitializeWebTier();

        //fetch the parameters for this request
        //
        GetRequestParameters();

        //Open a connection with the server
        //
        $createSession = true;

        $cred = new MgUserInformation();
        if($sessionId != '')
        {
            $cred->SetMgSessionId($sessionId);
            $createSession = false;
        }
        else if($username != '')
        {
            $cred->SetMgUsernamePassword($username, $password);
        }
        else
        {
            requestAuthentication();
            return;
        }

        $site = new MgSiteConnection();
        $cred->SetLocale($locale);

        $cred->SetClientIp(GetClientIp());
        $cred->SetClientAgent(GetClientAgent());

        $site->Open($cred);

        if($createSession)
        {
            $site1 = $site->GetSite();
            $sessionId = $site1->CreateSession();
            if($forDwf == false)
                $orgSessionId = $sessionId;
        }

        //Get a MgWebLayout object initialized with the specified web layout definition
        //
        $webLayout = null;
        try
        {
            $resourceSrvc = $site->CreateService(MgServiceType::ResourceService);
            $webLayoutId = new MgResourceIdentifier($webLayoutDefinition);
            $webLayout = new MgWebLayout($resourceSrvc, $webLayoutId);
        }
        catch(MgUnauthorizedAccessException $e)
        {
            header('WWW-Authenticate: Basic realm="' . $product . '"');
            header('HTTP/1.1 401 Unauthorized');
            header("Status: 401 " . GetLocalizedString("ACCESSDENIED", $locale));
            echo GetLocalizedString("NEEDLOGIN", $locale);
            return;
        }
        catch(MgException $e)
        {
            $shortError = $e->GetMessage();
            $longError = $e->GetDetails();
            header("HTTP/1.1 559 ");
            header("Status: 559 {$shortError}");
            echo "<html>\n<body>\n";
            echo "<h2>{$shortError}</h2>\n{$longError}";
            echo "</body>\n</html>\n";
            return;
        }

        //calculate the size of the variable elements of the viewer
        //
        $toolBar = $webLayout->GetToolBar();
        $statusBar = $webLayout->GetStatusBar();
        $taskPane = $webLayout->GetTaskPane();
        $infoPane = $webLayout->GetInformationPane();
        $taskBar = $taskPane->GetTaskBar();
        $mapDef = $webLayout->GetMapDefinition();

        $showTaskPane = $taskPane->IsVisible();
        $showTaskBar = $taskBar->IsVisible();
        $showStatusbar = $statusBar->IsVisible();
        $showToolbar = $toolBar->IsVisible();

        $taskPaneWidth = $taskPane->GetWidth();
        $toolbarHeight = 30;
        $statusbarHeight = 26;

        $taskWidth = $showTaskPane? $taskPaneWidth: 0;
        $toolbarHeight = $showToolbar? $toolbarHeight: 0;
        $statusbarHeight = $showStatusbar? $statusbarHeight: 0;
		
		//Encode the initial url so that it does not trip any sub-frames (especially if this url has parameters)
        $taskPaneUrl = urlencode($taskPane->GetInitialTaskUrl());
        $vpath = GetSurroundVirtualPath();
        $defHome = false;
        if($taskPaneUrl == "") {
            $taskPaneUrl = "gettingstarted.php";
            $defHome = true;
        }
        $mapDefinitionUrl = urlencode($mapDef);
        $title = $webLayout->GetTitle();

        $showLegend = $infoPane->IsLegendBandVisible();
        $showProperties = $infoPane->IsPropertiesBandVisible();

        if($showLegend || $showProperties)
        {
            if($infoPane->IsVisible())
            {
                $infoWidth = $infoPane->GetWidth();
                if($infoWidth < 5)
                    $infoWidth = 5;    //ensure visible
            }
            else
            {
                $showProperties = $showLegend = false;
                $infoWidth = 0;
            }
        }
        else
            $infoWidth = 0;

        //calculate the url of the inner pages
        //
        $srcToolbar = $showToolbar? ('src="' . $vpath . 'toolbar.php?LOCALE=' . $locale . '"'): '';
        $srcStatusbar = $showStatusbar? ('src="' . $vpath . 'statusbar.php?LOCALE=' . $locale . '"') : "";
        $srcTaskFrame = $showTaskPane? ('src="' . $vpath . 'taskframe.php?TASK=' . $taskPaneUrl . '&WEBLAYOUT=' . urlencode($webLayoutDefinition) . '&DWF=' . ($forDwf? "1": "0") . '&SESSION=' . ($sessionId != ""? $sessionId: "") . '&LOCALE=' . $locale . '"') : '';
        $srcTaskBar = 'src="' . $vpath . 'taskbar.php?LOCALE=' . $locale . '"';

        //view center
        //
        $ptCenter = $webLayout->GetCenter();
        if($ptCenter == null)
        $center = "null";
        else
        {
            $coord = $ptCenter->GetCoordinate();
            $center = sprintf("new Point(%f, %f)", $coord->GetX(), $coord->GetY());
        }

        //Process commands and declare command objects
        //
        $commands = $webLayout->GetCommands();
        $cmdObjects = "";
        $cmdObject = "";
        $navCmdIndex = 0;
        $searchCmdIndex = 0;
        $measureCmdIndex = 0;
        $printCmdIndex = 0;
        $scriptCmdIndex = 0;
        $userCode = "";
        $userCodeCalls = "\nswitch(funcIndex)\n{\n";
        $selAwareCmdCount = 0;
        $selAwareCmds = "";

        for($i = 0; $i < $commands->GetCount(); $i++)
        {
            $cmd = $commands->GetItem($i);
            if(!$cmd->IsUsed())
                continue;
            $tgtViewer = $cmd->GetTargetViewerType();
            if(($tgtViewer == MgWebTargetViewerType::Dwf) != ($forDwf == true) && ($tgtViewer != MgWebTargetViewerType::All))
                continue;
            $name = $cmd->GetName();
            $action = $cmd->GetAction();
            if($action == MgWebActions::Search)
            {
                $cols = "var resCols" . $searchCmdIndex . " = new Array();\n";
                if($cmd->GetResultColumnCount() > 0)
                {
                    for($j = 0; $j < $cmd->GetResultColumnCount(); $j++)
                    {
                        $col = sprintf("resCols%d[%d] = new ResultColumn(\"%s\", \"%s\");\n", $searchCmdIndex, $j, StrEscape($cmd->GetColumnDisplayNameAt($j)), StrEscape($cmd->GetColumnPropertyNameAt($j)));
                        $cols = $cols . $col;
                    }
                }
                $cmdObjects = $cmdObjects . $cols;

                // declare a new search command object
                $cmdObject = sprintf("commands[%d] = new SearchCommand(\"%s\", \"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", resCols%d, \"%s\", %d, %d, \"%s\");\n",
                                        $i, StrEscape($name),
                                        StrEscape($cmd->GetLabel()),
                                        $cmd->GetAction(),
                                        $cmd->GetIconUrl(),
                                        $cmd->GetDisabledIconUrl(),
                                        StrEscape($cmd->GetTooltip()),
                                        StrEscape($cmd->GetDescription()),
                                        $cmd->GetLayer(),
                                        StrEscape($cmd->GetPrompt()),
                                        $searchCmdIndex,
                                        StrEscape($cmd->GetFilter()),
                                        $cmd->GetMatchLimit(),
                                        $cmd->GetTarget(),
                                        $cmd->GetTargetName());

                $searchCmdIndex++;
            }
            else if($action == MgWebActions::InvokeUrl)
            {
                // create the parameter objects
                $params = "var navParams" . $navCmdIndex . " = new Array();\n";
                $layers = "var layers" . $navCmdIndex . " = new Array();\n";
                if($cmd->GetParameterCount() > 0)
                {
                    for($j = 0; $j < $cmd->GetParameterCount(); $j++)
                    {
                        $param = sprintf("navParams%d[%d] = new NavParam(\"%s\", \"%s\");\n", $navCmdIndex, $j, $cmd->GetParameterNameAt($j), $cmd->GetParameterValueAt($j));
                        $params = $params . $param;
                    }
                    for($j = 0; $j < $cmd->GetLayerCount(); $j++)
                    {
                        $layer = sprintf("layers%d[%d] = \"%s\";\n", $navCmdIndex, $j, $cmd->GetLayerNameAt($j));
                        $layers = $layers . $layer;
                    }
                }
                $cmdObjects = $cmdObjects . $params . $layers;
                if($cmd->DisabledIfSelectionEmpty() || $cmd->GetLayerCount() > 0)
                {
                    $selAwareCmds = $selAwareCmds . sprintf("selectionAwareCmds[%d] = %d;\n", $selAwareCmdCount, $i);
                    $selAwareCmdCount ++;
                }

                // declare a new invokeurl command object
                $cmdObject = sprintf("commands[%d] = new InvokeUrlCommand(\"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", navParams%d, %s, layers%d, %d, \"%s\");\n",
                                        $i,
                                        StrEscape($name),
                                        $cmd->GetAction(),
                                        $cmd->GetIconUrl(),
                                        $cmd->GetDisabledIconUrl(),
                                        StrEscape($cmd->GetTooltip()),
                                        StrEscape($cmd->GetDescription()),
                                        $cmd->GetUrl(),
                                        $navCmdIndex,
                                        $cmd->DisabledIfSelectionEmpty()?"true":"false",
                                        $navCmdIndex,
                                        $cmd->GetTarget(),
                                        $cmd->GetTargetName());

                $navCmdIndex++;
            }
            else if($action == MgWebActions::Buffer || $action == MgWebActions::SelectWithin ||
            $action == MgWebActions::Measure || $action == MgWebActions::ViewOptions || $action == MgWebActions::GetPrintablePage)
            {
                if($action == MgWebActions::Measure)
                {
                    if($measureCmdIndex != 0)
                    throw new Exception(GetLocalizedString("ALREADYINMEASURE", $locale));
                    $measureCmdIndex = $i;
                }
                else if($action == MgWebActions::SelectWithin)
                {
                    $selAwareCmds = $selAwareCmds . sprintf("selectionAwareCmds[%d] = %d;\n", $selAwareCmdCount, $i);
                    $selAwareCmdCount ++;
                }

                // declare a new ui target command object
                $cmdObject = sprintf("commands[%d] = new UiTargetCommand(\"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\", %d, \"%s\");\n",
                                        $i,
                                        StrEscape($name),
                                        $cmd->GetAction(),
                                        $cmd->GetIconUrl(),
                                        $cmd->GetDisabledIconUrl(),
                                        StrEscape($cmd->GetTooltip()),
                                        StrEscape($cmd->GetDescription()),
                                        $cmd->GetTarget(),
                                        $cmd->GetTargetName());
            }
            else if($action == MgWebActions::Help)
            {
                // declare a new help  command object
                $cmdObject = sprintf("commands[%d] = new HelpCommand(\"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %d, \"%s\");\n",
                                        $i,
                                        StrEscape($name),
                                        $cmd->GetAction(),
                                        $cmd->GetIconUrl(),
                                        $cmd->GetDisabledIconUrl(),
                                        StrEscape($cmd->GetTooltip()),
                                        StrEscape($cmd->GetDescription()),
                                        $cmd->GetUrl(),
                                        $cmd->GetTarget(),
                                        $cmd->GetTargetName());
            }
            else if($action == MgWebActions::PrintMap)
            {
                // declare the print layouts
                $layouts = "var layouts" . $printCmdIndex . " = new Array();\n";
                for($j = 0; $j < $cmd->GetPrintLayoutCount(); $j++)
                {
                    $layout = "";
                    $layout = sprintf("layouts%d[%d] = \"%s\";\n", $printCmdIndex, $j, $cmd->GetPrintLayoutAt($j));
                    $layouts = $layouts . $layout;
                }
                $cmdObjects = $cmdObjects . $layouts;

                // declare a new print command object
                $cmdObject = sprintf("commands[%d] = new PrintCommand(\"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\", layouts%d);\n",
                                        $i,
                                        StrEscape($name),
                                        $cmd->GetAction(),
                                        $cmd->GetIconUrl(),
                                        $cmd->GetDisabledIconUrl(),
                                        StrEscape($cmd->GetTooltip()),
                                        StrEscape($cmd->GetDescription()),
                $printCmdIndex);

                $printCmdIndex++;
            }
            else if($action == MgWebActions::InvokeScript)
            {
                // declare a new basic command object
                $cmdObject = sprintf("commands[%d] = new InvokeScriptCommand(\"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\", %d);\n",
                                        $i,
                                        StrEscape($name),
                                        $cmd->GetAction(),
                                        $cmd->GetIconUrl(),
                                        $cmd->GetDisabledIconUrl(),
                                        StrEscape($cmd->GetTooltip()),
                                        StrEscape($cmd->GetDescription()),
                $scriptCmdIndex);

                $userCode = $userCode . "\nfunction UserFunc" . $scriptCmdIndex . "()\n{\n". $cmd->GetCode() . "\n}\n";
                $userCodeCalls = $userCodeCalls . sprintf("case %d: UserFunc%d(); break;\n", $scriptCmdIndex, $scriptCmdIndex);

                $scriptCmdIndex++;
            }
            else
            {
                // declare a new basic command object
                $cmdObject = sprintf("commands[%d] = new BasicCommand(\"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\");\n",
                                        $i,
                                        $name,
                                        $cmd->GetAction(),
                                        $cmd->GetIconUrl(),
                                        $cmd->GetDisabledIconUrl(),
                                        StrEscape($cmd->GetTooltip()),
                                        StrEscape($cmd->GetDescription()));
            }
            $cmdObjects = $cmdObjects . $cmdObject;
            $cmds[$name] = $i;
        }
        $userCodeCalls = $userCodeCalls . "\n}\n";

        //Declare toolbar items
        //
        $toolbarDef = DeclareUiItems($toolBar->GetWidgets(), "toolbarItems");

        //Declare task items
        $taskListDef = DeclareUiItems($taskBar->GetTaskList(), "taskItems");

        //Declare context menu items
        $ctxMenu = $webLayout->GetContextMenu();
        if($ctxMenu->IsVisible())
            $ctxMenuDef = DeclareUiItems($ctxMenu, "ctxMenuItems");
        else
            $ctxMenuDef = "";

        //task items texts
        //
        $taskItemTexts = "";
        $taskButtons = $taskBar->GetTaskButtons();
        for($i = 0; $i < 4; $i ++)
        {
            $btn = $taskButtons->GetWidget($i);
            if($i > 0)
                $taskItemTexts = $taskItemTexts . ",";
            $taskItemTexts = $taskItemTexts .
                                '"' . StrEscape($btn->GetName()) . '",' .
                                '"' . StrEscape($btn->GetTooltip()). '",' .
                                '"' . StrEscape($btn->GetDescription()). '",' .
                                '"' . StrEscape($btn->GetIconUrl()). '",' .
                                '"' . StrEscape($btn->GetDisabledIconUrl()). '"';
        }

        //transmit the session to the map pane if one was specified to this request
        if($orgSessionId != "")
            $sessionParam = "&SESSION=" . $orgSessionId;
        else
            $sessionParam = "";

        //load the frameset template and format it
        $frameset = "";

        $viewerType = $forDwf? "DWF": "HTML";

        if($showTaskBar)
        {
            $frameSetTempl = file_get_contents("../viewerfiles/framesettaskbar.templ");
            $frameset = sprintf($frameSetTempl,
                                    $statusbarHeight,
                                    $taskWidth, $toolbarHeight,
                                    $srcToolbar,
                                    $vpath . "mapframe.php",
                                    $mapDefinitionUrl,
                                    $viewerType,
                                    $showLegend? 1: 0, $showProperties? 1: 0, $infoWidth,
                                    $locale,
                                    $webLayout->GetHyperlinkTarget(), $webLayout->GetHyperlinkTargetFrame(),
                                    $webLayout->IsZoomControlVisible()? 1: 0,
                                    $sessionParam,
                                    $vpath . "formframe.php",
                                    $toolbarHeight+1, $srcTaskBar,
                                    $srcTaskFrame,
                                    $srcStatusbar);
        }
        else
        {
            $frameSetTempl = file_get_contents("../viewerfiles/framesetnotaskbar.templ");
            $frameset = sprintf($frameSetTempl,
                                    $toolbarHeight,
                                    $statusbarHeight,
                                    $srcToolbar,
                                    $taskWidth,
                                    $vpath . "mapframe.php",
                                    $mapDefinitionUrl,
                                    $viewerType,
                                    $showLegend? 1: 0,
                                    $showProperties? 1: 0,
                                    $infoWidth,
                                    $locale,
                                    $webLayout->GetHyperlinkTarget(),
                                    $webLayout->GetHyperlinkTargetFrame(),
                                    $webLayout->IsZoomControlVisible()? 1: 0,
                                    $sessionParam,
                                    $srcTaskFrame,
                                    $vpath . "formframe.php",
                                    $srcStatusbar);
        }

        $homePageUrl = $taskPaneUrl;
        if(strncasecmp($homePageUrl, "http://", 7) != 0)
            $homePageUrl = $vpath . $homePageUrl;

        //load the HTML template and format it
        //
        $templ = Localize(file_get_contents("../viewerfiles/mainframe.templ"), $locale, GetClientOS());
        print sprintf($templ,
                    $title,
                    $locale,
                    $showToolbar? 1: 0,
                    $showStatusbar? 1: 0,
                    $showTaskPane? 1: 0,
                    $showTaskPane? 0: ($showTaskBar? 1: 0),
                    $homePageUrl,
                    $defHome? "1": "0",
                    $webLayoutDefinition,
                    $mapDef,
                    $taskWidth,
                    $center,
                    $webLayout->GetScale(),
                    StrEscape($title),
                    $forDwf? "1": "0",
                    $cmdObjects,
                    $toolbarDef,
                    $taskListDef,
                    $ctxMenuDef,
                    $userCode,
                    $taskItemTexts,
                    $selAwareCmds,
                    $vpath . "printablepageui.php",
                    $vpath . "measureui.php",
                    $vpath . "searchprompt.php",
                    $vpath . "bufferui.php",
                    $vpath . "selectwithinui.php",
                    $userCodeCalls,
                    $vpath . "viewoptions.php",
                    $frameset);
    }
    catch(MgException $e)
    {
        echo $e->GetDetails();
    }
    catch(Exception $ne)
    {
        echo $ne->GetMessage();
    }
}

function DeclareUiItems($coll, $varname)
{
    global $cmds, $curFlyout;
    $def = "";

    if($coll != null)
    {
        for($i = 0, $j = 0; $i < $coll->GetCount(); $i++)
        {
            $item = $coll->GetWidget($i);
            $it = $item->GetType();
            if($it == MgWebWidgetType::Separator)
                $def = $def . sprintf("%s[%d] = new UiItem('');\n", $varname, $j++);
            else if($it == MgWebWidgetType::Command)
            {
                $cmd = $item->GetCommand();
                if(!isset($cmds[$cmd->GetName()]))
                    continue;
                $cmdIndex = $cmds[$cmd->GetName()];
                $def = $def . sprintf("%s[%d] = new CommandItem(\"%s\", %d);\n", $varname, $j++, StrEscape($cmd->GetLabel()), $cmdIndex);
            }
            else
            {
                $curFlyout++;
                $subVarname = "flyoutDef" . $curFlyout;
                $htmlName = "FlyoutDiv" . $curFlyout;
                $def = $def . sprintf("var %s = new Array()\n", $subVarname);
                $def = $def . DeclareUiItems($item->GetSubItems(), $subVarname);
                $def = $def . sprintf("%s[%d] = new FlyoutItem(\"%s\", %s, \"%s\", \"%s\");\n", $varname, $j++, StrEscape($item->GetLabel()), $subVarname, StrEscape($htmlName), $item->GetIconUrl());
            }
        }
    }
    return $def;
}

function GetParameters($params)
{
    global $debug, $webLayoutDefinition;
    global $sessionId, $username, $password, $orgSessionId, $locale;

    $webLayoutDefinition = $params['WEBLAYOUT'];

    if(isset($params['LOCALE']))
        $locale = $params['LOCALE'];
    else
        $locale = GetDefaultLocale();

    if(isset($params['SESSION']))
    {
        $sessionId = $params['SESSION'];
        $orgSessionId = $sessionId;
    }
    else
    {
        if(isset($params['USERNAME']))
        {
            $username = $params['USERNAME'];
            if(isset($params['PASSWORD']))
                $password = $params['PASSWORD'];
            return;
        }

        // No session, no credentials explicitely passed. Check for HTTP Auth user/passwd.  Under Apache CGI, the
        // PHP_AUTH_USER and PHP_AUTH_PW are not set.  However, the Apache admin may
        // have rewritten the authentication information to REMOTE_USER.  This is a
        // suggested approach from the Php.net website.

        // Has REMOTE_USER been rewritten?
        if (!isset($_SERVER['PHP_AUTH_USER']) && isset($_SERVER['REMOTE_USER']) &&
        preg_match('/Basic +(.*)$/i', $_SERVER['REMOTE_USER'], $matches))
        {
            list($name, $password) = explode(':', base64_decode($matches[1]));
            $_SERVER['PHP_AUTH_USER'] = strip_tags($name);
            $_SERVER['PHP_AUTH_PW']    = strip_tags($password);
        }


        // REMOTE_USER may also appear as REDIRECT_REMOTE_USER depending on CGI setup.
        //  Check for this as well.
        if (!isset($_SERVER['PHP_AUTH_USER']) && isset($_SERVER['REDIRECT_REMOTE_USER']) &&
        preg_match('/Basic (.*)$/i', $_SERVER['REDIRECT_REMOTE_USER'], $matches))
        {
            list($name, $password) = explode(':', base64_decode($matches[1]));
            $_SERVER['PHP_AUTH_USER'] = strip_tags($name);
            $_SERVER['PHP_AUTH_PW'] = strip_tags($password);
        }

        // Finally, PHP_AUTH_USER may actually be defined correctly.  If it is set, or
        // has been pulled from REMOTE_USER rewriting then set our USERNAME and PASSWORD
        // parameters.
        if (isset($_SERVER['PHP_AUTH_USER']) && strlen($_SERVER['PHP_AUTH_USER']) > 0)
        {
            $username = $_SERVER['PHP_AUTH_USER'];
            if (isset($_SERVER['PHP_AUTH_PW']) && strlen($_SERVER['PHP_AUTH_PW']) > 0)
                $password = $_SERVER['PHP_AUTH_PW'];
            return;
        }
    }
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

function StrEscape($str, $single=false)
{
    $c = $single? "'" : "\"";
    if(strpos($str, $c) < 0)
        return $str;

    return str_replace($c, "\\" . $c , $str);
}

function requestAuthentication()
{
    global $product, $locale;

    header('WWW-Authenticate: Basic realm="' . $product . '"');
    header('HTTP/1.1 401 Unauthorized');
    header("Status: 401 Access Denied");
    echo GetLocalizedString("NEEDLOGIN", $locale);
}

?>
