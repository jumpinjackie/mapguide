//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "PlatformBase.h"
#include "FeatureService.h"

MG_IMPL_DYNCREATE(MgLockFeatures)

INT32 MgLockFeatures::GetCommandType() const
{
    return MgFeatureCommandType::LockFeatures;
}

MgLockFeatures::MgLockFeatures()
{
}

MgLockFeatures::MgLockFeatures(CREFSTRING className, CREFSTRING filterText)
{
    if (className.empty() || filterText.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgLockFeatures.MgLockFeatures",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    m_className = className;
    m_filterText = filterText;
}

STRING MgLockFeatures::GetFeatureClassName() const
{
    return m_className;
}

STRING MgLockFeatures::GetFilterText() const
{
    return m_filterText;
}

INT32 MgLockFeatures::GetClassId()
{
    return m_cls_id;
}

void MgLockFeatures::Dispose()
{
    delete this;
}

void MgLockFeatures::Serialize(MgStream* stream)
{
    stream->WriteString(m_className);
    stream->WriteString(m_filterText);
}

void MgLockFeatures::Deserialize(MgStream* stream)
{
    stream->GetString(m_className);
    stream->GetString(m_filterText);
}
