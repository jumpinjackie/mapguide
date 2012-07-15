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

#ifndef _MGFDOFILTERCOLLECTION_H_
#define _MGFDOFILTERCOLLECTION_H_

#include <FdoStd.h>

class FdoFilter;

/// \brief
/// The MgFdoFilterCollection class represents a collection of FdoFilter objects.
class MgFdoFilterCollection : public FdoCollection<FdoFilter, FdoException>
{
protected:
/// \cond DOXYGEN-IGNORE
    MgFdoFilterCollection() : FdoCollection<FdoFilter, FdoException>()
    {
    }

    virtual ~MgFdoFilterCollection()
    {
    }

    virtual void Dispose()
    {
        delete this;
    }
/// \endcond

public:
    /// \brief
    /// Constructs a default instance of an MgFdoFilterCollection.
    ///
    /// \return
    /// Returns MgFdoFilterCollection
    ///
    /*FDO_API*/ static MgFdoFilterCollection* Create();
};
#endif
