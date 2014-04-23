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
#include "GetConnectionPropertyValues.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/CryptoDefs.h"

MgdGetConnectionPropertyValues::MgdGetConnectionPropertyValues()
{
}

MgdGetConnectionPropertyValues::~MgdGetConnectionPropertyValues()
{
}

// Executes the describe schema command and serializes the schema to XML
MgStringCollection* MgdGetConnectionPropertyValues::GetConnectionPropertyValues( CREFSTRING providerName,
                                                                                      CREFSTRING propertyName,
                                                                                      CREFSTRING partialConnString )
{
    Ptr<MgStringCollection> stringCol;

    MG_FEATURE_SERVICE_TRY()

    if (providerName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdGetConnectionPropertyValues.GetConnectionPropertyValues",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (propertyName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdGetConnectionPropertyValues.GetConnectionPropertyValues",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    // Decrypt the partial connection string if it was encrypted.

    STRING decryptedPartialConnString;

    {
        MG_CRYPTOGRAPHY_TRY()

        MgCryptographyUtil cryptoUtil;
        string cipherText, plainText;

        MgUtil::WideCharToMultiByte(partialConnString, cipherText);
        cryptoUtil.DecryptString(cipherText, plainText);
        MgUtil::MultiByteToWideChar(plainText, decryptedPartialConnString);

        MG_CRYPTOGRAPHY_CATCH(L"MgdGetConnectionPropertyValues.GetConnectionPropertyValues")

        if (cryptographyException != NULL)
        {
            decryptedPartialConnString = partialConnString;
        }
    }

    // ODBC provider returns list of DataSourceNames and does not require connection string for this.
    // Therefore, limitation on connection string to be valid is removed. Also, as ODBC provider does
    // provide any connection string, connection can not be opened and it will remain in closed state.
    // Therefore validity of connection state does not apply.

    // Connect to provider
    Ptr<MgdFeatureConnection> msfc = new MgdFeatureConnection(providerName, decryptedPartialConnString);
    {
        if(NULL == msfc.p)
        {
            throw new MgdConnectionFailedException(L"MgdGetConnectionPropertyValues.GetConnectionPropertyValues", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // The reference to the FDO connection from the MgdFeatureConnection object must be cleaned up before the parent object
        // otherwise it leaves the FDO connection marked as still in use.
        FdoPtr<FdoIConnection> fdoConn = msfc->GetConnection();
        CHECKNULL((FdoIConnection*)fdoConn, L"MgdGetConnectionPropertyValues.GetConnectionPropertyValues");

        // Get Connection Info
        FdoPtr<FdoIConnectionInfo> connInfo = fdoConn->GetConnectionInfo();
        CHECKNULL((FdoIConnectionInfo*)connInfo, L"MgdGetConnectionPropertyValues.GetConnectionPropertyValues");

        // Get Connection Property Dictionary
        FdoPtr<FdoIConnectionPropertyDictionary> fdoConnPropDict = connInfo->GetConnectionProperties();
        CHECKNULL((FdoIConnectionPropertyDictionary*)fdoConnPropDict, L"MgdGetConnectionPropertyValues.GetConnectionPropertyValues");

        bool enumerable = fdoConnPropDict->IsPropertyEnumerable((FdoString*)propertyName.c_str());
        if (!enumerable)
        {
            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(propertyName);

            throw new MgInvalidArgumentException(L"MgdGetConnectionPropertyValues.GetConnectionPropertyValues",
                __LINE__, __WFILE__, &arguments, L"MgPropertyNotEnumerable", NULL);
        }

        FdoInt32 cnt = 0;
        FdoString** propertyValues = fdoConnPropDict->EnumeratePropertyValues((FdoString*)propertyName.c_str(), cnt);

        if (propertyValues != NULL && cnt > 0)
        {
            stringCol = new MgStringCollection();
            for( FdoInt32 i=0; i < cnt; i++ )
            {
                stringCol->Add(propertyValues[i]);
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGetConnectionPropertyValues.GetConnectionPropertyValues")

    return stringCol.Detach();
}
