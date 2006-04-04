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

#include "ServerFeatureServiceDefs.h"
#include "ServerGetConnectionPropertyValues.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureUtil.h"
#include "CryptographyUtil.h"

#define CHECKOPEN(methodName) \
    if (!IsConnectionOpen())  \
        throw new MgConnectionNotOpenException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \

MgServerGetConnectionPropertyValues::MgServerGetConnectionPropertyValues()
{
    m_fdoConn = NULL;
}

MgServerGetConnectionPropertyValues::~MgServerGetConnectionPropertyValues()
{
    if (NULL != m_fdoConn)
        m_fdoConn->Release();
}

// Executes the describe schema command and serializes the schema to XML
MgStringCollection* MgServerGetConnectionPropertyValues::GetConnectionPropertyValues( CREFSTRING providerName,
                                                                                      CREFSTRING propertyName,
                                                                                      CREFSTRING partialConnString )
{
    Ptr<MgStringCollection> stringCol;
    stringCol = NULL;

    MG_FEATURE_SERVICE_TRY()

    if (providerName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerGetConnectionPropertyValues.GetConnectionPropertyValues",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (propertyName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerGetConnectionPropertyValues.GetConnectionPropertyValues",
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

        MG_CRYPTOGRAPHY_CATCH(L"MgServerGetConnectionPropertyValues.GetConnectionPropertyValues")

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
    MgServerFeatureConnection msfc(providerName, decryptedPartialConnString);
    m_fdoConn = msfc.GetConnection();
    CHECKNULL((FdoIConnection*)m_fdoConn, L"MgServerGetConnectionPropertyValues.GetConnectionPropertyValues");

    // Get Connection Info
    GisPtr<FdoIConnectionInfo> connInfo = m_fdoConn->GetConnectionInfo();
    CHECKNULL((FdoIConnectionInfo*)connInfo, L"MgServerGetConnectionPropertyValues.GetConnectionPropertyValues");

    // Get Connection Property Dictionary
    GisPtr<FdoIConnectionPropertyDictionary> fdoConnPropDict = connInfo->GetConnectionProperties();
    CHECKNULL((FdoIConnectionPropertyDictionary*)fdoConnPropDict, L"MgServerGetConnectionPropertyValues.GetConnectionPropertyValues");

    bool enumerable = fdoConnPropDict->IsPropertyEnumerable((GisString*)propertyName.c_str());
    if (!enumerable)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(propertyName);

        throw new MgInvalidArgumentException(L"MgServerGetConnectionPropertyValues.GetConnectionPropertyValues",
            __LINE__, __WFILE__, &arguments, L"MgPropertyNotEnumerable", NULL);
    }

    GisInt32 cnt = 0;
    GisString** propertyValues = fdoConnPropDict->EnumeratePropertyValues((GisString*)propertyName.c_str(), cnt);

    if (propertyValues != NULL && cnt > 0)
    {
        stringCol = new MgStringCollection();
        for( GisInt32 i=0; i < cnt; i++ )
        {
            stringCol->Add(propertyValues[i]);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetConnectionPropertyValues.GetConnectionPropertyValues")

    return SAFE_ADDREF((MgStringCollection*)stringCol);
}
