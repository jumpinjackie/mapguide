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
#include "CryptographyManager.h"
#include "CryptographyUtil.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgCryptographyManager::MgCryptographyManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgCryptographyManager::~MgCryptographyManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Disposes the object.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyManager::Dispose()
{
    delete this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the class ID.
/// </summary>
///----------------------------------------------------------------------------

INT32 MgCryptographyManager::GetClassId()
{
    return m_cls_id;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines if the specified string is decryptable.
/// </summary>
///----------------------------------------------------------------------------

bool MgCryptographyManager::IsStringDecryptable(CREFSTRING str) const
{
    return m_cryptoUtil.IsStringDecryptable(str);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines if the specified string is encrypted.
/// </summary>
///----------------------------------------------------------------------------

bool MgCryptographyManager::IsStringEncrypted(CREFSTRING str) const
{
    return m_cryptoUtil.IsStringEncrypted(str);
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts credentials.
/// </summary>
///----------------------------------------------------------------------------

STRING MgCryptographyManager::EncryptCredentials(CREFSTRING username,
    CREFSTRING password)
{
    STRING credentials;

    MG_CRYPTOGRAPHY_TRY()

    m_cryptoUtil.EncryptCredentials(username, password, credentials);

    MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgCryptographyManager.EncryptCredentials");

    return credentials;
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts a string.
/// </summary>
///----------------------------------------------------------------------------

STRING MgCryptographyManager::EncryptString(CREFSTRING plainText)
{
    STRING cipherText;

    MG_CRYPTOGRAPHY_TRY()

    m_cryptoUtil.EncryptString(plainText, cipherText);

    MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgCryptographyManager.EncryptString")

    return cipherText;
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts a username.
/// </summary>
///----------------------------------------------------------------------------

STRING MgCryptographyManager::EncryptUsername(CREFSTRING username)
{
    STRING encryptedUsername;

    MG_CRYPTOGRAPHY_TRY()

    m_cryptoUtil.EncryptUsername(username, encryptedUsername);

    MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgCryptographyManager.EncryptUsername")

    return encryptedUsername;
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts a password.
/// </summary>
///----------------------------------------------------------------------------

STRING MgCryptographyManager::EncryptPassword(CREFSTRING password)
{
    STRING encryptedPassword;

    MG_CRYPTOGRAPHY_TRY()

    m_cryptoUtil.EncryptPassword(password, encryptedPassword);

    MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgCryptographyManager.EncryptPassword")

    return encryptedPassword;
}
