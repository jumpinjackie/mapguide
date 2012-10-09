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
#include "GetFeatureProviders.h"

MgdGetFeatureProviders::MgdGetFeatureProviders()
{
    FdoPtr<IProviderRegistry> providerReg = FdoFeatureAccessManager::GetProviderRegistry();
    CHECKNULL(providerReg, L"MgdGetFeatureProviders.MgdGetFeatureProviders()");

    FdoPtr<IConnectionManager> connManager = FdoFeatureAccessManager::GetConnectionManager();
    CHECKNULL(connManager, L"MgdGetFeatureProviders.MgdGetFeatureProviders()");

    m_fdoProviderCol = providerReg->GetProviders();
    CHECKNULL(m_fdoProviderCol, L"MgdGetFeatureProviders.MgdGetFeatureProviders()");

    // this XML follows the FeatureProviderRegistry-1.0.0.xsd schema
    m_xmlUtil = new MgXmlUtil("FeatureProviderRegistry" /* NOXLATE */);
    CHECKNULL(m_xmlUtil, L"MgdGetFeatureProviders.MgdGetFeatureProviders()");

    // no more risk of exceptions, so we can now assign these
    m_providerReg = providerReg.Detach();
    m_connManager = connManager.Detach();
}

MgdGetFeatureProviders::~MgdGetFeatureProviders()
{
    FDO_SAFE_RELEASE(m_providerReg);
    FDO_SAFE_RELEASE(m_connManager);

    // do not release this one
    m_fdoProviderCol = NULL;

    delete m_xmlUtil;
}


MgByteReader* MgdGetFeatureProviders::GetFeatureProviders()
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    CreateFeatureProvidersDocument();
    byteReader = m_xmlUtil->ToReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGetFeatureProviders.GetFeatureProviders")

    return byteReader.Detach();
}

void MgdGetFeatureProviders::CreateFeatureProvidersDocument()
{
    CHECKNULL(m_fdoProviderCol, L"MgdGetFeatureProviders.CreateFeatureProvidersDocument");

    INT32 cnt = m_fdoProviderCol->GetCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        FdoPtr<FdoProvider> fdoProvider = m_fdoProviderCol->GetItem(i);

        // Get ProviderName
        FdoString* providerName = fdoProvider->GetName();

        const char *name        = MgUtil::WideCharToMultiByte(providerName);
        const char *dispname    = MgUtil::WideCharToMultiByte(fdoProvider->GetDisplayName());
        const char *description = MgUtil::WideCharToMultiByte(fdoProvider->GetDescription());
        const char *version     = MgUtil::WideCharToMultiByte(fdoProvider->GetVersion());
        const char *fdoversion  = MgUtil::WideCharToMultiByte(fdoProvider->GetFeatureDataObjectsVersion());

        // Add Feature Provider element
        DOMElement* rootElem = m_xmlUtil->GetRootNode();
        DOMElement* featureProviderElem = m_xmlUtil->AddChildNode(rootElem, "FeatureProvider" /* NOXLATE */);

        // Add Provider Name
        m_xmlUtil->AddTextNode(featureProviderElem, "Name" /* NOXLATE */, name);
        // Display Name
        m_xmlUtil->AddTextNode(featureProviderElem, "DisplayName" /* NOXLATE */, dispname);
        // Description
        m_xmlUtil->AddTextNode(featureProviderElem, "Description" /* NOXLATE */, description);
        // Version
        m_xmlUtil->AddTextNode(featureProviderElem, "Version" /* NOXLATE */, version);
        // FDO version
        m_xmlUtil->AddTextNode(featureProviderElem, "FeatureDataObjectsVersion" /* NOXLATE */, fdoversion);

        delete[] name;
        delete[] dispname;
        delete[] description;
        delete[] version;
        delete[] fdoversion;

        AddConnectionProperties(featureProviderElem, providerName);
    }
}

void MgdGetFeatureProviders::AddConnectionProperties(DOMElement* providerElem, FdoString* providerName)
{
    CHECKNULL(providerElem, L"MgdGetFeatureProviders.AddConnectionProperties");

    // Get Properties
    FdoInt32 totalProperties = 0;

    // Add ConnnectionProperties element (mandatory element)
    DOMElement* connPropRootElem = m_xmlUtil->AddChildNode(providerElem, "ConnectionProperties" /* NOXLATE */ );
    CHECKNULL(connPropRootElem, L"MgdGetFeatureProviders.AddConnectionProperties");

    // We ignore any exception thrown here so that even if client dll/so is missing, GetFeatureProviders
    // will continue to work.
    MG_FEATURE_SERVICE_TRY()

    // Get FdoIConnection instance
    // TODO: Should this connection be cached?
    FdoPtr<FdoIConnection> fdoConn = m_connManager->CreateConnection(providerName);
    CHECKNULL((FdoIConnection*)fdoConn, L"MgdGetFeatureProviders.AddConnectionProperties");

    // Get FdoIConnectionInfo
    FdoPtr<FdoIConnectionInfo> fdoConnInfo = fdoConn->GetConnectionInfo();
    CHECKNULL((FdoIConnectionInfo*)fdoConnInfo, L"MgdGetFeatureProviders.AddConnectionProperties");

    // Get FdoIConnectionPropertyDictionary
    FdoPtr<FdoIConnectionPropertyDictionary> fdoConnPropertyDict = fdoConnInfo->GetConnectionProperties();
    CHECKNULL((FdoIConnectionPropertyDictionary*)fdoConnPropertyDict, L"MgdGetFeatureProviders.AddConnectionProperties");

    // Get list of all properties
    FdoString** properties = fdoConnPropertyDict->GetPropertyNames(totalProperties);
    CHECKNULL(properties, L"MgdGetFeatureProviders.AddConnectionProperties");

    for ( FdoInt32 i=0; i < totalProperties; i++ )
    {
        AddConnectionProperty(connPropRootElem, properties[i], fdoConnPropertyDict);
    }

    MG_FEATURE_SERVICE_CATCH(L"MgdGetFeatureProviders.GetFeatureProviders") // do not rethrow so that GetFeatureProviders works
}

void MgdGetFeatureProviders::AddConnectionProperty(DOMElement* connPropRootElem,
                                                   FdoString* propertyName,
                                                   FdoIConnectionPropertyDictionary* fdoConnPropertyDict)
{
    CHECKNULL(connPropRootElem,     L"MgdGetFeatureProviders.AddConnectionProperty");
    CHECKNULL(propertyName,         L"MgdGetFeatureProviders.AddConnectionProperty");
    CHECKNULL(fdoConnPropertyDict,  L"MgdGetFeatureProviders.AddConnectionProperty");

    DOMElement* connPropElem = m_xmlUtil->AddChildNode(connPropRootElem, "ConnectionProperty"  /* NOXLATE */ );

    bool isEnumerable = fdoConnPropertyDict->IsPropertyEnumerable(propertyName);
    bool isProtected  = fdoConnPropertyDict->IsPropertyProtected(propertyName);
    bool isRequired   = fdoConnPropertyDict->IsPropertyRequired(propertyName);

    // Set Required attribute
    if (isRequired)
        connPropElem->setAttribute(X("Required"), X("true") /* NOXLATE */);
    else
        connPropElem->setAttribute(X("Required"), X("false") /* NOXLATE */);

    // Set protected attribute
    if (isProtected)
        connPropElem->setAttribute(X("Protected"), X("true") /* NOXLATE */);
    else
        connPropElem->setAttribute(X("Protected"), X("false") /* NOXLATE */);

    // Set enumerable attribute
    if (isEnumerable)
        connPropElem->setAttribute(X("Enumerable"), X("true") /* NOXLATE */);
    else
        connPropElem->setAttribute(X("Enumerable"), X("false") /* NOXLATE */);

    // Add property name
    const char *propName = MgUtil::WideCharToMultiByte(propertyName);
    m_xmlUtil->AddTextNode(connPropElem, "Name" /* NOXLATE */, propName);
    delete[] propName;

    // Add Localized Name
    FdoString* propLocalizedName = fdoConnPropertyDict->GetLocalizedName(propertyName);
    if (propLocalizedName)
    {
        const char *localname = MgUtil::WideCharToMultiByte(propLocalizedName);
        m_xmlUtil->AddTextNode(connPropElem, "LocalizedName" /* NOXLATE */, localname);
        delete[] localname;
    }

    // Add Default value
    FdoString* propDefaultValue = fdoConnPropertyDict->GetPropertyDefault(propertyName);
    if (propDefaultValue)
    {
        const char *defvalue = MgUtil::WideCharToMultiByte(propDefaultValue);
        m_xmlUtil->AddTextNode(connPropElem, "DefaultValue" /* NOXLATE */, defvalue);
        delete[] defvalue;
    }

    // Add the values of enumerable properties if we can
    if (isEnumerable)
    {
        FdoInt32 propCnt;
        FdoString** propValues = fdoConnPropertyDict->EnumeratePropertyValues(propertyName, propCnt);
        for ( FdoInt32 j = 0; j < propCnt; j++ )
        {
            const char *value = MgUtil::WideCharToMultiByte(propValues[j]);
            m_xmlUtil->AddTextNode(connPropElem, "Value" /* NOXLATE */, value);
            delete[] value;
        }
    }
}
