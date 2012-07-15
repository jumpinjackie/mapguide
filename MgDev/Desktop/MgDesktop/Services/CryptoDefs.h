#ifndef DESKTOP_CRYPTO_DEFS_H
#define DESKTOP_CRYPTO_DEFS_H

#include "SecurityDefs.h"
#include "CryptographyUtil.h"

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

#endif