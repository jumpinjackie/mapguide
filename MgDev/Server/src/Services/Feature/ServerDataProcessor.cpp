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

#include "ServerFeatureServiceDefs.h"
#include "Services/FeatureService.h"
#include "ServerDataProcessor.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureUtil.h"

MgServerDataProcessor::MgServerDataProcessor()
{
    m_bpCol = NULL;
    m_dataReader = NULL;
    m_propDefCol = NULL;
}


MgServerDataProcessor::MgServerDataProcessor(MgServerDataReader* dataReader)
{
    CHECKNULL((MgServerDataReader*)dataReader, L"MgServerDataProcessor.MgServerDataProcessor");

    m_bpCol = NULL;
    m_propDefCol = NULL;
    m_dataReader = SAFE_ADDREF(dataReader);
}

MgServerDataProcessor::~MgServerDataProcessor()
{
    ClearDataReader();
}


void MgServerDataProcessor::ClearDataReader()
{
    // clear the reader member variable before releasing it, since the final
    // release could cause the processor to also be destroyed (due to a
    // circular reference), which would result in this method being called
    // a second time...
    MgServerDataReader* dataReader = m_dataReader;
    if (dataReader != NULL)
    {
        m_dataReader = NULL;
        dataReader->Close();
        dataReader->Release();
    }
}


MgBatchPropertyCollection* MgServerDataProcessor::GetRows(INT32 count)
{
    CHECKNULL((MgServerDataReader*)m_dataReader, L"MgServerDataProcessor.GetRows");

    MG_FEATURE_SERVICE_TRY()

    INT32 featCnt = count;

    // All Column properties
    if (NULL == (MgPropertyDefinitionCollection*)m_propDefCol)
    {
        // Get MgPropertyDefinitionCollection
        Ptr<MgPropertyDefinitionCollection> mgPropDef = this->GetColumnDefinitions();
        CHECKNULL((MgPropertyDefinitionCollection*)mgPropDef, L"MgServerDataProcessor.GetRows");

        m_propDefCol = SAFE_ADDREF((MgPropertyDefinitionCollection*)mgPropDef);
    }

    if (NULL == (MgBatchPropertyCollection*)m_bpCol)
    {
        // Create a feature set for a pool of features
        m_bpCol = new MgBatchPropertyCollection();
        CHECKNULL((MgBatchPropertyCollection*)m_bpCol, L"MgServerDataProcessor.GetRows");
    }
    else
    {
        m_bpCol->Clear();
    }

    // If we have raster property, we only send one feature
    // override the configuration property setting
    STRING rasterProp = m_dataReader->GetRasterPropertyName();
    if (!rasterProp.empty())
    {
        featCnt = 1;
    }

    // Add all features to feature set
    this->AddRows(featCnt);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataProcessor.GetRows")

    return SAFE_ADDREF((MgBatchPropertyCollection*)m_bpCol);
}

MgPropertyDefinitionCollection* MgServerDataProcessor::GetColumnDefinitions()
{
    CHECKNULL((MgServerDataReader*)m_dataReader, L"MgServerDataProcessor.GetRows");

    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgPropertyDefinitionCollection*)m_propDefCol)
    {
        // Create a new collection
        m_propDefCol = MgServerFeatureUtil::GetPropertyDefinitions(m_dataReader);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataProcessor.GetColumnDefinitions")

    return SAFE_ADDREF((MgPropertyDefinitionCollection*)m_propDefCol);
}


void MgServerDataProcessor::AddRows(INT32 count)
{
    CHECKNULL((MgServerDataReader*)m_dataReader, L"MgServerDataProcessor.AddRows");
    CHECKNULL((MgBatchPropertyCollection*)m_bpCol, L"MgServerDataProcessor.AddRows");

    INT32 desiredFeatures = 0;

    // Access the property definition collection
    Ptr<MgPropertyDefinitionCollection> propDefCol = this->GetColumnDefinitions();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerDataProcessor.AddRows");

    bool found = false;

    // FDO throws exception on ReadNext() which is not correct.
    // We catch the exception and make it false as it could not fetch the rows any more
    try { found = m_dataReader->ReadNext(); } catch(...) {found = false;}
    while (found)
    {
        AddRow((MgPropertyDefinitionCollection*)propDefCol);
        if (count > 0)
        {
            desiredFeatures++;
            if (desiredFeatures == count) // Collected required features therefore do not process more
            {
                break;
            }
        }
        // FDO throws exception on ReadNext() which is not correct.
        // We catch the exception and make it false as it could not fetch the rows any more
        try { found = m_dataReader->ReadNext(); } catch(...) {found = false;}
    }
}


void MgServerDataProcessor::AddRow(MgPropertyDefinitionCollection* propDefCol)
{
    CHECKNULL((MgServerDataReader*)m_dataReader, L"MgServerDataProcessor.AddRow");
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerDataProcessor.AddRow");

    Ptr<MgPropertyCollection> propCol = new MgPropertyCollection();
    INT32 cnt = propDefCol->GetCount();

    for (INT32 i=0; i < cnt; i++)
    {
        // Access the property definition
        Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(i);
        // Get the name of property
        STRING propName = propDef->GetName();
        INT16 type = propDef->GetPropertyType();

        Ptr<MgProperty> prop = this->GetMgProperty(propName, type);
        if (prop != NULL)
        {
            propCol->Add(prop);
        }
    }

    m_bpCol->Add(propCol);
}

MgProperty* MgServerDataProcessor::GetMgProperty(CREFSTRING propName, INT16 type)
{
    Ptr<MgNullableProperty> prop;

    // No propertyname specified, return NULL
    if (!propName.empty())
    {
        switch(type)
        {
            case MgPropertyType::Boolean: /// Boolean true/false value
            {
                bool val = false;
                bool isNull = true;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetBoolean(propName.c_str());
                    isNull = false;
                }

                prop = new MgBooleanProperty(propName, val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Byte: /// Unsigned 8 bit value
            {
                FdoByte val = 0;
                bool isNull = true;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetByte(propName.c_str());
                    isNull = false;
                }

                prop = new MgByteProperty(propName, (BYTE)val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::DateTime: /// DateTime object
            {
                bool isNull = true;
                Ptr<MgDateTime> dateTime =  (MgDateTime*)NULL;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    dateTime = m_dataReader->GetDateTime(propName.c_str());
                    isNull = false;
                }
                prop = new MgDateTimeProperty(propName, dateTime);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Single: /// Single precision floating point value
            {
                float val = 0;
                bool isNull = true;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetSingle(propName.c_str());
                    isNull = false;
                }

                prop = new MgSingleProperty(propName, (float)val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Double: /// Double precision floating point value
            {
                double val = 0;
                bool isNull = true;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetDouble(propName.c_str());
                    isNull = false;
                }

                prop = new MgDoubleProperty(propName, (double)val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Int16: /// 16 bit signed integer value
            {
                FdoInt16 val = 0;
                bool isNull = true;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetInt16(propName.c_str());
                    isNull = false;
                }

                prop = new MgInt16Property(propName, (INT16)val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Int32: // 32 bit signed integer value
            {
                FdoInt32 val = 0;
                bool isNull = true;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetInt32(propName.c_str());
                    isNull = false;
                }

                prop = new MgInt32Property(propName, (INT32)val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Int64: // 64 bit signed integer value
            {
                FdoInt64 val = 0;
                bool isNull = true;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetInt64(propName.c_str());
                    isNull = false;
                }

                prop = new MgInt64Property(propName, (INT64)val);
                prop->SetNull(isNull);

                break;
            }
            case MgPropertyType::String: // String MgProperty
            {
                STRING val = L"";
                bool isNull = true;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetString(propName.c_str());
                    isNull = false;
                }

                prop = new MgStringProperty(propName, val);
                prop->SetNull(isNull);

                break;
            }
            case MgPropertyType::Blob: // BLOB
            {
                bool isNull = true;
                Ptr<MgByteReader> val;
                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    isNull = false;
                    val = m_dataReader->GetBLOB(propName);
                }
                prop = new MgBlobProperty(propName,val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Clob: // CLOB
            {
                bool isNull = true;
                Ptr<MgByteReader> val;
                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    isNull = false;
                    val = m_dataReader->GetCLOB(propName);
                }
                prop = new MgClobProperty(propName,val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Geometry: // Geometry object
            {
                Ptr<MgByteReader> val;
                bool isNull = true;
                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetGeometry(propName.c_str());
                    isNull = false;
                }
                prop = new MgGeometryProperty(propName, val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Raster: // Raster object
            {
                Ptr<MgRaster> val;
                bool isNull = true;

                if (!m_dataReader->IsNull(propName.c_str()))
                {
                    val = m_dataReader->GetRaster(propName.c_str());
                    isNull = false;
                }

                prop = new MgRasterProperty(propName, val);
                prop->SetNull(isNull);

                break;
            }
        }
    }

    return prop.Detach();
}

MgByteReader* MgServerDataProcessor::GetRaster(INT32 xSize, INT32 ySize, STRING propName)
{
    CHECKNULL((MgServerDataReader*)m_dataReader, L"MgServerDataProcessor.GetRaster");

    return m_dataReader->GetRaster(xSize, ySize, propName);
}
