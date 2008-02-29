//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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


#ifndef MGCRYPTOGRAPHYMANAGER_H_
#define MGCRYPTOGRAPHYMANAGER_H_

#include "../Security/SecurityDefs.h"
#include "../Security/CryptographyUtil.h"

///////////////////////////////////////////////////////////////////////////////
/// Helper macros.
///
#define MG_CRYPTOGRAPHY_TRY()                                                 \
    Ptr<MgException> cryptographyException;                                   \
                                                                              \
    try                                                                       \
    {                                                                         \

#define MG_CRYPTOGRAPHY_CATCH(methodName)                                     \
    }                                                                         \
    catch (MgErrorCode errorCode)                                             \
    {                                                                         \
        switch (errorCode)                                                    \
        {                                                                     \
            case ecOk:                                                        \
                break;                                                        \
                                                                              \
            case ecInvalidOperation:                                          \
                cryptographyException = new MgInvalidOperationException(      \
                    methodName, __LINE__, __WFILE__, NULL, L"", NULL);        \
                break;                                                        \
                                                                              \
            case ecNullArgument:                                              \
                cryptographyException = new MgNullArgumentException(          \
                    methodName, __LINE__, __WFILE__, NULL, L"", NULL);        \
                break;                                                        \
                                                                              \
            case ecInvalidArgument:                                           \
                cryptographyException = new MgInvalidArgumentException(       \
                    methodName, __LINE__, __WFILE__, NULL, L"", NULL);        \
                break;                                                        \
                                                                              \
            case ecLengthError:                                               \
                cryptographyException = new MgLengthException(                \
                    methodName, __LINE__, __WFILE__, NULL, L"", NULL);        \
                break;                                                        \
                                                                              \
            case ecDateTimeError:                                             \
                cryptographyException = new MgDateTimeException(              \
                    methodName, __LINE__, __WFILE__, NULL, L"", NULL);        \
                break;                                                        \
                                                                              \
            case ecFileIoError:                                               \
            case ecReadError:                                                 \
            case ecWriteError:                                                \
                cryptographyException = new MgFileIoException(                \
                    methodName, __LINE__, __WFILE__, NULL, L"", NULL);        \
                break;                                                        \
                                                                              \
            case ecEncryptionError:                                           \
                cryptographyException = new MgEncryptionException(            \
                    methodName, __LINE__, __WFILE__, NULL, L"", NULL);        \
                break;                                                        \
                                                                              \
            case ecDecryptionError:                                           \
                cryptographyException = new MgDecryptionException(            \
                    methodName, __LINE__, __WFILE__, NULL, L"", NULL);        \
                break;                                                        \
                                                                              \
            case ecUnclassified:                                              \
            default:                                                          \
                cryptographyException = new MgUnclassifiedException(          \
                    methodName, __LINE__, __WFILE__, NULL, L"", NULL);        \
                break;                                                        \
        }                                                                     \
    }                                                                         \
    catch (MgException* e)                                                    \
    {                                                                         \
        cryptographyException = e;                                            \
        cryptographyException->AddStackTraceInfo(                             \
            methodName, __LINE__, __WFILE__);                                 \
    }                                                                         \
    catch (exception& e)                                                      \
    {                                                                         \
        cryptographyException = MgSystemException::Create(                    \
            e, methodName, __LINE__, __WFILE__);                              \
    }                                                                         \
    catch (...)                                                               \
    {                                                                         \
        cryptographyException = new MgUnclassifiedException(                  \
            methodName, __LINE__, __WFILE__, NULL, L"", NULL);                \
    }                                                                         \

#define MG_CRYPTOGRAPHY_THROW()                                               \
    if (cryptographyException != NULL)                                        \
    {                                                                         \
        (*cryptographyException).AddRef();                                    \
        cryptographyException->Raise();                                       \
    }                                                                         \

#define MG_CRYPTOGRAPHY_CATCH_AND_THROW(methodName)                           \
    MG_CRYPTOGRAPHY_CATCH(methodName)                                         \
                                                                              \
    MG_CRYPTOGRAPHY_THROW()                                                   \

#define MG_CRYPTOGRAPHY_CATCH_AND_RELEASE()                                   \
    }                                                                         \
    catch (MgException* e)                                                    \
    {                                                                         \
        cryptographyException = e;                                            \
    }                                                                         \
    catch (...)                                                               \
    {                                                                         \
    }                                                                         \

/// \defgroup MgCryptographyManager MgCryptographyManager
/// \ingroup Common_Module
/// \{
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Encryption methods.
///
class MG_MAPGUIDE_API MgCryptographyManager : public MgDisposable
{
    DECLARE_CLASSNAME(MgCryptographyManager)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs the object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgCryptographyManager();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgCryptographyManager();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgCryptographyManager();
    /// \htmlinclude SyntaxBottom.html
    MgCryptographyManager();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Encrypts credentials.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string EncryptCredentials(string username, string password);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String EncryptCredentials(String username, String password);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string EncryptCredentials(string username, string password);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param username (String/string)
    /// Username to be encrypted.
    /// \param password (String/string)
    /// Password to be encrypted.
    ///
    /// \return
    /// Returns the encrypted credentials.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    /// \exception MgLengthException
    /// \exception MgEncryptionException
    ///
    STRING EncryptCredentials(CREFSTRING username, CREFSTRING password);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Encrypts a string.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string EncryptString(string plainText);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String EncryptString(String plainText);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string EncryptString(string plainText);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param plainText (String/string)
    /// String to be encrypted.
    ///
    /// \return
    /// Returns the encrypted string.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    /// \exception MgLengthException
    /// \exception MgEncryptionException
    ///
    STRING EncryptString(CREFSTRING plainText);

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the object.
    ///
    virtual ~MgCryptographyManager();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Encrypts a username.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string EncryptUsername(string username);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String EncryptUsername(String username);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string EncryptUsername(string username);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param username (String/string)
    /// Username to be encrypted.
    ///
    /// \return
    /// Returns the encrypted username.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    /// \exception MgLengthException
    /// \exception MgEncryptionException
    ///
    ///

    STRING EncryptUsername(CREFSTRING username);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Encrypts a password.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string EncryptPassword(string password);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String EncryptPassword(String password);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string EncryptPassword(string password);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param password (String/string)
    /// Password to be encrypted.
    ///
    /// \return
    /// Returns the encrypted password.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    /// \exception MgLengthException
    /// \exception MgEncryptionException
    ///
    ///

    STRING EncryptPassword(CREFSTRING password);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the specified string is decryptable.
    ///
    /// \param str
    /// String to be checked.
    ///
    /// \return
    /// Returns true if the specified string is decryptable, false otherwise.
    ///
    ///

    bool IsStringDecryptable(CREFSTRING str) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the specified string is encrypted.
    ///
    /// \param str
    /// String to be checked.
    ///
    /// \return
    /// Returns true if the specified string is encrypted, false otherwise.
    ///
    ///

    bool IsStringEncrypted(CREFSTRING str) const;

protected:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes the object.
    ///
    /// \return
    /// Nothing.
    ///
    ///

    virtual void Dispose();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the class ID.
    ///
    /// \return
    /// Class ID.
    ///
    ///

    INT32 GetClassId();

private:

    /// Unimplemented Constructors/Methods

    MgCryptographyManager(const MgCryptographyManager&);
    MgCryptographyManager& operator=(const MgCryptographyManager&);

/// Data Members

CLASS_ID:

    static const INT32 m_cls_id = Foundation_System_CryptographyManager;

private:

    MgCryptographyUtil m_cryptoUtil;
};
/// \}

#endif
