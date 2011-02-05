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

#ifndef _MGNAMEDSERIALIZABLE_H_
#define _MGNAMEDSERIALIZABLE_H_

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// Root class for all named serializable objects.
class MgNamedSerializable;
template class MG_FOUNDATION_API Ptr<MgNamedSerializable>;

class MG_FOUNDATION_API MgNamedSerializable : public MgSerializable
{
EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default destructor for this class.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual ~MgNamedSerializable();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the name of the object.
    ///
    /// \return
    /// Returns the name of object
    ///
    virtual STRING GetName() = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    virtual bool CanSetName() = 0;
};
/// \endcond

#endif
