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

#ifndef _MGWEBUITARGETCOMMAND_H_
#define _MGWEBUITARGETCOMMAND_H_

class MgWebUiTargetCommand;
template class MG_WEBAPP_API Ptr<MgWebUiTargetCommand>;

class MG_WEBAPP_API MgWebUiTargetCommand : public MgWebCommand
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the UI target for the command. One value of the MgWebTargetType class.
    /// </summary>
    /// <returns>
    /// Returns the UI target for the command.
    /// </returns>
    INT32 GetTarget();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the name of the target for the command.
    /// Applicable only if the type of target is 'SpecifiedFrame'
    /// </summary>
    /// <returns>
    /// Returns the name of the target for the command.
    /// </returns>
    STRING GetTargetName();

INTERNAL_API:

    MgWebUiTargetCommand();

    ///////////////////////////////////////////////////////////////////////////
    // Set the UI target
    //
    void SetTarget(INT32 targetType);

    ///////////////////////////////////////////////////////////////////////////
    // Set the UI target name
    //
    void SetTargetName(STRING targetName);

CLASS_ID:
    static const INT32 m_cls_id = WebApp_UiTargetCommand;

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Get the unique identifier for the class
    //
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    // Dispose this object
    //
    virtual void Dispose();

protected:
    INT32       m_target;
    STRING      m_targetName;
};

#endif // _MGWEBUITARGETCOMMAND_H_
