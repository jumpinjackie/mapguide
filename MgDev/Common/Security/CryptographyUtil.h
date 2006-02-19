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

    bool IsStringDecryptable(const wstring& str) const;
    bool IsStringEncrypted(const wstring& str) const;

    void EncryptCredentials(const wstring& username,
        const wstring& password, wstring& credentials);
    void DecryptCredentials(const wstring& credentials,
        wstring& username, wstring& password, bool validateKey = false);

    void EncryptUsername(const wstring& plainText, wstring& cipherText);
    void DecryptUsername(const wstring& cipherText, wstring& plainText,
        bool validateKey = false);

    void EncryptPassword(const wstring& plainText, wstring& cipherText);
    void DecryptPassword(const wstring& cipherText, wstring& plainText,
        bool validateKey = false);

    void EncryptString(const wstring& plainText, wstring& cipherText);
    void DecryptString(const wstring& cipherText, wstring& plainText,
        bool validateKey = false);

protected:

    void CombineStrings(const wstring& inStr1, const wstring& inStr2,
        wstring& outStr);
    void SplitStrings(const wstring& inStr, wstring& outStr1,
        wstring& outStr2);

    void GenerateCryptographKey(wstring& key);
    void ValidateCryptographKey(const wstring& key);

    void BinToHex(const wstring& binStr, wstring& hexStr);
    void HexToBin(const wstring& hexStr, wstring& binStr);

private:

    void EncryptStringByTransposition(const wstring& inStr,
        wstring& outStr, int numOfColumn);
    void DecryptStringByTransposition(const wstring& inStr,
        wstring& outStr, int numOfColumn);

    void EncryptStringByTransposition(const wstring& inStr,
        wstring& outStr);
    void DecryptStringByTransposition(const wstring& inStr,
        wstring& outStr);

    void EncryptStringWithKey(const wstring& inStr, wstring& outStr,
        const wstring& key);
    void DecryptStringWithKey(const wstring& inStr, wstring& outStr,
        const wstring& key);

    void EncryptStrings(const wstring& plainText1, const wstring& plainText2,
        wstring& cipherText, const wstring& reservedChars = L"");
    void DecryptStrings(const wstring& cipherText,
        wstring& plainText1, wstring& plainText2, bool validateKey = false);

/// Data Members

protected:

    static const int sm_minCipherTextLength;
    static const int sm_minKeyLength;
    static const int sm_maxKeyLength;

private:

    static const wchar_t sm_stringDelimiter;
    static const wstring sm_reservedCharactersForStrings;
    static const wstring sm_reservedCharactersForCredentials;
};

/// Inline Methods

#endif
