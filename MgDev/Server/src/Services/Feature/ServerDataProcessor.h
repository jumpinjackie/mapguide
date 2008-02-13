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

#ifndef _MGSERVERDATAPROCESSOR_H_
#define _MGSERVERDATAPROCESSOR_H_

#include "MapGuideCommon.h"
#include "Fdo.h"
#include "ServerFeatureProcessor.h"
#include "ServerDataReader.h"
#include "Services/FeatureService.h"

class MgServerDataProcessor : public MgServerFeatureProcessor
{
    DECLARE_CLASSNAME(MgServerDataProcessor)

public:

    MgServerDataProcessor();
    MgServerDataProcessor(MgServerDataReader* dataReader);
    ~MgServerDataProcessor();

    MgBatchPropertyCollection* GetRows(INT32 count = 0); // If zero means feature all records
    MgByteReader* GetLOBFromDataReader(CREFSTRING propName, MgServerDataReader* dataReader);
    MgPropertyDefinitionCollection* GetColumnDefinitions();
    virtual MgByteReader* GetRaster(INT32 xSize, INT32 ySize, STRING propName);
    virtual MgServerFeatureProcessorType GetProcessorType() { return msfptDataProcessor; }

    void ClearDataReader();

protected:

    void Dispose()
    {
        delete this;
    }

private:

    //MgPropertyDefinition* GetMgPropertyDefinition(FdoPropertyDefinition* fdoPropDef);
    //MgDataPropertyDefinition* GetDataPropertyDefinition(FdoDataPropertyDefinition* fdoPropDef);
    //MgGeometricPropertyDefinition* GetGeometricPropertyDefinition(FdoGeometricPropertyDefinition* fdoPropDef);
    //INT32 GetMgPropertyType(FdoDataType fdoDataType);

    void AddRows(INT32 count);
    void AddRow(MgPropertyDefinitionCollection* propDefCol);

    MgProperty* GetMgProperty(CREFSTRING propName, INT16 type);

    INT16 GetMgPropertyType(MgPropertyDefinition* propDef);

private:

    MgServerDataReader*                  m_dataReader;
    Ptr<MgBatchPropertyCollection>       m_bpCol;
    Ptr<MgPropertyDefinitionCollection>  m_propDefCol;
};

#endif
