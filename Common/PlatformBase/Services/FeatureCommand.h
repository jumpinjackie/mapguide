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

#ifndef _MGFEATURECOMMAND_H
#define _MGFEATURECOMMAND_H

/// \defgroup MgFeatureCommand MgFeatureCommand
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Is the abstract base class for feature command classes:
/// MgDeleteFeatures, MgInsertFeatures, and MgUpdateFeatures.
/// \remarks
/// Feature commands are added to a an MgFeatureCommandCollection
/// object, which is then passed in a call to the
/// MgFeatureService::UpdateFeatures method.
///

class MG_PLATFORMBASE_API  MgFeatureCommand : public MgSerializable
{
PUBLISHED_API:
    ////////////////////////////////////////////
    /// \brief
    /// Gets the type of the command.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetCommandType() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetCommandType() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetCommandType() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns one of the constants defined in
    /// MgFeatureCommandType.
    ///
    virtual INT32 GetCommandType() const = 0;  /// __get
};
/// \}

#endif // _MGFEATUREMANIPULATIONCOMMAND_H
