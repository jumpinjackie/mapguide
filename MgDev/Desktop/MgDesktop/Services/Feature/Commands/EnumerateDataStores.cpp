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
#include "EnumerateDataStores.h"

MgdEnumerateDataStores::MgdEnumerateDataStores()
{
    // This XML follows the DataStoreList-1.0.0.xsd schema
    m_xmlUtil = new MgXmlUtil("DataStoreList" /* NOXLATE */);
    CHECKNULL(m_xmlUtil, L"MgdEnumerateDataStores.EnumerateDataStores()");
}

MgdEnumerateDataStores::~MgdEnumerateDataStores()
{
    delete m_xmlUtil;
    m_xmlUtil = NULL;
}


MgByteReader* MgdEnumerateDataStores::EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Connect to the provider
    FdoPtr<FdoIDataStoreReader> fdoDataStoreReader;

    Ptr<MgdFeatureConnection> msfc = new MgdFeatureConnection(providerName, partialConnString);
    if ((NULL != msfc.p) && (( msfc->IsConnectionOpen() ) || ( msfc->IsConnectionPending() )))
    {
        // The reference to the FDO connection from the MgdFeatureConnection object must be cleaned up before the parent object
        // otherwise it leaves the FDO connection marked as still in use.
        FdoPtr<FdoIConnection> fdoConnection;
        fdoConnection = msfc->GetConnection();

        FdoPtr<FdoIListDataStores> fdoCommand = (FdoIListDataStores*)fdoConnection->CreateCommand(FdoCommandType_ListDataStores);
        CHECKNULL((FdoIListDataStores*)fdoCommand, L"MgdEnumerateDataStores.EnumerateDataStores");

        fdoCommand->SetIncludeNonFdoEnabledDatastores(true);

        // Execute the command
        fdoDataStoreReader = fdoCommand->Execute();
        CHECKNULL((FdoIDataStoreReader*)fdoDataStoreReader, L"MgdEnumerateDataStores.EnumerateDataStores");

        // Add Feature Provider element
        DOMElement* rootElem = m_xmlUtil->GetRootNode();

        while(fdoDataStoreReader->ReadNext())
        {
            FdoString* dataStoreName = fdoDataStoreReader->GetName();
            const char *name = MgUtil::WideCharToMultiByte(dataStoreName);

            DOMElement* featureProviderElem = m_xmlUtil->AddChildNode(rootElem, "DataStore" /* NOXLATE */);

            // Add DataStore Name
            m_xmlUtil->AddTextNode(featureProviderElem, "Name" /* NOXLATE */, name);

            // Add FDO Enabled
            m_xmlUtil->AddTextNode(featureProviderElem, "FdoEnabled" /* NOXLATE */, fdoDataStoreReader->GetIsFdoEnabled());

            delete[] name;
            name = NULL;
        }

        // Close the reader
        fdoDataStoreReader->Close();

        // Convert the XML to a byte reader
        byteReader = m_xmlUtil->ToReader();
    }
    else
    {
        throw new MgdConnectionFailedException(L"MgdEnumerateDataStores::EnumerateDataStores()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdEnumerateDataStores.EnumerateDataStores")

    return byteReader.Detach();
}
