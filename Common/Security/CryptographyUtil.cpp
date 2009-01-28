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

#include "CryptographyUtil.h"
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
/// Constants.
///
static const string MG_CRYPTOGRAPHY_PRIVATE_KEY         = "WutsokeedbA";
static const char MG_CRYPTOGRAPHY_DEC_CHARS[]           = "0123456789";
static const char MG_CRYPTOGRAPHY_HEX_CHARS[]           = "0123456789abcdef";
static const int MG_CRYPTOGRAPHY_MAGIC_NUMBER_1         = 42;
static const int MG_CRYPTOGRAPHY_MAGIC_NUMBER_2         = 3;
static const int MG_CRYPTOGRAPHY_MIN_COLUMN_NUMBER      = 5;

const int MgCryptographyUtil::sm_minCipherTextLength    = 34; // based on delimiter length and minimum key length
const int MgCryptographyUtil::sm_minKeyLength           = 14;
const int MgCryptographyUtil::sm_maxKeyLength           = 32;
const char MgCryptographyUtil::sm_stringDelimiter       = '\v';
const string MgCryptographyUtil::sm_reservedCharactersForStrings = "\v\f";
const string MgCryptographyUtil::sm_reservedCharactersForCredentials = "\t\r\n\v\f";

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgCryptographyUtil::MgCryptographyUtil()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgCryptographyUtil::~MgCryptographyUtil()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Combines the two specified strings into one.
/// The input string must NOT contain the reserved sm_stringDelimiter.
///
void MgCryptographyUtil::CombineStrings(const string& inStr1,
    const string& inStr2, string& outStr)
{
    outStr  = inStr1;
    outStr += sm_stringDelimiter;
    outStr += inStr2;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Splits the specified string to two.
/// The input string must contain the reserved sm_stringDelimiter.
///
void MgCryptographyUtil::SplitStrings(const string& inStr,
    string& outStr1, string& outStr2)
{
    if (inStr.empty())
    {
        throw ecNullArgument;
    }

    string::size_type i = inStr.find(sm_stringDelimiter);

    if (string::npos == i)
    {
        throw ecInvalidArgument;
    }
    else
    {
        outStr1 = inStr.substr(0, i);

        string::size_type j = inStr.find(sm_stringDelimiter, ++i);

        if (string::npos == j)
        {
            outStr2 = inStr.substr(i);
        }
        else
        {
            throw ecInvalidArgument;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Generates an asymmetric key.
/// This key is simply the current coordinated universal time (UTC).
///
void MgCryptographyUtil::GenerateCryptographKey(string& key)
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
    char buf[size];
    ::memset(buf, 0, sizeof(buf));

    if (sm_minKeyLength != ::strftime(buf, size, "%Y%m%d%H%M%S", utcTime))
    {
        throw ecDateTimeError;
    }

    key = buf;
    assert(key.length() == sm_minKeyLength);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Validates an asymmetric key.
///
void MgCryptographyUtil::ValidateCryptographKey(const string& key)
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
    else if (string::npos != key.find_first_not_of(MG_CRYPTOGRAPHY_DEC_CHARS))
    {
        throw ecInvalidArgument;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Converts a binary string to a hexadecimal string.
///
void MgCryptographyUtil::BinToHex(const string& binStr, string& hexStr)
{
    int binStrLen = (int)binStr.length();

    hexStr.clear();
    hexStr.reserve((binStrLen * 2) + 1);

    for (int i = 0; i < binStrLen; ++i)
    {
        int num = binStr[i];

        for (int j = 1; j >= 0; --j)
        {
            hexStr += MG_CRYPTOGRAPHY_HEX_CHARS[(num >> j*4) & 0xF];
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Converts a hexadecimal string to a binary string.
///
void MgCryptographyUtil::HexToBin(const string& hexStr, string& binStr)
{
    int binStrLen = (int)hexStr.length() / 2;

    binStr.clear();
    binStr.reserve(binStrLen + 1);

    const char* buf = hexStr.c_str();

    for (int i = 0; i < binStrLen; ++i)
    {
        int hex;

        ::sscanf(buf+i*2, "%2x", &hex);
        binStr += char(hex);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts a string using transposition.
///
void MgCryptographyUtil::EncryptStringByTransposition(const string& inStr,
    string& outStr, int numOfColumn)
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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Decrypts a string using transposition.
///
void MgCryptographyUtil::DecryptStringByTransposition(const string& inStr,
    string& outStr, int numOfColumn)
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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts a string using transposition.
///
void MgCryptographyUtil::EncryptStringByTransposition(const string& inStr,
    string& outStr)
{
    string tmpStr;
    int inStrLen = (int)inStr.length();

    int numOfColumn = MG_CRYPTOGRAPHY_MIN_COLUMN_NUMBER;
    EncryptStringByTransposition(inStr, tmpStr, numOfColumn);

    numOfColumn += inStrLen % 6;
    EncryptStringByTransposition(tmpStr, outStr, numOfColumn);
    assert(inStr.length() == outStr.length());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Decrypts a string using transposition.
///
void MgCryptographyUtil::DecryptStringByTransposition(const string& inStr,
    string& outStr)
{
    string tmpStr;
    int inStrLen = (int)inStr.length();

    int numOfColumn = MG_CRYPTOGRAPHY_MIN_COLUMN_NUMBER + inStrLen % 6;
    DecryptStringByTransposition(inStr, tmpStr, numOfColumn);

    numOfColumn -= inStrLen % 6;
    DecryptStringByTransposition(tmpStr, outStr, numOfColumn);
    assert(inStr.length() == outStr.length());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts a string using the one-time-pad algorithm.
/// The asymmetric key is normally between 5 - 10 characters.
/// The cipher text MUST be at least twice the size of the plain text.
///
void MgCryptographyUtil::EncryptStringWithKey(const string& inStr,
    string& outStr, const string& key)
{
    char prevChar = MG_CRYPTOGRAPHY_MAGIC_NUMBER_1;
    char currChar;
    int keyIdx = 0;
    int keyLen = (int)key.length();
    int outStrLen = (int)inStr.length();
    string tmpStr;

    tmpStr.reserve(outStrLen + 1);

    for (int i = 0; i < outStrLen; ++i)
    {
        currChar = inStr[i];
        tmpStr += char(currChar ^ key[keyIdx] ^ prevChar ^ ((i / MG_CRYPTOGRAPHY_MAGIC_NUMBER_2) % 255));
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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Decrypts a string using the one-time-pad algorithm.
/// The asymmetric key is normally between 5 - 10 characters.
/// The cipher text MUST be at least twice the size of the plain text.
///
void MgCryptographyUtil::DecryptStringWithKey(const string& inStr,
    string& outStr, const string& key)
{
    char prevChar = MG_CRYPTOGRAPHY_MAGIC_NUMBER_1;
    char currChar;
    int keyIdx = 0;
    int keyLen = (int)key.length();
    int outStrLen = (int)inStr.length() / 2;
    string tmpStr;

    HexToBin(inStr, tmpStr);

    outStr.clear();
    outStr.reserve(outStrLen + 1);

    for (int i = 0; i < outStrLen; ++i)
    {
        currChar = tmpStr[i];
        outStr += char(currChar ^ key[keyIdx] ^ prevChar ^ ((i / MG_CRYPTOGRAPHY_MAGIC_NUMBER_2) % 255));
        prevChar = outStr[i];

        ++keyIdx;

        if (keyIdx >= keyLen)
        {
           keyIdx = 0;
        }
    }

    assert(inStr.length() == (outStr.length() * 2));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts the specified strings using an asymmetric key and transposition.
///
void MgCryptographyUtil::EncryptStrings(const string& plainText1,
    const string& plainText2, string& cipherText, const string& reservedChars)
{
    try
    {
        // Validate arguments.

        if (string::npos != plainText1.find_first_of(reservedChars) ||
            string::npos != plainText2.find_first_of(reservedChars))
        {
            throw ecInvalidArgument;
        }

        // Generate a public key.

        string publicKey;

        GenerateCryptographKey(publicKey);

        // Encrypt the string using a public key.

        string tmpStr1, tmpStr2;

        CombineStrings(plainText1, plainText2, tmpStr1);
        EncryptStringWithKey(tmpStr1, tmpStr2, publicKey);

        // Encrypt the string using a private key.

        CombineStrings(tmpStr2, publicKey, tmpStr1);
        EncryptStringWithKey(tmpStr1, tmpStr2, MG_CRYPTOGRAPHY_PRIVATE_KEY);

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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Decrypts the specified strings using an asymmetric key and transposition.
///
void MgCryptographyUtil::DecryptStrings(const string& cipherText,
    string& plainText1, string& plainText2, bool validateKey)
{
    try
    {
        // Validate arguments.

        if (!IsStringDecryptable(cipherText))
        {
            throw ecInvalidArgument;
        }

        // Decrypt the string using transposition.

        string tmpStr1;

        DecryptStringByTransposition(cipherText, tmpStr1);

        // Decrypt the string using a private key.

        string tmpStr2;

        DecryptStringWithKey(tmpStr1, tmpStr2, MG_CRYPTOGRAPHY_PRIVATE_KEY);

        string publicKey;

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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determines if the specified string is decryptable.
///
/// \param str
/// String to be checked.
///
/// \return
/// Returns true if the specified string is decryptable, false otherwise.
///
bool MgCryptographyUtil::IsStringDecryptable(const string& str) const
{
    bool decryptable = true;

    if (str.empty()
        || str.length() < sm_minCipherTextLength
        || 0 != (str.length() % 2)
        || string::npos != str.find_first_not_of(MG_CRYPTOGRAPHY_HEX_CHARS))
    {
        decryptable = false;
    }

    return decryptable;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determines if the specified string is encrypted.
///
/// \param str
/// String to be checked.
///
/// \return
/// Returns true if the specified string is encrypted, false otherwise.
///
bool MgCryptographyUtil::IsStringEncrypted(const string& str) const
{
    // Perform the validation by checking if the string is decryptable.

    bool encrypted = false;

    try
    {
        string plainText1, plainText2;

        const_cast<MgCryptographyUtil*>(this)->DecryptStrings(str, plainText1, plainText2);

        encrypted = true;
    }
    catch (...)
    {
    }

    return encrypted;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts credentials.
///
/// \param username
/// Username to be encrypted.
/// \param password
/// Password to be encrypted.
/// \param credentials
/// Encrypted credentials.
///
/// EXCEPTIONS:
/// ecNullArgument
/// ecInvalidArgument
/// ecLengthError
/// ecEncryptionError
///
void MgCryptographyUtil::EncryptCredentials(
    const string& username, const string& password, string& credentials)
{
    EncryptStrings(username, password, credentials, sm_reservedCharactersForCredentials);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Decrypts credentials.
///
/// \param credentials
/// Credentials to be decrypted.
/// \param username
/// Decrypted username.
/// \param password
/// Decrypted password.
/// \param validateKey
/// Flag to determine whether or not the cryptography key should be validated.
///
/// EXCEPTIONS:
/// ecNullArgument
/// ecInvalidArgument
/// ecLengthError
/// ecDecryptionError
///
void MgCryptographyUtil::DecryptCredentials(
    const string& credentials, string& username, string& password,
    bool validateKey)
{
    DecryptStrings(credentials, username, password, validateKey);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts a username.
///
/// \param plainText
/// Username to be encrypted.
/// \param cipherText
/// Encrypted username.
///
/// EXCEPTIONS:
/// ecNullArgument
/// ecInvalidArgument
/// ecLengthError
/// ecEncryptionError
///
void MgCryptographyUtil::EncryptUsername(
    const string& plainText, string& cipherText)
{
    string dummyStr;

    EncryptCredentials(plainText, dummyStr, cipherText);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Decrypts a username.
///
/// \param cipherText
/// Username to be decrypted.
/// \param plainText
/// Decrypted username.
/// \param validateKey
/// Flag to determine whether or not the cryptography key should be validated.
///
/// EXCEPTIONS:
/// ecNullArgument
/// ecInvalidArgument
/// ecLengthError
/// ecDecryptionError
///
void MgCryptographyUtil::DecryptUsername(
    const string& cipherText, string& plainText, bool validateKey)
{
    string dummyStr;

    DecryptCredentials(cipherText, plainText, dummyStr, validateKey);

    if (!dummyStr.empty())
    {
        throw ecInvalidOperation;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts a password.
///
/// \param plainText
/// Password to be encrypted.
/// \param cipherText
/// Encrypted password.
///
/// EXCEPTIONS:
/// ecNullArgument
/// ecInvalidArgument
/// ecLengthError
/// ecEncryptionError
///
void MgCryptographyUtil::EncryptPassword(
    const string& plainText, string& cipherText)
{
    string dummyStr;

    EncryptCredentials(dummyStr, plainText, cipherText);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Decrypts a password.
///
/// \param cipherText
/// Password to be decrypted.
/// \param plainText
/// Decrypted password.
/// \param validateKey
/// Flag to determine whether or not the cryptography key should be validated.
///
/// EXCEPTIONS:
/// ecNullArgument
/// ecInvalidArgument
/// ecLengthError
/// ecDecryptionError
///
void MgCryptographyUtil::DecryptPassword(
    const string& cipherText, string& plainText, bool validateKey)
{
    string dummyStr;

    DecryptCredentials(cipherText, dummyStr, plainText, validateKey);

    if (!dummyStr.empty())
    {
        throw ecInvalidOperation;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encrypts a string.
///
/// \param plainText
/// String to be encrypted.
/// \param cipherText
/// Encrypted string.
///
/// EXCEPTIONS:
/// ecNullArgument
/// ecInvalidArgument
/// ecLengthError
/// ecEncryptionError
///
void MgCryptographyUtil::EncryptString(
    const string& plainText, string& cipherText)
{
    string dummyStr;

    EncryptStrings(plainText, dummyStr, cipherText, sm_reservedCharactersForStrings);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Decrypts a string.
///
/// \param cipherText
/// String to be decrypted.
/// \param plainText
/// Decrypted string.
/// \param validateKey
/// Flag to determine whether or not the cryptography key should be validated.
///
/// EXCEPTIONS:
/// ecNullArgument
/// ecInvalidArgument
/// ecLengthError
/// ecDecryptionError
///
void MgCryptographyUtil::DecryptString(
    const string& cipherText, string& plainText, bool validateKey)
{
    string dummyStr;

    DecryptStrings(cipherText, plainText, dummyStr, validateKey);

    if (!dummyStr.empty())
    {
        throw ecInvalidOperation;
    }
}
