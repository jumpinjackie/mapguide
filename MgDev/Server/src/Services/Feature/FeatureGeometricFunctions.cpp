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

#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureUtil.h"
#include "FeatureDistribution.h"
#include "FeatureGeometricFunctions.h"
#include "DataReaderCreator.h"
#include "StringDataReaderCreator.h"
#include "GeometryDataReaderCreator.h"
#include <algorithm>
#include "UniqueFunction.h"

MgFeatureGeometricFunctions::MgFeatureGeometricFunctions()
{
    m_type = MgPropertyType::Null;
    m_reader = NULL;
    m_customFunction = NULL;
    m_propertyAlias = L"";
    m_extentsInitialized = false;
}

MgFeatureGeometricFunctions::MgFeatureGeometricFunctions(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias)
{
    Initialize(reader, customFunction, propertyAlias); // Initialize the instance
}

void MgFeatureGeometricFunctions::Initialize(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias)
{
    CHECKNULL((MgReader*)reader, L"MgFeatureDistribution.Initialize");
    CHECKNULL((FdoFunction*)customFunction, L"MgFeatureDistribution.Initialize");

    m_type = MgServerFeatureUtil::GetPropertyDefinition(reader, m_propertyName);

    // TODO: Should we really check this, may be we can ignore ??
    // because we can only come to here if property type is numeric
    this->CheckSupportedPropertyType();

    // We must have an property alias
    // Though we can name a property with same name as function expression
    // But Fdo forces to have an alias. Therefore we implement this restriction.
    if (propertyAlias.empty())
    {
        STRING message = MgServerFeatureUtil::GetMessage(L"MgMissingPropertyAlias");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgFeatureDistribution.Initialize", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    m_reader = SAFE_ADDREF(reader);
    m_customFunction = FDO_SAFE_ADDREF(customFunction);
    m_propertyAlias = propertyAlias;
    m_extentsInitialized = false;
}


MgFeatureGeometricFunctions::~MgFeatureGeometricFunctions()
{
    if (m_customFunction != NULL)
    {
        m_customFunction->Release();
    }
}

// Execute the function
MgReader* MgFeatureGeometricFunctions::Execute()
{
    CHECKNULL((MgReader*)m_reader, L"MgFeatureGeometricFunctions.Execute");
    CHECKNULL(m_customFunction, L"MgFeatureGeometricFunctions.Execute");

    Ptr<MgGeometryCollection> geomCol = new MgGeometryCollection();

    MG_LOG_TRACE_ENTRY(L"MgFeatureGeometricFunctions::Execute");
    while(m_reader->ReadNext())
    {
        // Get the geometry
        Ptr<MgGeometry> val = GetValue();

        // Get the envelope
        Ptr<MgEnvelope> envl = val->Envelope();
        Ptr<MgCoordinate> lcoord;
        Ptr<MgCoordinate> rcoord;
        if (envl != NULL)
        {
            lcoord = envl->GetLowerLeftCoordinate();
            rcoord = envl->GetUpperRightCoordinate();
        }

        // Compare with min/max coordinates
        ComputeExtents(lcoord, rcoord);
    }

    // Execute the operation on the collected extents
    Ptr<MgGeometryCollection> finalResult = ExecuteOperation();

    // Create FeatureReader from geometric values
    Ptr<MgReader> reader = GetReader(finalResult);

    return SAFE_ADDREF((MgReader*)reader);
}

void MgFeatureGeometricFunctions::ComputeExtents(MgCoordinate* lowerLeft,
                                                    MgCoordinate* upperRight)
{
    CHECKNULL((MgCoordinate*)lowerLeft, L"MgFeatureGeometricFunctions.ComputeExtents");
    CHECKNULL((MgCoordinate*)upperRight, L"MgFeatureGeometricFunctions.ComputeExtents");

    if (!m_extentsInitialized)
    {
        m_minX = lowerLeft->GetX();
        m_minY = lowerLeft->GetY();
        m_minZ = lowerLeft->GetZ();
        m_minM = lowerLeft->GetM();

        m_maxX = upperRight->GetX();
        m_maxY = upperRight->GetY();
        m_maxZ = upperRight->GetZ();
        m_maxM = upperRight->GetM();

        m_extentsInitialized = true;
    }
    else
    {
        if (lowerLeft->GetX() < m_minX)
            m_minX = lowerLeft->GetX();
        if (lowerLeft->GetY() < m_minY)
            m_minY = lowerLeft->GetY();
        if (lowerLeft->GetZ() < m_minZ)
            m_minZ = lowerLeft->GetZ();
        if (lowerLeft->GetM() < m_minM)
            m_minM = lowerLeft->GetM();

        if (upperRight->GetX() > m_maxX)
            m_maxX = upperRight->GetX();
        if (upperRight->GetY() > m_maxY)
            m_maxY = upperRight->GetY();
        if (upperRight->GetZ() > m_maxZ)
            m_maxZ = upperRight->GetZ();
        if (upperRight->GetM() > m_maxM)
            m_maxM = upperRight->GetM();
    }
}

MgGeometryCollection* MgFeatureGeometricFunctions::ExecuteOperation()
{
    INT32 funcCode = -1;

    Ptr<MgGeometryCollection> result;

    // Get the arguments from the FdoFunction
    STRING propertyName;
    bool supported = MgServerFeatureUtil::FindCustomFunction(m_customFunction, funcCode);

    if (supported)
    {
        switch(funcCode)
        {
            case EXTENT:
            {
                // Create the geometry collection for the extents
                MgGeometryFactory factory;

                // TODO: Assuming we are two dimensional for now.

                //Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXYZM(minX, minY, minZ, minM);
                //Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXYZM(maxX, maxY, maxZ, maxM);

                Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXY(m_minX, m_minY);
                Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXY(m_maxX, m_minY);
                Ptr<MgCoordinate> coord3 = factory.CreateCoordinateXY(m_maxX, m_maxY);
                Ptr<MgCoordinate> coord4 = factory.CreateCoordinateXY(m_minX, m_maxY);

                Ptr<MgCoordinateCollection> coordCol = new MgCoordinateCollection();
                coordCol->Add(coord1);
                coordCol->Add(coord2);
                coordCol->Add(coord3);
                coordCol->Add(coord4);

                Ptr<MgLinearRing> outerRing = factory.CreateLinearRing(coordCol);
                Ptr<MgGeometry> geom = factory.CreatePolygon(outerRing, NULL);

                result = new MgGeometryCollection();
                result->Add(geom);

                break;
            }
            default:
            {
                STRING message = MgServerFeatureUtil::GetMessage(L"MgCustomFunctionNotSupported");

                MgStringCollection arguments;
                arguments.Add(message);
                throw new MgFeatureServiceException(
                    L"MgFeatureGeometricFunctions.ExecuteOperation",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }
    }

    return result.Detach();
}

// Check whether property type is a supported type
void MgFeatureGeometricFunctions::CheckSupportedPropertyType()
{
    bool supported = false;
    switch(m_type)
    {
        case MgPropertyType::Geometry:
        {
            supported = true;
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(
                L"MgFeatureGeometricFunctions.CheckSupportedPropertyType", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    return;
}


// Get the value of property
MgGeometry* MgFeatureGeometricFunctions::GetValue()
{
    Ptr<MgGeometry> geom;

    if (!m_reader->IsNull(m_propertyName))
    {
        switch(m_type)
        {
            case MgPropertyType::Geometry:
            {
                Ptr<MgByteReader> reader = m_reader->GetGeometry(m_propertyName);
                if (reader != NULL)
                {
                    MgAgfReaderWriter agfReader;
                    geom = agfReader.Read(reader);
                }
                break;
            }
            default:
            {
                throw new MgInvalidPropertyTypeException(
                    L"MgFeatureGeometricFunctions.GetValue",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }

    return geom.Detach();
}




// Create the reader for string properties
MgReader* MgFeatureGeometricFunctions::GetReader(MgGeometryCollection* geomCol)
{
    Ptr<MgDataReader> dataReader;

    switch(m_type)
    {
        case MgPropertyType::Geometry:
        {
            Ptr<MgGeometryDataReaderCreator> drCreator = new MgGeometryDataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(geomCol);
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(
                L"MgFeatureGeometricFunctions.GetReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    return dataReader.Detach();
}
