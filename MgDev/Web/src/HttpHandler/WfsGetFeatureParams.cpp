//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "HttpRequestParam.h"
#include "HttpResourceStrings.h"
#include "XmlParser.h"
#include "WmsMapUtil.h"

#include "WfsGetFeatureParams.h"

/// <summary>
/// Initializes the parameters of the request from a set of pre-parsed parameters
/// (ie, from an HTTP GET request)
/// </summary>
/// <param name="requestParams">Input
/// This contains all the parameters of the request.
/// </param>
/// <returns>
/// nothing
/// </returns>
WfsGetFeatureParams::WfsGetFeatureParams(MgOgcWfsServer& oServer/*MgHttpRequestParam* params*/)
:   m_maxFeatures(-1)
,   m_filterStrings(new MgStringCollection())
,   m_featureTypeList(new MgStringCollection())
,   m_pNamespaces(new MgXmlNamespaceManager())
{

    // Get the required properties
    STRING propertyNames = GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsPropertyName);
    m_requiredPropertiesList = GetParenthesisedList(propertyNames);

    // Get the requested feature types
    STRING featureTypes = GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsTypeName);
    if(featureTypes.length() > 0)
    {
        m_featureTypeList = MgStringCollection::ParseCollection(featureTypes, L",");
    }
    else
    {
        m_featureTypeList = NULL;
    }

    // Get the requested feature IDs
    STRING featureIds = GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsFeatureId);

    // Get the requested filter
    STRING filters = GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsFilter);

    // Get the requested bounding box
    STRING bbox = GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsBbox);

    // Build the filter strings from the feature IDs, filters or bounding box
    BuildFilterStrings(filters, featureIds, bbox);

    // Get the requested SRS value
    m_srs = GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsSrsName);
    if(m_srs.empty())
    {
        m_srs = GetSRSFromBbox(bbox);
    }

    // Get the SRS in WKT form
    STRING srsWkt;
    if(!m_srs.empty())
    {
        MgWmsMapUtil::SrsToWktMapping(oServer,m_srs,srsWkt);
        if(!srsWkt.empty())
        {
            m_srs = srsWkt;
        }
    }

    // Get the maximum number of features to return
    string maxFeaturesParam = MgUtil::WideCharToMultiByte(GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsMaxFeatures));
    if(maxFeaturesParam.length() > 0)
    {
        m_maxFeatures = atoi(maxFeaturesParam.c_str());
    }
    else
    {
        // Return all features by default
        m_maxFeatures = -1;
    }

    // Get the requested output format
    m_outputFormat = GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsOutputFormat);

    // Get the wfs version
    m_version = GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsVersion);

    // Get the sortby property name
    m_sortCriteria = GetRequestParameter(oServer,MgHttpResourceStrings::reqWfsSortBy);

}

WfsGetFeatureParams::~WfsGetFeatureParams()
{
    delete m_pNamespaces;
}

MgXmlNamespaceManager& WfsGetFeatureParams::NamespaceManager()
{
    return *m_pNamespaces;
}



/// <summary>
/// Initializes the parameters of the request from an XML document
/// (ie, from an HTTP POST request)
/// </summary>
/// <param name="xmlParams">Input
/// This contains the XML document to parse
/// </param>
/// <returns>
/// nothing
/// </returns>
WfsGetFeatureParams::WfsGetFeatureParams(MgOgcWfsServer& oServer,CREFSTRING xmlRequestString)
:   m_maxFeatures(-1)
,   m_filterStrings(new MgStringCollection())
,   m_featureTypeList(new MgStringCollection())
,   m_pNamespaces(new MgXmlNamespaceManager())
{
    MgXmlParser parser(xmlRequestString.c_str());
    MgXmlNamespaceManager oNamespaces;

    // Set parsing options
    parser.SetOptions(keSkipWhitespace|keSkipComments|keSkipProcessingInstructions);

    parser.Next();
    MgXmlSynchronizeOnNamespaceElement oGetFeatureElement(parser,
                                                          _("http://www.opengis.net/wfs:GetFeature"),
                                                          oNamespaces);

    // Some "Tolerance and Forgiveness"
    FixupMissingWfsNamespaceForGetFeature(oGetFeatureElement,parser,oNamespaces);

    MgXmlBeginElement* pBegin;
    if(oGetFeatureElement.AtBegin(&pBegin)) {
        // Sanity check: this is the same code as the IsValidXmlRequest code,
        // and theoretically has already been done.  If we can rest assured that
        // the IsValid... call has always already been done, we can dispense with
        // this redundancy.
        // ------------------------------------------------------------------------
        // While strictly speaking service="WFS" should be present, it isn't always.
        // In general, if there's a namespace defined "http://www.opengis.net/wfs"
        // then you can be forgiving, (secretly wagging your finger at the person
        // who forgot to put that into their request... ;-)
        STRING sService;
        if( (pBegin->GetAttribute(_("service"), sService)
            && _wcsicmp(sService.c_str(), _("WFS")) == 0)
            || oNamespaces.HasNamespace(_("http://www.opengis.net/wfs")) ) {

            STRING sMaxFeatures;
            if(pBegin->GetAttribute(_("maxFeatures"), sMaxFeatures) && sMaxFeatures.length() > 0)
                m_maxFeatures = (int)MgUtil::StringToInt32(sMaxFeatures);
            else
                m_maxFeatures = -1;

            STRING sVersion;
            if(pBegin->GetAttribute(_("version"), sVersion) && sVersion.length() > 0)
                m_version = sVersion;

            STRING sOutputFormat;
            if(pBegin->GetAttribute(_("outputFormat"), sOutputFormat) && sOutputFormat.length() > 0)
                m_outputFormat = sOutputFormat;


            // We want to hang onto the namespaces that are
            // defined in the GetFeature request, since that will
            // likely contain namespaces that help us track feature type
            // names.
            m_pNamespaces->TrackBeginElement(*pBegin);
            /*
            int i=0,iTotalNamespaces = oNamespaces.TotalCount();
            for(int i=0;i<iTotalNamespaces;i++) {
                if(!oNamespaces.IsEclipsed(i)) {
                }
            }
            */

            // We've gotten all we need out of the <GetFeature> begin-element
            // so now let's dig into its contents: queries...
            parser.Next();
            while(!oGetFeatureElement.AtEnd()) {
                ParseQueryElement(oServer,parser,oNamespaces);
            };
        }
    }
}

// Default constructor to keep Ptr<> happy
WfsGetFeatureParams::WfsGetFeatureParams()
:   m_maxFeatures(-1)
,   m_filterStrings(new MgStringCollection())
,   m_featureTypeList(new MgStringCollection())
,   m_pNamespaces(new MgXmlNamespaceManager())
{
}

// Break up a string composed of multiple sections in parentheses into a collection
// of sub-strings. Sample input string: (Hello)(World1,World2)
MgStringCollection* WfsGetFeatureParams::GetParenthesisedList(CREFSTRING sourceString)
{
    MgStringCollection* stringList = new MgStringCollection();
    if(sourceString.length() > 0)
    {
        // Create a collection of strings
        STRING remaining = MgUtil::Trim(sourceString);
        while(remaining.length() > 0)
        {
            STRING::size_type openParenthesis = remaining.find_first_of(L"(");
            if(openParenthesis != string::npos)
            {
                STRING::size_type closeParenthesis = remaining.find_first_of(L")");
                if(closeParenthesis != string::npos)
                {
                    STRING thisString = remaining.substr(openParenthesis + 1, closeParenthesis - openParenthesis - 1);
                    stringList->Add(thisString);
                    remaining = remaining.substr(closeParenthesis + 1);
                }
            }
            else
            {
                stringList->Add(remaining);
                break;
            }
        }
    }
    return stringList;
}

// Build OGC filter XML stirngs based on the provided input filters.
void WfsGetFeatureParams::BuildFilterStrings(CREFSTRING filters, CREFSTRING featureIds, CREFSTRING bbox)
{
    // Create the required feature filters
    m_filterStrings = GetParenthesisedList(filters);
    if(m_filterStrings->GetCount() == 0)
    {
        // If no FILTER was specified, look for a BBOX
        if(bbox.length() > 0)
        {
            // Build a filter from the bounding box
            Ptr<MgStringCollection> bboxCoords = MgStringCollection::ParseCollection(bbox, L",");
            if(bboxCoords->GetCount() >= 4)
            {
                // TODO: Look into using StringStream and XmlElementEmitter for simplified generation
                // of these elements.
                STRING filterString = L"<ogc:Filter><ogc:BBOX><ogc:PropertyName>%MG_GEOM_PROP%</ogc:PropertyName><gml:Box><gml:coordinates>";
                filterString.append(MgUtil::Trim(bboxCoords->GetItem(0)));
                filterString.append(L",");
                filterString.append(MgUtil::Trim(bboxCoords->GetItem(1)));
                filterString.append(L" ");
                filterString.append(MgUtil::Trim(bboxCoords->GetItem(2)));
                filterString.append(L",");
                filterString.append(MgUtil::Trim(bboxCoords->GetItem(3)));
                filterString.append(L"</gml:coordinates></gml:Box></ogc:BBOX></ogc:Filter>");
                m_filterStrings->Add(filterString);
            }
        }
        // If no FILTER or BBOX, look for FEATUREID
        else if(featureIds.length() > 0)
        {
            // Build a filter from the list of feature Ids
            Ptr<MgStringCollection> featureIdList = MgStringCollection::ParseCollection(featureIds, L",");
            if(featureIdList->GetCount() > 0)
            {
                STRING filterString = L"<ogc:Filter>";
                for(int i = 0; i < featureIdList->GetCount(); i++)
                {
                    STRING thisFeatureId = MgUtil::Trim(featureIdList->GetItem(i));
                    if(thisFeatureId.length() > 0)
                    {
                        filterString.append(L"<ogc:GmlObjectId gml:id=");
                        filterString.append(thisFeatureId);
                        filterString.append(L"/>");
                    }
                }
                filterString.append(L"</ogc:Filter>");
                m_filterStrings->Add(filterString);
            }
        }
    }
}


// Get SRS from BBOX parameter
STRING WfsGetFeatureParams::GetSRSFromBbox(CREFSTRING bbox)
{
    if(bbox.length() > 0)
    {
        // Build a filter from the bounding box
        Ptr<MgStringCollection> bboxCoords = MgStringCollection::ParseCollection(bbox, L",");
        if(bboxCoords->GetCount() >= 5)
        {
            return MgUtil::Trim(bboxCoords->GetItem(4));
        }
    }

    return L"";
}

/// <summary>
/// Retrieves the requested feature types
/// </summary>
/// <returns>
/// An MgStringCollection containing the feature types
/// </returns>
MgStringCollection* WfsGetFeatureParams::GetFeatureTypes()
{
    return SAFE_ADDREF((MgStringCollection*)m_featureTypeList);
}

/// <summary>
/// Retrieves the filter strings for the request
/// </summary>
/// <returns>
/// An MgStringCollection containing the filter strings
/// </returns>
MgStringCollection* WfsGetFeatureParams::GetFilterStrings()
{
    return SAFE_ADDREF((MgStringCollection*)m_filterStrings);
}

/// <summary>
/// Retrieves the required property strings for the request
/// </summary>
/// <returns>
/// An MgStringCollection containing the property strings
/// </returns>
MgStringCollection* WfsGetFeatureParams::GetRequiredProperties()
{
    return SAFE_ADDREF((MgStringCollection*)m_requiredPropertiesList);
}

/// <summary>
/// Retrieves the maximum number of features to return
/// </summary>
/// <returns>
/// An INT32 values corresponding to the max features to return
/// </returns>
INT32 WfsGetFeatureParams::GetMaxFeatures()
{
    return m_maxFeatures;
}

/// <summary>
/// Retrieves the SRS in which to return geometries
/// </summary>
/// <returns>
/// A STRING defining the SRS
/// </returns>
STRING WfsGetFeatureParams::GetSrs()
{
    return m_srs;
}

/// <summary>
/// Retrieves the output format of the feautre information
/// </summary>
/// <returns>
/// A STRING defining the output format
/// </returns>
STRING WfsGetFeatureParams::GetOutputFormat()
{
    return m_outputFormat;
}

/// <summary>
/// Retrieves the wfs request version
/// </summary>
/// <returns>
/// A STRING defining the wfs version
/// </returns>
STRING WfsGetFeatureParams::GetVersion()
{
    return m_version;
}

/// <summary>
/// Retrieves the property name which is used sort the GetFeature result
/// </summary>
/// <returns>
/// A STRING defining the property name which is used sort the GetFeature result
/// </returns>
STRING WfsGetFeatureParams::GetSortCriteria()
{
    return m_sortCriteria;
}

/// <summary>
/// Performs a minimum amount of request parsing in order to determine
/// whether this class can handle the specified XML request
/// </summary>
/// <param name="xmlString">Input
/// This contains the XML document to parse
/// </param>
/// <returns>
/// True, if the XML request can be handled by this class.
/// </returns>
bool WfsGetFeatureParams::IsValidXmlRequest(CREFSTRING xmlString)
{
    MgXmlParser parser(xmlString.c_str());
    MgXmlNamespaceManager oNamespaces;

    // Set parsing options
    parser.SetOptions(keSkipWhitespace|keSkipComments|keSkipProcessingInstructions);

    parser.Next();

    // TODO: Probably don't need this: no need for a probe to
    // digest the entire element; we can just check to make sure
    // that the begin->Name is correct and that key attributes are
    // in place.
    MgXmlSynchronizeOnNamespaceElement oGetFeatureElement(parser,
                                                          _("http://www.opengis.net/wfs:GetFeature"),
                                                          oNamespaces);
    MgXmlBeginElement* pBegin;

    // Some "Tolerance and Forgiveness"
    FixupMissingWfsNamespaceForGetFeature(oGetFeatureElement,parser,oNamespaces);

    if(oGetFeatureElement.AtBegin(&pBegin)) {
        // While strictly speaking service="WFS" should be present, it isn't always.
        // In general, if there's a namespace defined "http://www.opengis.net/wfs"
        // then you can be forgiving, (secretly wagging your finger at the person
        // who forgot to put that into their request... ;-)
        STRING sService;
        if( (pBegin->GetAttribute(_("service"), sService)
            && _wcsicmp(sService.c_str(), _("WFS")) == 0)
            || oNamespaces.HasNamespace(_("http://www.opengis.net/wfs")) ) {
              return true;
        }
    }
    return false;
}

// Obsolete: the main overload does all the work now.
// Determine if this is a valid WFS GetFeature XML request string
// static method -- just probes whether the request meets minimal conformance.
bool WfsGetFeatureParams::IsValidXmlRequest(MgXmlParser& parser,MgXmlNamespaceManager& oNamespaces)
{
  return false; // TODO: remove...
}

// Skip the named XML element
bool WfsGetFeatureParams::MoveToEndOfElement(MgXmlParser& parser,MgXmlNamespaceManager& oMgr, const wchar_t* pszElementName)
{
    bool skipped = false;
    STRING sName;

    if(pszElementName == NULL && parser.Current().Type() == keBeginElement)
    {
        MgXmlBeginElement& begin = (MgXmlBeginElement&)parser.Current();
        sName = begin.Name();
        pszElementName = sName.c_str();
    }

    if(pszElementName != NULL)
    {
        do
        {
            if(parser.Current().Type() == keEndElement)
            {
                MgXmlEndElement& end = (MgXmlEndElement&)parser.Current();
                oMgr.TrackEndElement(end);
                if(_wcsicmp(end.Name().c_str(), pszElementName) == 0)
                {
                    skipped = true;
                    break;
                }
            }
        }
        while(parser.Next());
    }

    return skipped;
}

// Returns the entire current element as a string
STRING WfsGetFeatureParams::GetElementContents(MgXmlParser& parser)
{
    STRING contents;

    if(parser.Current().Type() == keBeginElement)
    {
        MgXmlBeginElement& begin = (MgXmlBeginElement&)parser.Current();
        STRING elementName = begin.Name().c_str();
        do
        {
            contents.append(parser.Current().Contents());
            if(parser.Current().Type() == keEndElement)
            {
                MgXmlEndElement& end = (MgXmlEndElement&)parser.Current();
                if(_wcsicmp(end.Name().c_str(), elementName.c_str()) == 0)
                {
                    // We've reached the end of the element
                    break;
                }
            }
        }
        while(parser.Next());
    }

    return contents;
}

// Parse a Query element
bool WfsGetFeatureParams::ParseQueryElement(MgOgcWfsServer& oServer,MgXmlParser& parser,MgXmlNamespaceManager& oNamespaces)
{
    MgXmlSynchronizeOnNamespaceElement oQueryElement(parser,
                                                     _("http://www.opengis.net/wfs:Query"),
                                                     oNamespaces);

    MgXmlBeginElement* pBegin;
    // Not at a Query element?  Unexpected; get out.
    if(!oQueryElement.AtBegin(&pBegin))
        return false;

    STRING sTypeName;
    if(pBegin->GetAttribute(_("typeName"),sTypeName))
        m_featureTypeList->Add(oServer.ProcessArgumentAs(_("typeName"),sTypeName.c_str()));

    // Done slurping attributes; now let's look inside.
    if(!pBegin->IsEmpty()) {
        // Step inside...
        parser.Next();

        while(!oQueryElement.AtEnd())
            // If we didn't find a Filter element, move on.
            if(!ParseFilterElement(oServer,parser,oNamespaces))
                parser.Next();
    }

    return true;
}


// Parse a Filter element
bool WfsGetFeatureParams::ParseFilterElement(MgOgcWfsServer& oServer,MgXmlParser& parser,MgXmlNamespaceManager& oNamespaces)
{
    MgXmlSynchronizeOnNamespaceElement oFilterElement(parser,
                                                     _("http://www.opengis.net/ogc:Filter"),
                                                     oNamespaces);

    MgXmlBeginElement* pBegin;
    // Not at a Filter element?  Unexpected; get out.
    if(!oFilterElement.AtBegin(&pBegin))
        return false;

    if(!pBegin->IsEmpty()) {
        STRING filterString = GetElementContents(parser);
        if(filterString.length() > 0) {
            m_filterStrings->Add(oServer.ProcessArgumentAs(_("filter"),filterString.c_str()));
        }
    }

    // It's a filter element, and we "processed" it, but
    // it's possible that it contributed nothing to the filter list.
    return true;
}

STRING WfsGetFeatureParams::GetRequestParameter(MgOgcWfsServer& oServer,CREFSTRING sParameterName)
{
    CPSZ psz = oServer.RequestParameter(sParameterName.c_str());
    return STRING(psz? psz : _(""));
}


void WfsGetFeatureParams::FixupMissingWfsNamespaceForGetFeature(MgXmlSynchronizeOnNamespaceElement& oGetFeatureElement,
                                                                MgXmlParser& parser,MgXmlNamespaceManager& oNamespaces)

{
    MgXmlBeginElement* pBegin;

    // --------------------------------------------------------
    // Some Tolerance and Forgiveness logic: what if the client
    // forgot to put the request into the correct namespace, but
    // everything else seems correct?
    // If we're not at the *expected* begin element, but we are
    // at another begin element, let's probe deeper.
    if(!oGetFeatureElement.AtBegin(&pBegin) && parser.Current().Type() == keBeginElement) {
        MgXmlBeginElement& oBegin=(MgXmlBeginElement&)parser.Current();
        if(oBegin.Name() == _("GetFeature")) {
            // That's promising.  The element name is right, but
            // it's just not in the proper namespace.  If there isn't
            // one already, let's cruft up a default WFS namespace
            // This should make the AtBegin check work iff the
            // <GetFeature> element simply forgot the xmlns="..." declaration.
            // A bad request, I know, but there's code out there sending
            // this sort of stuff, and other WFS's handle it.
            if(oNamespaces.NamespaceFrom(MgXmlNamespaceManager::ksDefaultNsPrefix).length() == 0)
                oNamespaces.AddNamespace(MgXmlNamespaceManager::ksDefaultNsPrefix,_("http://www.opengis.net/wfs"));
        }
    }
    // --------------------------------------------------------
}
