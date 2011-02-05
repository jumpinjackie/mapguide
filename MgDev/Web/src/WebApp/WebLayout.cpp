//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "WebApp.h"

///////////////////////////////////////////////////////////////////////////
/// Constructs an MgWebLayout object.
//
MgWebLayout::MgWebLayout(MgResourceService* resourceService, MgResourceIdentifier* webLayoutId)
:  m_scale(-1.0),
   m_hyperlinkTarget(MgWebTargetType::TaskPane),
   m_zoomControlVisible(true)
{
    MG_TRY()

    m_commands = new MgWebCommandCollection();
    m_toolBar = new MgWebToolBar();
    m_statusBar = new MgWebUiSizablePane();
    m_taskPane = new MgWebTaskPane();
    m_informationPane = new MgWebInformationPane();
    m_contextMenu = new MgWebContextMenu();
    if (!m_commands || !m_toolBar || !m_statusBar || !m_taskPane || !m_informationPane || !m_contextMenu)
        throw new MgOutOfMemoryException(L"MgWebLayout.MgWebLayout", __LINE__, __WFILE__, NULL, L"", NULL);

    // get the resource service to query the web layout definition
    Ptr<MgByteReader> xmlWebLayout = resourceService->GetResourceContent(webLayoutId);

    // The default value of enable ping server is false so existing web layout will not change behavior.
    m_enablePingServer = false;

    ParseWebLayoutDefinition(xmlWebLayout);

    MG_CATCH_AND_THROW(L"MgWebLayout.MgWebLayout")
}


///////////////////////////////////////////////////////////////////////////
// Default ctor
//
MgWebLayout::MgWebLayout()
{
}


///////////////////////////////////////////////////////////////////////////
// Returns the title of this application
//
STRING MgWebLayout::GetTitle()
{
    return m_title;
}


///////////////////////////////////////////////////////////////////////////
// Returns the map definition of the map displayed by the viewer
//
STRING MgWebLayout::GetMapDefinition()
{
    return m_mapDefinition;
}

///////////////////////////////////////////////////////////////////////////
// Returns the time interval which Ajax viewer pings server
//
bool MgWebLayout::GetEnablePingServer()
{
    return m_enablePingServer;
}

///////////////////////////////////////////////////////////////////////////
// Returns the initial scale of the view. Return -1. if scale is not set
//
double MgWebLayout::GetScale()
{
    return m_scale;
}


///////////////////////////////////////////////////////////////////////////
// Returns the initial center of the view. Return NULL is the center is not set
//
MgPoint* MgWebLayout::GetCenter()
{
    return SAFE_ADDREF((MgPoint*)m_center);
}


///////////////////////////////////////////////////////////////////////////
// Gets the task pane
//
MgWebTaskPane* MgWebLayout::GetTaskPane()
{
    return SAFE_ADDREF((MgWebTaskPane*)m_taskPane);
}


///////////////////////////////////////////////////////////////////////////
// Gets the information pane
//
MgWebInformationPane* MgWebLayout::GetInformationPane()
{
    return SAFE_ADDREF((MgWebInformationPane*)m_informationPane);
}


///////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the URL used to preview the web layout with a Dwf viewer.
/// Defaults to the Dwf viewer page in the MapGuide virtual folder.
/// </summary>
/// <returns>
/// URL used to preview the web layout with a Dwf viewer
/// </returns>
STRING MgWebLayout::GetDwfPreviewUrl()
{
    return m_dwfPreviewUrl;
}


///////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the URL used to preview the web layout with an Ajax viewer.
/// Defaults to the Ajax viewer page in the MapGuide virtual folder.
/// </summary>
/// <returns>
/// URL used to preview the web layout with an Ajax viewer
/// </returns>
STRING MgWebLayout::GetAjaxPreviewUrl()
{
    return m_ajaxPreviewUrl;
}


///////////////////////////////////////////////////////////////////////////
// Returns the commands defined in this web layout
//
MgWebCommandCollection* MgWebLayout::GetCommands()
{
    return SAFE_ADDREF((MgWebCommandCollection*)m_commands);
}


///////////////////////////////////////////////////////////////////////////
// Returns the toolbar defined in this web layout
//
MgWebToolBar* MgWebLayout::GetToolBar()
{
    return SAFE_ADDREF((MgWebToolBar*)m_toolBar);
}


///////////////////////////////////////////////////////////////////////////
// Gets the status bar
//
MgWebUiPane* MgWebLayout::GetStatusBar()
{
    return SAFE_ADDREF((MgWebUiPane*)m_statusBar);
}


///////////////////////////////////////////////////////////////////////////
// Returns the contextual menu list defined in this web layout
//
MgWebContextMenu* MgWebLayout::GetContextMenu()
{
    return SAFE_ADDREF((MgWebContextMenu*)m_contextMenu);
}


///////////////////////////////////////////////////////////////////////////
// Returns the UI target type for the hyperlink action. One value of the MgWebTargetType class.
//
INT32 MgWebLayout::GetHyperlinkTarget()
{
    return m_hyperlinkTarget;
}


///////////////////////////////////////////////////////////////////////////
// Returns the name of the UI target frame name for the hyperlink action.
//
STRING MgWebLayout::GetHyperlinkTargetFrame()
{
    return m_hyperlinkTargetFrame;
}


///////////////////////////////////////////////////////////////////////////
/// Tells if the zoom control is visible in the AJAX viewer
//
bool MgWebLayout::IsZoomControlVisible()
{
    return m_zoomControlVisible;
}


///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebLayout::GetClassId()
{
    return m_cls_id;
}


///////////////////////////////////////////////////////////////////////////
//Dispose this object
//
void MgWebLayout::Dispose()
{
    delete this;
}


///////////////////////////////////////////////////////////////////////////
// parse the xml layout definition
//
void MgWebLayout::ParseWebLayoutDefinition(MgByteReader* xmlWebLayout)
{
    MG_TRY()

    string xmlString = MgUtil::GetTextFromReader(xmlWebLayout);

    MgXmlUtil xmlUtil;
    xmlUtil.ParseString(xmlString.c_str());

    DOMElement* root = xmlUtil.GetRootNode();
    DOMNode* child = MgXmlUtil::GetFirstChild(root);
    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Title") // NOXLATE
            {
                m_title = GetStringFromElement(elt);
            }
            else if (strName == L"Map") // NOXLATE
            {
                ParseMapProperties(elt);
            }
            else if(strName == L"EnablePingServer")
            {
                m_enablePingServer = GetBooleanFromElement(elt);
            }
            else if(strName == L"InformationPane")
            {
                ParseInformationPane(elt);
            }
            else if (strName == L"StatusBar") // NOXLATE
            {
                ParseStatusBar(elt);
            }
            else if (strName == L"ToolBar") // NOXLATE
            {
                ParseToolBar(elt);
            }
            else if (strName == L"TaskPane") // NOXLATE
            {
                ParseTaskPane(elt);
            }
            else if (strName == L"ContextMenu") // NOXLATE
            {
                ParseContextMenu(elt);
            }
            else if (strName == L"CommandSet") // NOXLATE
            {
                ParseCommandSet(elt);
            }
            else if (strName == L"ZoomControl") // NOXLATE
            {
                ParseZoomControl(elt);
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseWebLayoutDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    // link ui items with commands
    for (CMD_WIDGET_MAP::const_iterator it = m_cmdWidgets.begin(); it != m_cmdWidgets.end(); it++)
    {
        MgWebCommandWidget* widget = it->first;  //no ref counting for objects in this map
        STRING cmdName = it->second;

        Ptr<MgWebCommand> cmd = m_commands->GetItem(cmdName);
        widget->SetCommand(cmd);
    }
    m_cmdWidgets.clear();

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseWebLayoutDefinition")
}


///////////////////////////////////////////////////////////////////////////
// get the boolean value from the specified element
//
bool MgWebLayout::GetBooleanFromElement(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);
    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::TEXT_NODE)
        {
            bool val, valid = true;
            STRING strval = GetStringFromElement(elt);
            if (strval == L"true") // NOXLATE
                val = true;
            else if (strval == L"false") // NOXLATE
                val = false;
            else
                valid = false;

            if (!valid)
            {
                throw new MgXmlParserException(L"MgWebLayout.GetBooleanFromElement", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            return val;
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.GetBooleanFromElement")

    return false;
}


///////////////////////////////////////////////////////////////////////////
// get the integer value from the specified element
//
INT32 MgWebLayout::GetIntegerFromElement(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);
    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::TEXT_NODE)
        {
            return atoi(MgUtil::WideCharToMultiByte(GetStringFromElement(elt)).c_str());
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.GetIntegerFromElement")

    return 0;
}


///////////////////////////////////////////////////////////////////////////
// get the double value from the specified element
//
double MgWebLayout::GetDoubleFromElement(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);
    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::TEXT_NODE)
        {
            return atof(MgUtil::WideCharToMultiByte(GetStringFromElement(elt)).c_str());
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.GetIntegerFromElement")

    return 0.0;
}


///////////////////////////////////////////////////////////////////////////
// get the string value from the specified element
//
STRING MgWebLayout::GetStringFromElement(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);
    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::TEXT_NODE)
        {
            wstring strval = MgXmlUtil::GetNodeValue(child);
            return MgUtil::Trim(strval);
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.GetStringFromElement")

    return L"";
}


///////////////////////////////////////////////////////////////////////////
// parse a Command element and returns an MgWebCommand object
//
MgWebCommand* MgWebLayout::ParseCommand(DOMElement* elt)
{
    MG_TRY()

    // get the type of command
    wstring wType = MgXmlUtil::GetAttribute(elt, "xsi:type"); // NOXLATE
    string type;
    UnicodeString::WideCharToMultiByte(wType.c_str(), type);

    // skip the namespace
    const char* ptype = type.c_str();
    const char* psep = strchr(ptype, ':');
    if (psep != NULL)
        ptype = psep + 1;

    INT32 cmdType = -1;
    Ptr<MgWebCommand> cmd;

    if (!strcmp(ptype, "BasicCommandType")) // NOXLATE
    {
        cmdType = 0;
        cmd = new MgWebCommand();
    }
    else if (!strcmp(ptype, "InvokeURLCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::InvokeUrl;
        cmd = new MgWebInvokeUrlCommand();
    }
    else if (!strcmp(ptype, "SearchCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::Search;
        cmd = new MgWebSearchCommand();
    }
    else if (!strcmp(ptype, "BufferCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::Buffer;
        cmd = new MgWebBufferCommand();
    }
    else if (!strcmp(ptype, "PrintCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::PrintMap;
        cmd = new MgWebPrintCommand();  //TODO rename this class
    }
    else if (!strcmp(ptype, "SelectWithinCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::SelectWithin;
        cmd = new MgWebSelectWithinCommand();
    }
    else if (!strcmp(ptype, "MeasureCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::Measure;
        cmd = new MgWebMeasureCommand();
    }
    else if (!strcmp(ptype, "ViewOptionsCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::ViewOptions;
        cmd = new MgWebViewOptionsCommand();
    }
    else if (!strcmp(ptype, "GetPrintablePageCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::GetPrintablePage;
        cmd = new MgWebGetPrintablePageCommand();
    }
    else if (!strcmp(ptype, "InvokeScriptCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::InvokeScript;
        cmd = new MgWebInvokeScriptCommand();
    }
    else if (!strcmp(ptype, "HelpCommandType")) // NOXLATE
    {
        cmdType = MgWebActions::Help;
        cmd = new MgWebHelpCommand();
    }
    else
    {
        throw new MgXmlParserException(L"MgWebLayout.ParseCommand", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (cmd == NULL)
        throw new MgOutOfMemoryException(L"MgWebLayout.ParseCommand", __LINE__, __WFILE__, NULL, L"", NULL);

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);
    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* eltInner = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(eltInner);

            if (strName == L"Name") // NOXLATE
            {
                cmd->SetName(GetStringFromElement(eltInner));
            }
            else if (strName == L"Label") // NOXLATE
            {
                cmd->SetLabel(GetStringFromElement(eltInner));
            }
            else if (strName == L"Tooltip") // NOXLATE
            {
                cmd->SetTooltip(GetStringFromElement(eltInner));
            }
            else if (strName == L"Description") // NOXLATE
            {
                cmd->SetDescription(GetStringFromElement(eltInner));
            }
            else if (strName == L"ImageURL") // NOXLATE
            {
                cmd->SetIconUrl(GetStringFromElement(eltInner));
            }
            else if (strName == L"DisabledImageURL") // NOXLATE
            {
                cmd->SetDisabledIconUrl(GetStringFromElement(eltInner));
            }
            else if (strName == L"TargetViewer") // NOXLATE
            {
                cmd->SetTargetViewerType(ValidateTargetViewerType(GetStringFromElement(eltInner)));
                break;
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    if (child == NULL)
    {
        throw new MgXmlParserException(L"MgWebLayout.ParseCommand", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // continue parsing for element specifics to the various type of commands
    // TODO: very low priority: change parsing of search, invoke url, invoke script and print
    //       so that they call ParseUiTargetCommand. Indeed this 4 commands all parse target/targetframe,
    //       which is also done by rseUiTargetCommand. Not a big deal, would make the code look slightly better
    switch (cmdType)
    {
        case 0:
            ParseBuiltInCommand(MgXmlUtil::GetNextSibling(child), cmd);
            break;
        case MgWebActions::Search:
            ParseSearchCommand(MgXmlUtil::GetNextSibling(child), (MgWebSearchCommand*)cmd.p);
            break;
        case MgWebActions::InvokeUrl:
            ParseInvokeUrlCommand(MgXmlUtil::GetNextSibling(child), (MgWebInvokeUrlCommand*)cmd.p);
            break;
        case MgWebActions::InvokeScript:
            ParseInvokeScriptCommand(MgXmlUtil::GetNextSibling(child), (MgWebInvokeScriptCommand*)cmd.p);
            break;
        case MgWebActions::PrintMap:
            ParsePrintCommand(MgXmlUtil::GetNextSibling(child), (MgWebPrintCommand*)cmd.p);
            break;
        case MgWebActions::Help:
            ParseHelpCommand(MgXmlUtil::GetNextSibling(child), (MgWebHelpCommand*)cmd.p);
            break;
        case MgWebActions::Buffer:
        case MgWebActions::SelectWithin:
        case MgWebActions::Measure:
        case MgWebActions::ViewOptions:
        case MgWebActions::GetPrintablePage:
            ParseUiTargetCommand(MgXmlUtil::GetNextSibling(child), (MgWebUiTargetCommand*)cmd.p);
            break;
    }

    return cmd.Detach();

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseCommand")

    return NULL;
}


///////////////////////////////////////////////////////////////////////////
// parse a Command element representing a built-in command
//
void MgWebLayout::ParseBuiltInCommand(DOMNode* node, MgWebCommand* cmd)
{
    MG_TRY()

    if (node == NULL)
        throw new MgNullArgumentException(L"MgWebLayout.ParseBuiltInCommand", __LINE__, __WFILE__, NULL, L"", NULL);

    do
    {
        if (MgXmlUtil::GetNodeType(node) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)node;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Action") // NOXLATE
            {
                cmd->SetAction(ValidateAction(GetStringFromElement(elt)));
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseBuiltInCommand", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        node = MgXmlUtil::GetNextSibling(node);
    }
    while (node != NULL);

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseBuiltInCommand")
}


///////////////////////////////////////////////////////////////////////////
// parse invokeUrl specific elements
//
void MgWebLayout::ParseInvokeUrlCommand(DOMNode* node, MgWebInvokeUrlCommand* cmd)
{
    Ptr<MgPropertyCollection> params = cmd->GetParameters();
    Ptr<MgStringCollection> layers = cmd->GetLayers();

    MG_TRY()

    if (node == NULL)
        throw new MgNullArgumentException(L"MgWebLayout.ParseInvokeUrlCommand", __LINE__, __WFILE__, NULL, L"", NULL);

    do
    {
        if (MgXmlUtil::GetNodeType(node) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)node;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"URL") // NOXLATE
            {
                cmd->SetUrl(GetStringFromElement(elt));
            }
            else if (strName == L"AdditionalParameter") // NOXLATE
            {
                DOMNode* lchild = MgXmlUtil::GetFirstChild(elt);
                STRING key, value;
                while (NULL != lchild)
                {
                    if (MgXmlUtil::GetNodeType(lchild) == DOMNode::ELEMENT_NODE)
                    {
                        strName = MgXmlUtil::GetTagName((DOMElement*)lchild);
                        if (strName == L"Key") // NOXLATE
                        {
                            key = GetStringFromElement((DOMElement*)lchild);
                        }
                        else if (strName == L"Value") // NOXLATE
                        {
                            value = GetStringFromElement((DOMElement*)lchild);
                        }
                        else
                        {
                            throw new MgXmlParserException(L"MgWebLayout.ParseInvokeUrlCommand", __LINE__, __WFILE__, NULL, L"", NULL);
                        }
                    }
                    lchild = MgXmlUtil::GetNextSibling(lchild);
                }
                if (key != L"")
                {
                    Ptr<MgStringProperty> prop = new MgStringProperty(key, value);
                    if (prop == NULL)
                        throw new MgOutOfMemoryException(L"MgWebLayout.ParseInvokeUrlCommand", __LINE__, __WFILE__, NULL, L"", NULL);
                    params->Add(prop);
                }
            }
            else if (strName == L"DisableIfSelectionEmpty") // NOXLATE
            {
                cmd->SetDisabledWhenSelectionEmpty(GetBooleanFromElement(elt));
            }
            else if (strName == L"LayerSet") // NOXLATE
            {
                DOMNode* lchild = MgXmlUtil::GetFirstChild(elt);
                while (NULL != lchild)
                {
                    if (MgXmlUtil::GetNodeType(lchild) == DOMNode::ELEMENT_NODE)
                    {
                        strName = MgXmlUtil::GetTagName((DOMElement*)lchild);
                        if (strName == L"Layer") // NOXLATE
                        {
                            layers->Add(GetStringFromElement((DOMElement*)lchild));
                        }
                        else
                        {
                            throw new MgXmlParserException(L"MgWebLayout.ParseInvokeUrlCommand", __LINE__, __WFILE__, NULL, L"", NULL);
                        }
                    }
                    lchild = MgXmlUtil::GetNextSibling(lchild);
                }
            }
            else if (strName == L"Target") // NOXLATE
            {
                cmd->SetTarget(ValidateTargetType(GetStringFromElement(elt)));
            }
            else if (strName == L"TargetFrame") // NOXLATE
            {
                cmd->SetTargetName(GetStringFromElement(elt));
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseInvokeUrlCommand", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        node = MgXmlUtil::GetNextSibling(node);
    }
    while (node != NULL);

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseInvokeUrlCommand")
}


///////////////////////////////////////////////////////////////////////////
// parse search specific elements
//
void MgWebLayout::ParseSearchCommand(DOMNode* node, MgWebSearchCommand* cmd)
{
    MG_TRY()

    if (node == NULL)
        throw new MgNullArgumentException(L"MgWebLayout.ParseSearchCommand", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgPropertyCollection> resultColumns = cmd->GetResultColumns();

    do
    {
        if (MgXmlUtil::GetNodeType(node) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)node;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Layer") // NOXLATE
            {
                cmd->SetLayer(GetStringFromElement(elt));
            }
            else if (strName == L"Prompt") // NOXLATE
            {
                cmd->SetPrompt(GetStringFromElement(elt));
            }
            else if (strName == L"Filter") // NOXLATE
            {
                cmd->SetFilter(GetStringFromElement(elt));
            }
            else if (strName == L"MatchLimit") // NOXLATE
            {
                cmd->SetMatchLimit(GetIntegerFromElement(elt));
            }
            else if (strName == L"Target") // NOXLATE
            {
                cmd->SetTarget(ValidateTargetType(GetStringFromElement(elt)));
            }
            else if (strName == L"TargetFrame") // NOXLATE
            {
                cmd->SetTargetName(GetStringFromElement(elt));
            }
            else if (strName == L"ResultColumns") // NOXLATE
            {
                DOMNode* lchild = MgXmlUtil::GetFirstChild(elt);
                while (NULL != lchild)
                {
                    if (MgXmlUtil::GetNodeType(lchild) == DOMNode::ELEMENT_NODE)
                    {
                        strName = MgXmlUtil::GetTagName((DOMElement*)lchild);
                        if (strName == L"Column") // NOXLATE
                        {
                            DOMNode* lchild1 = MgXmlUtil::GetFirstChild(lchild);
                            STRING displayName, property;
                            while (NULL != lchild1)
                            {
                                if (MgXmlUtil::GetNodeType(lchild1) == DOMNode::ELEMENT_NODE)
                                {
                                    strName = MgXmlUtil::GetTagName((DOMElement*)lchild1);
                                    if (strName == L"Name") // NOXLATE
                                    {
                                        displayName = GetStringFromElement((DOMElement*)lchild1);
                                    }
                                    else if (strName == L"Property") // NOXLATE
                                    {
                                        property = GetStringFromElement((DOMElement*)lchild1);
                                    }
                                    else
                                    {
                                        throw new MgXmlParserException(L"MgWebLayout.ParseSearchCommand", __LINE__, __WFILE__, NULL, L"", NULL);
                                    }
                                }
                                lchild1 = MgXmlUtil::GetNextSibling(lchild1);
                            }
                            if (property != L"")
                            {
                                Ptr<MgStringProperty> prop = new MgStringProperty(property, displayName);
                                if (prop == NULL)
                                    throw new MgOutOfMemoryException(L"MgWebLayout.ParseSearchCommand", __LINE__, __WFILE__, NULL, L"", NULL);
                                resultColumns->Add(prop);
                            }
                            else
                            {
                                throw new MgXmlParserException(L"MgWebLayout.ParseSearchCommand", __LINE__, __WFILE__, NULL, L"", NULL);
                            }
                        }
                        else
                        {
                            throw new MgXmlParserException(L"MgWebLayout.ParseSearchCommand", __LINE__, __WFILE__, NULL, L"", NULL);
                        }
                    }
                    lchild = MgXmlUtil::GetNextSibling(lchild);
                }
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseSearchCommand", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        node = MgXmlUtil::GetNextSibling(node);
    }
    while (node != NULL);

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseSearchCommand")
}


///////////////////////////////////////////////////////////////////////////
// parse invoke script specific elements
//
void MgWebLayout::ParseInvokeScriptCommand(DOMNode* node, MgWebInvokeScriptCommand* cmd)
{
    MG_TRY()

    if (node == NULL)
        throw new MgNullArgumentException(L"MgWebLayout.ParseInvokeScriptCommand", __LINE__, __WFILE__, NULL, L"", NULL);

    do
    {
        if (MgXmlUtil::GetNodeType(node) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)node;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Target") // NOXLATE
            {
                cmd->SetTarget(ValidateTargetType(GetStringFromElement(elt)));
            }
            else if (strName == L"TargetFrame") // NOXLATE
            {
                cmd->SetTargetName(GetStringFromElement(elt));
            }
            else if (strName == L"Script") // NOXLATE
            {
                cmd->SetScriptCode(GetStringFromElement(elt));
            }
            else
            {
               throw new MgXmlParserException(L"MgWebLayout.ParseInvokeScriptCommand", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        node = MgXmlUtil::GetNextSibling(node);
    }
    while (node != NULL);

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseInvokeScriptCommand")
}


///////////////////////////////////////////////////////////////////////////
// parse print specific elements
//
void MgWebLayout::ParsePrintCommand(DOMNode* node, MgWebPrintCommand* cmd)
{
    MG_TRY()

    if (node == NULL)
        throw new MgNullArgumentException(L"MgWebLayout.ParsePrintCommand", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgStringCollection> printLayouts = cmd->GetPrintLayouts();

    do
    {
        if (MgXmlUtil::GetNodeType(node) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)node;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"PrintLayout") // NOXLATE
            {
                DOMNode* lchild = MgXmlUtil::GetFirstChild(elt);
                while (NULL != lchild)
                {
                    if (MgXmlUtil::GetNodeType(lchild) == DOMNode::ELEMENT_NODE)
                    {
                        strName = MgXmlUtil::GetTagName((DOMElement*)lchild);
                        if (strName == L"ResourceId") // NOXLATE
                        {
                            printLayouts->Add(GetStringFromElement((DOMElement*)lchild));
                        }
                        else
                        {
                            throw new MgXmlParserException(L"MgWebLayout.ParsePrintCommand", __LINE__, __WFILE__, NULL, L"", NULL);
                        }
                    }
                    lchild = MgXmlUtil::GetNextSibling(lchild);
                }
            }
            else
            {
               throw new MgXmlParserException(L"MgWebLayout.ParsePrintCommand", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        node = MgXmlUtil::GetNextSibling(node);
    }
    while (node != NULL);

    MG_CATCH_AND_THROW(L"MgWebLayout.ParsePrintCommand")
}


///////////////////////////////////////////////////////////////////////////
// parse help specific elements
//
void MgWebLayout::ParseHelpCommand(DOMNode* node, MgWebHelpCommand* cmd)
{
    MG_TRY()

    if (node == NULL)
        throw new MgNullArgumentException(L"MgWebLayout.ParseHelpCommand", __LINE__, __WFILE__, NULL, L"", NULL);

    do
    {
        if (MgXmlUtil::GetNodeType(node) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)node;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Target") // NOXLATE
            {
                cmd->SetTarget(ValidateTargetType(GetStringFromElement(elt)));
            }
            else if (strName == L"TargetFrame") // NOXLATE
            {
                cmd->SetTargetName(GetStringFromElement(elt));
            }
            else if (strName == L"URL") // NOXLATE
            {
                cmd->SetUrl(GetStringFromElement(elt));
            }
            else
            {
               throw new MgXmlParserException(L"MgWebLayout.ParseHelpCommand", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        node = MgXmlUtil::GetNextSibling(node);
    }
    while (node != NULL);

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseHelpCommand")
}


///////////////////////////////////////////////////////////////////////////
// parse ui target specific elements
//
void MgWebLayout::ParseUiTargetCommand(DOMNode* node, MgWebUiTargetCommand* cmd)
{
    MG_TRY()

    if (node == NULL)
        throw new MgNullArgumentException(L"MgWebLayout.ParseUiTargetCommand", __LINE__, __WFILE__, NULL, L"", NULL);

    do
    {
        if (MgXmlUtil::GetNodeType(node) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)node;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Target") // NOXLATE
            {
                cmd->SetTarget(ValidateTargetType(GetStringFromElement(elt)));
            }
            else if (strName == L"TargetFrame") // NOXLATE
            {
                cmd->SetTargetName(GetStringFromElement(elt));
            }
            else
            {
               throw new MgXmlParserException(L"MgWebLayout.ParseUiTargetCommand", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        node = MgXmlUtil::GetNextSibling(node);
    }
    while (node != NULL);

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseUiTargetCommand")
}


///////////////////////////////////////////////////////////////////////////
// Validate that the target viewer type is valid
//
INT32 MgWebLayout::ValidateTargetViewerType(CREFSTRING tt)
{
    MG_TRY()

    if (tt == L"Dwf")
        return MgWebTargetViewerType::Dwf;
    else if (tt == L"Ajax")
        return MgWebTargetViewerType::Ajax;
    else if (tt == L"All")
        return MgWebTargetViewerType::All;
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(tt);

        throw new MgInvalidArgumentException(L"MgWebLayout.ValidateTargetViewerType",
            __LINE__, __WFILE__, &arguments, L"MgInvalidWebTargetViewerType", NULL);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ValidateTargetViewerType")

    return 0;
}


///////////////////////////////////////////////////////////////////////////
// Validate that the action is valid
//
INT32 MgWebLayout::ValidateAction(CREFSTRING action)
{
    MG_TRY()

    if (action == L"Pan") // NOXLATE
        return MgWebActions::Pan;
    else if (action == L"PanUp") // NOXLATE
        return MgWebActions::PanUp;
    else if (action == L"PanDown") // NOXLATE
        return MgWebActions::PanDown;
    else if (action == L"PanLeft") // NOXLATE
        return MgWebActions::PanLeft;
    else if (action == L"PanRight") // NOXLATE
        return MgWebActions::PanRight;
    else if (action == L"Zoom") // NOXLATE
        return MgWebActions::Zoom;
    else if (action == L"ZoomIn") // NOXLATE
        return MgWebActions::ZoomIn;
    else if (action == L"ZoomOut") // NOXLATE
        return MgWebActions::ZoomOut;
    else if (action == L"ZoomRectangle") // NOXLATE
        return MgWebActions::ZoomRectangle;
    else if (action == L"ZoomToSelection") // NOXLATE
        return MgWebActions::ZoomSelection;
    else if (action == L"FitToWindow") // NOXLATE
        return MgWebActions::FitWindow;
    else if (action == L"PreviousView") // NOXLATE
        return MgWebActions::ViewPrevious;
    else if (action == L"NextView") // NOXLATE
        return MgWebActions::ViewNext;
    else if (action == L"RestoreView") // NOXLATE
        return MgWebActions::ViewRestore;
    else if (action == L"Select") // NOXLATE
        return MgWebActions::Select;
    else if (action == L"SelectRadius") // NOXLATE
        return MgWebActions::SelectRadius;
    else if (action == L"SelectPolygon") // NOXLATE
        return MgWebActions::SelectPolygon;
    else if (action == L"SelectWithin") // NOXLATE
        return MgWebActions::SelectWithin;
    else if (action == L"ClearSelection") // NOXLATE
        return MgWebActions::SelectClear;
    else if (action == L"Refresh") // NOXLATE
        return MgWebActions::Refresh;
    else if (action == L"CopyMap") // NOXLATE
        return MgWebActions::Copy;
    else if (action == L"GetPrintablePage") // NOXLATE
        return MgWebActions::GetPrintablePage;
    else if (action == L"Buffer") // NOXLATE
        return MgWebActions::Buffer;
    else if (action == L"Measure") // NOXLATE
        return MgWebActions::Measure;
    else if (action == L"ViewOptions") // NOXLATE
        return MgWebActions::ViewOptions;
    else if (action == L"InvokeScript") // NOXLATE
        return MgWebActions::InvokeScript;
    else if (action == L"About") // NOXLATE
        return MgWebActions::About;
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(action);

        throw new MgInvalidArgumentException(L"MgWebLayout.ValidateAction",
            __LINE__, __WFILE__, &arguments, L"MgInvalidWebAction", NULL);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ValidateAction")

    return 0;
}


///////////////////////////////////////////////////////////////////////////
// Validate that the target type is valid
//
INT32 MgWebLayout::ValidateTargetType(CREFSTRING tt)
{
    MG_TRY()

    if (tt == L"TaskPane") // NOXLATE
        return MgWebTargetType::TaskPane;
    else if (tt == L"NewWindow") // NOXLATE
        return MgWebTargetType::NewWindow;
    else if (tt == L"SpecifiedFrame") // NOXLATE
        return MgWebTargetType::SpecifiedFrame;
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(tt);

        throw new MgInvalidArgumentException(L"MgWebLayout.ValidateTargetType",
            __LINE__, __WFILE__, &arguments, L"MgInvalidWebTargetType", NULL);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ValidateTargetType")

    return 0;
}


///////////////////////////////////////////////////////////////////////////
// parse the Map element
//
void MgWebLayout::ParseMapProperties(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"ResourceId") // NOXLATE
            {
                m_mapDefinition = GetStringFromElement(elt);
            }
            else if (strName == L"InitialView") // NOXLATE
            {
                bool centerXDefined = false, centerYDefined = false;
                double centerX = 0.0, centerY = 0.0;

                DOMNode* ichild = MgXmlUtil::GetFirstChild(elt);
                while (NULL != ichild)
                {
                    if (MgXmlUtil::GetNodeType(ichild) == DOMNode::ELEMENT_NODE)
                    {
                        DOMElement* eltInner = (DOMElement*)ichild;
                        strName = MgXmlUtil::GetTagName(eltInner);

                        if (strName == L"CenterX") // NOXLATE
                        {
                            centerX = GetDoubleFromElement(eltInner);
                            centerXDefined = true;
                        }
                        else if (strName == L"CenterY") // NOXLATE
                        {
                            centerY = GetDoubleFromElement(eltInner);
                            centerYDefined = true;
                        }
                        else if (strName == L"Scale") // NOXLATE
                            m_scale = GetDoubleFromElement(eltInner);
                        else
                        {
                            throw new MgXmlParserException(L"MgWebLayout.ParseMapProperties", __LINE__, __WFILE__, NULL, L"", NULL);
                        }
                    }
                    ichild = MgXmlUtil::GetNextSibling(ichild);
                }
                if (centerXDefined && centerYDefined)
                {
                    Ptr<MgCoordinateXY> coord = new MgCoordinateXY(centerX, centerY);
                    m_center = new MgPoint(coord);
                }
                else if (centerXDefined != centerYDefined)
                {
                    throw new MgXmlParserException(L"MgWebLayout.ParseMapProperties", __LINE__, __WFILE__, NULL, L"", NULL);
                }
            }
            else if (strName == L"HyperlinkTarget") // NOXLATE
            {
                m_hyperlinkTarget = ValidateTargetType(GetStringFromElement(elt));
            }
            else if (strName == L"HyperlinkTargetFrame") // NOXLATE
            {
                m_hyperlinkTargetFrame = GetStringFromElement(elt);
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseMapProperties", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseMapProperties")
}


///////////////////////////////////////////////////////////////////////////
// parse the InformationPane element
//
void MgWebLayout::ParseInformationPane(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Visible") // NOXLATE
            {
                m_informationPane->SetVisible(GetBooleanFromElement(elt));
            }
            else if (strName == L"Width") // NOXLATE
            {
                m_informationPane->SetWidth(GetIntegerFromElement(elt));
            }
            else if (strName == L"LegendVisible") // NOXLATE
            {
                m_informationPane->SetLegendBandVisible(GetBooleanFromElement(elt));
            }
            else if (strName == L"PropertiesVisible") // NOXLATE
            {
                m_informationPane->SetPropertiesBandVisible(GetBooleanFromElement(elt));
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseInformationPane", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseInformationPane")
}


///////////////////////////////////////////////////////////////////////////
// parse the StatusBar element
//
void MgWebLayout::ParseStatusBar(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Visible") // NOXLATE
            {
                m_statusBar->SetVisible(GetBooleanFromElement(elt));
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseStatusBar", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseStatusBar")
}


///////////////////////////////////////////////////////////////////////////
// parse the ToolBar element
//
void MgWebLayout::ParseToolBar(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);
    Ptr<MgWebWidgetCollection> widgets = m_toolBar->GetWidgets();

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Visible") // NOXLATE
            {
                m_toolBar->SetVisible(GetBooleanFromElement(elt));
            }
            else if (strName == L"Button") // NOXLATE
            {
                Ptr<MgWebWidget> widget = ParseWidget(elt);
                widgets->Add(widget);
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseToolBar", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseToolBar")
}


///////////////////////////////////////////////////////////////////////////
// parse a widget (MenuButton, Button, MenuItem)
//
MgWebWidget* MgWebLayout::ParseWidget(DOMElement* elt)
{
    Ptr<MgWebWidget> widget;

    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Function") // NOXLATE
            {
                STRING function = GetStringFromElement(elt);
                switch (ValidateUiItemFunction(function))
                {
                    case MgWebWidgetType::Separator:
                        if ((widget = new MgWebSeparatorWidget()) == NULL)
                            throw new MgOutOfMemoryException(L"MgWebLayout.ParseWidget", __LINE__, __WFILE__, NULL, L"", NULL);
                        break;

                    case MgWebWidgetType::Command:
                        if ((widget = new MgWebCommandWidget()) == NULL)
                            throw new MgOutOfMemoryException(L"MgWebLayout.ParseWidget", __LINE__, __WFILE__, NULL, L"", NULL);
                        ParseCommandWidget(MgXmlUtil::GetNextSibling(child), (MgWebCommandWidget*)widget.p);
                        break;

                    case MgWebWidgetType::Flyout:
                        if ((widget = new MgWebFlyoutWidget()) == NULL)
                            throw new MgOutOfMemoryException(L"MgWebLayout.ParseWidget", __LINE__, __WFILE__, NULL, L"", NULL);
                        ParseFlyoutWidget(MgXmlUtil::GetNextSibling(child), (MgWebFlyoutWidget*)widget.p);
                        break;

                    default:
                    {
                        throw new MgXmlParserException(L"MgWebLayout.ParseWidget", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }
                break;
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseWidget", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    return widget.Detach();

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseWidget")

    return NULL;
}


///////////////////////////////////////////////////////////////////////////
// Parse a command widget
// Note: containing element has been already partially parsed. This method
//       parses the remainder of the elements, specific to a command widget
//
void MgWebLayout::ParseCommandWidget(DOMNode* node, MgWebCommandWidget* widget)
{
    MG_TRY()

    while (NULL != node)
    {
        if (MgXmlUtil::GetNodeType(node) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)node;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Command") // NOXLATE
            {
                m_cmdWidgets[widget] = GetStringFromElement(elt);
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseCommandWidget", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        node = MgXmlUtil::GetNextSibling(node);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseCommandWidget")
}


///////////////////////////////////////////////////////////////////////////
// Parse a flyout widget
// Note: containing element has been already partially parsed. This method
//       parses the remainder of the elements, specific to a flyout widget
//
void MgWebLayout::ParseFlyoutWidget(DOMNode* node, MgWebFlyoutWidget* widget)
{
    MG_TRY()

    while (NULL != node)
    {
        if (MgXmlUtil::GetNodeType(node) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)node;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Label") // NOXLATE
            {
                widget->SetLabel(GetStringFromElement(elt));
            }
            else if (strName == L"ImageURL") // NOXLATE
            {
                widget->SetIconUrl(GetStringFromElement(elt));
            }
            else if (strName == L"DisabledImageURL") // NOXLATE
            {
                widget->SetDisabledIconUrl(GetStringFromElement(elt));
            }
            else if (strName == L"SubItem") // NOXLATE
            {
                Ptr<MgWebWidget> subWidget = ParseWidget(elt);
                widget->AddSubItem(subWidget);
            }
            else if (strName == L"Tooltip") // NOXLATE
            {
                widget->SetTooltip(GetStringFromElement(elt));
            }
            else if (strName == L"Description") // NOXLATE
            {
                widget->SetDescription(GetStringFromElement(elt));
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseFlyoutWidget", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        node = MgXmlUtil::GetNextSibling(node);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseCommandWidget")
}


///////////////////////////////////////////////////////////////////////////
// Validate that the ui item function viewer type is valid
//
INT32 MgWebLayout::ValidateUiItemFunction(CREFSTRING function)
{
    MG_TRY()

    if (function == L"Separator") // NOXLATE
        return MgWebWidgetType::Separator;
    else if (function == L"Command") // NOXLATE
        return MgWebWidgetType::Command;
    else if (function == L"Flyout") // NOXLATE
        return MgWebWidgetType::Flyout;
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(function);

        throw new MgInvalidArgumentException(L"MgWebLayout.ValidateUiItemFunction",
            __LINE__, __WFILE__, &arguments, L"MgInvalidWebWidgetType", NULL);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ValidateUiItemFunction")

    return 0;
}


///////////////////////////////////////////////////////////////////////////
// parse the ContextMenu element
//
void MgWebLayout::ParseContextMenu(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Visible") // NOXLATE
            {
                m_contextMenu->SetVisible(GetBooleanFromElement(elt));
            }
            else if (strName == L"MenuItem") // NOXLATE
            {
                Ptr<MgWebWidget> widget = ParseWidget(elt);
                m_contextMenu->Add(widget);
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseContextMenu", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseContextMenu")
}


///////////////////////////////////////////////////////////////////////////
// parse the TaskPane element
//
void MgWebLayout::ParseTaskPane(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Visible") // NOXLATE
            {
                m_taskPane->SetVisible(GetBooleanFromElement(elt));
            }
            else if (strName == L"Width") // NOXLATE
            {
                m_taskPane->SetWidth(GetIntegerFromElement(elt));
            }
            else if (strName == L"InitialTask") // NOXLATE
            {
                m_taskPane->SetInitialTaskUrl(GetStringFromElement(elt));
            }
            else if (strName == L"TaskBar") // NOXLATE
            {
                ParseTaskBar(elt);
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseTaskPane", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseTaskPane")
}


///////////////////////////////////////////////////////////////////////////
// parse the TaskBar element
//
void MgWebLayout::ParseTaskBar(DOMElement* elt)
{
    Ptr<MgWebTaskBar> taskBar = m_taskPane->GetTaskBar();
    Ptr<MgWebWidgetCollection> taskList = taskBar->GetTaskList();
    Ptr<MgWebWidgetCollection> taskButtons = taskBar->GetTaskButtons();

    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Visible") // NOXLATE
            {
                taskBar->SetVisible(GetBooleanFromElement(elt));
            }
            else if (strName == L"Home") // NOXLATE
            {
                Ptr<MgWebTaskBarWidget> home = (MgWebTaskBarWidget*)taskButtons->GetWidget(MgWebTaskButtonType::Home);
                ParseTaskBarButton(elt, home);
            }
            else if (strName == L"Back") // NOXLATE
            {
                Ptr<MgWebTaskBarWidget> back = (MgWebTaskBarWidget*)taskButtons->GetWidget(MgWebTaskButtonType::Back);
                ParseTaskBarButton(elt, back);
            }
            else if (strName == L"Forward") // NOXLATE
            {
                Ptr<MgWebTaskBarWidget> forward = (MgWebTaskBarWidget*)taskButtons->GetWidget(MgWebTaskButtonType::Forward);
                ParseTaskBarButton(elt, forward);
            }
            else if (strName == L"Tasks") // NOXLATE
            {
                Ptr<MgWebTaskBarWidget> tasks = (MgWebTaskBarWidget*)taskButtons->GetWidget(MgWebTaskButtonType::Tasks);
                ParseTaskBarButton(elt, tasks);
            }
            else if (strName == L"MenuButton") // NOXLATE
            {
                Ptr<MgWebWidget> widget = ParseWidget(elt);
                taskList->Add(widget);
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseTaskBar", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseTaskBar")
}


///////////////////////////////////////////////////////////////////////////
// parse a task bar button
//
void MgWebLayout::ParseTaskBarButton(DOMElement* elt, MgWebTaskBarWidget* btn)
{
    DOMNode* ichild = MgXmlUtil::GetFirstChild(elt);
    STRING strName;
    while (NULL != ichild)
    {
        if (MgXmlUtil::GetNodeType(ichild) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* eltInner = (DOMElement*)ichild;
            strName = MgXmlUtil::GetTagName(eltInner);

            if (strName == L"Tooltip") // NOXLATE
            {
                btn->SetTooltip(GetStringFromElement(eltInner));
            }
            else if (strName == L"Description") // NOXLATE
            {
                btn->SetDescription(GetStringFromElement(eltInner));
            }
            else if (strName == L"Name") // NOXLATE
            {
                btn->SetName(GetStringFromElement(eltInner));
            }
            else if (strName == L"ImageURL") // NOXLATE
            {
                btn->SetIconUrl(GetStringFromElement(eltInner));
            }
            else if (strName == L"DisabledImageURL") // NOXLATE
            {
                btn->SetDisabledIconUrl(GetStringFromElement(eltInner));
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseTaskBarButton", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        ichild = MgXmlUtil::GetNextSibling(ichild);
    }
}


///////////////////////////////////////////////////////////////////////////
// parse the CommandSet element
//
void MgWebLayout::ParseCommandSet(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Command") // NOXLATE
            {
                Ptr<MgWebCommand> cmd = ParseCommand(elt);
                m_commands->Add(cmd);
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseCommandSet", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseCommandSet")
}


///////////////////////////////////////////////////////////////////////////
// parse the ZoomControl element
//
void MgWebLayout::ParseZoomControl(DOMElement* elt)
{
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);

    while (NULL != child)
    {
        if (MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if (strName == L"Visible") // NOXLATE
            {
                m_zoomControlVisible = GetBooleanFromElement(elt);
            }
            else
            {
                throw new MgXmlParserException(L"MgWebLayout.ParseZoomControl", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgWebLayout.ParseZoomControl")
}
