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

#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureUtil.h"
#include "FeatureDistribution.h"
#include "FeatureGeometricFunctions.h"
#include "ProxyFeatureReader.h"
#include "ProxyDataReader.h"
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
    m_customFunction = GIS_SAFE_ADDREF(customFunction);
    m_propertyAlias = propertyAlias;
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
    // TODO: Can this be optimized to process them as they are read?
    while(m_reader->ReadNext())
    {
        Ptr<MgGeometry> val = GetValue();
        if (val != NULL)
        {
            geomCol->Add(val);
        }
    }

    // Execute the operation on the collected values
    Ptr<MgGeometryCollection> finalResult = ExecuteOperation(geomCol);

    // Create FeatureReader from geometric values
    Ptr<MgReader> reader = GetReader(finalResult);

    return SAFE_ADDREF((MgReader*)reader);
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
    Ptr<MgGeometry> geom = (MgGeometry*)NULL;

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
                    L"MgFeatureGeometricFunctions.CheckSupportedPropertyType",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }

    return SAFE_ADDREF((MgGeometry*)geom);
}


// Execute the function
MgGeometryCollection* MgFeatureGeometricFunctions::ExecuteOperation(MgGeometryCollection* geomCol)
{
    INT32 funcCode = -1;

    Ptr<MgGeometryCollection> result = (MgGeometryCollection*)NULL;

    // Get the arguments from the FdoFunction
    STRING propertyName;
    bool supported = MgServerFeatureUtil::FindCustomFunction(m_customFunction, funcCode);

    if (supported)
    {
        switch(funcCode)
        {
            case EXTENT:
            {
                result = ComputeExtents(geomCol);
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

    return SAFE_ADDREF((MgGeometryCollection*)result);
}

MgGeometryCollection* MgFeatureGeometricFunctions::ComputeExtents(MgGeometryCollection* geomCol)
{
    Ptr<MgGeometryCollection> result = (MgGeometryCollection*)NULL;

    // NULL input or no geometry, we have nothing to do
    if ((geomCol == NULL) ||
        (0 == geomCol->GetCount()))
    {
        return result; // NULL
    }

    CoordinateCollection lowerLeft;
    CoordinateCollection upperRight;

    INT32 cnt = geomCol->GetCount();
    for (int i = 0; i < cnt; i++)
    {
        Ptr<MgGeometry> geom = geomCol->GetItem(i);
        if (geom != NULL)
        {
            Ptr<MgEnvelope> envl = geom->Envelope();
            if (envl != NULL)
            {
                Ptr<MgCoordinate> lcoord = envl->GetLowerLeftCoordinate();
                Ptr<MgCoordinate> rcoord = envl->GetUpperRightCoordinate();

                AddCoordinate(lcoord, lowerLeft);
                AddCoordinate(rcoord, upperRight);
            }
        }
    }

    // Extent will be sent back as polygon (rectangle)
    Ptr<MgGeometry> extent = ComputeExtents(lowerLeft, upperRight);

    // The reason to return collection instead of a single geometry is
    // that it might be required in future. But there is no use-case
    // right now which has this requirement.
    // This is just a room for extensiblity.
    Ptr<MgGeometryCollection> resultGeomCol = new MgGeometryCollection();
    resultGeomCol->Add(extent);

    return SAFE_ADDREF((MgGeometryCollection*)resultGeomCol);
}

MgGeometry* MgFeatureGeometricFunctions::ComputeExtents(CoordinateCollection& lowerLeft,
                                                        CoordinateCollection& upperRight)
{
    // TODO:
    // How to deal here with more than two dimension
    // It is possible to have different geometry types in a single class
    // some may be two dimensional and others may be 3 or 4.
    double minX = MgServerFeatureUtil::Minimum(lowerLeft.x_coords);
    double minY = MgServerFeatureUtil::Minimum(lowerLeft.y_coords);
    double minZ = MgServerFeatureUtil::Minimum(lowerLeft.z_coords);
    double minM = MgServerFeatureUtil::Minimum(lowerLeft.m_coords);

    double maxX = MgServerFeatureUtil::Maximum(upperRight.x_coords);
    double maxY = MgServerFeatureUtil::Maximum(upperRight.y_coords);
    double maxZ = MgServerFeatureUtil::Maximum(upperRight.z_coords);
    double maxM = MgServerFeatureUtil::Maximum(upperRight.m_coords);

    MgGeometryFactory factory;

    // TODO: Assuming we are two dimensional for now.

    //Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXYZM(minX, minY, minZ, minM);
    //Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXYZM(maxX, maxY, maxZ, maxM);

    Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXY(minX, minY);
    Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXY(maxX, minY);
    Ptr<MgCoordinate> coord3 = factory.CreateCoordinateXY(maxX, maxY);
    Ptr<MgCoordinate> coord4 = factory.CreateCoordinateXY(minX, maxY);

    Ptr<MgCoordinateCollection> coordCol = new MgCoordinateCollection();
    coordCol->Add(coord1);
    coordCol->Add(coord2);
    coordCol->Add(coord3);
    coordCol->Add(coord4);

    Ptr<MgLinearRing> outerRing = factory.CreateLinearRing(coordCol);
    Ptr<MgGeometry> geom = factory.CreatePolygon(outerRing, NULL);

    return SAFE_ADDREF((MgGeometry*)geom);
}

void MgFeatureGeometricFunctions::AddCoordinate(MgCoordinate* coord,
                                                CoordinateCollection& coords)
{
    if (coord == NULL) { return; } // Nothing to do if NULL;

    INT32 geomType = coord->GetDimension();

    switch (geomType)
    {
        case MgCoordinateDimension::XY:
        {
            coords.x_coords.push_back(coord->GetX());
            coords.y_coords.push_back(coord->GetY());
            break;
        }
        case MgCoordinateDimension::XYZ:
        {
            coords.x_coords.push_back(coord->GetX());
            coords.y_coords.push_back(coord->GetY());
            coords.z_coords.push_back(coord->GetZ());
            break;
        }
        case (MgCoordinateDimension::XY | MgCoordinateDimension::M):
        {
            coords.x_coords.push_back(coord->GetX());
            coords.y_coords.push_back(coord->GetY());
            coords.m_coords.push_back(coord->GetM());
            break;
        }
        case (MgCoordinateDimension::XYZ | MgCoordinateDimension::M):
        {
            coords.x_coords.push_back(coord->GetX());
            coords.y_coords.push_back(coord->GetY());
            coords.z_coords.push_back(coord->GetZ());
            coords.m_coords.push_back(coord->GetM());
            break;
        }
    }
}

// Create the reader for string properties
MgReader* MgFeatureGeometricFunctions::GetReader(MgGeometryCollection* geomCol)
{
    Ptr<MgDataReader> dataReader = (MgDataReader*)NULL;

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
    return SAFE_ADDREF((MgDataReader*)dataReader);
}
