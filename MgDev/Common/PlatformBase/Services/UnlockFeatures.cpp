//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

MG_IMPL_DYNCREATE(MgUnlockFeatures)

MgUnlockFeatures::MgUnlockFeatures()
{
}

MgUnlockFeatures::MgUnlockFeatures(CREFSTRING className, CREFSTRING filterText)
{
    if (className.empty() || filterText.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgUnlockFeatures.MgUnlockFeatures",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    m_className = className;
    m_filterText = filterText;
}

void MgUnlockFeatures::Dispose()
{
    delete this;
}

INT32 MgUnlockFeatures::GetClassId()
{
    return m_cls_id;
}

INT32 MgUnlockFeatures::GetCommandType() const
{
    return MgFeatureCommandType::UnlockFeatures;
}

STRING MgUnlockFeatures::GetFeatureClassName() const
{
    return m_className;
}

STRING MgUnlockFeatures::GetFilterText() const
{
    return m_filterText;
}

void MgUnlockFeatures::Serialize(MgStream* stream)
{
    stream->WriteString(m_className);
    stream->WriteString(m_filterText);
}

void MgUnlockFeatures::Deserialize(MgStream* stream)
{
    stream->GetString(m_className);
    stream->GetString(m_filterText);
}
