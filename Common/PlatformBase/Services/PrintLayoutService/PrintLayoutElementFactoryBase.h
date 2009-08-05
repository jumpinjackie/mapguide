//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MGPRINTLAYOUTELEMENTFACTORYBASE_H_
#define MGPRINTLAYOUTELEMENTFACTORYBASE_H_

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Defines the MgPrintLayoutElementFactoryBase abstract class.
///
class MG_PLATFORMBASE_API MgPrintLayoutElementFactoryBase : public MgDisposable
{
    DECLARE_CLASSNAME(MgPrintLayoutElementFactoryBase)

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgPrintLayoutElementFactoryBase object.
    ///
    MgPrintLayoutElementFactoryBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgPrintLayoutElementFactoryBase object.
    ///
    virtual ~MgPrintLayoutElementFactoryBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgPrintLayoutElementBase object.
    ///
    /// \return
    /// Returns the MgPrintLayoutElementBase object.
    ///
    virtual MgPrintLayoutElementBase* CreateObject();

protected:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes this object.
    ///
    virtual void Dispose();
};

#endif
