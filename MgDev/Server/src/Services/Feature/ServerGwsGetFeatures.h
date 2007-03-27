//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _MGSERVERGWSGETFEATURES_H_
#define _MGSERVERGWSGETFEATURES_H_

#include "MapGuideCommon.h"
#include "Fdo.h"
#include "ServerGwsFeatureReader.h"
#include "Services/FeatureService.h"
#include "GwsQueryEngine.h"

class MgServerGwsFeatureReader;

class MgServerGwsGetFeatures : public MgServerFeatureProcessor
{
public:

    MgServerGwsGetFeatures();
    MgServerGwsGetFeatures(IGWSFeatureIterator* gwsFeatureReader, MgServerGwsFeatureReader* serverGwsFeatureReader);
    ~MgServerGwsGetFeatures();

    MgFeatureSet* GetFeatures(INT32 count = 0);
    MgClassDefinition* GetMgClassDefinition(bool bSerialize = true);
    MgClassDefinition* GetMgClassDefinition(FdoClassDefinition* fdoClassDefinition, bool bSerialize = true);

    virtual MgByteReader* GetRaster(INT32 xSize, INT32 ySize, STRING propName);
    MgByteReader* GetLOBFromFdo(CREFSTRING propName, IGWSFeatureIterator* gwsFeatureIterator);
    virtual MgServerFeatureProcessorType GetProcessorType() { return msfptFeatureProcessor; }

    void GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                            FdoDataPropertyDefinitionCollection* fdoPropDefCol);

    void SetRelationNames(FdoStringCollection* relationNames);
    void SetExtensionName(CREFSTRING extensionName);

    FdoStringCollection* GetRelationNames();
    STRING GetExtensionName();

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

    INT32 GetMgPropertyType(FdoDataType fdoDataType);

    void AddFeatures(INT32 count);
    void AddFeature(MgPropertyDefinitionCollection* propDefCol);

    MgProperty* GetMgProperty(CREFSTRING propName, INT16 type);
    INT16 GetMgPropertyType(MgPropertyDefinition* propDef);
    MgByteReader* SerializeToXml(FdoClassDefinition* classDef);

    bool DeterminePropertyFeatureSource(CREFSTRING inputPropName, IGWSFeatureIterator** gwsFeatureIter, STRING& parsedPropName);

private:

    IGWSFeatureIterator*                 m_gwsFeatureReader;
    MgServerGwsFeatureReader*      m_serverGwsFeatureReader;
    Ptr<MgFeatureSet>                    m_featureSet;
    Ptr<MgClassDefinition>               m_classDef;
    FdoPtr<FdoStringCollection>          m_relationNames;
    STRING                               m_extensionName;

    bool                                          m_bAdvanceSecondaryIterator;

};

#endif
