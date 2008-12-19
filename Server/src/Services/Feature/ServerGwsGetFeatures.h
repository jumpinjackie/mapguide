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

#ifndef _MGSERVERGWSGETFEATURES_H_
#define _MGSERVERGWSGETFEATURES_H_

#include "MapGuideCommon.h"
#include "Fdo.h"
#include "ServerGwsFeatureReader.h"
#include "Services/FeatureService.h"
#include "GwsQueryEngine.h"

typedef std::multimap<STRING, IGWSFeatureIterator*> GwsFeatureIteratorMap;
typedef std::pair<STRING, IGWSFeatureIterator*> GwsFeatureIteratorPair;

class FdoExpressionEngine;
class FdoFilter;
class MgServerGwsFeatureReader;

class MgServerGwsGetFeatures : public MgServerFeatureProcessor
{
public:

    MgServerGwsGetFeatures();
    MgServerGwsGetFeatures(MgServerGwsFeatureReader* gwsFeatureReader,
                           IGWSFeatureIterator* gwsFeatureIterator,
                           IGWSFeatureIterator* gwsFeatureIteratorCopy,
                           MgStringCollection* attributeNameDelimiters,
                           IGWSExtendedFeatureDescription* primaryExtendedFeatureDescription,
                           CREFSTRING extensionName,
                           FdoStringCollection* relationNames,
                           bool bForceOneToOne = true);
    ~MgServerGwsGetFeatures();

    MgFeatureSet* GetFeatures(INT32 count = 0);
    MgClassDefinition* GetMgClassDefinition(bool bSerialize = true);
    MgClassDefinition* GetMgClassDefinition(FdoClassDefinition* fdoClassDefinition, bool bSerialize = true);

    virtual MgByteReader* GetRaster(INT32 xSize, INT32 ySize, STRING propName);
    MgByteReader* GetLOBFromFdo(CREFSTRING propName, IGWSFeatureIterator* gwsFeatureIterator);
    virtual MgServerFeatureProcessorType GetProcessorType() { return msfptFeatureJoinProcessor; }

    void GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                            FdoDataPropertyDefinitionCollection* fdoPropDefCol);

    void ClearGwsFeatureIterator();
    MgServerGwsFeatureReader* GetGwsFeatureReader();

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
    bool ReadNext();

private:

    MgServerGwsFeatureReader*            m_gwsFeatureReader;
    IGWSFeatureIterator*                 m_gwsFeatureIterator;
    IGWSFeatureIterator*                 m_gwsFeatureIteratorCopy;
    Ptr<MgStringCollection>              m_attributeNameDelimiters;
    Ptr<MgFeatureSet>                    m_featureSet;
    Ptr<MgClassDefinition>               m_classDef;
    FdoPtr<FdoStringCollection>          m_relationNames;
    STRING                               m_extensionName;
    GwsFeatureIteratorMap                m_secondaryGwsFeatureIteratorMap;

    FdoPtr<IGWSExtendedFeatureDescription> m_primaryExtendedFeatureDescription;

    bool m_bAdvancePrimaryIterator;
    bool m_bForceOneToOne;
    bool m_bNoMoreData;
    bool m_bAdvanceSecondaryIterator;
};

#endif
