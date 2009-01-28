//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "CryptographyManager.h"
#include "CryptographyUtil.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgCryptographyManager::MgCryptographyManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgCryptographyManager::~MgCryptographyManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes the object.
///
void MgCryptographyManager::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the class ID.
///
INT32 MgCryptographyManager::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determines if the specified string is decryptable.
///
bool MgCryptographyManager::IsStringDecryptable(CREFSTRING str) const
{
    return m_cryptoUtil.IsStringDecryptable(MgUtil::WideCharToMultiByte(str));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determines if the specified string is encrypted.
///
bool MgCryptographyManager::IsStringEncrypted(CREFSTRING str) const
{
    return m_cryptoUtil.IsStringEncrypted(MgUtil::WideCharToMultiByte(str));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts credentials.
///
STRING MgCryptographyManager::EncryptCredentials(CREFSTRING username,
    CREFSTRING password)
{
    STRING credentials;

    MG_CRYPTOGRAPHY_TRY()

    string mbStr;

    m_cryptoUtil.EncryptCredentials(MgUtil::WideCharToMultiByte(username),
        MgUtil::WideCharToMultiByte(password), mbStr);
    MgUtil::MultiByteToWideChar(mbStr, credentials);

    MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgCryptographyManager.EncryptCredentials");

    return credentials;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts a string.
///
STRING MgCryptographyManager::EncryptString(CREFSTRING plainText)
{
    STRING cipherText;

    MG_CRYPTOGRAPHY_TRY()

    string mbStr;

    m_cryptoUtil.EncryptString(MgUtil::WideCharToMultiByte(plainText), mbStr);
    MgUtil::MultiByteToWideChar(mbStr, cipherText);

    MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgCryptographyManager.EncryptString")

    return cipherText;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts a username.
///
STRING MgCryptographyManager::EncryptUsername(CREFSTRING username)
{
    STRING encryptedUsername;

    MG_CRYPTOGRAPHY_TRY()

    string mbStr;

    m_cryptoUtil.EncryptUsername(MgUtil::WideCharToMultiByte(username), mbStr);
    MgUtil::MultiByteToWideChar(mbStr, encryptedUsername);

    MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgCryptographyManager.EncryptUsername")

    return encryptedUsername;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts a password.
///
STRING MgCryptographyManager::EncryptPassword(CREFSTRING password)
{
    STRING encryptedPassword;

    MG_CRYPTOGRAPHY_TRY()

    string mbStr;

    m_cryptoUtil.EncryptPassword(MgUtil::WideCharToMultiByte(password), mbStr);
    MgUtil::MultiByteToWideChar(mbStr, encryptedPassword);

    MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgCryptographyManager.EncryptPassword")

    return encryptedPassword;
}
