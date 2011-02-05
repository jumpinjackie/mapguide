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

// Predefined object identifiers ranges for each class of object.  By convention,
// each library has a range of 10 thousand ids.  Ids are broken into groups.
#define FOUNDATION_COLLECTION_ID          1000
#define FOUNDATION_DATA_ID                1250
#define FOUNDATION_EXCEPTION_ID           1500
#define FOUNDATION_PROPERTY_ID            2000
#define FOUNDATION_SYSTEM_ID              2250

// Foundation Collection
#define Foundation_Collection_Collection                     FOUNDATION_COLLECTION_ID+0
#define Foundation_Collection_NamedCollection                FOUNDATION_COLLECTION_ID+1
#define Foundation_Collection_PropertyCollection             FOUNDATION_COLLECTION_ID+2
#define Foundation_Collection_StringCollection               FOUNDATION_COLLECTION_ID+3
#define Foundation_Collection_StringPropertyCollection       FOUNDATION_COLLECTION_ID+4
#define Foundation_Collection_BatchPropertyCollection        FOUNDATION_COLLECTION_ID+5

// Foundation Data
#define Foundation_Data_ByteReader                        FOUNDATION_DATA_ID+0
#define Foundation_Data_ByteSource                        FOUNDATION_DATA_ID+1
#define Foundation_Data_ByteSourceFileImpl                FOUNDATION_DATA_ID+2
#define Foundation_Data_ByteSourceImpl                    FOUNDATION_DATA_ID+3
#define Foundation_Data_ByteSourceMemoryImpl              FOUNDATION_DATA_ID+4
#define Foundation_Data_ByteSourceSocketStreamImpl        FOUNDATION_DATA_ID+5
#define Foundation_Data_DateTime                          FOUNDATION_DATA_ID+6
#define Foundation_Data_ByteSink                          FOUNDATION_DATA_ID+7
#define Foundation_Data_Stream                            FOUNDATION_DATA_ID+8

// Foundation Exception
#define Foundation_Exception_MgApplicationException                             FOUNDATION_EXCEPTION_ID+0
#define Foundation_Exception_MgArgumentOutOfRangeException                      FOUNDATION_EXCEPTION_ID+1
#define Foundation_Exception_MgClassNotFoundException                           FOUNDATION_EXCEPTION_ID+2
#define Foundation_Exception_MgConfigurationException                           FOUNDATION_EXCEPTION_ID+3
#define Foundation_Exception_MgConfigurationLoadFailedException                 FOUNDATION_EXCEPTION_ID+4
#define Foundation_Exception_MgConfigurationSaveFailedException                 FOUNDATION_EXCEPTION_ID+5
#define Foundation_Exception_MgDateTimeException                                FOUNDATION_EXCEPTION_ID+6
#define Foundation_Exception_MgDecryptionException                              FOUNDATION_EXCEPTION_ID+7
#define Foundation_Exception_MgDirectoryNotFoundException                       FOUNDATION_EXCEPTION_ID+8
#define Foundation_Exception_MgDivideByZeroException                            FOUNDATION_EXCEPTION_ID+9
#define Foundation_Exception_MgDomainException                                  FOUNDATION_EXCEPTION_ID+10
#define Foundation_Exception_MgDuplicateDirectoryException                      FOUNDATION_EXCEPTION_ID+11
#define Foundation_Exception_MgDuplicateFileException                           FOUNDATION_EXCEPTION_ID+12
#define Foundation_Exception_MgDuplicateObjectException                         FOUNDATION_EXCEPTION_ID+13
#define Foundation_Exception_MgEncryptionException                              FOUNDATION_EXCEPTION_ID+14
#define Foundation_Exception_MgException                                        FOUNDATION_EXCEPTION_ID+15
#define Foundation_Exception_MgFileIoException                                  FOUNDATION_EXCEPTION_ID+16
#define Foundation_Exception_MgFileNotFoundException                            FOUNDATION_EXCEPTION_ID+17
#define Foundation_Exception_MgIndexOutOfRangeException                         FOUNDATION_EXCEPTION_ID+18
#define Foundation_Exception_MgInvalidArgumentException                         FOUNDATION_EXCEPTION_ID+19
#define Foundation_Exception_MgInvalidCastException                             FOUNDATION_EXCEPTION_ID+20
#define Foundation_Exception_MgInvalidMimeTypeException                         FOUNDATION_EXCEPTION_ID+21
#define Foundation_Exception_MgInvalidOperationException                        FOUNDATION_EXCEPTION_ID+22
#define Foundation_Exception_MgInvalidPropertyTypeException                     FOUNDATION_EXCEPTION_ID+23
#define Foundation_Exception_MgInvalidStreamHeaderException                     FOUNDATION_EXCEPTION_ID+24
#define Foundation_Exception_MgIoException                                      FOUNDATION_EXCEPTION_ID+25
#define Foundation_Exception_MgLengthException                                  FOUNDATION_EXCEPTION_ID+26
#define Foundation_Exception_MgLogicException                                   FOUNDATION_EXCEPTION_ID+27
#define Foundation_Exception_MgNotFiniteNumberException                         FOUNDATION_EXCEPTION_ID+28
#define Foundation_Exception_MgNotImplementedException                          FOUNDATION_EXCEPTION_ID+29
#define Foundation_Exception_MgNullArgumentException                            FOUNDATION_EXCEPTION_ID+30
#define Foundation_Exception_MgNullPropertyValueException                       FOUNDATION_EXCEPTION_ID+31
#define Foundation_Exception_MgNullReferenceException                           FOUNDATION_EXCEPTION_ID+32
#define Foundation_Exception_MgObjectNotFoundException                          FOUNDATION_EXCEPTION_ID+33
#define Foundation_Exception_MgOutOfMemoryException                             FOUNDATION_EXCEPTION_ID+34
#define Foundation_Exception_MgOutOfRangeException                              FOUNDATION_EXCEPTION_ID+35
#define Foundation_Exception_MgOverflowException                                FOUNDATION_EXCEPTION_ID+36
#define Foundation_Exception_MgPlatformNotSupportedException                    FOUNDATION_EXCEPTION_ID+37
#define Foundation_Exception_MgResourcesException                               FOUNDATION_EXCEPTION_ID+38
#define Foundation_Exception_MgResourcesLoadFailedException                     FOUNDATION_EXCEPTION_ID+39
#define Foundation_Exception_MgResourceTagNotFoundException                     FOUNDATION_EXCEPTION_ID+40
#define Foundation_Exception_MgRuntimeException                                 FOUNDATION_EXCEPTION_ID+41
#define Foundation_Exception_MgStreamIoException                                FOUNDATION_EXCEPTION_ID+42
#define Foundation_Exception_MgSystemException                                  FOUNDATION_EXCEPTION_ID+43
#define Foundation_Exception_MgTemporaryFileNotAvailableException               FOUNDATION_EXCEPTION_ID+44
#define Foundation_Exception_MgThirdPartyException                              FOUNDATION_EXCEPTION_ID+45
#define Foundation_Exception_MgUnauthorizedAccessException                      FOUNDATION_EXCEPTION_ID+46
#define Foundation_Exception_MgUnclassifiedException                            FOUNDATION_EXCEPTION_ID+47
#define Foundation_Exception_MgUnderflowException                               FOUNDATION_EXCEPTION_ID+48
#define Foundation_Exception_MgXmlException                                     FOUNDATION_EXCEPTION_ID+49
#define Foundation_Exception_MgXmlParserException                               FOUNDATION_EXCEPTION_ID+50
#define Foundation_Exception_MgGridDensityException                             FOUNDATION_EXCEPTION_ID+51

// Foundation Property

#define Foundation_Property_Property                        FOUNDATION_PROPERTY_ID+0
#define Foundation_Property_StringProperty                  FOUNDATION_PROPERTY_ID+1
#define Foundation_Property_PropertyDefinition              FOUNDATION_PROPERTY_ID+2

// Foundation System
#define Foundation_System_Disposable                        FOUNDATION_SYSTEM_ID+1
#define Foundation_System_Object                            FOUNDATION_SYSTEM_ID+2
#define Foundation_System_CryptographyManager               FOUNDATION_SYSTEM_ID+3
