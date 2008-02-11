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

#ifndef _MG_SERVER_GET_FEATURES_H_
#define _MG_SERVER_GET_FEATURES_H_

#include "MapGuideCommon.h"
#include "Fdo.h"
#include "ServerFeatureProcessor.h"
#include "ServerFeatureReaderIdentifier.h"
#include "Services/FeatureService.h"

class MgServerGetFeatures : public MgServerFeatureProcessor
{
    DECLARE_CLASSNAME(MgServerGetFeatures)

public:

    MgServerGetFeatures();
    MgServerGetFeatures(MgServerFeatureReaderIdentifier* featureReaderIdentifier);
    ~MgServerGetFeatures();
    MgFeatureSet* GetFeatures(INT32 count = 0); // If zero means feature all records
    MgClassDefinition* GetMgClassDefinition(bool bSerialize = true);
    MgClassDefinition* GetMgClassDefinition(FdoClassDefinition* fdoClassDefinition, bool bSerialize = true);
    virtual MgByteReader* GetRaster(INT32 xSize, INT32 ySize, STRING propName);
    MgByteReader* GetLOBFromFdo(CREFSTRING propName, FdoIFeatureReader* featureReader);
    virtual MgServerFeatureProcessorType GetProcessorType() { return msfptFeatureProcessor; }

    void GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                            FdoDataPropertyDefinitionCollection* fdoPropDefCol);
    INT32 GetMgPropertyType(FdoDataType fdoDataType);

protected:

    void Dispose() { delete this; }

private:

    void GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                            FdoPropertyDefinitionCollection* fdoPropDefCol);

    MgPropertyDefinition* GetMgPropertyDefinition(FdoPropertyDefinition* fdoPropDef);

    MgDataPropertyDefinition* GetDataPropertyDefinition(FdoDataPropertyDefinition* fdoPropDef);
    MgGeometricPropertyDefinition* GetGeometricPropertyDefinition(FdoGeometricPropertyDefinition* fdoPropDef);
    MgRasterPropertyDefinition* GetRasterPropertyDefinition(FdoRasterPropertyDefinition* fdoPropDef);
    MgObjectPropertyDefinition* GetObjectPropertyDefinition(FdoObjectPropertyDefinition* fdoPropDef);

    void AddFeatures(INT32 count);
    void AddFeature(MgPropertyDefinitionCollection* propDefCol);
    MgProperty* GetMgProperty(CREFSTRING propName, INT16 type);
    INT16 GetMgPropertyType(MgPropertyDefinition* propDef);
    MgByteReader* SerializeToXml(FdoClassDefinition* classDef);

private:

    FdoIFeatureReader*      m_featureReader;
    Ptr<MgFeatureSet>       m_featureSet;
    Ptr<MgStringCollection> m_desiredProperties;
    Ptr<MgClassDefinition>  m_classDef;
};

#endif
