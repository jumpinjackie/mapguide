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

#ifndef _MgdFeatureServiceCommand_H_
#define _MgdFeatureServiceCommand_H_

class MgdFeatureServiceCommand : public MgDisposable
{
public:
    static MgdFeatureServiceCommand* CreateCommand(MgResourceIdentifier* resource, FdoCommandType commandType);

    virtual FdoIdentifierCollection* GetPropertyNames() = 0;

    virtual void SetDistinct( bool value ) = 0;
    virtual bool GetDistinct( ) = 0;

    virtual void SetFetchSize(FdoInt32 fetchSize) = 0;
    virtual FdoInt32 GetFetchSize() = 0;

    virtual FdoIdentifierCollection* GetOrdering() = 0;
    virtual void SetOrderingOption( FdoOrderingOption  option ) = 0;
    virtual FdoOrderingOption GetOrderingOption( ) = 0;

    virtual FdoIdentifierCollection* GetGrouping() = 0;
    virtual void SetGroupingFilter( FdoFilter* filter ) = 0;
    virtual FdoFilter* GetGroupingFilter( ) = 0;

    virtual void SetFeatureClassName(FdoString* value) = 0;
    virtual void SetFilter(FdoString* value) = 0;
    virtual void SetFilter(FdoFilter* value) = 0;

    virtual FdoFilter* GetFilter() = 0;

    virtual bool IsSupportedFunction(FdoFunction* fdoFunc) = 0;
    virtual bool SupportsSelectGrouping() = 0;
    virtual bool SupportsSelectOrdering() = 0;
    virtual bool SupportsSelectDistinct() = 0;

    virtual MgReader* Execute() = 0;
    virtual MgReader* ExecuteWithLock() = 0;

protected:

    virtual bool IsFdoSupportedFunction(FdoIConnection* connection, FdoFunction* fdoFunc);
    virtual bool SupportsSelectGrouping(FdoIConnection* connection);
    virtual bool SupportsSelectOrdering(FdoIConnection* connection);
    virtual bool SupportsSelectDistinct(FdoIConnection* connection);
};

#endif
