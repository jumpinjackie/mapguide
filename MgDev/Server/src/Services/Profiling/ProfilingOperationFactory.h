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

#ifndef MGPROFILINGOPERATIONFACTORY_H
#define MGPROFILINGOPERATIONFACTORY_H

///////////////////////////////////////////////////////////////////////////
/// The MgProfilingOperationFactory class is an object that allows one to get
/// an IMgOperationHandler object that corresponds to given ID and version
/// parameters.  It currently is merely a wrapper object for one static
/// function.
///
/// TODO:  there could be several operation factories dependent on
/// id, version, and service, so this static function will have to
/// be replaced.
class MgProfilingOperationFactory
{
    ///////////////////////////////////////////////////////////////////////////
    /// Constructors/Destructor
    protected :
        MgProfilingOperationFactory();

    ///////////////////////////////////////////////////////////////////////////
    /// Methods
    public :
        static IMgOperationHandler* GetOperation(ACE_UINT32 operationId,
            ACE_UINT32 operationVersion);
};

#endif
