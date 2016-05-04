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

#include "Services/Feature/FeatureDefs.h"
#include "MgDesktop.h"
#include "Services/FeatureService.h"
#include "Services/Feature/FeatureConnection.h"
#include "FeatureManipulationCommand.h"
#include "InsertCommand.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/FeatureReader.h"
#include "Services/Feature/FeatureSetReader.h"

MgdInsertCommand::MgdInsertCommand()
{
    m_srvrFeatConn = NULL;
    m_featCommand = NULL;
}

MgdInsertCommand::MgdInsertCommand(MgFeatureCommand* command, MgdFeatureConnection* connection, INT32 cmdId)
{
    CHECKNULL(command, L"MgdInsertCommand.MgdInsertCommand");
    CHECKNULL(connection, L"MgdInsertCommand.MgdInsertCommand");

    m_srvrFeatConn = SAFE_ADDREF((MgdFeatureConnection*)connection);
    m_featCommand = SAFE_ADDREF((MgInsertFeatures*)command);
    m_cmdId = cmdId;
}

MgdInsertCommand::~MgdInsertCommand()
{
    m_srvrFeatConn = NULL;
}

MgProperty* MgdInsertCommand::Execute()
{
    Ptr<MgFeatureProperty> prop;

    STRING clsName = m_featCommand->GetFeatureClassName();
    Ptr<MgBatchPropertyCollection> srcCol = m_featCommand->GetBatchPropertyValues();

    if ((srcCol == NULL) || srcCol->GetCount() == 0)
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgNoFeaturesForInsert");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdInsertCommand::Execute", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    FdoPtr<FdoIConnection> fdoConn = m_srvrFeatConn->GetConnection();

    // Create the SQL command
    FdoPtr<FdoIInsert> fdoCommand = (FdoIInsert*)fdoConn->CreateCommand(FdoCommandType_Insert);
    CHECKNULL((FdoIInsert*)fdoCommand, L"MgdInsertCommand.Execute");

    fdoCommand->SetFeatureClassName(clsName.c_str());

    // TODO: Fdo returns NULL here for Oracle and SDF, ArcSDE supports it.
    FdoPtr<FdoBatchParameterValueCollection> bParamValCol = fdoCommand->GetBatchParameterValues();
    if (bParamValCol != NULL && bParamValCol->GetCount() > 1)
    {
        prop = BatchInsert(srcCol, bParamValCol, fdoCommand, fdoConn);
    }
    else
    {
        FdoPtr<FdoPropertyValueCollection> propValCol = fdoCommand->GetPropertyValues();
        prop = SingleInsert(srcCol, propValCol, fdoCommand, fdoConn);
    }

    return prop.Detach();
}

MgFeatureProperty* MgdInsertCommand::BatchInsert( MgBatchPropertyCollection* srcCol,
                                                       FdoBatchParameterValueCollection* bParamValCol,
                                                       FdoIInsert* fdoCommand,
                                                       FdoIConnection* fdoConn)
{
    INT32 cnt = srcCol->GetCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        Ptr<MgPropertyCollection> propCol = srcCol->GetItem(i);
        FdoPtr<FdoParameterValueCollection> paramCol = MgdFeatureUtil::CreateFdoParameterCollection(propCol);
        bParamValCol->Add(paramCol);
    }
    FdoPtr<FdoIFeatureReader> reader = fdoCommand->Execute();
    CHECKNULL((FdoIFeatureReader*)reader, L"MgdInsertCommand.BatchInsert");

    // TODO: This is FDO defect, they should not require ReadNext() for class definition
    bool available = false;
    try
    {
        available = reader->ReadNext();
    }
    catch(FdoException* e)
    {
        FDO_SAFE_RELEASE(e);
        available = false;
    }
    catch (...)
    {
        available = false;
    }

    if (!available)
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgInsertError");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgServerGwsGetFeatures.BatchInsert", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    char buff[32];
    sprintf(buff, "%d", m_cmdId);
    STRING str = MgUtil::MultiByteToWideChar(string(buff));

    Ptr<MgFeatureReader> mgFeatureReader = new MgdFeatureReader(m_srvrFeatConn, reader);
    return new MgFeatureProperty(str, mgFeatureReader);
}

MgFeatureProperty* MgdInsertCommand::SingleInsert( MgBatchPropertyCollection* srcCol,
                                                        FdoPropertyValueCollection* propValCol,
                                                        FdoIInsert* fdoCommand,
                                                        FdoIConnection* fdoConn )
{
    Ptr<MgFeatureSet> featureSet = new MgFeatureSet();
    featureSet->SetClassDefinition(NULL);
    Ptr<MgPropertyDefinitionCollection> keyProps;

    INT32 cnt = srcCol->GetCount();
    for (INT32 i=0; i<cnt; i++)
    {
        Ptr<MgPropertyCollection> propCol = srcCol->GetItem(i);

        propValCol->Clear();
        MgdFeatureUtil::FillFdoPropertyCollection(propCol, propValCol);

        FdoPtr<FdoIFeatureReader> reader = fdoCommand->Execute();
        CHECKNULL((FdoIFeatureReader*)reader, L"MgdInsertCommand.SingleInsert");

        if (keyProps == NULL)
        {
            FdoPtr<FdoClassDefinition> classDef = reader->GetClassDefinition();
            FdoPtr<FdoDataPropertyDefinitionCollection> idProps = classDef->GetIdentityProperties();
            if (idProps->GetCount() > 0)
            {
                keyProps = new MgPropertyDefinitionCollection();
                MgdFeatureUtil::GetClassProperties(keyProps, idProps);
            }
        }

        if (keyProps == NULL)
        {
            STRING message = MgdFeatureUtil::GetMessage(L"MgInsertError");

            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgFeatureServiceException(L"MgdInsertCommand.SingleInsert", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        bool available = false;
        try
        {
            available = reader->ReadNext();
        }
        catch(FdoException* e)
        {
            FDO_SAFE_RELEASE(e);
            available = false;
        }
        catch (...)
        {
            available = false;
        }

        if (!available)
        {
            STRING message = MgdFeatureUtil::GetMessage(L"MgInsertError");

            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgFeatureServiceException(L"MgdInsertCommand.SingleInsert", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // Performance improvement: Only return the key values
        Ptr<MgPropertyCollection> idProperties = new MgPropertyCollection();
        INT32 nProps = keyProps->GetCount();
        for (int j=0; j<nProps; j++)
        {
            Ptr<MgDataPropertyDefinition> propDef = dynamic_cast<MgDataPropertyDefinition*>(keyProps->GetItem(j));
            if (propDef == NULL) continue;

            //TODO: should we use qualified name here?
            STRING propName = propDef->GetName();
            INT32 propType = propDef->GetDataType();
            switch (propType)
            {
                case MgPropertyType::Int16:
                {
                    Ptr<MgInt16Property> intProp = new MgInt16Property(propName, reader->GetInt16(propName.c_str()));
                    idProperties->Add(intProp);
                }
                break;

                case MgPropertyType::Int32:
                {
                    Ptr<MgInt32Property> intProp = new MgInt32Property(propName, reader->GetInt32(propName.c_str()));
                    idProperties->Add(intProp);
                }
                break;

                case MgPropertyType::Int64:
                {
                    Ptr<MgInt64Property> intProp = new MgInt64Property(propName, reader->GetInt64(propName.c_str()));
                    idProperties->Add(intProp);
                }
                break;

                case MgPropertyType::String:
                {
                    Ptr<MgStringProperty> strProp = new MgStringProperty(propName, reader->GetString(propName.c_str()));
                    idProperties->Add(strProp);
                }
                break;

                default:
                break;
            }
        }

        featureSet->AddFeature(idProperties);
    }

    Ptr<MgFeatureReader> featureSetReader = new MgdFeatureSetReader(featureSet);

    char buff[32];
    sprintf(buff, "%d", m_cmdId);
    STRING str = MgUtil::MultiByteToWideChar(string(buff));

    return new MgFeatureProperty(str, featureSetReader);
}
