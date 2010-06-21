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

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of properties in the result set.
/// </summary>
/// <returns>Returns the number of properties.</returns>
INT32 MgFeatureReader::GetPropertyCount()
{
    Ptr<MgClassDefinition> classDef = this->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgFeatureReader.GetPropertyCount");

    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgFeatureReader.GetPropertyCount");

    return propDefCol->GetCount();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the property at the given ordinal position.
/// </summary>
/// <param name="index">Input the position of the property.</param>
/// <returns>Returns the property name</returns>
STRING MgFeatureReader::GetPropertyName(INT32 index)
{
    Ptr<MgClassDefinition> classDef = this->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgFeatureReader.GetPropertyName");

    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgFeatureReader.GetPropertyName");

    Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(index);
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgFeatureReader.GetPropertyName");

    return propDef->GetName();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the index of the property with the specified property name.
/// </summary>
/// <param name="propertyName">Input the name of the property.</param>
/// <returns>Returns the property index</returns>
INT32 MgFeatureReader::GetPropertyIndex(CREFSTRING propertyName)
{
    Ptr<MgClassDefinition> classDef = this->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgFeatureReader.GetPropertyIndex");

    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgFeatureReader.GetPropertyIndex");

    INT32 index = propDefCol->IndexOf(propertyName);
    if (-1 != index)
    {
        return index;
    }
    else
    {
        throw new MgObjectNotFoundException(L"MgFeatureReader.GetPropertyIndex", __LINE__, __WFILE__, NULL, L"", NULL);
        return -1; // to suppress compiler warning.
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the data type of the property with the specified name.
/// Please refer to MgPropertyType for list of values
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the type of the property.</returns>
INT32 MgFeatureReader::GetPropertyType(CREFSTRING propertyName)
{
    Ptr<MgClassDefinition> classDef = this->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgFeatureReader.GetPropertyType");

    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgFeatureReader.GetPropertyType");

    Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(propertyName);
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgFeatureReader.GetPropertyType");

    return GetMgPropertyType(propDef);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the data type of the property at the specified index.
/// Please refer to MgPropertyType for list of values
/// </summary>
/// <param name="index">Input the property index.</param>
/// <returns>Returns the type of the property.</returns>
INT32 MgFeatureReader::GetPropertyType(INT32 index)
{
    Ptr<MgClassDefinition> classDef = this->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgFeatureReader.GetPropertyType");

    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgFeatureReader.GetPropertyType");

    Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(index);
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgFeatureReader.GetPropertyType");

    return GetMgPropertyType(propDef);
}

INT16 MgFeatureReader::GetMgPropertyType(MgPropertyDefinition* propDef)
{
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgFeatureReader.GetMgPropertyType")
    INT32 mgPropType = 0;

    // Whether it is data,geometry,raster,object or association property
    INT16 type = propDef->GetPropertyType();
    switch(type)
    {
        case MgFeaturePropertyType::DataProperty:
        {
            mgPropType = ((MgDataPropertyDefinition*)propDef)->GetDataType();
            break;
        }
        case MgFeaturePropertyType::GeometricProperty:
        {
            mgPropType = MgPropertyType::Geometry;
            break;
        }
        case MgFeaturePropertyType::RasterProperty:
        {
            mgPropType = MgPropertyType::Raster;
            break;
        }
        case MgFeaturePropertyType::ObjectProperty:
        {
            break;
        }
        case MgFeaturePropertyType::AssociationProperty:
        {
            break;
        }
    }
    return ((INT16)mgPropType);
}

INT32 MgFeatureReader::GetReaderType()
{
    return MgReaderType::FeatureReader;
}
