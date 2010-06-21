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

#ifndef _FS_WFS_GET_FEATURE_PARAMS_H
#define _FS_WFS_GET_FEATURE_PARAMS_H

// Forward declaration
class MgXmlParser;
class MgXmlNamespaceManager;
class MgXmlSynchronizeOnNamespaceElement;
class MgXmlBeginElement;

class MgOgcWfsServer;

class WfsGetFeatureParams : public MgDisposable
{
public:
    /// <summary>
    /// Initializes the parameters of the request from a set of pre-parsed parameters
    /// </summary>
    /// <param name="requestParams">Input
    /// This contains all the parameters of the request.
    /// </param>
    /// <returns>
    /// nothing
    /// </returns>
    WfsGetFeatureParams(MgOgcWfsServer& oServer/*MgHttpRequestParam* requestParams*/);

    /// <summary>
    /// Initializes the parameters of the request from an XML document
    /// </summary>
    /// <param name="xmlParams">Input
    /// This contains the XML document to parse
    /// </param>
    /// <returns>
    /// nothing
    /// </returns>
    WfsGetFeatureParams(MgOgcWfsServer& oServer, CREFSTRING sXmlParams);

    /// <summary>
    /// Retrieves the requested feature types
    /// </summary>
    /// <returns>
    /// An MgStringCollection containing the feature types
    /// </returns>
    MgStringCollection* GetFeatureTypes();

    /// <summary>
    /// Class destructor
    /// </summary>
    /// <returns>
    /// none
    /// </returns>
    ~WfsGetFeatureParams();

    /// <summary>
    /// Retrieves the filter strings for the request
    /// </summary>
    /// <returns>
    /// An MgStringCollection containing the filter strings
    /// </returns>
    MgStringCollection* GetFilterStrings();


    /// <summary>
    /// Retrieves the required property strings for the request
    /// </summary>
    /// <returns>
    /// An MgStringCollection containing the property strings
    /// </returns>
    MgStringCollection* GetRequiredProperties();


    /// <summary>
    /// Retrieves the maximum number of features to return
    /// </summary>
    /// <returns>
    /// An INT32 values corresponding to the max features to return
    /// </returns>
    INT32 GetMaxFeatures();

    /// <summary>
    /// Retrieves the SRS in which to return geometries
    /// </summary>
    /// <returns>
    /// A STRING defining the SRS
    /// </returns>
    STRING GetSrs();

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
    static bool IsValidXmlRequest(CREFSTRING xmlString);

    MgXmlNamespaceManager& NamespaceManager();

INTERNAL_API:
    // Default constructor to keep Ptr<> happy
    WfsGetFeatureParams();

protected:
    virtual void Dispose(){delete this;}

private:
    // Use this in lieu of direct access to parameters.
    STRING GetRequestParameter(MgOgcWfsServer& oServer,CREFSTRING sParameterName);

    MgStringCollection* GetParenthesisedList(CREFSTRING sourceString);
    void BuildFilterStrings(CREFSTRING filters, CREFSTRING featureIds, CREFSTRING bbox);
    bool ParseQueryElement(MgOgcWfsServer& oServer,MgXmlParser& parser,MgXmlNamespaceManager& oNamespaces);
    bool ParseFilterElement(MgOgcWfsServer& oServer,MgXmlParser& parser,MgXmlNamespaceManager& oNamespaces);

    static bool MoveToEndOfElement(MgXmlParser& parser, MgXmlNamespaceManager& oMgr, const wchar_t* pszElementName);
    static STRING GetElementContents(MgXmlParser& parser);
    static bool IsValidXmlRequest(MgXmlParser& parser,MgXmlNamespaceManager& nsmgr);

    // Discretionary logic that implements "Tolerance and Forgiveness"
    // Some clients frame a <GetFeature> request sans namespace.  At the very
    // least they should do <GetFeature xmlns="http://www.opengis.net/wfs" ...>
    // but of course they don't always.  This call allows the former to be treated like
    // the latter by inserting a default namespace; if everything else is
    // to spec, then we'll simply be tolerant of the fact that they didn't
    // attach the right namespace.
    static void FixupMissingWfsNamespaceForGetFeature(MgXmlSynchronizeOnNamespaceElement& oGetFeatureElement,
                                                      MgXmlParser& parser,MgXmlNamespaceManager& nsmgr);


    Ptr<MgStringCollection> m_featureTypeList;
    Ptr<MgStringCollection> m_requiredPropertiesList;
    Ptr<MgStringCollection> m_filterStrings;
    MgXmlNamespaceManager*  m_pNamespaces; // collect namespaces (ns#) namespaces
    INT32 m_maxFeatures;
    STRING m_srs;
};

#endif  // _FS_WFS_GET_FEATURE_PARAMS_H
