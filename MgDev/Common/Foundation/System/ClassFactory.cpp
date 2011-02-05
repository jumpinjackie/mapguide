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

#include "Foundation.h"

MgClassFactory::MgClassFactory()
{
}

MgClassFactory* MgClassFactory::GetInstance()
{
    static Ptr<MgClassFactory> classFactory;
    MG_TRY()

    ACE_TRACE ("MgClassFactory::GetInstance");

    if (classFactory == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (classFactory == NULL)
        {
            classFactory = new MgClassFactory();
        }
    }

    MG_RESOURCES_CATCH_AND_THROW(L"MgClassFactory.GetInstance")

    // To avoid overhead and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return classFactory;
}


void MgClassFactory::Dispose()
{
    delete this;
}


MgObject* MgClassFactory::CreateMgObject(int classId)
{
    ClassCreatorFunc func = m_classCreators[classId];
    if (NULL == func)
    {
        throw new MgClassNotFoundException(L"MgClassFactory.CreateMgObject", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgObject* obj = (*func)();
    return obj;
}


void MgClassFactory::Register(int classId, ClassCreatorFunc creator)
{
    if (0 >= classId || NULL == creator)
    {
        throw new MgInvalidArgumentException(L"MgClassFactory.RegisterClass", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_classCreators[classId] = creator;
}



static bool InitializeStaticData();

static bool initStatic = InitializeStaticData();

#define EXCEPTION_CLASS_CREATOR(className) \
    fact->Register(Foundation_Exception_##className, className::CreateObject);

bool InitializeStaticData()
{
    MgClassFactory* fact = MgClassFactory::GetInstance();

    //put in the map any class that can be serialized
    fact->Register(Foundation_Data_ByteReader, MgByteReader::CreateObject);

    fact->Register(Foundation_Data_DateTime, MgDateTime::CreateObject);
    fact->Register(Foundation_Collection_PropertyCollection,  MgPropertyCollection::CreateObject);
    fact->Register(Foundation_Collection_StringCollection, MgStringCollection::CreateObject);
    fact->Register(Foundation_Collection_BatchPropertyCollection, MgBatchPropertyCollection::CreateObject);
    fact->Register(Foundation_Property_StringProperty,MgStringProperty::CreateObject);
    fact->Register(Foundation_Property_PropertyDefinition,MgPropertyDefinition::CreateObject);

    EXCEPTION_CLASS_CREATOR(MgArgumentOutOfRangeException)
    EXCEPTION_CLASS_CREATOR(MgClassNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgConfigurationException)
    EXCEPTION_CLASS_CREATOR(MgConfigurationLoadFailedException)
    EXCEPTION_CLASS_CREATOR(MgConfigurationSaveFailedException)
    EXCEPTION_CLASS_CREATOR(MgDateTimeException)
    EXCEPTION_CLASS_CREATOR(MgDecryptionException)
    EXCEPTION_CLASS_CREATOR(MgDirectoryNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgDivideByZeroException)
    EXCEPTION_CLASS_CREATOR(MgDomainException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateDirectoryException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateFileException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateObjectException)
    EXCEPTION_CLASS_CREATOR(MgEncryptionException)
    EXCEPTION_CLASS_CREATOR(MgFileIoException)
    EXCEPTION_CLASS_CREATOR(MgFileNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgIndexOutOfRangeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidArgumentException)
    EXCEPTION_CLASS_CREATOR(MgInvalidCastException)
    EXCEPTION_CLASS_CREATOR(MgInvalidMimeTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidOperationException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPropertyTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidStreamHeaderException)
    EXCEPTION_CLASS_CREATOR(MgIoException)
    EXCEPTION_CLASS_CREATOR(MgLengthException)
    EXCEPTION_CLASS_CREATOR(MgLogicException)
    EXCEPTION_CLASS_CREATOR(MgNotFiniteNumberException)
    EXCEPTION_CLASS_CREATOR(MgNotImplementedException)
    EXCEPTION_CLASS_CREATOR(MgNullArgumentException)
    EXCEPTION_CLASS_CREATOR(MgNullPropertyValueException)
    EXCEPTION_CLASS_CREATOR(MgNullReferenceException)
    EXCEPTION_CLASS_CREATOR(MgObjectNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgOutOfMemoryException)
    EXCEPTION_CLASS_CREATOR(MgOutOfRangeException)
    EXCEPTION_CLASS_CREATOR(MgOverflowException)
    EXCEPTION_CLASS_CREATOR(MgPlatformNotSupportedException)
    EXCEPTION_CLASS_CREATOR(MgResourcesException)
    EXCEPTION_CLASS_CREATOR(MgResourcesLoadFailedException)
    EXCEPTION_CLASS_CREATOR(MgResourceTagNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgRuntimeException)
    EXCEPTION_CLASS_CREATOR(MgStreamIoException)
    EXCEPTION_CLASS_CREATOR(MgTemporaryFileNotAvailableException)
    EXCEPTION_CLASS_CREATOR(MgUnclassifiedException)
    EXCEPTION_CLASS_CREATOR(MgUnderflowException)
    EXCEPTION_CLASS_CREATOR(MgXmlException)
    EXCEPTION_CLASS_CREATOR(MgXmlParserException)

    return true;
}

