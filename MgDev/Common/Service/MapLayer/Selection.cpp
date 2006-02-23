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

#include "AceCommon.h"
#include "Util/XmlDefs.h"
#include "Util/XmlUtil.h"
#include <algorithm>

IMPLEMENT_CREATE_OBJECT(MgSelection)

/////////////////////////////////////////
// Dummy constructor for Ptr<> template
//
MgSelection::MgSelection()
{
    m_stream = new MgMemoryStreamHelper();
}


// Initialize selection generation from a map
MgSelection::MgSelection(MgMap* map)
{
    m_map = SAFE_ADDREF(map);
    m_stream = new MgMemoryStreamHelper();
}


// Initialize selection from a map and xml selection
MgSelection::MgSelection(MgMap* map, CREFSTRING xmlSelectionString)
{
    m_map = SAFE_ADDREF(map);
    m_stream = new MgMemoryStreamHelper();
    FromXml(xmlSelectionString);
}


// Destructor
MgSelection::~MgSelection()
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


void MgSelection::Dispose()
{
    delete this;
}


// Read selection from XML document.  Previous selection is cleared.
void MgSelection::FromXml(CREFSTRING xmlSelectionString)
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
STRING MgSelection::ToXml()
{
    return ToXml(true);
}


/////////////////////////////////////////
// Write selection as XML document
// The XML generated may or may not contain a heading XML declarator
//
STRING MgSelection::ToXml(bool withXmlDecl)
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
void MgSelection::AddFeatures(MgLayer* layer, MgFeatureReader* featureReader, INT32 nFeatures)
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
        MgLayer::IdPropertyList propList = layer->GetIdPropertyList();
        for (MgLayer::IdPropertyList::iterator idIter = propList.begin(); idIter != propList.end(); idIter++)
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
void MgSelection::AddFeatureIds(MgLayer* layer, CREFSTRING className, MgPropertyCollection* props )
{
    MgLayer::IdPropertyList propList = layer->GetIdPropertyList();
    MgLayer::IdPropertyList::iterator idIter;
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


void MgSelection::AddFeatureIdInt16(MgLayer* layer, CREFSTRING className, INT16 identifier)
{
    wstring layerName = layer->GetObjectId();
    m_stream->Clear();
    m_stream->WriteUINT16((UINT16)identifier);
    wstring b64;
    UnicodeString::MultiByteToWideChar(m_stream->ToBase64().c_str(), b64);
    Add(layerName, className, b64);
}


void MgSelection::AddFeatureIdInt32(MgLayer* layer, CREFSTRING className, INT32 identifier)
{
    wstring layerName = layer->GetObjectId();
    m_stream->Clear();
    m_stream->WriteUINT32((UINT32)identifier);
    wstring b64;
    UnicodeString::MultiByteToWideChar(m_stream->ToBase64().c_str(), b64);
    Add(layerName, className, b64);
}


void MgSelection::AddFeatureIdInt64(MgLayer* layer, CREFSTRING className, INT64 identifier)
{
    wstring layerName = layer->GetObjectId();
    m_stream->Clear();
    m_stream->WriteINT64(identifier);
    wstring b64;
    UnicodeString::MultiByteToWideChar(m_stream->ToBase64().c_str(), b64);
    Add(layerName, className, b64);
}


void MgSelection::AddFeatureIdString(MgLayer* layer, CREFSTRING className, CREFSTRING identifier)
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
MgReadOnlyLayerCollection* MgSelection::GetLayers()
{
    if (0 == (MgMap*)m_map)
    {
        throw new MgInvalidMapDefinitionException(L"MgSelection::GetLayers", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgReadOnlyLayerCollection> selLayers = new MgReadOnlyLayerCollection();

    Ptr<MgLayerCollection> layers = m_map->GetLayers();
    for (int i=0; i<layers->GetCount(); i++)
    {
        Ptr<MgLayer> layer = layers->GetItem(i);
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


STRING MgSelection::GetClass(CREFSTRING layer)
{
    Ptr<MgStringCollection> strCol = this->GetClasses(layer);
    return strCol->GetItem(0);
}


// Returns list of selected classes for a layer
MgStringCollection* MgSelection::GetClasses(CREFSTRING layer)
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
bool MgSelection::Contains(MgLayer* layer, CREFSTRING className)
{
    bool bContains = false;

    MG_TRY()

    SelectionList* selList = GetSelections(layer, className);

    if (0 != selList)
    {
        bContains = true;
    }

    MG_CATCH_AND_THROW(L"MgSelection::Contains");

    return bContains;
}


// Generates an Fdo filter text for the selections of the
//specified layer and class
STRING MgSelection::GenerateFilter(MgLayer* layer, CREFSTRING className)
{
    STRING filter;
    bool filterReserved = false;

    MG_TRY()

    SelectionList* selList = GetSelections(layer, className);
    MgLayer::IdPropertyList& idList = layer->GetIdPropertyList();

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
            MgLayer::IdPropertyList::iterator idIter;
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
                default:
                    throw new MgNotImplementedException(L"MgSelection::GenerateFilter", __LINE__, __WFILE__, NULL, L"", NULL);
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

    MG_CATCH_AND_THROW(L"MgSelection::GenerateFilter");

    return filter;
}


void MgSelection::Add(CREFSTRING layerId, CREFSTRING className, CREFSTRING id)
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


void MgSelection::Add(wstring& layerId, wstring& className, SelectionList* idList)
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


MgSelection::SelectionList* MgSelection::GetSelections(MgLayer* layer, CREFSTRING className)
{
    SelectionList* selList = NULL;

    if (0 == layer)
    {
        throw new MgNullArgumentException(L"MgSelection::GetSelections", __LINE__, __WFILE__, NULL, L"", NULL);
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


MgEnvelope* MgSelection::GetExtents(MgFeatureService* featureService)
{
    CHECKNULL((MgFeatureService*)featureService, L"MgSelection.GetExtents");

    Ptr<MgEnvelope> env = (MgEnvelope*)NULL;
    Ptr<MgReadOnlyLayerCollection> roLayerCol = this->GetLayers();

    if (roLayerCol != NULL)
    {
        INT32 cnt = roLayerCol->GetCount();
        for (INT32 i=0; i < cnt; i++)
        {
            Ptr<MgLayer> layer = roLayerCol->GetItem(i); // Collection can not have a NULL layer

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
                env = SAFE_ADDREF((MgEnvelope*)clsEnv);
            }
        }
    }
    return SAFE_ADDREF((MgEnvelope*)env);
}


MgEnvelope* MgSelection::GetFeatureExtents(MgFeatureService* featureService,
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

    Ptr<MgGeometry> geomFilter = (MgGeometry*)NULL;
    Ptr<MgEnvelope> env = (MgEnvelope*)NULL;
    while (geomDataReader->ReadNext())
    {
        Ptr<MgByteReader> byteReader = geomDataReader->GetGeometry(L"EXTENT_PROP");
        MgAgfReaderWriter agfWriter;

        Ptr<MgGeometry> geom = agfWriter.Read(byteReader);
        Ptr<MgEnvelope> geomExtn = geom->Envelope();
        if (env == NULL)
        {
            env = SAFE_ADDREF((MgEnvelope*)geomExtn);
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

    return SAFE_ADDREF((MgEnvelope*)env);
}


void MgSelection::SetMap(MgMap* map)
{
    m_map = SAFE_ADDREF((MgMap*)map);
}


void MgSelection::Serialize(MgStream* stream)
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


void MgSelection::Deserialize(MgStream* stream)
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
STRING MgSelection::GetName()
{
    return L"";
}


//////////////////////////////////////////////////////////////////
// Does it allow to set the name
//
bool MgSelection::CanSetName()
{
    return false;
}


//////////////////////////////////////////////////////////////////
// Open the resource
//
void MgSelection::Open(MgResourceService* resourceService, CREFSTRING mapName)
{
    STRING sessionId;
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL) sessionId = userInfo->GetMgSessionId();
    if (sessionId.empty())
    {
        throw new MgSessionExpiredException(L"MgSelection.Open",__LINE__,__WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(GetResourceName(sessionId, mapName));
    MgResource::Open(resourceService, resId);
}


//////////////////////////////////////////////////////////////////
// Save the resource
//
void MgSelection::Save(MgResourceService* resourceService, CREFSTRING mapName)
{
    STRING sessionId;
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL) sessionId = userInfo->GetMgSessionId();
    if (sessionId.empty())
    {
        throw new MgSessionExpiredException(L"MgSelection.Save",__LINE__,__WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(GetResourceName(sessionId, mapName));
    MgResource::Save(resourceService, resId);
}


//////////////////////////////////////////////////////////////////
STRING MgSelection::GetResourceName(CREFSTRING sessionId, CREFSTRING mapName)
{
    return L"Session:" + sessionId + L"//" + mapName + L"." + MgResourceType::Selection;
}

//////////////////////////////////////////////////////////////////
const char* MgSelection::GetResourceTypeName() 
{
    return "Selection";
}
