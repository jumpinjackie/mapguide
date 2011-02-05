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

#ifndef MGCRYPTOGRAPHYUTIL_H_
#define MGCRYPTOGRAPHYUTIL_H_

#include "SecurityDefs.h"

class MgCryptographyUtil
{
/// Constructors/Destructor

public:

    MgCryptographyUtil();
    virtual ~MgCryptographyUtil();

private:

    // Unimplemented Constructors/Methods

    MgCryptographyUtil(const MgCryptographyUtil&);
    MgCryptographyUtil& operator=(const MgCryptographyUtil&);

/// Methods

public:

    bool IsStringDecryptable(const string& str) const;
    bool IsStringEncrypted(const string& str) const;

    void EncryptCredentials(const string& username,
        const string& password, string& credentials);
    void DecryptCredentials(const string& credentials,
        string& username, string& password, bool validateKey = false);

    void EncryptUsername(const string& plainText, string& cipherText);
    void DecryptUsername(const string& cipherText, string& plainText,
        bool validateKey = false);

    void EncryptPassword(const string& plainText, string& cipherText);
    void DecryptPassword(const string& cipherText, string& plainText,
        bool validateKey = false);

    void EncryptString(const string& plainText, string& cipherText);
    void DecryptString(const string& cipherText, string& plainText,
        bool validateKey = false);

protected:

    void CombineStrings(const string& inStr1, const string& inStr2,
        string& outStr);
    void SplitStrings(const string& inStr, string& outStr1,
        string& outStr2);

    void GenerateCryptographKey(string& key);
    void ValidateCryptographKey(const string& key);

    void BinToHex(const string& binStr, string& hexStr);
    void HexToBin(const string& hexStr, string& binStr);

private:

    void EncryptStringByTransposition(const string& inStr,
        string& outStr, int numOfColumn);
    void DecryptStringByTransposition(const string& inStr,
        string& outStr, int numOfColumn);

    void EncryptStringByTransposition(const string& inStr,
        string& outStr);
    void DecryptStringByTransposition(const string& inStr,
        string& outStr);

    void EncryptStringWithKey(const string& inStr, string& outStr,
        const string& key);
    void DecryptStringWithKey(const string& inStr, string& outStr,
        const string& key);

    void EncryptStrings(const string& plainText1, const string& plainText2,
        string& cipherText, const string& reservedChars = "");
    void DecryptStrings(const string& cipherText,
        string& plainText1, string& plainText2, bool validateKey = false);

/// Data Members

protected:

    static const int sm_minCipherTextLength;
    static const int sm_minKeyLength;
    static const int sm_maxKeyLength;

private:

    static const char sm_stringDelimiter;
    static const string sm_reservedCharactersForStrings;
    static const string sm_reservedCharactersForCredentials;
};

/// Inline Methods

#endif
