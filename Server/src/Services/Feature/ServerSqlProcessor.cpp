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
#include "Services/FeatureService.h"
#include "ServerSqlProcessor.h"
#include "ServerFeatureConnection.h"

MgServerSqlProcessor::MgServerSqlProcessor()
{
    m_bpCol = NULL;
    m_sqlDataReader = NULL;
    m_propDefCol = NULL;
}


MgServerSqlProcessor::MgServerSqlProcessor(MgServerSqlDataReader* sqlReader)
{
    CHECKNULL((MgServerSqlDataReader*)sqlReader, L"MgServerSqlProcessor.MgServerSqlProcessor");

    m_bpCol = NULL;
    m_propDefCol = NULL;
    m_sqlDataReader = SAFE_ADDREF(sqlReader);
}

MgServerSqlProcessor::~MgServerSqlProcessor()
{
    // TODO: Should we close the SqlDataReader or just releasing is fine??
    if (m_sqlDataReader != NULL)
        m_sqlDataReader->Close();

    SAFE_RELEASE(m_sqlDataReader);
}

MgBatchPropertyCollection* MgServerSqlProcessor::GetRows(INT32 count)
{
    CHECKNULL((MgServerSqlDataReader*)m_sqlDataReader, L"MgServerSqlProcessor.GetRows");

    MG_FEATURE_SERVICE_TRY()

    // All column properties
    if (NULL == (MgPropertyDefinitionCollection*)m_propDefCol)
    {
        // Get MgPropertyDefinitionCollection
        Ptr<MgPropertyDefinitionCollection> mgPropDef = this->GetColumnDefinitions();
        CHECKNULL((MgPropertyDefinitionCollection*)mgPropDef, L"MgServerSqlProcessor.GetRows");

        m_propDefCol = SAFE_ADDREF((MgPropertyDefinitionCollection*)mgPropDef);
    }

    if (NULL == (MgBatchPropertyCollection*)m_bpCol)
    {
        // Create a feature set for a pool of features
        m_bpCol = new MgBatchPropertyCollection();
        CHECKNULL((MgBatchPropertyCollection*)m_bpCol, L"MgServerSqlProcessor.GetRows");
    }
    else
    {
        m_bpCol->Clear();
    }

    // Add all features to feature set
    this->AddRows(count);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlProcessor.GetRows")

    return SAFE_ADDREF((MgBatchPropertyCollection*)m_bpCol);
}

MgPropertyDefinitionCollection* MgServerSqlProcessor::GetColumnDefinitions()
{
    CHECKNULL((MgServerSqlDataReader*)m_sqlDataReader, L"MgServerSqlProcessor.GetRows");

    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgPropertyDefinitionCollection*)m_propDefCol)
    {
        // Create a new collection
        m_propDefCol = new MgPropertyDefinitionCollection();

        // Collect all property names and types
        INT32 cnt = m_sqlDataReader->GetPropertyCount();
        for (INT32 i = 0; i < cnt; i++)
        {
            STRING colName = m_sqlDataReader->GetPropertyName(i);
            INT32 colType = m_sqlDataReader->GetPropertyType(colName);
            Ptr<MgPropertyDefinition> propDef = new MgPropertyDefinition(colName, colType);
            m_propDefCol->Add(propDef);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlProcessor.GetColumnDefinitions")

    return SAFE_ADDREF((MgPropertyDefinitionCollection*)m_propDefCol);
}


void MgServerSqlProcessor::AddRows(INT32 count)
{
    CHECKNULL((MgServerSqlDataReader*)m_sqlDataReader, L"MgServerSqlProcessor.AddRows");
    CHECKNULL((MgBatchPropertyCollection*)m_bpCol, L"MgServerSqlProcessor.AddRows");

    INT32 desiredFeatures = 0;

    // Access the property definition collection
    Ptr<MgPropertyDefinitionCollection> propDefCol = this->GetColumnDefinitions();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerSqlProcessor.AddRows");

    while (m_sqlDataReader->ReadNext())
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
        else // 0 or -ve value means fetch all features
        {
            continue;
        }
    }
}


void MgServerSqlProcessor::AddRow(MgPropertyDefinitionCollection* propDefCol)
{
    CHECKNULL((MgServerSqlDataReader*)m_sqlDataReader, L"MgServerSqlProcessor.AddRow");
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerSqlProcessor.AddRow");

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

MgProperty* MgServerSqlProcessor::GetMgProperty(CREFSTRING propName, INT16 type)
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

                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    val = m_sqlDataReader->GetBoolean(propName.c_str());
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

                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    val = m_sqlDataReader->GetByte(propName.c_str());
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

                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    dateTime = m_sqlDataReader->GetDateTime(propName.c_str());
                    isNull = false;
                }
                prop = new MgDateTimeProperty(propName, dateTime);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Single: /// Single precision floating point value
            {
                float val = 0.0f;
                bool isNull = true;

                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    val = m_sqlDataReader->GetSingle(propName.c_str());
                    isNull = false;
                }

                prop = new MgSingleProperty(propName, (float)val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Double: /// Double precision floating point value
            {
                double val = 0.0;
                bool isNull = true;

                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    val = m_sqlDataReader->GetDouble(propName.c_str());
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

                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    val = m_sqlDataReader->GetInt16(propName.c_str());
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

                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    val = m_sqlDataReader->GetInt32(propName.c_str());
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

                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    val = m_sqlDataReader->GetInt64(propName.c_str());
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

                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    val = m_sqlDataReader->GetString(propName.c_str());
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
                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    isNull = false;
                    val = m_sqlDataReader->GetBLOB(propName);
                }
                prop = new MgBlobProperty(propName,val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Clob: // CLOB
            {
                bool isNull = true;
                Ptr<MgByteReader> val;
                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    isNull = false;
                    val = m_sqlDataReader->GetCLOB(propName);
                }
                prop = new MgClobProperty(propName,val);
                prop->SetNull(isNull);
                break;
            }
            case MgPropertyType::Geometry: // Geometry object
            {
                Ptr<MgByteReader> val;
                bool isNull = true;
                if (!m_sqlDataReader->IsNull(propName.c_str()))
                {
                    val = m_sqlDataReader->GetGeometry(propName.c_str());
                    isNull = false;
                }
                prop = new MgGeometryProperty(propName, val);
                prop->SetNull(isNull);
                break;
            }
        }
    }

    return prop.Detach();
}
