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

/// \cond INTERNAL
// INTERNAL USE ONLY

#ifndef _MGOBJECTCHANGE_H_
#define _MGOBJECTCHANGE_H_

class MgObjectChange;
template class MG_PLATFORMBASE_API Ptr<MgObjectChange>;

class MG_PLATFORMBASE_API MgObjectChange : public MgDisposable
{
    DECLARE_CLASSNAME(MgObjectChange)

INTERNAL_API:

    enum ChangeType
    {
        removed,
        added,
        visibilityChanged,
        displayInLegendChanged,
        legendLabelChanged,
        parentChanged,
        selectabilityChanged,
        definitionChanged
    };

    //////////////////////////////////////////////////////////////////
    /// Construct an MgObjectChange object
    //
    MgObjectChange();      //for use by serialization only
    MgObjectChange(ChangeType type);

    //////////////////////////////////////////////////////////////////
    /// Get the type of change at the specified index
    //
    ChangeType GetType();

    //////////////////////////////////////////////////////////////////
    /// Get the parameter value for this change
    //
    STRING GetParam();

    //////////////////////////////////////////////////////////////////
    /// Set the parameter value for this change
    //
    void SetParam(CREFSTRING param);

protected:

    //////////////////////////////////////////////////////////////////
    /// Dispose this object
    //
    virtual void Dispose();

    ChangeType  m_type;
    STRING      m_param;
};

#endif

/// \endcond
