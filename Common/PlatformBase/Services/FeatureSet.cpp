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

#include "PlatformBase.h"
#include "FeatureService.h"

MG_IMPL_DYNCREATE(MgFeatureSet)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgFeatureSet::MgFeatureSet()
{
    m_bpCollection = new MgBatchPropertyCollection();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgFeatureSet::~MgFeatureSet()
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Populates this feature set from a FeatureReader.
/// </summary>
/// <param name="featureReader">FeatureReader input</param>
/// <remarks>This methods is typically used to creates a feature
/// set based on the result of a select command</remarks>
void MgFeatureSet::FromReader(MgFeatureReader* featureReader)
{
    MG_UNUSED_ARG(featureReader);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of features in this feature set.
/// </summary>
/// <returns>Returns the number of features in this set.</returns>
INT32 MgFeatureSet::GetCount()
{
    return m_bpCollection->GetCount();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a feature from the feature set.
/// </summary>
/// <param name="index">Index of the feature in the feature set</param>
/// <returns>PropertyValueCollection representing the feature.</returns>
MgPropertyCollection* MgFeatureSet::GetFeatureAt(INT32 index)
{
    return m_bpCollection->GetItem(index);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds a feature to the feature set.
/// </summary>
/// <param name="feature">PropertyValueCollection representing the feature to be added</param>
void MgFeatureSet::AddFeature(MgPropertyCollection* feature)
{
    m_bpCollection->Add(feature);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the definition of the feature class.
/// </summary>
/// <returns>Returns the definition of the feature class.</returns>
MgClassDefinition* MgFeatureSet::GetClassDefinition()
{
    return SAFE_ADDREF((MgClassDefinition*)m_classDefinition);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the definition of the feature class.
/// </summary>
/// <param name="classDef">Definition of the feature class</param>
void MgFeatureSet::SetClassDefinition(MgClassDefinition* classDef)
{
    m_classDefinition = SAFE_ADDREF(classDef);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the set of features as a BatchPropertyValueCollection
/// </summary>
/// <returns>BatchPropertyValueCollection containing all the features</returns>
/// <remarks>This methods is used to pass a set of features to an insert command</remarks>
MgBatchPropertyCollection* MgFeatureSet::GetFeatures()
{
    return SAFE_ADDREF((MgBatchPropertyCollection*)m_bpCollection);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgFeatureSet::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
///<summary>
/// Dispose this object.
///</summary>
void MgFeatureSet::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgFeatureSet::Serialize(MgStream* stream)
{
    stream->WriteObject((MgBatchPropertyCollection*)m_bpCollection);
    stream->WriteObject((MgClassDefinition*)m_classDefinition);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgFeatureSet::Deserialize(MgStream* stream)
{
    m_bpCollection = (MgBatchPropertyCollection*)stream->GetObject();
    m_classDefinition = (MgClassDefinition*)stream->GetObject();
}


void MgFeatureSet::ClearFeatures()
{
    CHECKNULL(m_bpCollection, L"MgFeatureSet.ClearFeatures");
    m_bpCollection->Clear();
}


void MgFeatureSet::AddFeatures(MgBatchPropertyCollection* bpCol)
{
    if (NULL == bpCol)
        return;

    // TODO: This algorithm is not efficient we should find a way
    // to directly set new batch property collection and remove the old one.
    INT32 cnt = bpCol->GetCount();
    for (INT32 i=0; i < cnt; i++)
    {
        Ptr<MgPropertyCollection> ptrCol = bpCol->GetItem(i);
        this->AddFeature(ptrCol);
    }
}
