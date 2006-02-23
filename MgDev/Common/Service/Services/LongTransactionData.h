//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef _MG_LONG_TRANSACTION_DATA_H
#define _MG_LONG_TRANSACTION_DATA_H

/// \ingroup NOT_EXPOSED_module

class MgStream;

class MgLongTransactionData;
template class Ptr<MgLongTransactionData>;

// INTERNAL_CLASS
class MG_SERVICE_API MgLongTransactionData : public MgNamedSerializable
{
    DECLARE_CREATE_OBJECT()
    DECLARE_CLASSNAME(MgLongTransactionData)

public:

    MgLongTransactionData();

    STRING GetName();
    STRING GetDescription();
    STRING GetOwner();
    MgDateTime* GetCreationDate();
    bool IsActive();
    bool IsFrozen();

    void SetName(CREFSTRING name);
    void SetDescription(CREFSTRING desc);
    void SetOwner(CREFSTRING owner);
    void SetCreationDate(MgDateTime* dateTime);
    void SetActiveStatus(bool active);
    void SetFrozenStatus(bool frozen);

    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    void ToXml(string& xmlStr);
    bool CanSetName();

EXTERNAL_API:

    ~MgLongTransactionData();

protected:

    virtual void Dispose()
    {
        delete this;
    }

private:

    STRING m_name;
    STRING m_desc;
    STRING m_owner;
    Ptr<MgDateTime> m_dateTime;
    bool m_isActive;
    bool m_isFrozen;

INTERNAL_API:

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

CLASS_ID:
    static const INT32 m_cls_id = FeatureService_LongTransactionData;
};

#endif
