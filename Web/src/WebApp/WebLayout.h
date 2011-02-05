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

#ifndef MGWEBLAYOUT_H_
#define MGWEBLAYOUT_H_

#ifdef _WIN32
#pragma warning(disable: 4251)
#endif

typedef map<MgWebCommandWidget*, STRING>    CMD_WIDGET_MAP;

class MgResourceIdentifier;

class MG_WEBAPP_API MgWebLayout : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs an MgWebLayout object.
    /// </summary>
    MgWebLayout(MgResourceService* resourceService, MgResourceIdentifier* webLayoutId);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the title of the application
    /// </summary>
    /// <returns>
    /// Title
    /// </returns>
    STRING GetTitle();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the map definition of the map displayed by the viewer
    /// </summary>
    /// <returns>
    /// Map definition
    /// </returns>
    STRING GetMapDefinition();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the time interval which Ajax viewer pings server
    /// </summary>
    /// <returns>
    /// The time interval measured in seconds
    /// </returns>
    bool GetEnablePingServer();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the initial scale of the view. Return -1. if scale is not set
    /// </summary>
    /// <returns>
    /// Initial scale of the view
    /// </returns>
    double GetScale();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the initial center of the view. Return NULL is the center is not set
    /// </summary>
    /// <returns>
    /// Initial center of the view, or NULL.
    /// </returns>
    MgPoint* GetCenter();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the toolbar
    /// </summary>
    /// <returns>
    /// Toolbar
    /// </returns>
    MgWebToolBar* GetToolBar();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the status bar
    /// </summary>
    /// <returns>
    /// Status bar
    /// </returns>
    MgWebUiPane* GetStatusBar();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the task pane
    /// </summary>
    /// <returns>
    /// Task pane
    /// </returns>
    MgWebTaskPane* GetTaskPane();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the information pane
    /// </summary>
    /// <returns>
    /// Information pane
    /// </returns>
    MgWebInformationPane* GetInformationPane();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the URL used to preview the web layout with a Dwf viewer.
    /// Defaults to the Dwf viewer page in the MapGuide virtual folder.
    /// </summary>
    /// <returns>
    /// URL used to preview the web layout with a Dwf viewer
    /// </returns>
    STRING GetDwfPreviewUrl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the URL used to preview the web layout with an Ajax viewer.
    /// Defaults to the Ajax viewer page in the MapGuide virtual folder.
    /// </summary>
    /// <returns>
    /// URL used to preview the web layout with an Ajax viewer
    /// </returns>
    STRING GetAjaxPreviewUrl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the commands defined in this web layout
    /// </summary>
    /// <returns>
    /// Collection of MgWebCommand derived objects
    /// </returns>
    MgWebCommandCollection* GetCommands();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the contextual menu list defined in this web layout
    /// </summary>
    /// <returns>
    /// MgWebContextMenu object
    /// </returns>
    MgWebContextMenu* GetContextMenu();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the UI target type for the hyperlink action. One value of the MgWebTargetType class.
    /// </summary>
    /// <returns>
    /// The UI target type for the hyperlink action.
    /// </returns>
    INT32 GetHyperlinkTarget();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the name of the UI target frame name for the hyperlink action.
    /// </summary>
    /// <returns>
    /// Name of the UI target frame name for the hyperlink aciton.
    /// </returns>
    STRING GetHyperlinkTargetFrame();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Tells if the zoom control is visible in the AJAX viewer
    /// </summary>
    /// <returns>
    /// true is the zoom control should be visible, false if it should be hidden
    /// </returns>
    bool IsZoomControlVisible();

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Process the getting started page to filter out the commands that are not in
    /// use in this web layout
    /// </summary>
    /// <returns>
    /// An HTML page containing the original text minus the description of the commands
    /// not part of this web layout
    /// </returns>
    MgByteReader* ProcessGettingStartedPage(MgByteReader* htmlGettingStarted, bool forDwf);

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs an empty un-initialized MgWebLayout object.
    /// </summary>
    MgWebLayout();

protected:

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Get the unique identifier for the class
    ///</summary>
    ///<returns>Class Identifider.</returns>
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    // <summary>Dispose this object.</summary>
    // <returns>Returns nothing</returns>
    virtual void Dispose();

    ///////////////////////////////////////////////////////////////////////////
    // parse the xml layout definition
    //
    void ParseWebLayoutDefinition(MgByteReader* xmlWebLayout);

    ///////////////////////////////////////////////////////////////////////////
    // get the boolean value from the specified element
    //
    bool GetBooleanFromElement(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // get the integer value from the specified element
    //
    INT32 GetIntegerFromElement(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // get the double value from the specified element
    //
    double GetDoubleFromElement(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // get the string value from the specified element
    //
    STRING GetStringFromElement(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse a Command element and returns an MgWebCommand object
    //
    MgWebCommand* ParseCommand(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse basic command specific elements
    //
    void ParseBuiltInCommand(DOMNode* node, MgWebCommand* cmd);

    ///////////////////////////////////////////////////////////////////////////
    // parse invoke url representing an action command
    //
    void ParseInvokeUrlCommand(DOMNode* node, MgWebInvokeUrlCommand* cmd);

    ///////////////////////////////////////////////////////////////////////////
    // parse search specific elements
    //
    void ParseSearchCommand(DOMNode* node, MgWebSearchCommand* cmd);

    ///////////////////////////////////////////////////////////////////////////
    // parse invoke script specific elements
    //
    void ParseInvokeScriptCommand(DOMNode* node, MgWebInvokeScriptCommand* cmd);

    ///////////////////////////////////////////////////////////////////////////
    // parse print specific elements
    //
    void ParsePrintCommand(DOMNode* node, MgWebPrintCommand* cmd);

    ///////////////////////////////////////////////////////////////////////////
    // parse help specific elements
    //
    void ParseHelpCommand(DOMNode* node, MgWebHelpCommand* cmd);

    ///////////////////////////////////////////////////////////////////////////
    // parse ui target specific elements
    //
    void ParseUiTargetCommand(DOMNode*, MgWebUiTargetCommand* cmd);

    ///////////////////////////////////////////////////////////////////////////
    // parse a UIItem element and returns an MgWebWidget object
    //
    MgWebWidget* ParseWidget(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse a command widget
    //
    void ParseCommandWidget(DOMNode* node, MgWebCommandWidget* widget);

    ///////////////////////////////////////////////////////////////////////////
    // parse a flyout widget
    //
    void ParseFlyoutWidget(DOMNode* node, MgWebFlyoutWidget* widget);

    ///////////////////////////////////////////////////////////////////////////
    // parse the Map element
    //
    void ParseMapProperties(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse the InformationPane element
    //
    void ParseInformationPane(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse the StatusBar element
    //
    void ParseStatusBar(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse the ToolBar element
    //
    void ParseToolBar(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse the ContextMenu element
    //
    void ParseContextMenu(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse the TaskPane element
    //
    void ParseTaskPane(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse the TaskBar element
    //
    void ParseTaskBar(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse a task bar button
    //
    void ParseTaskBarButton(XERCES_CPP_NAMESPACE::DOMElement* elt, MgWebTaskBarWidget* btn);

    ///////////////////////////////////////////////////////////////////////////
    // parse the CommandSet element
    //
    void ParseCommandSet(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // parse the ZoomControl element
    //
    void ParseZoomControl(XERCES_CPP_NAMESPACE::DOMElement* elt);

    ///////////////////////////////////////////////////////////////////////////
    // Validate that the target viewer type is valid
    //
    INT32 ValidateTargetViewerType(CREFSTRING tt);

    ///////////////////////////////////////////////////////////////////////////
    // Validate that the action is valid
    //
    INT32 ValidateAction(CREFSTRING action);

    ///////////////////////////////////////////////////////////////////////////
    // Validate that the ui item function viewer type is valid
    //
    INT32 ValidateUiItemFunction(CREFSTRING function);

    ///////////////////////////////////////////////////////////////////////////
    // Validate that the target type is valid
    //
    INT32 ValidateTargetType(CREFSTRING tt);

    ///////////////////////////////////////////////////////////////////////////
    // Get a command from the command code in the getting started page
    //
    MgWebCommand* GetCommandFromCode(string code);

    ///////////////////////////////////////////////////////////////////////////
    // Tells if the specified command is in use in this web layout
    //
    bool IsActionInUse(MgWebCommand* cmd, INT32 targetViewer);

CLASS_ID:
    static const INT32 m_cls_id = WebApp_WebLayout;

protected:
    STRING       m_title;
    STRING       m_mapDefinition;
    bool         m_enablePingServer;
    INT32        m_hyperlinkTarget;
    STRING       m_hyperlinkTargetFrame;
    double       m_scale;
    Ptr<MgPoint> m_center;
    Ptr<MgWebInformationPane> m_informationPane;
    Ptr<MgWebToolBar>   m_toolBar;
    Ptr<MgWebUiPane>   m_statusBar;
    Ptr<MgWebTaskPane> m_taskPane;
    STRING       m_dwfPreviewUrl;
    STRING       m_ajaxPreviewUrl;
    Ptr<MgWebCommandCollection> m_commands;
    Ptr<MgWebContextMenu>  m_contextMenu;
    CMD_WIDGET_MAP         m_cmdWidgets;
    bool         m_zoomControlVisible;
};

#endif // _MGWEBLAYOUT_H_
