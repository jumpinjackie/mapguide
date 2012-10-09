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

#include "FeatureDefs.h"
#include "FeatureUtil.h"
#include "FeatureDistribution.h"
#include "FeatureGeometricFunctions.h"
#include "DataReaderCreator.h"
//#include "StringDataReaderCreator.h"
#include "GeometryDataReaderCreator.h"
#include <algorithm>
#include "UniqueFunction.h"

MgdFeatureGeometricFunctions::MgdFeatureGeometricFunctions()
{
    m_type = MgPropertyType::Null;
    m_reader = NULL;
    m_customFunction = NULL;
    m_propertyAlias = L"";
    m_extentsInitialized = false;
}

MgdFeatureGeometricFunctions::MgdFeatureGeometricFunctions(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias)
{
    Initialize(reader, customFunction, propertyAlias); // Initialize the instance
}

void MgdFeatureGeometricFunctions::Initialize(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias)
{
    CHECKNULL((MgReader*)reader, L"MgdFeatureGeometricFunctions.Initialize");
    CHECKNULL((FdoFunction*)customFunction, L"MgdFeatureGeometricFunctions.Initialize");

    if(1 == reader->GetPropertyCount())
    {
        m_type = MgdFeatureUtil::GetPropertyDefinition(reader, m_propertyName);
    }
    else
    {
        // Only get the property needed
        FdoPtr<FdoExpressionCollection> exprCol = customFunction->GetArguments();
        FdoInt32 cnt = exprCol->GetCount();
        FdoPtr<FdoExpression> expr;
        if(cnt == 1)
        {
            expr = exprCol->GetItem(0);
            FdoIdentifier* propName = dynamic_cast<FdoIdentifier*>(expr.p);
            CHECKNULL(propName, L"MgdFeatureGeometricFunctions.Initialize");
            m_propertyName = propName->GetName();
            m_type = reader->GetPropertyType(m_propertyName);
        }
        else
        {
            // Throw original exception
            m_type = MgdFeatureUtil::GetPropertyDefinition(reader, m_propertyName);
        }
    }

    // TODO: Should we really check this, may be we can ignore ??
    // because we can only come to here if property type is numeric
    if (!this->CheckSupportedPropertyType())
	{
		throw new MgInvalidPropertyTypeException(
			L"MgdFeatureGeometricFunctions.Initialize", __LINE__, __WFILE__, NULL, L"", NULL);
	}

    // We must have an property alias
    // Though we can name a property with same name as function expression
    // But Fdo forces to have an alias. Therefore we implement this restriction.
    if (propertyAlias.empty())
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgMissingPropertyAlias");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdFeatureGeometricFunctions.Initialize", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    m_reader = SAFE_ADDREF(reader);
    m_customFunction = FDO_SAFE_ADDREF(customFunction);
    m_propertyAlias = propertyAlias;
    m_extentsInitialized = false;
}


MgdFeatureGeometricFunctions::~MgdFeatureGeometricFunctions()
{
}

// Execute the function
MgReader* MgdFeatureGeometricFunctions::Execute()
{
    CHECKNULL((MgReader*)m_reader, L"MgdFeatureGeometricFunctions.Execute");
    CHECKNULL(m_customFunction, L"MgdFeatureGeometricFunctions.Execute");

    Ptr<MgGeometryCollection> geomCol = new MgGeometryCollection();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureGeometricFunctions::Execute");
    while(m_reader->ReadNext())
    {
        // Get the geometry
        Ptr<MgGeometry> val;
        try 
        {
            val = GetValue();
        }
        catch (MgException* ex) //Don't let a bad geometry derail us, just discard and continue 
        {
            ex->Release();
            continue;
        }
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
    return GetReader(finalResult);
}

void MgdFeatureGeometricFunctions::ComputeExtents(MgCoordinate* lowerLeft,
                                                    MgCoordinate* upperRight)
{
    CHECKNULL((MgCoordinate*)lowerLeft, L"MgdFeatureGeometricFunctions.ComputeExtents");
    CHECKNULL((MgCoordinate*)upperRight, L"MgdFeatureGeometricFunctions.ComputeExtents");

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

MgGeometryCollection* MgdFeatureGeometricFunctions::ExecuteOperation()
{
    INT32 funcCode = -1;

    Ptr<MgGeometryCollection> result;

    // Get the arguments from the FdoFunction
    STRING propertyName;
    bool supported = MgdFeatureUtil::FindCustomFunction(m_customFunction, funcCode);

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
                STRING message = MgdFeatureUtil::GetMessage(L"MgCustomFunctionNotSupported");

                MgStringCollection arguments;
                arguments.Add(message);
                throw new MgFeatureServiceException(
                    L"MgdFeatureGeometricFunctions.ExecuteOperation",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }
    }

    return result.Detach();
}

// Check whether property type is a supported type
bool MgdFeatureGeometricFunctions::CheckSupportedPropertyType()
{
	return MgPropertyType::Geometry == m_type;
}


// Get the value of property
MgGeometry* MgdFeatureGeometricFunctions::GetValue()
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
                    L"MgdFeatureGeometricFunctions.GetValue",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }

    return geom.Detach();
}




// Create the reader for string properties
MgReader* MgdFeatureGeometricFunctions::GetReader(MgGeometryCollection* geomCol)
{
    Ptr<MgDataReader> dataReader;

    switch(m_type)
    {
        case MgPropertyType::Geometry:
        {
            Ptr<MgdGeometryDataReaderCreator> drCreator = new MgdGeometryDataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(geomCol);
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(
                L"MgdFeatureGeometricFunctions.GetReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    return dataReader.Detach();
}
