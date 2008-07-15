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

#include "ServerFeatureServiceDefs.h"
#include "ServerGwsFeatureReader.h"
#include "ServerFeatureUtil.h"
#include "JoinFeatureReader.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgServerGwsFeatureReader object from a byte source
///</summary>
///<param name="byteSource">Byte  source object</param>
///
MgServerGwsFeatureReader::MgServerGwsFeatureReader(
    IGWSFeatureIterator* gwsFeatureIterator, bool bForceOneToOne, MgStringCollection* attributeNameDelimiters) :
        m_bAdvancePrimaryIterator(true),
        m_bForceOneToOne(bForceOneToOne)
{
    MG_FEATURE_SERVICE_TRY()

    m_gwsFeatureIterator = FDO_SAFE_ADDREF(gwsFeatureIterator);
    m_attributeNameDelimiters = SAFE_ADDREF(attributeNameDelimiters);

    // Get the Extended Feature Description
    m_gwsFeatureIterator->DescribeFeature(&m_primaryExtendedFeatureDescription);

    m_gwsGetFeatures = new MgServerGwsGetFeatures(this,
                                                  m_gwsFeatureIterator,
                                                  m_attributeNameDelimiters,
                                                  m_primaryExtendedFeatureDescription,
                                                  m_bForceOneToOne);
    m_removeFromPoolOnDestruction = false;
    m_bNoMoreData = false;
    m_classDef = NULL;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.MgServerGwsFeatureReader")
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgServerGwsFeatureReader object
///</summary>
///
MgServerGwsFeatureReader::MgServerGwsFeatureReader()
{
    m_gwsFeatureIterator = NULL;
    m_gwsFeatureIteratorCopy = NULL;
    m_gwsGetFeatures = NULL;
    m_bForceOneToOne = true;
    m_attributeNameDelimiters = NULL;
    m_joinReader = NULL;
    m_expressionEngine = NULL;
    m_filter = NULL;
    m_removeFromPoolOnDestruction = false;
    m_bNoMoreData = false;
    m_classDef = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgServerGwsFeatureReader object
///</summary>
///
MgServerGwsFeatureReader::~MgServerGwsFeatureReader()
{
    //DO NOT Close() the FDO reader from here -- we may be reading
    //incrementally from the web tier and the ServerFeatureInstance
    //will live much shorter than the ProxyFeatureReader on the
    //web tier which needs to keep reading from the underlying
    //FDO feature reader

    SAFE_RELEASE(m_gwsGetFeatures);

    // Force the expression engine to clean up
    m_filter = NULL;
    m_expressionEngine = NULL;
    m_joinReader = NULL;

    m_gwsFeatureIterator = NULL;
    m_gwsFeatureIteratorCopy = NULL;
    m_attributeNameDelimiters = NULL;
    m_primaryExtendedFeatureDescription = NULL;

    // Let the FDO Connection Manager know that we are no longer using a FDO provider connection.
    MgFdoConnectionManager* fdoConnectionManager = MgFdoConnectionManager::GetInstance();
    ACE_ASSERT(NULL != fdoConnectionManager);

    if (NULL != fdoConnectionManager)
    {
        fdoConnectionManager->RemoveUnusedFdoConnections();
    }
}

void MgServerGwsFeatureReader::SetFilter(FdoFilter* filter)
{
    // Create the join reader
    m_joinReader = new MgJoinFeatureReader(this);

    if(NULL != filter)
    {
        #ifdef _DEBUG
        STRING filterText = filter->ToString();
        ACE_DEBUG((LM_ERROR, ACE_TEXT("JOIN FILTER:\n%W\n\n"), filterText.c_str()));
        #endif

        m_filter = SAFE_ADDREF(filter);

        // Create the reader used by the expression engine
        FdoPtr<FdoClassDefinition> fdoClassDef = m_joinReader->GetClassDefinition();
        m_expressionEngine = FdoExpressionEngine::Create(m_joinReader, fdoClassDef, NULL);

        try
        {
            // Validate the filter.  An Fdo exception is thrown if it is not valid.
            m_expressionEngine->ValidateFilter(fdoClassDef, filter);
        }
        catch (FdoException* e)
        {
            //  Re-throw the exception so that it can be written to the logs.
            throw e;
        }
        catch (...)
        {
        }

        // Let the underlying MgServerGwsGetFeatures object know about the filter
        m_gwsGetFeatures->SetFilter(m_expressionEngine, m_filter);
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Advances the reader to the next item and returns true if there is
/// another object to read or false if reading is complete. The default
/// position of the reader is prior to the first item. Thus you must
/// call ReadNext to begin accessing any data.
/// </summary>
/// <returns>
/// Returns true if there is a next item.
/// </returns>
bool MgServerGwsFeatureReader::ReadNext()
{
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsFeatureReader.ReadNext");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    bool bDone = false;

    // Check to see if there is more data
    // This flag is used because of a ReadNext() defect with the join engine and FDO
    // which when you do another ReadNext() after a return result of false for a ReadNext()
    // it resets and returns true and allows reading to continue.
    if(m_bNoMoreData)
    {
        bDone = true;
    }

    while(!bDone)
    {
        // Advance the reader
        FdoPtr<IGWSFeatureIterator> secondaryIter;

        if (m_bForceOneToOne || m_bAdvancePrimaryIterator)
        {
            // advance the primary feature source iterator
            retVal = m_gwsFeatureIterator->ReadNext();
            if(false == retVal)
            {
                // This flag is used because of a ReadNext() defect with the join engine and FDO
                // which when you do another ReadNext() after a return result of false for a ReadNext()
                // it resets and returns true and allows reading to continue.
                m_bNoMoreData = true;
            }

            m_secondaryGwsFeatureIteratorMap.clear();
            m_bAdvancePrimaryIterator = false;

            if (retVal)
            {
                // retrieve the secondary feature source iterators, advance the iterators and store into a collection

                //  Check if a delimiter has been defined for each Attribute Relate,
                //  i.e. the number of delimiters collected should equal the number of joined secondary features
                if (m_attributeNameDelimiters->GetCount() != (INT32)m_primaryExtendedFeatureDescription->GetCount())
                {
                    // Should never get here
                    throw new MgInvalidArgumentException(L"MgServerGwsFeatureReader.ReadNext", __LINE__, __WFILE__, NULL, L"MgInvalidCollectionSize", NULL);
                }

                for (int i = 0; i < m_primaryExtendedFeatureDescription->GetCount(); i++)
                {
                    // Retrieve the AttributeNameDelimiter from the collection for the properties from this secondary feature source
                    STRING attributeNameDelimiter = m_attributeNameDelimiters->GetItem(i);

                    try
                    {
                        retVal = false;
                        secondaryIter = m_gwsFeatureIterator->GetJoinedFeatures(i);
                        if(NULL != secondaryIter.p)
                        {
                            retVal = secondaryIter->ReadNext();
                            if (retVal)
                            {
                                // Since key values in this Pair do not need to be unqiue, we will use it to store the delimiter string that is defined
                                // for the extended properties that originate from this secondary feature source
                                m_secondaryGwsFeatureIteratorMap.insert(GwsFeatureIteratorPair(attributeNameDelimiter, secondaryIter));
                            }
                            else
                            {
                                m_bAdvancePrimaryIterator = true;
                            }
                        }
                        else
                        {
                            m_bAdvancePrimaryIterator = true;
                        }
                    }
                    catch (FdoException* e)
                    {
                        FDO_SAFE_RELEASE(e);
                        continue;
                    }
                }

                if(m_bForceOneToOne)
                {
                    if(retVal)
                    {
                        bDone = true;
                    }
                    else
                    {
                        // Secondary failed
                    }
                }
            }
        }
        else
        {
                // advance the secondary iterator
                IGWSFeatureIterator* secondaryFeatureIter = NULL;

                // Advance the last iterator inserted into the collection until no more features.
                // Then remove from the collection.
                // Repeat until the collection is empty.

                GwsFeatureIteratorMap::iterator iter;
                if (m_secondaryGwsFeatureIteratorMap.size() > 0)
                {
                    iter = m_secondaryGwsFeatureIteratorMap.begin();
                    secondaryFeatureIter = iter->second;
                    retVal = secondaryFeatureIter->ReadNext();
                }

                if (!retVal)
                {
                    if (!m_secondaryGwsFeatureIteratorMap.empty())
                    {
                        m_secondaryGwsFeatureIteratorMap.erase(iter);
                    }
                    if (m_secondaryGwsFeatureIteratorMap.size() > 0)
                    {
                        iter = m_secondaryGwsFeatureIteratorMap.begin();
                        secondaryFeatureIter = iter->second;
                        retVal = secondaryFeatureIter->ReadNext();
                    }
                }

                // if no more secondary records, then advance the primary reader (along with its associated secondary reader)
                if (!retVal)
                {
                    // advance the primary feature source iterator
                    retVal = m_gwsFeatureIterator->ReadNext();
                    if(false == retVal)
                    {
                        // This flag is used because of a ReadNext() defect with the join engine and FDO
                        // which when you do another ReadNext() after a return result of false for a ReadNext()
                        // it resets and returns true and allows reading to continue.
                        m_bNoMoreData = true;
                    }
                    m_bAdvancePrimaryIterator = false;

                    if (retVal)
                    {
                        // retrieve the secondary feature source iterators, advance the iterators and store into a collection
                        m_secondaryGwsFeatureIteratorMap.clear();

                        //  Check if each a delimiter has been defined for each Attribute Relate,
                        //  i.e. the number of delimiters collected should equal the number of joined secondary features
                        if (m_attributeNameDelimiters->GetCount() != (INT32)m_primaryExtendedFeatureDescription->GetCount())
                        {
                            // Should never get here
                            throw new MgInvalidArgumentException(L"MgServerGwsFeatureReader.ReadNext", __LINE__, __WFILE__, NULL, L"MgInvalidCollectionSize", NULL);
                        }

                        for (int i = 0; i < m_primaryExtendedFeatureDescription->GetCount(); i++)
                        {
                            // Retrieve the AttributeNameDelimiter from the collection for the properties from this secondary feature source
                            STRING attributeNameDelimiter = m_attributeNameDelimiters->GetItem(i);

                            try
                            {
                                FdoPtr<IGWSFeatureIterator> featureIter2 = m_gwsFeatureIterator->GetJoinedFeatures(i);
                                if(NULL != featureIter2.p)
                                {
                                    if (featureIter2->ReadNext())
                                    {
                                        // Since key values in this Pair do not need to be unqiue, we will use it to store the delimiter string that is defined
                                        // for the extended properties that originate from this secondary feature source
                                        m_secondaryGwsFeatureIteratorMap.insert(GwsFeatureIteratorPair(attributeNameDelimiter, featureIter2));
                                    }
                                    else
                                    {
                                        m_bAdvancePrimaryIterator = true;
                                    }
                                }
                                else
                                {
                                    m_bAdvancePrimaryIterator = true;
                                }
                            }
                            catch (FdoException* e)
                            {
                                FDO_SAFE_RELEASE(e);
                                continue;
                            }
                        }
                    }
                }
        }

        if(!m_bAdvancePrimaryIterator)
        {
            if(true == retVal)
            {
                // Are we doing a post filter?
                if(NULL != m_expressionEngine.p)
                {
                    // Check if the data matches the filter
                    bool bFilterMatch = m_expressionEngine->ProcessFilter(m_filter);
                    if(bFilterMatch)
                    {
                        // The reader is on a feature that matches the filter
                        bDone = true;
                    }
                    else
                    {
                        // The reader is on a feature that does NOT match the filter
                        // Reset the return value and loop again
                        retVal = false;
                        bDone = false;
                    }
                }
                else
                {
                    bDone = true;
                }
            }
            else
            {
                // Nothing more to read
                bDone = true;
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.ReadNext")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the definition of the object currently being read. If the user
/// has requested only a subset of the class properties, the class
/// definition reflects what the user has asked, rather than the full class
/// definition.
/// </summary>
/// <returns>A MgClassDefinition representing the current object
///</returns>
MgClassDefinition* MgServerGwsFeatureReader::GetClassDefinition()
{
    CHECKNULL(m_gwsGetFeatures, L"MgServerGwsFeatureReader.GetClassDefinition");

    Ptr<MgClassDefinition> classDef;

    if(NULL == m_classDef.p)
    {
        MG_FEATURE_SERVICE_TRY()

        Ptr<MgServerGwsGetFeatures> gwsGetFeatures = new MgServerGwsGetFeatures(this, m_gwsFeatureIteratorCopy, m_attributeNameDelimiters, m_primaryExtendedFeatureDescription);
        gwsGetFeatures->SetRelationNames(FdoPtr<FdoStringCollection>(m_gwsGetFeatures->GetRelationNames()));
        gwsGetFeatures->SetExtensionName(m_gwsGetFeatures->GetExtensionName());
        m_classDef = gwsGetFeatures->GetMgClassDefinition(false);

        MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetClassDefinition")
    }

    classDef = m_classDef;

    return classDef.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the definition of the object currently being read. If the user
/// has requested only a subset of the class properties, the class
/// definition reflects what the user has asked, rather than the full class
/// definition.
/// </summary>
/// <returns>A MgClassDefinition representing the current object
///</returns>
/// NOTE: This is internal API used by the mapping service
/// in the case where we do not want to spend time
/// serializing the class definition to XML
MgClassDefinition* MgServerGwsFeatureReader::GetClassDefinitionNoXml()
{
    CHECKNULL(m_gwsGetFeatures, L"MgServerGwsFeatureReader.GetClassDefinitionNoXml");

    Ptr<MgClassDefinition> classDef;

    if(NULL == m_classDef.p)
    {
        MG_FEATURE_SERVICE_TRY()

        Ptr<MgServerGwsGetFeatures> gwsGetFeatures = new MgServerGwsGetFeatures(this, m_gwsFeatureIteratorCopy, m_attributeNameDelimiters, m_primaryExtendedFeatureDescription);
        gwsGetFeatures->SetRelationNames(FdoPtr<FdoStringCollection>(m_gwsGetFeatures->GetRelationNames()));
        gwsGetFeatures->SetExtensionName(m_gwsGetFeatures->GetExtensionName());
        m_classDef = gwsGetFeatures->GetMgClassDefinition(false);

        MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetClassDefinitionNoXml")
    }

    classDef = m_classDef;

    return classDef.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgServerGwsFeatureReader::IsNull(CREFSTRING propertyName)
{
    bool retVal = true;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    if (gwsFeatureIter)
    {
        retVal = gwsFeatureIter->IsNull(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.IsNull");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgServerGwsFeatureReader::GetBoolean(CREFSTRING propertyName)
{
    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retreive the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetBoolean");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetBoolean",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetBoolean(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetBoolean");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgServerGwsFeatureReader::GetByte(CREFSTRING propertyName)
{
    BYTE retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetByte");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetByte",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (BYTE)gwsFeatureIter->GetByte(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetByte");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgServerGwsFeatureReader::GetDateTime(CREFSTRING propertyName)
{
    Ptr<MgDateTime> retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetDateTime");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetDateTime",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoDateTime val = gwsFeatureIter->GetDateTime(parsedPropertyName.c_str());
        retVal = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                                (INT8)val.hour, (INT8)val.minute, val.seconds);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetDateTime");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the single value.</returns>
float MgServerGwsFeatureReader::GetSingle(CREFSTRING propertyName)
{
    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetSingle");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetSingle",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetSingle(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetSingle");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the double value.</returns>
double MgServerGwsFeatureReader::GetDouble(CREFSTRING propertyName)
{
    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetDouble");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetDouble",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetDouble(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetDouble");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgServerGwsFeatureReader::GetInt16(CREFSTRING propertyName)
{
    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetInt16");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetInt16",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT16)gwsFeatureIter->GetInt16(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetInt16");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgServerGwsFeatureReader::GetInt32(CREFSTRING propertyName)
{
    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetInt32");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetInt32",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT32)gwsFeatureIter->GetInt32(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetInt32");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 64 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 64 bits or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 64 bits value.
/// Note: INT64 is actually a pointer to an Integer64 object
///</returns>
INT64 MgServerGwsFeatureReader::GetInt64(CREFSTRING propertyName)
{
    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetInt64");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetInt64",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT64)gwsFeatureIter->GetInt64(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetInt64");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
STRING MgServerGwsFeatureReader::GetString(CREFSTRING propertyName)
{
    STRING retVal = L"";

    MG_FEATURE_SERVICE_TRY()

    if(this->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        INT32 length = 0;
        const wchar_t* str = this->GetString(propertyName.c_str(), length);
        if (str != NULL)
        {
            retVal = str;
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetString");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgServerGwsFeatureReader::GetBLOB(CREFSTRING propertyName)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetBLOB");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetBLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = m_gwsGetFeatures->GetLOBFromFdo(parsedPropertyName, gwsFeatureIter);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetBLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgServerGwsFeatureReader::GetCLOB(CREFSTRING propertyName)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetCLOB");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetCLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = m_gwsGetFeatures->GetLOBFromFdo(parsedPropertyName, gwsFeatureIter);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetCLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the FeatureReader to access this object value.
/// The property must be of an object type{} otherwise, the result is NULL.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the feature reader to access this object.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgFeatureReader* MgServerGwsFeatureReader::GetFeatureObject(CREFSTRING propertyName)
{
    // TODO: Figure out how to support object properties.
    Ptr<MgServerFeatureReader> featureReader;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetFeatureObject");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetFeatureObject",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoIFeatureReader> featureObjectReader = gwsFeatureIter->GetFeatureObject(parsedPropertyName.c_str());

        if (featureObjectReader != NULL)
        {
            // Create a feature reader identifier
            Ptr<MgServerFeatureReaderIdentifier> featReaderId = new MgServerFeatureReaderIdentifier(featureObjectReader);
            featureReader = new MgServerFeatureReader(featReaderId);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetFeatureObject");

    return featureReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgServerGwsFeatureReader::GetGeometry(CREFSTRING propertyName)
{
    Ptr<MgByteReader> retVal;

    MG_FEATURE_SERVICE_TRY()

    if(this->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        INT32 len;
        BYTE_ARRAY_OUT data = this->GetGeometry(propertyName.c_str(), len);

        if (data != NULL)
        {
            Ptr<MgByte> mgBytes = new MgByte((BYTE_ARRAY_IN)data, len);
            Ptr<MgByteSource> bSource = new MgByteSource(mgBytes);
            retVal = bSource->GetReader();
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetGeometry");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the geometry value of the specified property as a GeometryReader.
/// Because no conversion is performed, the property must be
/// of Geometric type{} otherwise, the result is NULL.</summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the Geometry object.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgRaster* MgServerGwsFeatureReader::GetRaster(CREFSTRING propertyName)
{
    Ptr<MgRaster> retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetRaster");

    // TODO: The IsNull() check is returning true for WFS/WMS FDO providers when there is valid data.
    //       In this case it should really be returning false so that the data can be retrieved.
    //       For now the check will be commented out until this can be resolved in FDO.
    if(false) // TODO: Force the exception throwing code to be skipped until issue is resolved.
//    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetRaster",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoIRaster> raster = gwsFeatureIter->GetRaster(parsedPropertyName.c_str());
        CHECKNULL((FdoIRaster*)raster, L"MgServerGwsFeatureReader.GetRaster");

        retVal = MgServerFeatureUtil::GetMgRaster(raster, parsedPropertyName);
        CHECKNULL((MgRaster*)retVal, L"MgServerGwsFeatureReader.GetRaster");

        // Set the service to MgRaster for GetRaster operation called in GetStream method
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        assert(NULL != serviceMan);

        // Get the service from service manager
        Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
            serviceMan->RequestService(MgServiceType::FeatureService));
        assert(featureService != NULL);

        retVal->SetMgService(featureService);
        retVal->SetHandle((INT32)m_gwsGetFeatures);

        // Collect the feature reader into a pool for GetRaster operation
        MgServerFeatureReaderIdentifierPool* featPool = MgServerFeatureReaderIdentifierPool::GetInstance();
        CHECKNULL(featPool, L"MgServerGwsFeatureReader.GetRaster");

        if (!featPool->Contains(m_gwsGetFeatures))
        {
            featPool->Add(m_gwsGetFeatures); // Add the reference
            m_removeFromPoolOnDestruction = true;
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetRaster");

    return retVal.Detach();
}

void MgServerGwsFeatureReader::Serialize(MgStream* stream)
{
    INT32 count = 1; // Get value from MgConfiguration
    bool operationCompleted = false;
    Ptr<MgFeatureSet> featureSet;

    MG_FEATURE_SERVICE_TRY()

    // Find out the counts from Configuration
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    // Collect the feature reader into a pool for ReadNext operation
    MgServerFeatureReaderIdentifierPool* featPool = MgServerFeatureReaderIdentifierPool::GetInstance();
    CHECKNULL(featPool, L"MgServerFeatureReader.Serialize");

    if (!featPool->Contains(m_gwsGetFeatures))
        featPool->Add(m_gwsGetFeatures); // Add the reference

    featureSet = m_gwsGetFeatures->GetFeatures(count);

    operationCompleted = true;

    MG_FEATURE_SERVICE_CATCH(L"MgServerGwsFeatureReader.Serialize");

    // Mark operation is completed successfully
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted && (mgException == 0))
    {
        stream->WriteInt32((INT32)m_gwsGetFeatures);  // Write the pointer value so we can retrieve it for later use
        stream->WriteObject((MgFeatureSet*)featureSet); // Write the feature set
    }
    else
    {
        stream->WriteObject((MgException*)mgException);
    }

    MG_FEATURE_SERVICE_THROW();
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgServerGwsFeatureReader::Deserialize(MgStream* stream)
{
    throw new MgInvalidOperationException(L"MgServerGwsFeatureReader.Deserialize",
        __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes all features into an XML.
/// XML is serialized from the current position of feature reader in the order
/// data are retrieved.
/// <returns>MgByteReader holding XML.</returns>
MgByteReader* MgServerGwsFeatureReader::ToXml()
{
    throw new MgInvalidOperationException(L"MgServerGwsFeatureReader.ToXml",
        __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgServerGwsFeatureReader::Close()
{
    MG_FEATURE_SERVICE_TRY()

    // If m_gwsGetFeatures was added to pool by the local service
    // this flag will be set to true. In this case we need to
    // remove this from pool on ServerGwsFeatureReader close operation
    if (m_removeFromPoolOnDestruction)
    {
        MgServerFeatureReaderIdentifierPool* featPool = MgServerFeatureReaderIdentifierPool::GetInstance();
        if ((featPool != NULL) && (featPool->Contains(m_gwsGetFeatures)))
            featPool->Remove(m_gwsGetFeatures);
    }

    if (!m_secondaryGwsFeatureIteratorMap.empty())
    {
        for (GwsFeatureIteratorMap::iterator iter = m_secondaryGwsFeatureIteratorMap.begin();
             iter != m_secondaryGwsFeatureIteratorMap.end();
             iter++)
        {
            IGWSFeatureIterator* pSecondaryFeatureIter = iter->second;
            if (pSecondaryFeatureIter)
            {
                pSecondaryFeatureIter->Close();
            }
        }
    }

    if(m_gwsFeatureIterator)
    {
        m_gwsFeatureIterator->Close();
        m_gwsFeatureIterator = NULL;
    }

    if (m_gwsFeatureIteratorCopy)
    {
        m_gwsFeatureIteratorCopy->Close();
        m_gwsFeatureIteratorCopy = NULL;
    }

    // Force resource cleanup
    m_attributeNameDelimiters = NULL;
    SAFE_RELEASE(m_gwsGetFeatures);
    m_primaryExtendedFeatureDescription = NULL;

    // Force the expression engine to clean up
    m_filter = NULL;
    m_expressionEngine = NULL;
    m_joinReader = NULL;

    // Let the FDO Connection Manager know that we are no longer using a FDO provider connection.
    MgFdoConnectionManager* fdoConnectionManager = MgFdoConnectionManager::GetInstance();
    ACE_ASSERT(NULL != fdoConnectionManager);

    if (NULL != fdoConnectionManager)
    {
        fdoConnectionManager->RemoveUnusedFdoConnections();
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.Close");
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgServerGwsFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length)
{
    const FdoByte* data = NULL;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetGeometry");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoInt32 len = 0;
        data = gwsFeatureIter->GetGeometry(parsedPropertyName.c_str(), &len);
        length = len;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetGeometry");

    return (BYTE_ARRAY_OUT)data;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgServerGwsFeatureReader::GetString(CREFSTRING propertyName, INT32& length)
{
    FdoString* retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetString");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetString(parsedPropertyName.c_str());
        if (retVal != NULL)
        {
            length = (INT32)wcslen((const wchar_t*)retVal);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetString");

    return ((const wchar_t*)retVal);
}

void MgServerGwsFeatureReader::DeterminePropertyFeatureSource(CREFSTRING inputPropName,
                                                              IGWSFeatureIterator** gwsFeatureIter,
                                                              STRING& relationName,
                                                              STRING& className,
                                                              STRING& parsedPropName)
{
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsFeatureReader.DeterminePropertyFeatureSource");

    IGWSFeatureIterator* secondaryFeatureIter = NULL;

    // Parse the input property name to determine the feature source,
    // and the property name.  The qualified name could
    // look something like this: Join1PropA, where
    // Join1 = relation name
    // PropA = property name

    STRING qualifiedName;

    if (inputPropName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerGwsFeatureReader.DeterminePropertyFeatureSource",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    // Check if the input propName is prefixed with the relationName
    // by comparing with primary feature source property names
    FdoPtr<FdoStringCollection> primaryPropNames = m_primaryExtendedFeatureDescription->PropertyNames();

    int primaryIndex = primaryPropNames->IndexOf(inputPropName.c_str());
    if( -1 != primaryIndex)
    {
        // No prefix, but the property name does exist in the primary feature source
        relationName.clear();

        GWSQualifiedName primQualifiedClassName = m_primaryExtendedFeatureDescription->ClassName();

        FdoString* fclassName = primQualifiedClassName.Name();
        className = (wchar_t*)fclassName;

        parsedPropName = inputPropName;
        *gwsFeatureIter = m_gwsFeatureIterator;
    }
    else if (!m_secondaryGwsFeatureIteratorMap.empty())
    {
        // cycle thru secondary feature sources and check property names
        for (GwsFeatureIteratorMap::iterator iter = m_secondaryGwsFeatureIteratorMap.begin();
             iter != m_secondaryGwsFeatureIteratorMap.end();
             iter++)
        {
            secondaryFeatureIter = iter->second;
            CHECKNULL(secondaryFeatureIter, L"MgServerGwsFeatureReader.DeterminePropertyFeatureSource");

            FdoPtr<IGWSExtendedFeatureDescription> secondaryDesc;
            secondaryFeatureIter->DescribeFeature(&secondaryDesc);

            GWSQualifiedName secQualifiedClassName = secondaryDesc->ClassName();

            FdoString* featureSource = secQualifiedClassName.FeatureSource();
            FdoString* fclassName = secQualifiedClassName.Name();
            className = (wchar_t*)fclassName;

            FdoPtr<FdoStringCollection> secondaryPropNames = secondaryDesc->PropertyNames();

            // cycle through secondaryPropNames looking for substring occurrency in inputPropName
            STRING attributeNameDelimiter = iter->first;
            FdoInt32 secPropCnt = secondaryPropNames->GetCount();
            relationName.clear();
            for (FdoInt32 secPropIndex = 0; secPropIndex < secPropCnt; secPropIndex++)
            {
                STRING secondaryProp = (STRING)secondaryPropNames->GetString(secPropIndex);
                ParseSecondaryPropertyName(inputPropName, attributeNameDelimiter, secondaryProp,
                    relationName, parsedPropName);

                if ( wcscmp(featureSource, relationName.c_str()) == 0 )
                {
                    *gwsFeatureIter = secondaryFeatureIter;
                    iter = m_secondaryGwsFeatureIteratorMap.end();
                    iter--;
                    break;
                }

            }  // end for loop through secondary property names
        }  //  end for loop through secondary feature sources
    }  //  end else if (!m_secondaryGwsFeatureIteratorMap.empty())
    else
    {
        // The m_secondaryGwsFeatureIteratorMap is empty, which means there are no secondary properties for this feature.
        // But we still need to set the relation name, secondary class name, and the property name.

        CGwsQueryResultDescriptors * primaryDescriptors = dynamic_cast<CGwsQueryResultDescriptors *> (m_primaryExtendedFeatureDescription.p);
        int descriptorCount = primaryDescriptors->GetCount();
        for (int dcIndex = 0; dcIndex < descriptorCount; dcIndex++)
        {
            CGwsQueryResultDescriptors* secondaryDescriptors = dynamic_cast<CGwsQueryResultDescriptors *> ( primaryDescriptors->GetItem(dcIndex) );
            GWSQualifiedName resclassName = secondaryDescriptors->ClassName();

            FdoString* fclassName = resclassName.Name();
            className = (wchar_t*)fclassName;

            STRING attributeNameDelimiter = m_attributeNameDelimiters->GetItem(dcIndex);
            const std::vector<CGwsPropertyDesc> & secondaryProps =
                                                secondaryDescriptors->GetPropertyDescriptors ();
            int secPropCnt = (int)secondaryProps.size();
            for (int secPropIndex = 0; secPropIndex < secPropCnt; secPropIndex++)
            {
                CGwsPropertyDesc gwsPropertyDescriptor = secondaryProps[secPropIndex];
                STRING secondaryProp = (STRING)gwsPropertyDescriptor.m_name;
                ParseSecondaryPropertyName(inputPropName, attributeNameDelimiter, secondaryProp,
                    relationName, parsedPropName);
            }
        }
    }
}


void MgServerGwsFeatureReader::PrepareGwsGetFeatures(CREFSTRING extensionName, FdoStringCollection* relationNames)
{
    m_gwsGetFeatures->SetExtensionName(extensionName);
    m_gwsGetFeatures->SetRelationNames(relationNames);
}

void MgServerGwsFeatureReader::DeterminePropertyFeatureSource(CREFSTRING inputPropName, IGWSFeatureIterator** gwsFeatureIter, STRING& parsedPropName)
{
    STRING relationName;
    STRING className;
    this->DeterminePropertyFeatureSource(inputPropName, gwsFeatureIter, relationName, className, parsedPropName);
}

void MgServerGwsFeatureReader::SetGwsIteratorCopy(IGWSFeatureIterator* iterCopy)
{
    m_gwsFeatureIteratorCopy = FDO_SAFE_ADDREF(iterCopy);
}

GwsFeatureIteratorMap MgServerGwsFeatureReader::GetSecondaryGwsFeatureIteratorMap()
{
    return m_secondaryGwsFeatureIteratorMap;
}

void MgServerGwsFeatureReader::SetAdvancePrimaryIterator(bool bAdvancePrimary)
{
    m_bAdvancePrimaryIterator = bAdvancePrimary;
}

void MgServerGwsFeatureReader::SetGwsFeatureIteratorMap(GwsFeatureIteratorMap featureIterMap)
{
    m_secondaryGwsFeatureIteratorMap = featureIterMap;
}

MgStringCollection* MgServerGwsFeatureReader::GetAttributeNameDelimiters()
{
    Ptr<MgStringCollection> stringCol = new MgStringCollection();

    for (int i = 0; i < m_attributeNameDelimiters->GetCount(); i++)
    {
        stringCol->Add( m_attributeNameDelimiters->GetItem(i) );
    }

    return stringCol.Detach();
}

bool MgServerGwsFeatureReader::IsForceOneToOne()
{
    return m_bForceOneToOne;
}

IGWSFeatureIterator* MgServerGwsFeatureReader::GetFeatureIterator()
{
    return FDO_SAFE_ADDREF(m_gwsFeatureIterator.p);
}

void MgServerGwsFeatureReader::ParseSecondaryPropertyName(CREFSTRING inputPropName, CREFSTRING delimiter, CREFSTRING secondaryProp, STRING& relationName, STRING& parsedPropName)
{
    // Check if AttributeNameDelimiter is used.
    // If no delimiter is used, look for matching secondaryPropName in the inputPropName.

    STRING::size_type delimiterIndex = inputPropName.find(delimiter);
    if (!delimiter.empty() && STRING::npos != delimiterIndex)
    {
        parsedPropName = inputPropName.substr(delimiterIndex + 1).c_str();
        relationName = inputPropName.substr(0, delimiterIndex).c_str();
    }
    else
    {
        STRING::size_type nPropStartIndex = inputPropName.find(secondaryProp);
        if (std::wstring::npos != nPropStartIndex)
        {
            parsedPropName = inputPropName.substr(nPropStartIndex).c_str();
            relationName = inputPropName.substr(0, nPropStartIndex).c_str();
        }
    }
}

FdoIFeatureReader* MgServerGwsFeatureReader::GetJoinFeatureReader()
{
    return FDO_SAFE_ADDREF(m_joinReader.p);
}
