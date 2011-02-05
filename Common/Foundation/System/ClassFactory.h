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

#ifndef MGCLASSFACTORY_H_
#define MGCLASSFACTORY_H_

#include <map>


/// Typedef for class creation functions
typedef MgObject* (*ClassCreatorFunc)();


/// \cond INTERNAL

/// INTERNAL_CLASS
/// \brief
/// Class registration for Foundation derived applications
class MG_FOUNDATION_API MgClassFactory : public MgDisposable
{
    DECLARE_CLASSNAME(MgClassFactory)

INTERNAL_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns global object instance
    ///
    /// \return
    /// Class factory for application
    ///
    static MgClassFactory* GetInstance();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Registers a class creator with the factory
    ///
    ///
    void Register(int classId, ClassCreatorFunc creator);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates object
    ///
    /// \return
    /// Newly created object for requested classId
    ///
    MgObject* CreateMgObject(int classId);

protected:

    virtual void Dispose();

private:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Object constructor
    ///
    MgClassFactory();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Declaration of class Registry
    std::map<int, ClassCreatorFunc> m_classCreators;

};
/// \endcond

#endif
