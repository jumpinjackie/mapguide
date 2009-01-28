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

#ifndef _MGCHANGELIST_H_
#define _MGCHANGELIST_H_

class MgChangeList;
template class MG_PLATFORMBASE_API Ptr<MgChangeList>;

/// \cond INTERNAL
class MG_PLATFORMBASE_API MgChangeList : public MgNamedSerializable
{
    DECLARE_CLASSNAME(MgChangeList)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// Construct a MgChangeList object
    //
    MgChangeList();      //for use by serialization only
    MgChangeList(CREFSTRING objectid, bool isLayer);

    //////////////////////////////////////////////////////////////////
    /// Get the object id this change is applied to
    //
    STRING GetObjectId();

    //////////////////////////////////////////////////////////////////
    /// Get the parameter value for this change
    //
    STRING GetParam();

    //////////////////////////////////////////////////////////////////
    /// Get the name for this change (alias for GetObjectId)
    //
    STRING GetName();

    //////////////////////////////////////////////////////////////////
    /// Tells if this object is a layer (if not, it's a group)
    //
    bool IsLayer();

    //////////////////////////////////////////////////////////////////
    /// Add a change for this object
    //
    void AddChange(MgObjectChange* change);

    //////////////////////////////////////////////////////////////////
    /// Get the number of changes for this object
    //
    int GetChangeCount();

    //////////////////////////////////////////////////////////////////
    /// Get the type of change at the specified index
    //
    MgObjectChange* GetChangeAt(INT32 index);

    //////////////////////////////////////////////////////////
    /// Serializes data to a TCP/IP stream.
    ///
    void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////
    /// Deserializes data from a TCP/IP stream.
    ///
    void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// Does it allow to set the name
    ///
    bool CanSetName();

protected:

    //////////////////////////////////////////////////////////////////
    /// Dispose this object
    //
    virtual void Dispose();

    STRING      m_objectId;
    bool        m_isLayer;

    Ptr<MgDisposableCollection>  m_changes;
};
/// \endcond

#endif
