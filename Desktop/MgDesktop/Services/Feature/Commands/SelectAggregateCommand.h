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

#ifndef _MgdSelectAggregateCommand_H_
#define _MgdSelectAggregateCommand_H_

class MgdSelectAggregateCommand : public MgdFeatureServiceCommand
{
    DECLARE_CLASSNAME(MgdSelectAggregateCommand)

public:
    MgdSelectAggregateCommand(MgResourceIdentifier* resource);
    virtual ~MgdSelectAggregateCommand();

    virtual FdoIdentifierCollection* GetPropertyNames();

    virtual void SetDistinct( bool value );
    virtual bool GetDistinct( );

    virtual void SetFetchSize(FdoInt32 fetchSize);
    virtual FdoInt32 GetFetchSize();

    virtual FdoIdentifierCollection* GetOrdering();
    virtual void SetOrderingOption( FdoOrderingOption  option );
    virtual FdoOrderingOption GetOrderingOption( );

    virtual FdoIdentifierCollection* GetGrouping();
    virtual void SetGroupingFilter( FdoFilter* filter );
    virtual FdoFilter* GetGroupingFilter( );

    virtual void SetFeatureClassName(FdoString* value);
    virtual void SetFilter(FdoString* value);
    virtual void SetFilter(FdoFilter* value);

    virtual FdoFilter* GetFilter();

    virtual MgReader* Execute();
    MgReader* ExecuteJoined(MgStringCollection* idPropNames, bool bForceOneToOne);
    virtual bool IsSupportedFunction(FdoFunction* fdoFunc);
    virtual bool SupportsSelectGrouping();
    virtual bool SupportsSelectOrdering();
    virtual bool SupportsSelectDistinct();

    virtual MgReader* ExecuteWithLock() { NOT_IMPLEMENTED(L"MgdSelectAggregateCommand::ExecuteWithLock"); }

    virtual void Dispose()
    {
        delete this;
    }

    virtual FdoJoinCriteriaCollection* GetJoinCriteria();
    virtual void SetAlias(FdoString* alias);

private:
    Ptr<MgdFeatureConnection> m_connection;
    STRING m_providerName;
    FdoPtr<FdoISelectAggregates> m_command;

    FdoPtr<FdoFilter> m_filter;
};

#endif
