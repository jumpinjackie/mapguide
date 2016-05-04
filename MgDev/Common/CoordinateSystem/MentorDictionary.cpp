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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CriticalSection.h"

#include "CoordSysUtil.h"       //for Convert_Wide_To_UTF8, CsDictionaryOpenMode
#include "MentorDictionary.h"
#include "MentorUtil.h"         //for OpenDictionaryFile()


//This function opens a dictionary file.  Caller provides, as input:
//
//    1.    a function pointer, ValidMagic, for determining whether a
//        given value is a valid magic number for this file format
//    2.    the path to the file (as a wchar_t*)
//    3.    the desired open mode of the file
//    4.    the current mode of the file
//    5.    a file to use for access
//
//The function then provides the following as output:
//
//    1.  path to the file (as a C++ "char *" string, which the
//        caller is responsible for deleting)
//    2.    the resultant open mode of the file
//    3.    the magic number which was read from the file
//    4.    error code.
//
csFILE* MentorDictionary::Open(
    /*[out]*/ INT32& lMagic,
    /*[in]*/ CsDictionaryOpenMode (*ValidMagic)(long),
    /*[in]*/ const wchar_t* kpPath,
    /*[in]*/ CsDictionaryOpenMode nOpenMode)
{
    csFILE *pFile=NULL;

    MG_TRY()

    //Make sure they've specified a valid mode
    bool bWriting;
    switch (nOpenMode)
    {
    case Read:
        bWriting = false;
        break;
    case Write:
        bWriting = true;
        break;
    default:
        throw new MgInvalidArgumentException(L"MentorDictionary.Open", __LINE__, __WFILE__, NULL, L"", NULL);
        break;
    }

    //Validate the existence of the file
    EFileValidity reason;
    if (!ValidateFile(
        kpPath,        //the file to validate
        true,        //must already exist
        false,        //mustn't be a directory,
        bWriting,    //whether we need write access
        &reason))    //reason for failure
    {
        switch (reason)
        {
        case kFileInvalidEmptyString:
            throw new MgInvalidArgumentException(L"MentorDictionary.Open", __LINE__, __WFILE__, NULL, L"", NULL);
            break;
        case kFileInvalidPath:
        case kFileInvalidDoesNotExist:
        case kFileInvalidIsADir:
            {
            MgStringCollection arguments;
            arguments.Add(kpPath);
            throw new MgFileNotFoundException(L"MentorDictionary.Open", __LINE__, __WFILE__, &arguments, L"", NULL);
            }
            break;
        case kFileInvalidCantWrite:
            {
            MgStringCollection arguments;
            arguments.Add(kpPath);
            throw new MgCoordinateSystemLoadFailedException(L"MentorDictionary.Open", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemDictionaryReadOnlyException", NULL);
            }
            break;
        default:
            throw new MgUnclassifiedException(L"MentorDictionary.Open", __LINE__, __WFILE__, NULL, L"", NULL);
            break;
        }
        //return;
    }

    //Try to open the dictionary file
    char szMode[10];
    GetFileModes(nOpenMode, szMode);
    if (NULL==(pFile=OpenDictionaryFile(kpPath, szMode, lMagic, ValidMagic)))
    {
        throw new MgFileIoException(L"MentorDictionary.Open", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryOpenFailedException", NULL);
    }

    //If the Mentor version is anything other than current, we're not allowed
    //to write it.
    if ((Write == nOpenMode) && (Write != ValidMagic(lMagic)))
    {
        //Sorry, old version.  Write access not allowed.
        CS_fclose(pFile);
        pFile=NULL;
        throw new MgInvalidArgumentException(L"MentorDictionary.Open", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryNoWriteToOldException", NULL);
    }

    MG_CATCH_AND_THROW(L"MentorDictionary.Open")
    return pFile;
}

//This function opens a dictionary file.  Caller provides, as input:
//
//    1.    a function pointer, ValidMagic, for determining whether a
//        given value is a valid magic number for this file format
//    2.    the path to the file (as a wchar_t*)
//    3.    the desired open mode of the file
//    4.    the current mode of the file
//    5.    a file to use for access
//
//The function then provides the following as output:
//
//    1.  path to the file (as a C++ "char *" string, which the
//        caller is responsible for deleting)
//    2.    the resultant open mode of the file
//    3.    the magic number which was read from the file
//    4.    error code.
//
void MentorDictionary::SetFileName(
    /*[out]*/ INT32& lMagic,
    /*[in]*/ CsDictionaryOpenMode (*ValidMagic)(long),
    /*[in]*/ CREFSTRING sDirectory,
    /*[in]*/ CREFSTRING sFileName,
    /*[IN/OUT]*/ REFSTRING sFileNameSet,
    /*[in]*/ const wchar_t* kpMethodName)
{
    MG_TRY()

    _ASSERT(!sFileName.empty());

    if (!sFileName.empty())
    {
        if (sDirectory.empty())
        {
            //Directory hasn't been specified yet.
            throw new MgCoordinateSystemInitializationFailedException(kpMethodName, __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
        }

        STRING sPath = sDirectory + sFileName;

        //Validate the existence of the file
        EFileValidity reason;
        if (!ValidateFile(
            sPath.c_str(),  //the file to validate
            true,           //must already exist
            false,          //mustn't be a directory,
            false,          //whether we need write access
            &reason))       //reason for failure
        {
            ThrowFileError(kpMethodName, sPath, reason);
        }

        //Try to open the dictionary file
        if (!GetMagicFromDictionaryFile(sPath.c_str(), lMagic, ValidMagic))
        {
            throw new MgFileIoException(kpMethodName, __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryOpenFailedException", NULL);
        }
    }//if dictionary is specified

    //ok, so, save the file name
    sFileNameSet=sFileName;

    MG_CATCH_AND_THROW(kpMethodName)
}

//Default callback used to force opening an arbitrary dictionary file in reading mode
CsDictionaryOpenMode MentorDictionary::OpenReadDictionaryOpenCallback(long dictMagicNumber)
{
    return Read;
}
