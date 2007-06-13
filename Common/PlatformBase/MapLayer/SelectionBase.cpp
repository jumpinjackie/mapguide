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

#include "PlatformBase.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include <algorithm>

IMPLEMENT_CREATE_OBJECT(MgSelectionBase)

/////////////////////////////////////////
// Dummy constructor for Ptr<> template
//
MgSelectionBase::MgSelectionBase()
{
    m_stream = new MgMemoryStreamHelper();
}


// Initialize selection generation from a map
MgSelectionBase::MgSelectionBase(MgMapBase* map)
{
    m_map = SAFE_ADDREF(map);
    m_stream = new MgMemoryStreamHelper();
}


// Initialize selection from a map and xml selection
MgSelectionBase::MgSelectionBase(MgMapBase* map, CREFSTRING xmlSelectionString)
{
    m_map = SAFE_ADDREF(map);
    m_stream = new MgMemoryStreamHelper();
    FromXml(xmlSelectionString);
}


// Destructor
MgSelectionBase::~MgSelectionBase()
{
    // Delete selection manually instead of relying on deep template deletion
    SelectionMap::iterator lIter;
    for (lIter = m_selections.begin(); lIter != m_selections.end(); lIter++)
    {
        SelectedClassMap* clsmap = lIter->second;
        SelectedClassMap::iterator cIter;
        for (cIter = clsmap->begin(); cIter != clsmap->end(); cIter++)
        {
            SelectionList* list = cIter->second;
            cIter->second = NULL;
            delete list;
        }
        lIter->second = NULL;
        delete clsmap;
    }
    m_selections.clear();

    if (m_stream != NULL)
    {
        delete m_stream;
        m_stream = NULL;
    }
}


void MgSelectionBase::Dispose()
{
    delete this;
}


// Read selection from XML document.  Previous selection is cleared.
void MgSelectionBase::FromXml(CREFSTRING xmlSelectionString)
{
    // TODO: Validate that data is legal by parsing each selection
    //       string.  MgMap will be required to do this.

    MG_TRY()

    //parse the selection set and extract all information
    string text = MgUtil::WideCharToMultiByte(xmlSelectionString);
    MgXmlUtil xmlUtil;
    xmlUtil.ParseString(text.c_str());

    DOMElement* root = xmlUtil.GetRootNode();
    DOMNode* layerChild = MgXmlUtil::GetFirstChild(root);
    while(0 != layerChild)
    {
        if(MgXmlUtil::GetNodeType(layerChild) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)layerChild;
            wstring strName = MgXmlUtil::GetTagName(elt);
            wstring elemName = L"Layer";     //NOXLATE

            if(strName.compare(elemName) == 0)
            {
                wstring layerId = MgXmlUtil::GetAttribute(elt, "id"); //NOXLATE

                DOMNode* classChild = MgXmlUtil::GetFirstChild(elt);
                while(0 != classChild)
                {
                    if(MgXmlUtil::GetNodeType(classChild) == DOMNode::ELEMENT_NODE)
                    {
                        elt = (DOMElement*)classChild;
                        strName = MgXmlUtil::GetTagName(elt);
                        elemName = L"Class";  //NOXLATE

                        if(strName.compare(elemName) == 0)
                        {
                            wstring classId = MgXmlUtil::GetAttribute(elt, "id"); //NOXLATE
                            SelectionList* idList = new SelectionList();

                            DOMNode* selChild = MgXmlUtil::GetFirstChild(elt);
                            while(0 != selChild)
                            {
                                if(MgXmlUtil::GetNodeType(selChild) == DOMNode::ELEMENT_NODE)
                                {
                                    DOMElement* ielt = (DOMElement*)selChild;
                                    strName = MgXmlUtil::GetTagName(ielt);
                                    elemName = L"ID";      //NOXLATE

                                    if(strName.compare(elemName) == 0)
                                    {
                                        wstring strval;
                                        MgXmlUtil::GetTextFromElement(ielt, strval);
                                        idList->push_back(strval);
                                    }
                                }
                                selChild = MgXmlUtil::GetNextSibling(selChild);
                            }

                            if (idList->size() > 0)
                            {
                                Add(layerId, classId, idList);
                            }
                        }
                    }
                    classChild = MgXmlUtil::GetNextSibling(classChild);
                }
            }
        }
        layerChild = MgXmlUtil::GetNextSibling(layerChild);
    }

    MG_CATCH_AND_THROW(L"MgSelection.FromXml")
}


// Write selection as XML document.  This document will typically
// be passed back to a viewer to select the desired features.
STRING MgSelectionBase::ToXml()
{
    return ToXml(true);
}


/////////////////////////////////////////
// Write selection as XML document
// The XML generated may or may not contain a heading XML declarator
//
STRING MgSelectionBase::ToXml(bool withXmlDecl)
{
    STRING xml;

    // Start with a reasonable string buffer size
    xml.reserve(2048);

    if(withXmlDecl)
        xml = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    // this XML follows the FeatureSet-1.0.0.xsd schema
    xml.append(L"<FeatureSet xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"FeatureSet-1.0.0.xsd\">");

    SelectionMap::iterator lIter;
    for (lIter = m_selections.begin(); lIter != m_selections.end(); lIter++)
    {
        xml.append(L" <Layer id=\"");
        xml.append(lIter->first);
        xml.append(L"\">");

        SelectedClassMap::iterator cIter;
        for (cIter = lIter->second->begin(); cIter != lIter->second->end(); cIter++)
        {
            xml.append(L"  <Class id=\"");
            xml.append(cIter->first);
            xml.append(L"\">");

            STRING selXml;

            // Reserve appropriate string space for IDs
            if (cIter->second->size() > 0)
            {
                size_t len = (16 + cIter->second->front().length()) * cIter->second->size();
                selXml.reserve(len);
            }

            SelectionList::iterator sIter;
            for (sIter = cIter->second->begin(); sIter != cIter->second->end(); sIter++)
            {
                selXml.append(L"   <ID>");
                selXml.append(*sIter);
                selXml.append(L"</ID>");
            }

            xml.append(selXml);
            xml.append(L"  </Class>");
        }

        xml.append(L" </Layer>");
    }

    xml.append(L"</FeatureSet>");

    return xml;
}


// Add nFeatures to selection set from the FeatureReader.
// Specify nFeatures = 0 for all features in reader.
// Internal identity mapping for layer dictates the properties used
void MgSelectionBase::AddFeatures(MgLayerBase* layer, MgFeatureReader* featureReader, INT32 nFeatures)
{
    // Zero features means all features.  Assume maximum integer
    if (nFeatures == 0)
    {
        nFeatures = INT_MAX;
    }

    STRING className = layer->GetFeatureClassName();

    bool readNext = featureReader->ReadNext();
    while (nFeatures > 0 && readNext == true)
    {
        MgLayerBase::IdPropertyList propList = layer->GetIdPropertyList();
        for (MgLayerBase::IdPropertyList::iterator idIter = propList.begin(); idIter != propList.end(); idIter++)
        {
            switch (idIter->type)
            {
            case MgPropertyType::Int16:
                AddFeatureIdInt16(layer, className, featureReader->GetInt16(idIter->name));
                break;
            case MgPropertyType::Int32:
                AddFeatureIdInt32(layer, className, featureReader->GetInt32(idIter->name));
                break;
            case MgPropertyType::Int64:
                AddFeatureIdInt64(layer, className, featureReader->GetInt64(idIter->name));
                break;
            case MgPropertyType::String:
                AddFeatureIdString(layer, className, featureReader->GetString(idIter->name));
                break;
            default:
                break;
            }
        }
        --nFeatures;
        readNext = featureReader->ReadNext();
    }
}


// Add a single selection to the set based on a collection of identity properties
void MgSelectionBase::AddFeatureIds(MgLayerBase* layer, CREFSTRING className, MgPropertyCollection* props )
{
    MgLayerBase::IdPropertyList propList = layer->GetIdPropertyList();
    MgLayerBase::IdPropertyList::iterator idIter;
    for (idIter = propList.begin(); idIter != propList.end(); idIter++)
    {
        Ptr<MgProperty> prop = props->GetItem(idIter->name);
        switch (idIter->type)
        {
        case MgPropertyType::Int16:
            AddFeatureIdInt16(layer, className,
                dynamic_cast<MgInt16Property*>((MgProperty*)prop)->GetValue());
            break;
        case MgPropertyType::Int32:
            AddFeatureIdInt32(layer, className,
                dynamic_cast<MgInt32Property*>((MgProperty*)prop)->GetValue());
            break;
        case MgPropertyType::Int64:
            AddFeatureIdInt64(layer, className,
                dynamic_cast<MgInt64Property*>((MgProperty*)prop)->GetValue());
            break;
        case MgPropertyType::String:
            AddFeatureIdString(layer, className,
                dynamic_cast<MgStringProperty*>((MgProperty*)prop)->GetValue());
            break;
        default:
            break;
        }
    }
}


void MgSelectionBase::AddFeatureIdInt16(MgLayerBase* layer, CREFSTRING className, INT16 identifier)
{
    wstring layerName = layer->GetObjectId();
    m_stream->Clear();
    m_stream->WriteUINT16((UINT16)identifier);
    wstring b64;
    UnicodeString::MultiByteToWideChar(m_stream->ToBase64().c_str(), b64);
    Add(layerName, className, b64);
}


void MgSelectionBase::AddFeatureIdInt32(MgLayerBase* layer, CREFSTRING className, INT32 identifier)
{
    wstring layerName = layer->GetObjectId();
    m_stream->Clear();
    m_stream->WriteUINT32((UINT32)identifier);
    wstring b64;
    UnicodeString::MultiByteToWideChar(m_stream->ToBase64().c_str(), b64);
    Add(layerName, className, b64);
}


void MgSelectionBase::AddFeatureIdInt64(MgLayerBase* layer, CREFSTRING className, INT64 identifier)
{
    wstring layerName = layer->GetObjectId();
    m_stream->Clear();
    m_stream->WriteINT64(identifier);
    wstring b64;
    UnicodeString::MultiByteToWideChar(m_stream->ToBase64().c_str(), b64);
    Add(layerName, className, b64);
}


void MgSelectionBase::AddFeatureIdString(MgLayerBase* layer, CREFSTRING className, CREFSTRING identifier)
{
    wstring layerName = layer->GetObjectId();
    string cName = MgUtil::WideCharToMultiByte(className);
    m_stream->Clear();
    m_stream->WriteNullTermString(identifier);
    wstring b64;
    UnicodeString::MultiByteToWideChar(m_stream->ToBase64().c_str(), b64);
    Add(layerName, className, b64);
}


// Returns list of selected layers
MgReadOnlyLayerCollection* MgSelectionBase::GetLayers()
{
    if (0 == (MgMapBase*)m_map)
    {
        throw new MgInvalidMapDefinitionException(L"MgSelectionBase.GetLayers", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgReadOnlyLayerCollection> selLayers = new MgReadOnlyLayerCollection();

    Ptr<MgLayerCollection> layers = m_map->GetLayers();
    for (int i=0; i<layers->GetCount(); i++)
    {
        Ptr<MgLayerBase> layer = layers->GetItem(i);
        wstring layerId = layer->GetObjectId();
        if (m_selections.find(layerId) != m_selections.end())
        {
            selLayers->Add(layer);
        }
    }

    if (selLayers->GetCount() == 0)
    {
        return NULL;
    }

    return selLayers.Detach();
}


STRING MgSelectionBase::GetClass(CREFSTRING layer)
{
    Ptr<MgStringCollection> strCol = this->GetClasses(layer);
    return strCol->GetItem(0);
}


// Returns list of selected classes for a layer
MgStringCollection* MgSelectionBase::GetClasses(CREFSTRING layer)
{
    Ptr<MgStringCollection> strColl = new MgStringCollection();

    SelectionMap::iterator lIter = m_selections.find(layer);
    if (lIter != m_selections.end())
    {
        SelectedClassMap* classMap = lIter->second;
        SelectedClassMap::iterator cIter;
        for (cIter = classMap->begin(); cIter != classMap->end(); cIter++)
        {
            strColl->Add(cIter->first);
        }
    }

    if (strColl->GetCount() == 0)
    {
        return NULL;
    }

    return strColl.Detach();
}


// Does a particular layer (and optional class) have selections
bool MgSelectionBase::Contains(MgLayerBase* layer, CREFSTRING className)
{
    bool bContains = false;

    MG_TRY()

    SelectionList* selList = GetSelections(layer, className);

    if (0 != selList)
    {
        bContains = true;
    }

    MG_CATCH_AND_THROW(L"MgSelectionBase.Contains");

    return bContains;
}


// Generates an Fdo filter text for the selections of the
//specified layer and class
STRING MgSelectionBase::GenerateFilter(MgLayerBase* layer, CREFSTRING className)
{
    STRING filter;
    bool filterReserved = false;

    MG_TRY()

    SelectionList* selList = GetSelections(layer, className);
    MgLayerBase::IdPropertyList& idList = layer->GetIdPropertyList();

    if (0 != selList && idList.size() > 0)
    {
        STRING selText;
        selText.reserve(128);

        bool bFirstSel = true;

        SelectionList::iterator sIter;
        for (sIter = selList->begin(); sIter != selList->end(); sIter++)
        {
            m_stream->FromBase64(*sIter);

            if (bFirstSel)
            {
                selText = L"(";
            }
            else
            {
                selText = L" OR (";
            }

            bool bFirstProp = true;
            MgLayerBase::IdPropertyList::iterator idIter;
            for (idIter = idList.begin(); idIter != idList.end(); idIter++)
            {
                if (!bFirstProp)
                {
                    selText.append(L" AND ");
                }

                selText.append(idIter->name);
                //selText.append(idIter->type == MgPropertyType::String? L" Like " : L"=");  //Temporary fix. Operator '=' is broken for strings, use operator 'Like' instead
                selText.append(L"=");

                switch (idIter->type)
                {
                case MgPropertyType::Int16:
                    {
                        UINT16 id;
                        m_stream->GetUINT16(id);

                        char buf[32];
                        sprintf(buf, "%d", id);
                        string tmp = buf;

                        STRING str = MgUtil::MultiByteToWideChar(tmp);

                        selText.append(str);
                    }
                    break;
                case MgPropertyType::Int32:
                    {
                        UINT32 id;
                        m_stream->GetUINT32(id);

                        char buf[32];
                        sprintf(buf, "%d", id);
                        string tmp = buf;

                        STRING str = MgUtil::MultiByteToWideChar(tmp);

                        selText.append(str);
                    }
                    break;
                case MgPropertyType::Int64:
                    {
                        INT64 id;
                        m_stream->GetINT64(id);

                        // couldn't find a precanned converter...
                        string buf;
                        char digit[2];
                        digit[0] = '\0'; digit[1] = '\0';
                        while (id > 0)
                        {
                            digit[0] = ((char)(id % 10) + '0');
                            buf.append(digit);
                            id /= 10;
                        }
                        reverse(buf.begin(), buf.end());

                        STRING str = MgUtil::MultiByteToWideChar(buf);

                        selText.append(str);
                    }
                    break;
                case MgPropertyType::String:
                    {
                        STRING strId;
                        m_stream->GetNullTermString(strId);
                        selText.append(L"'");
                        selText.append(strId);
                        selText.append(L"'");
                    }
                    break;
                case MgPropertyType::Single:
                    {
                        float id;
                        m_stream->GetSingle(id);

                        char buf[32];
                        sprintf(buf, "%f", id);
                        string tmp = buf;

                        STRING str = MgUtil::MultiByteToWideChar(tmp);

                        selText.append(str);
                    }
                    break;
                case MgPropertyType::Double:
                    {
                        double id;
                        m_stream->GetDouble(id);

                        char buf[32];
                        sprintf(buf, "%f", id);
                        string tmp = buf;

                        STRING str = MgUtil::MultiByteToWideChar(tmp);

                        selText.append(str);
                    }
                    break;
                default:
                    throw new MgNotImplementedException(L"MgSelectionBase.GenerateFilter", __LINE__, __WFILE__, NULL, L"", NULL);
                    break;
                }

                bFirstProp = false;
            }

            selText.append(L")");

            if (filterReserved == false)
            {
                size_t listSize = selList->size();
                filter.reserve(listSize * selText.length() * 2);
                filterReserved = true;
            }

            filter.append(selText);

            bFirstSel = false;
        }
    }

    MG_CATCH_AND_THROW(L"MgSelectionBase.GenerateFilter");

    return filter;
}


void MgSelectionBase::Add(CREFSTRING layerId, CREFSTRING className, CREFSTRING id)
{
    SelectionMap::iterator lIter = m_selections.find(layerId);
    if (lIter == m_selections.end())
    {
        m_selections[layerId] = new SelectedClassMap;
        lIter = m_selections.find(layerId);
    }

    SelectedClassMap* classList = lIter->second;

    SelectedClassMap::iterator cIter = classList->find(className);
    if (cIter == classList->end())
    {
        SelectionList* idList = new SelectionList();
        (*classList)[className] = idList;
        cIter = classList->find(className);
    }

    cIter->second->push_back(id);
}


void MgSelectionBase::Add(wstring& layerId, wstring& className, SelectionList* idList)
{
    SelectionMap::iterator lIter = m_selections.find(layerId);
    if (lIter == m_selections.end())
    {
        m_selections[layerId] = new SelectedClassMap;
        lIter = m_selections.find(layerId);
    }

    SelectedClassMap* classList = lIter->second;

    (*classList)[className] = idList;
}


MgSelectionBase::SelectionList* MgSelectionBase::GetSelections(MgLayerBase* layer, CREFSTRING className)
{
    SelectionList* selList = NULL;

    if (0 == layer)
    {
        throw new MgNullArgumentException(L"MgSelectionBase.GetSelections", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    wstring layerName = layer->GetObjectId();
    SelectionMap::iterator lIter = m_selections.find(layerName);
    if (lIter != m_selections.end())
    {
        SelectedClassMap::iterator cIter = lIter->second->find(className);
        if (cIter != lIter->second->end())
        {
            if (cIter->second->size() > 0)
            {
                selList = cIter->second;
            }
        }
    }

    return selList;
}


MgEnvelope* MgSelectionBase::GetExtents(MgFeatureService* featureService)
{
    CHECKNULL((MgFeatureService*)featureService, L"MgSelection.GetExtents");

    Ptr<MgEnvelope> env;
    Ptr<MgReadOnlyLayerCollection> roLayerCol = this->GetLayers();

    if (roLayerCol != NULL)
    {
        INT32 cnt = roLayerCol->GetCount();
        for (INT32 i=0; i < cnt; i++)
        {
            Ptr<MgLayerBase> layer = roLayerCol->GetItem(i); // Collection can not have a NULL layer

            STRING featureResId = layer->GetFeatureSourceId();
            STRING clsName = layer->GetFeatureClassName();
            STRING geomName = layer->GetFeatureGeometryName();
            STRING filterText = this->GenerateFilter(layer, clsName);

            Ptr<MgEnvelope> clsEnv = GetFeatureExtents(featureService,featureResId,clsName,filterText,geomName);
            if (env != NULL)
            {
                env->ExpandToInclude(clsEnv);
            }
            else
            {
                env = clsEnv.Detach();
            }
        }
    }
    return env.Detach();
}


MgEnvelope* MgSelectionBase::GetFeatureExtents(MgFeatureService* featureService,
                                           CREFSTRING resId,
                                           CREFSTRING clsName,
                                           CREFSTRING filterText,
                                           CREFSTRING geomName)
{
    CHECKNULL((MgFeatureService*)featureService, L"MgSelection.GetFeatureExtents");

    Ptr<MgFeatureAggregateOptions> geomQryOptions = new MgFeatureAggregateOptions();
    STRING cmd = L"EXTENT(" + geomName + L")";
    geomQryOptions->AddComputedProperty(L"EXTENT_PROP", cmd);
    geomQryOptions->SetFilter(filterText);

    Ptr<MgResourceIdentifier> featureResId = new MgResourceIdentifier(resId);
    Ptr<MgDataReader> geomDataReader = featureService->SelectAggregate(featureResId, clsName, geomQryOptions);
    Ptr<MgGeometry> geomFilter;
    Ptr<MgEnvelope> env;
    while (geomDataReader->ReadNext())
    {
        Ptr<MgByteReader> byteReader = geomDataReader->GetGeometry(L"EXTENT_PROP");
        MgAgfReaderWriter agfWriter;

        Ptr<MgGeometry> geom = agfWriter.Read(byteReader);
        Ptr<MgEnvelope> geomExtn = geom->Envelope();
        if (env == NULL)
        {
            env = geomExtn.Detach();
        }
        else
        {
            // We will never hit this code as EXTENT() only returns one envelope
            // but as we have reader, it is possible that it might return more than one
            // envelope theoretically
            env->ExpandToInclude(geomExtn);
        }
    }
    geomDataReader->Close();

    // Now convert the extent to the map coordinate system...

    // Parse the feature name for the schema and class
    STRING::size_type nDelimiter = clsName.find(L":");
    STRING schemaName;
    STRING className;

    if(STRING::npos == nDelimiter)
    {
        schemaName = L"";
        className = clsName;
    }
    else
    {
        schemaName = clsName.substr(0, nDelimiter);
        className = clsName.substr(nDelimiter + 1);
    }

    STRING spatialContextAssociation = L"";

    // Get the class definition so that we can find the spatial context association
    Ptr<MgClassDefinition> classDef = featureService->GetClassDefinition(featureResId, schemaName, className);
    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();

    // Find the spatial context for the geometric property.
    for(int index=0; index < propDefCol->GetCount(); index++)
    {
        Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(index);
        if (propDef->GetPropertyType () == MgFeaturePropertyType::GeometricProperty &&
            propDef->GetName() == geomName)
        {
            // We found the geometric property
            MgGeometricPropertyDefinition* geomProp = static_cast<MgGeometricPropertyDefinition*>(propDef.p);
            spatialContextAssociation = geomProp->GetSpatialContextAssociation();
            break;
        }
    }
    Ptr<MgSpatialContextReader> spatialContextReader = featureService->GetSpatialContexts(featureResId, false);
    STRING featureSourceCoordsys;
    if(spatialContextReader.p != NULL)
    {
        while(spatialContextReader->ReadNext())
        {
            STRING csrName = spatialContextReader->GetName();
            if(!spatialContextAssociation.empty() &&  csrName == spatialContextAssociation)
            {
                featureSourceCoordsys = spatialContextReader->GetCoordinateSystemWkt();
                break;
            }
            else if(featureSourceCoordsys.empty())
            {
                // This is the 1st spatial context returned
                // This will be overwritten if we find the association
                featureSourceCoordsys = spatialContextReader->GetCoordinateSystemWkt();
            }
        }
    }
    if(!featureSourceCoordsys.empty())
    {
        Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
        Ptr<MgCoordinateSystem> featCS = csFactory->Create(featureSourceCoordsys);
        if(m_map.p != NULL)
        {
            //get the map coordinate system
            STRING mapCoordsys = m_map->GetMapSRS();
            if(!mapCoordsys.empty())
            {
                //create the transform
                Ptr<MgCoordinateSystem> mapCS = csFactory->Create(mapCoordsys);
                Ptr<MgCoordinateSystemTransform> trans = new MgCoordinateSystemTransform(featCS, mapCS);

                //transform the envelope
                env = trans->Transform(env);
            }
        }
    }

    return env.Detach();
}


void MgSelectionBase::SetMap(MgMapBase* map)
{
    m_map = SAFE_ADDREF((MgMapBase*)map);
}


void MgSelectionBase::Serialize(MgStream* stream)
{
    stream->WriteInt32((INT32)m_selections.size());
    for(SelectionMap::iterator lIter = m_selections.begin(); lIter != m_selections.end(); lIter++)
    {
        stream->WriteString(lIter->first);
        SelectedClassMap* classMap = lIter->second;
        stream->WriteInt32((INT32)classMap->size());
        for(SelectedClassMap::iterator cIter = classMap->begin(); cIter != classMap->end(); cIter++)
        {
            stream->WriteString(cIter->first);
            SelectionList* selectionList = cIter->second;
            stream->WriteInt32((INT32)selectionList->size());
            for(SelectionList::iterator sIter = selectionList->begin(); sIter != selectionList->end(); sIter++)
                stream->WriteString(*sIter);
        }
    }
}


void MgSelectionBase::Deserialize(MgStream* stream)
{
    INT32 lMapCount;
    stream->GetInt32(lMapCount);

    for(INT32 i = 0; i < lMapCount; i++)
    {
        SelectedClassMap* classMap = new SelectedClassMap;
        STRING name;
        stream->GetString(name);
        m_selections[name] = classMap;
        INT32 cMapCount;
        stream->GetInt32(cMapCount);
        for(INT32 j = 0; j < cMapCount; j++)
        {
            SelectionList* selectionList = new SelectionList();
            STRING clsName;
            stream->GetString(clsName);
            (*classMap)[clsName] = selectionList;
            INT32 sMapCount;
            stream->GetInt32(sMapCount);
            for(INT32 k = 0; k < sMapCount; k++)
            {
                STRING sel;
                stream->GetString(sel);
                selectionList->push_back(sel);
            }
        }
    }
}


//////////////////////////////////////////////////////////////////
// Get the name of object
//
STRING MgSelectionBase::GetName()
{
    return L"";
}


//////////////////////////////////////////////////////////////////
// Does it allow to set the name
//
bool MgSelectionBase::CanSetName()
{
    return false;
}


//////////////////////////////////////////////////////////////////
// Open the resource
//
void MgSelectionBase::Open(MgResourceService* resourceService, CREFSTRING mapName)
{
    throw new MgNotImplementedException(L"MgSelectionBase.Save", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
// Save the resource
//
void MgSelectionBase::Save(MgResourceService* resourceService, CREFSTRING mapName)
{
    throw new MgNotImplementedException(L"MgSelectionBase.Save", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
STRING MgSelectionBase::GetResourceName(CREFSTRING sessionId, CREFSTRING mapName)
{
    return L"Session:" + sessionId + L"//" + mapName + L"." + MgResourceType::Selection;
}

//////////////////////////////////////////////////////////////////
const char* MgSelectionBase::GetResourceTypeName()
{
    return "Selection";
}


