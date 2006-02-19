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

#include "CryptographyUtil.h"

///////////////////////////////////////////////////////////////////////////////
/// Constants.
///
static const wstring MG_PRIVATE_KEY                     = L"WutsokeedbA";
static const wchar_t MG_CRYPTOMAN_DEC_CHARS[]           = L"0123456789";
static const wchar_t MG_CRYPTOMAN_HEX_CHARS[]           = L"0123456789abcdef";
static const int MG_CRYPTOMAN_MAGIC_NUMBER_1            = 42;
static const int MG_CRYPTOMAN_MAGIC_NUMBER_2            = 3;
static const int MG_CRYPTOMAN_MIN_COLUMN_NUMBER         = 5;

const int MgCryptographyUtil::sm_minCipherTextLength    = 34; // based on delimiter length and minimum key length
const int MgCryptographyUtil::sm_minKeyLength           = 14;
const int MgCryptographyUtil::sm_maxKeyLength           = 32;
const wchar_t MgCryptographyUtil::sm_stringDelimiter    = L'\v';
const wstring MgCryptographyUtil::sm_reservedCharactersForStrings = L"\v\f";
const wstring MgCryptographyUtil::sm_reservedCharactersForCredentials = L"\t\r\n\v\f";

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgCryptographyUtil::MgCryptographyUtil()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgCryptographyUtil::~MgCryptographyUtil()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Combines the two specified strings into one.
/// The input string must NOT contain the reserved sm_stringDelimiter.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::CombineStrings(const wstring& inStr1,
    const wstring& inStr2, wstring& outStr)
{
    outStr  = inStr1;
    outStr += sm_stringDelimiter;
    outStr += inStr2;
}

///----------------------------------------------------------------------------
/// <summary>
/// Splits the specified string to two.
/// The input string must contain the reserved sm_stringDelimiter.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::SplitStrings(const wstring& inStr,
    wstring& outStr1, wstring& outStr2)
{
    if (inStr.empty())
    {
        throw ecNullArgument;
    }

    wstring::size_type i = inStr.find(sm_stringDelimiter);

    if (wstring::npos == i)
    {
        throw ecInvalidArgument;
    }
    else
    {
        outStr1 = inStr.substr(0, i);

        wstring::size_type j = inStr.find(sm_stringDelimiter, ++i);

        if (wstring::npos == j)
        {
            outStr2 = inStr.substr(i);
        }
        else
        {
            throw ecInvalidArgument;
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Generates an asymmetric key.
/// This key is simply the current coordinated universal time (UTC).
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::GenerateCryptographKey(wstring& key)
{
    // Get the current local time.

    time_t now;
    ::time(&now);

    // Convert the local time to the coordinated universal time (UTC).

    struct tm* utcTime = ::gmtime(&now);

    if (NULL == utcTime)
    {
        throw ecDateTimeError;
    }


    const size_t size = sm_maxKeyLength + 1;
    wchar_t buf[size];
    ::memset(buf, 0, sizeof(buf));

    if (sm_minKeyLength != ::wcsftime(buf, size, L"%Y%m%d%H%M%S", utcTime))
    {
        throw ecDateTimeError;
    }

    key = buf;
    assert(key.length() == sm_minKeyLength);
}

///----------------------------------------------------------------------------
/// <summary>
/// Validates an asymmetric key.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::ValidateCryptographKey(const wstring& key)
{
    // TODO: We could do more security checks here.

    size_t keyLen = key.length();

    if (0 == keyLen)
    {
        throw ecNullArgument;
    }
    else if (keyLen < sm_minKeyLength || keyLen > sm_maxKeyLength)
    {
        throw ecLengthError;
    }
    else if (wstring::npos != key.find_first_not_of(MG_CRYPTOMAN_DEC_CHARS))
    {
        throw ecInvalidArgument;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts a binary string to a hexadecimal string.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::BinToHex(const wstring& binStr, wstring& hexStr)
{
    int binStrLen = (int)binStr.length();

    hexStr.clear();
    hexStr.reserve((binStrLen * 2) + 1);

    for (int i = 0; i < binStrLen; ++i)
    {
        int num = binStr[i];

        for (int j = 1; j >= 0; --j)
        {
            hexStr += MG_CRYPTOMAN_HEX_CHARS[(num >> j*4) & 0xF];
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts a hexadecimal string to a binary string.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::HexToBin(const wstring& hexStr, wstring& binStr)
{
    int binStrLen = (int)hexStr.length() / 2;

    binStr.clear();
    binStr.reserve(binStrLen + 1);

    const wchar_t* buf = hexStr.c_str();

    for (int i = 0; i < binStrLen; ++i)
    {
        int hex;

        ::swscanf(buf+i*2, L"%2x", &hex);
        binStr += wchar_t(hex);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts a string using transposition.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::EncryptStringByTransposition(const wstring& inStr,
    wstring& outStr, int numOfColumn)
{
    int inStrLen = (int)inStr.length();
    int numOfRow = (int)::ceil((double)inStrLen / (double)numOfColumn);

    outStr.clear();
    outStr.reserve(inStrLen + 1);

    for (int currColumn = 0; currColumn < numOfColumn; ++currColumn)
    {
        for (int currRow = 0; currRow < numOfRow; ++currRow)
        {
            int inIdx = currColumn + currRow * numOfColumn;

            if (inIdx < inStrLen)
            {
                outStr += inStr[inIdx];
            }
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Decrypts a string using transposition.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::DecryptStringByTransposition(const wstring& inStr,
    wstring& outStr, int numOfColumn)
{
    int inStrLen = (int)inStr.length();
    int numOfRow = (int)::ceil((double)inStrLen / (double)numOfColumn);
    int numOfColumnInLastRow = inStrLen - numOfColumn * (numOfRow - 1);
    int outIdx = 0;

    outStr.clear();
    outStr.reserve(inStrLen + 1);

    for (int currRow = 0; currRow < numOfRow; ++currRow)
    {
        int inIdx = currRow;

        for (int currColumn = 0; currColumn < numOfColumn; ++currColumn)
        {
            if (outIdx >= inStrLen)
            {
                break;
            }

            outStr += inStr[inIdx];
            ++outIdx;

            if (currColumn < numOfColumnInLastRow)
            {
                inIdx += numOfRow;
            }
            else
            {
                inIdx += numOfRow - 1;
            }
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts a string using transposition.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::EncryptStringByTransposition(const wstring& inStr,
    wstring& outStr)
{
    wstring tmpStr;
    int inStrLen = (int)inStr.length();

    int numOfColumn = MG_CRYPTOMAN_MIN_COLUMN_NUMBER;
    EncryptStringByTransposition(inStr, tmpStr, numOfColumn);

    numOfColumn += inStrLen % 6;
    EncryptStringByTransposition(tmpStr, outStr, numOfColumn);
    assert(inStr.length() == outStr.length());
}

///----------------------------------------------------------------------------
/// <summary>
/// Decrypts a string using transposition.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::DecryptStringByTransposition(const wstring& inStr,
    wstring& outStr)
{
    wstring tmpStr;
    int inStrLen = (int)inStr.length();

    int numOfColumn = MG_CRYPTOMAN_MIN_COLUMN_NUMBER + inStrLen % 6;
    DecryptStringByTransposition(inStr, tmpStr, numOfColumn);

    numOfColumn -= inStrLen % 6;
    DecryptStringByTransposition(tmpStr, outStr, numOfColumn);
    assert(inStr.length() == outStr.length());
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts a string using the one-time-pad algorithm.
///
/// The asymmetric key is normally between 5 - 10 characters.
///
/// The cipher text MUST be at least twice the size of the plain text.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::EncryptStringWithKey(const wstring& inStr,
    wstring& outStr, const wstring& key)
{
    wchar_t prevChar = MG_CRYPTOMAN_MAGIC_NUMBER_1;
    wchar_t currChar;
    int keyIdx = 0;
    int keyLen = (int)key.length();
    int outStrLen = (int)inStr.length();
    wstring tmpStr;

    tmpStr.reserve(outStrLen + 1);

    for (int i = 0; i < outStrLen; ++i)
    {
        currChar = inStr[i];
        tmpStr += wchar_t(currChar ^ key[keyIdx] ^ prevChar ^ ((i / MG_CRYPTOMAN_MAGIC_NUMBER_2) % 255));
        prevChar = currChar;

        ++keyIdx;

        if (keyIdx >= keyLen)
        {
           keyIdx = 0;
        }
    }

    BinToHex(tmpStr, outStr);
    assert((inStr.length() * 2) == outStr.length());
}

///----------------------------------------------------------------------------
/// <summary>
/// Decrypts a string using the one-time-pad algorithm.
///
/// The asymmetric key is normally between 5 - 10 characters.
///
/// The cipher text MUST be at least twice the size of the plain text.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::DecryptStringWithKey(const wstring& inStr,
    wstring& outStr, const wstring& key)
{
    wchar_t prevChar = MG_CRYPTOMAN_MAGIC_NUMBER_1;
    wchar_t currChar;
    int keyIdx = 0;
    int keyLen = (int)key.length();
    int outStrLen = (int)inStr.length() / 2;
    wstring tmpStr;

    HexToBin(inStr, tmpStr);

    outStr.clear();
    outStr.reserve(outStrLen + 1);

    for (int i = 0; i < outStrLen; ++i)
    {
        currChar = tmpStr[i];
        outStr += wchar_t(currChar ^ key[keyIdx] ^ prevChar ^ ((i / MG_CRYPTOMAN_MAGIC_NUMBER_2) % 255));
        prevChar = outStr[i];

        ++keyIdx;

        if (keyIdx >= keyLen)
        {
           keyIdx = 0;
        }
    }

    assert(inStr.length() == (outStr.length() * 2));
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts the specified strings using an asymmetric key and transposition.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::EncryptStrings(const wstring& plainText1,
    const wstring& plainText2, wstring& cipherText, const wstring& reservedChars)
{
    try
    {
        // Validate arguments.

        if (wstring::npos != plainText1.find_first_of(reservedChars) ||
            wstring::npos != plainText2.find_first_of(reservedChars))
        {
            throw ecInvalidArgument;
        }

        // Generate a public key.

        wstring publicKey;

        GenerateCryptographKey(publicKey);

        // Encrypt the string using a public key.

        wstring tmpStr1, tmpStr2;

        CombineStrings(plainText1, plainText2, tmpStr1);
        EncryptStringWithKey(tmpStr1, tmpStr2, publicKey);

        // Encrypt the string using a private key.

        CombineStrings(tmpStr2, publicKey, tmpStr1);
        EncryptStringWithKey(tmpStr1, tmpStr2, MG_PRIVATE_KEY);

        // Encrypt the string using transposition.

        EncryptStringByTransposition(tmpStr2, cipherText);
    }
    catch (MgErrorCode e)
    {
        throw e;
    }
    catch (...)
    {
        throw ecEncryptionError;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Decrypts the specified strings using an asymmetric key and transposition.
/// </summary>
///----------------------------------------------------------------------------

void MgCryptographyUtil::DecryptStrings(const wstring& cipherText,
    wstring& plainText1, wstring& plainText2, bool validateKey)
{
    try
    {
        // Validate arguments.

        if (!IsStringDecryptable(cipherText))
        {
            throw ecInvalidArgument;
        }

        // Decrypt the string using transposition.

        wstring tmpStr1;

        DecryptStringByTransposition(cipherText, tmpStr1);

        // Decrypt the string using a private key.

        wstring tmpStr2;

        DecryptStringWithKey(tmpStr1, tmpStr2, MG_PRIVATE_KEY);

        wstring publicKey;

        SplitStrings(tmpStr2, tmpStr1, publicKey);

        if (validateKey)
        {
            ValidateCryptographKey(publicKey);
        }

        // Decrypt the string using a public key.

        DecryptStringWithKey(tmpStr1, tmpStr2, publicKey);

        SplitStrings(tmpStr2, plainText1, plainText2);
    }
    catch (MgErrorCode e)
    {
        throw e;
    }
    catch (...)
    {
        throw ecDecryptionError;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines if the specified string is decryptable.
/// </summary>
///
/// <param name="str">
/// String to be checked.
/// </param>
///
/// <returns>
/// \Returns true if the specified string is decryptable, false otherwise.
/// </returns>
///----------------------------------------------------------------------------

bool MgCryptographyUtil::IsStringDecryptable(const wstring& str) const
{
    bool decryptable = true;

    if (str.empty()
        || str.length() < sm_minCipherTextLength
        || 0 != (str.length() % 2)
        || wstring::npos != str.find_first_not_of(MG_CRYPTOMAN_HEX_CHARS))
    {
        decryptable = false;
    }

    return decryptable;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines if the specified string is encrypted.
/// </summary>
///
/// <param name="str">
/// String to be checked.
/// </param>
///
/// <returns>
/// \Returns true if the specified string is encrypted, false otherwise.
/// </returns>
///----------------------------------------------------------------------------

bool MgCryptographyUtil::IsStringEncrypted(const wstring& str) const
{
    // Perform the validation by checking if the string is decryptable.

    bool encrypted = false;

    try
    {
        wstring plainText1, plainText2;

        const_cast<MgCryptographyUtil*>(this)->DecryptStrings(str, plainText1, plainText2);

        encrypted = true;
    }
    catch (...)
    {
    }

    return encrypted;
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts credentials.
/// </summary>
///
/// <param name="username">
/// Username to be encrypted.
/// </param>
/// <param name="password">
/// Password to be encrypted.
/// </param>
/// <param name="credentials">
/// Encrypted credentials.
/// </param>
///
/// EXCEPTIONS:
/// NullArgument
/// InvalidArgument
/// LengthError
/// EncryptionError
///----------------------------------------------------------------------------

void MgCryptographyUtil::EncryptCredentials(
    const wstring& username, const wstring& password, wstring& credentials)
{
    EncryptStrings(username, password, credentials, sm_reservedCharactersForCredentials);
}

///----------------------------------------------------------------------------
/// <summary>
/// Decrypts credentials.
/// </summary>
///
/// <param name="credentials">
/// Credentials to be decrypted.
/// </param>
/// <param name="username">
/// Decrypted username.
/// </param>
/// <param name="password">
/// Decrypted password.
/// </param>
/// <param name="validateKey">
/// Flag to determine whether or not the cryptography key should be validated.
/// </param>
///
/// EXCEPTIONS:
/// NullArgument
/// InvalidArgument
/// LengthError
/// DecryptionError
///----------------------------------------------------------------------------

void MgCryptographyUtil::DecryptCredentials(
    const wstring& credentials, wstring& username, wstring& password,
    bool validateKey)
{
    DecryptStrings(credentials, username, password, validateKey);
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts a username.
/// </summary>
///
/// <param name="plainText">
/// Username to be encrypted.
/// </param>
/// <param name="cipherText">
/// Encrypted username.
/// </param>
///
/// EXCEPTIONS:
/// NullArgument
/// InvalidArgument
/// LengthError
/// EncryptionError
///----------------------------------------------------------------------------

void MgCryptographyUtil::EncryptUsername(
    const wstring& plainText, wstring& cipherText)
{
    wstring dummyStr;

    EncryptCredentials(plainText, dummyStr, cipherText);
}

///----------------------------------------------------------------------------
/// <summary>
/// Decrypts a username.
/// </summary>
///
/// <param name="cipherText">
/// Username to be decrypted.
/// </param>
/// <param name="plainText">
/// Decrypted username.
/// </param>
/// <param name="validateKey">
/// Flag to determine whether or not the cryptography key should be validated.
/// </param>
///
/// EXCEPTIONS:
/// NullArgument
/// InvalidArgument
/// LengthError
/// DecryptionError
///----------------------------------------------------------------------------

void MgCryptographyUtil::DecryptUsername(
    const wstring& cipherText, wstring& plainText, bool validateKey)
{
    wstring dummyStr;

    DecryptCredentials(cipherText, plainText, dummyStr, validateKey);

    if (!dummyStr.empty())
    {
        throw ecInvalidOperation;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts a password.
/// </summary>
///
/// <param name="plainText">
/// Password to be encrypted.
/// </param>
/// <param name="cipherText">
/// Encrypted password.
/// </param>
///
/// EXCEPTIONS:
/// NullArgument
/// InvalidArgument
/// LengthError
/// EncryptionError
///----------------------------------------------------------------------------

void MgCryptographyUtil::EncryptPassword(
    const wstring& plainText, wstring& cipherText)
{
    wstring dummyStr;

    EncryptCredentials(dummyStr, plainText, cipherText);
}

///----------------------------------------------------------------------------
/// <summary>
/// Decrypts a password.
/// </summary>
///
/// <param name="cipherText">
/// Password to be decrypted.
/// </param>
/// <param name="plainText">
/// Decrypted password.
/// </param>
/// <param name="validateKey">
/// Flag to determine whether or not the cryptography key should be validated.
/// </param>
///
/// EXCEPTIONS:
/// NullArgument
/// InvalidArgument
/// LengthError
/// DecryptionError
///----------------------------------------------------------------------------

void MgCryptographyUtil::DecryptPassword(
    const wstring& cipherText, wstring& plainText, bool validateKey)
{
    wstring dummyStr;

    DecryptCredentials(cipherText, dummyStr, plainText, validateKey);

    if (!dummyStr.empty())
    {
        throw ecInvalidOperation;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Encrypts a string.
/// </summary>
///
/// <param name="plainText">
/// String to be encrypted.
/// </param>
/// <param name="cipherText">
/// Encrypted string.
/// </param>
///
/// EXCEPTIONS:
/// NullArgument
/// InvalidArgument
/// LengthError
/// EncryptionError
///----------------------------------------------------------------------------

void MgCryptographyUtil::EncryptString(
    const wstring& plainText, wstring& cipherText)
{
    wstring dummyStr;

    EncryptStrings(plainText, dummyStr, cipherText, sm_reservedCharactersForStrings);
}

///----------------------------------------------------------------------------
/// <summary>
/// Decrypts a string.
/// </summary>
///
/// <param name="cipherText">
/// String to be decrypted.
/// </param>
/// <param name="plainText">
/// Decrypted string.
/// </param>
/// <param name="validateKey">
/// Flag to determine whether or not the cryptography key should be validated.
/// </param>
///
/// EXCEPTIONS:
/// NullArgument
/// InvalidArgument
/// LengthError
/// DecryptionError
///----------------------------------------------------------------------------

void MgCryptographyUtil::DecryptString(
    const wstring& cipherText, wstring& plainText, bool validateKey)
{
    wstring dummyStr;

    DecryptStrings(cipherText, plainText, dummyStr, validateKey);

    if (!dummyStr.empty())
    {
        throw ecInvalidOperation;
    }
}
