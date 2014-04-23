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

static map<string, INT32> cmdCodes;

///////////////////////////////////////////////////////////////////////////
// Process the getting started page to filter out the commands that are not in
// use in this web layout
//
MgByteReader* MgWebLayout::ProcessGettingStartedPage(MgByteReader* htmlGettingStarted, bool forDwf)
{
    // Because the HTML text is not a valid XML document we cannot use the XML
    // parser. We need to perform our own parsing of the html code.
    //
    // A command is defined in the getting started by a <dt> section immediately followed
    // by a <dd> section. Inside the <dt> section an <a> tag contain an encoded name that
    // allows us to uniquely identify the command. If this command is used in this web
    // layout we carry the 2 sections over the html output, otherwise we discard them.
    //
    string htmlIn = MgUtil::GetTextFromReader(htmlGettingStarted);
    string htmlOut;
    INT32 tgt = MgWebTargetViewerType::Ajax;
    if (forDwf)
        tgt = MgWebTargetViewerType::Dwf;

    bool first = true;
    size_t dtBegin, ddEnd, endOfCommands = string::npos;
    for (size_t cur=0; cur != string::npos;)
    {
        // look for next <dt> tag
        dtBegin = htmlIn.find("<dt>", cur);
        if (dtBegin != string::npos)
        {
            if (first)
            {
                first = false;
                // copy the beginning of the document to the output
                htmlOut = htmlIn.substr(0, dtBegin);
            }

            // look for end of adjacent <dd> section
            ddEnd = htmlIn.find("</dd>", dtBegin);
            if (ddEnd == string::npos)
                return NULL;
            endOfCommands = ddEnd + 5;

            // get the command code
            size_t codeBegin = htmlIn.find("<a name=\"", dtBegin);
            if (codeBegin == string::npos || codeBegin > ddEnd)
                return NULL;

            // commands are encoded with this form: WS1a9193826455f5ff9110c71085341391d-5ca2
            // we retain only the last 4 digits
            size_t codeEnd = htmlIn.find('"', codeBegin += 45);
            if (codeEnd == string::npos)
                return NULL;

            Ptr<MgWebCommand> cmd = GetCommandFromCode(htmlIn.substr(codeBegin, codeEnd - codeBegin));
            if (cmd == NULL)
                return NULL;
            if (IsActionInUse(cmd, tgt))
            {
                // this action is defined and in use in this web layout. Carry it over in the HTML output
                htmlOut += "<dt>";
                string iconUrl;
                if (cmd->GetAction() != MgWebActions::Search)
                    iconUrl = MgUtil::WideCharToMultiByte(cmd->GetIconUrl());
                else
                    iconUrl = "../stdicons/icon_search.gif";    //there may be several search commands all using different icons, so use the default one
                htmlOut += "<img width=16 heigh=16 align=\"absbottom\" src=\"" + iconUrl + "\">&nbsp;";
                htmlOut += htmlIn.substr(dtBegin + 4, ddEnd - dtBegin - 1 + 6 - 4);
            }
        }
        else
        {
            if (first)
                return NULL;
            break;
        }
        cur = ddEnd + 1;
    }

    //copy the end of the document to the ouput
    htmlOut += htmlIn.substr(endOfCommands);

    // make a byte reader out of this string
    Ptr<MgByteSource> src = new MgByteSource((BYTE_ARRAY_IN)htmlOut.c_str(), (INT32)htmlOut.length());
    src->SetMimeType(MgMimeType::Html);

    return src->GetReader();
}

///////////////////////////////////////////////////////////////////////////
// Get a command from the command code in the getting started page
//
MgWebCommand* MgWebLayout::GetCommandFromCode(string code)
{
    map<string, INT32>::const_iterator it = cmdCodes.find(code);
    if (it == cmdCodes.end())
        return NULL;

    INT32 cmdId = it->second;
    for (INT32 ci=0; ci<m_commands->GetCount(); ++ci)
    {
        Ptr<MgWebCommand> cmd = m_commands->GetItem(ci);
        if (cmd->GetAction() == cmdId)
            return cmd.Detach();
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////
// Tells if the specified command is in use in this web layout
//
bool MgWebLayout::IsActionInUse(MgWebCommand* cmd, INT32 targetViewer)
{
    if ((cmd->GetTargetViewerType() & targetViewer) != targetViewer)
        return false;
    if (cmd->IsUsed())
        return true;
    return false;
}

// Initializes mapping between command codes in the HTML text and the command ids as known
// by web applications
//
static bool InitCmdCodes()
{
    cmdCodes["2e27"] = MgWebActions::Pan;
    cmdCodes["2e26"] = MgWebActions::PanUp;
    cmdCodes["2e25"] = MgWebActions::PanDown;
    cmdCodes["2e24"] = MgWebActions::PanRight;
    cmdCodes["2e23"] = MgWebActions::PanLeft;
    cmdCodes["2e22"] = MgWebActions::Zoom;
    cmdCodes["2e21"] = MgWebActions::ZoomIn;
    cmdCodes["2e20"] = MgWebActions::ZoomOut;
    cmdCodes["2e1f"] = MgWebActions::ZoomRectangle;
    cmdCodes["2e1e"] = MgWebActions::ZoomSelection;
    cmdCodes["2e1b"] = MgWebActions::FitWindow;
    cmdCodes["2e1d"] = MgWebActions::ViewPrevious;
    cmdCodes["2e1c"] = MgWebActions::ViewNext;
    cmdCodes["2e1a"] = MgWebActions::ViewRestore;
    cmdCodes["2e19"] = MgWebActions::Select;
    cmdCodes["2e18"] = MgWebActions::SelectRadius;
    cmdCodes["2e17"] = MgWebActions::SelectPolygon;
    cmdCodes["2e16"] = MgWebActions::SelectWithin;
    cmdCodes["2e15"] = MgWebActions::SelectClear;
    cmdCodes["2e14"] = MgWebActions::Refresh;
    cmdCodes["2e13"] = MgWebActions::Copy;
    cmdCodes["2e12"] = MgWebActions::PrintMap;
    cmdCodes["2e11"] = MgWebActions::GetPrintablePage;
    cmdCodes["2e10"] = MgWebActions::ViewOptions;
    cmdCodes["2e0f"] = MgWebActions::Buffer;
    cmdCodes["2e0e"] = MgWebActions::Measure;
    cmdCodes["2e0d"] = MgWebActions::Search;
    cmdCodes["2e0c"] = MgWebActions::MapTip;
    return true;
}

static bool initCmdCodes = InitCmdCodes();
