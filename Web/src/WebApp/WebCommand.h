//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _MGWEBCOMMAND_H_
#define _MGWEBCOMMAND_H_

class MgWebCommand;
template class MG_WEBAPP_API Ptr<MgWebCommand>;

class MG_WEBAPP_API MgWebCommand : public MgNamedSerializable
{
PUBLISHED_API:

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the command name
    /// </summary>
    /// <returns>
    /// Command name
    /// </returns>
    STRING GetName();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the command label
    /// </summary>
    /// <returns>
    /// Command name
    /// </returns>
    STRING GetLabel();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the action associated with this command. One value of the MgWebActions.h
    /// </summary>
    /// <returns>
    /// Action
    /// </returns>
    INT32 GetAction();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the url of the icon representing this command
    /// </summary>
    /// <returns>
    /// Url of the icon representing this command
    /// </returns>
    STRING GetIconUrl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the url of the icon representing this command in disabled state
    /// </summary>
    /// <returns>
    /// Url of the icon representing this command in disabled state
    /// </returns>
    STRING GetDisabledIconUrl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the tooltip describing this command
    /// </summary>
    /// <returns>
    /// Tooltip text
    /// </returns>
    STRING GetTooltip();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the long description of this comand. Appears in the status bar
    /// </summary>
    /// <returns>
    /// Tooltip text
    /// </returns>
    STRING GetDescription();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the type of viewer this command is intended for
    /// </summary>
    /// <returns>
    /// Target viewer type. One value of the MgWebTargetViewerType class
    /// </returns>
    INT32 GetTargetViewerType();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Tells if this commands is used by at least one button, task or menu option
    /// </summary>
    /// <returns>
    /// Target viewer type. One value of the MgWebTargetViewerType class
    /// </returns>
    bool IsUsed();

INTERNAL_API:

    MgWebCommand(CREFSTRING name, STRING iconUrl, STRING tooltip);
    MgWebCommand();

    void SetName(CREFSTRING name);
    void SetLabel(CREFSTRING label);
    void SetAction(INT32 action);
    void SetIconUrl(CREFSTRING iconUrl);
    void SetDisabledIconUrl(CREFSTRING iconUrl);
    void SetTooltip(CREFSTRING tooltipText);
    void SetDescription(CREFSTRING description);
    void SetTargetViewerType(INT32 targetViewerType);
    void SetUsed(bool used);

    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    bool CanSetName();

protected:
    virtual INT32 GetClassId();
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = WebApp_Command;

protected:
    STRING      m_name;
    STRING      m_label;
    INT32       m_action;
    STRING      m_iconUrl;
    STRING      m_disabledIconUrl;
    STRING      m_tooltip;
    STRING      m_description;
    INT32       m_targetViewerType;
    bool        m_isUsed;
};

#endif // _MGWEBCOMMAND_H_
