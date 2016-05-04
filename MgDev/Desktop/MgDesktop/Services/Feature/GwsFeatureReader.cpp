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

#include "FeatureDefs.h"
#include "Fdo.h"
#include "GwsFeatureReader.h"
#include "GwsQueryEngineImp.h"
#include "FeatureUtil.h"
#include "JoinFeatureReader.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgdGwsFeatureReader object from a byte source
///</summary>
///<param name="byteSource">Byte  source object</param>
///
MgdGwsFeatureReader::MgdGwsFeatureReader(
    MgdGwsConnectionPool* pool,
    IGWSFeatureIterator* gwsFeatureIterator,
    IGWSFeatureIterator* gwsFeatureIteratorCopy,
    CREFSTRING extensionName,
    FdoStringCollection* relationNames,
    bool bForceOneToOne,
    MgStringCollection* attributeNameDelimiters) :
        m_bAdvancePrimaryIterator(true),
        m_bForceOneToOne(bForceOneToOne)
{
    MG_FEATURE_SERVICE_TRY()

    m_gwsFeatureIterator = FDO_SAFE_ADDREF(gwsFeatureIterator);
    m_gwsFeatureIteratorCopy = FDO_SAFE_ADDREF(gwsFeatureIteratorCopy);
    m_attributeNameDelimiters = SAFE_ADDREF(attributeNameDelimiters);

    m_featureSet = NULL;
    m_relationNames = FDO_SAFE_ADDREF(relationNames);
    m_extensionName = extensionName;

    // Get the Extended Feature Description
    m_gwsFeatureIterator->DescribeFeature(&m_primaryExtendedFeatureDescription);

    // Get the class definition
    m_classDef = GetMgClassDefinition(false);

    m_removeFromPoolOnDestruction = false;
    m_bNoMoreData = false;

    m_pool = SAFE_ADDREF(pool);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.MgdGwsFeatureReader")
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgdGwsFeatureReader object
///</summary>
///
MgdGwsFeatureReader::MgdGwsFeatureReader()
{
    m_gwsFeatureIterator = NULL;
    m_gwsFeatureIteratorCopy = NULL;
    m_bForceOneToOne = true;
    m_attributeNameDelimiters = NULL;
    m_joinReader = NULL;
    m_expressionEngine = NULL;
    m_filter = NULL;
    m_removeFromPoolOnDestruction = false;
    m_bNoMoreData = false;
    m_classDef = NULL;
    m_relationNames = NULL;
    m_featureSet = NULL;
    m_pool = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgdGwsFeatureReader object
///</summary>
///
MgdGwsFeatureReader::~MgdGwsFeatureReader()
{
    // DO NOT Close() the FDO reader from here or free the reader resources because
    // we may be reading incrementally from the web tier and the ServerFeatureInstance
    // will live much shorter than the Proxy reader on the web tier which needs to
    // keep reading from the underlying FDO reader.

    // Force the expression engine to clean up
    m_filter = NULL;
    m_expressionEngine = NULL;
    m_joinReader = NULL;

    m_gwsFeatureIterator = NULL;
    m_gwsFeatureIteratorCopy = NULL;
    m_attributeNameDelimiters = NULL;
    m_primaryExtendedFeatureDescription = NULL;
    m_pool = NULL;
}

void MgdGwsFeatureReader::SetFilter(FdoFilter* filter)
{
    // Create the join reader
    m_joinReader = new MgdJoinFeatureReader(this);

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
bool MgdGwsFeatureReader::ReadNext()
{
    CHECKNULL(m_gwsFeatureIterator, L"MgdGwsFeatureReader.ReadNext");

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
                    throw new MgInvalidArgumentException(L"MgdGwsFeatureReader.ReadNext",
                        __LINE__, __WFILE__, NULL, L"MgInvalidCollectionSize", NULL);
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
                                m_secondaryGwsFeatureIteratorMap.insert(GwsFeatureIteratorPair(attributeNameDelimiter, GwsRightSideIterator(secondaryIter,true)));
                            }
                            else
                            {
                                FdoPtr<IGWSExtendedFeatureDescription> pDesc = m_primaryExtendedFeatureDescription->GetItem(i);
                                if(pDesc->LeftOuterJoin())
                                {
                                    // Fix ticket #1162
                                    // If we are processing left-outer joins, we don't skip the left side feature even though
                                    // there is no right side feature exists.
                                    m_secondaryGwsFeatureIteratorMap.insert(GwsFeatureIteratorPair(attributeNameDelimiter, GwsRightSideIterator(secondaryIter,false)));
                                    retVal = true;
                                }
                                else
                                {
                                    m_bAdvancePrimaryIterator = true;
                                }
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
                    secondaryFeatureIter = iter->second.Iterator();
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
                        secondaryFeatureIter = iter->second.Iterator();
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
                            throw new MgInvalidArgumentException(L"MgdGwsFeatureReader.ReadNext",
                                __LINE__, __WFILE__, NULL, L"MgInvalidCollectionSize", NULL);
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
                                        m_secondaryGwsFeatureIteratorMap.insert(GwsFeatureIteratorPair(attributeNameDelimiter, GwsRightSideIterator(featureIter2,true)));
                                    }
                                    else
                                    {
                                        FdoPtr<IGWSExtendedFeatureDescription> pDesc = m_primaryExtendedFeatureDescription->GetItem(i);
                                        if(pDesc->LeftOuterJoin())
                                        {
                                            // Fix ticket #1162
                                            // If we are processing left-outer joins, we don't skip the left side feature even though
                                            // there is no right side feature exists.
                                            m_secondaryGwsFeatureIteratorMap.insert(GwsFeatureIteratorPair(attributeNameDelimiter, GwsRightSideIterator(featureIter2,false)));
                                            retVal = true;
                                        }
                                        else
                                        {
                                            m_bAdvancePrimaryIterator = true;
                                        }
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.ReadNext")

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
MgClassDefinition* MgdGwsFeatureReader::GetClassDefinition()
{
    // Check to see if the class definition cached contains the XML information
    bool bGetClassDefinition = true;
    if (NULL != (MgClassDefinition*)m_classDef)
    {
        if(m_classDef->HasSerializedXml())
        {
            // We have the full object so don't update it
            bGetClassDefinition = false;
        }
    }

    if (bGetClassDefinition)
    {
        m_classDef = GetMgClassDefinition(true);
    }


    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
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
MgClassDefinition* MgdGwsFeatureReader::GetClassDefinitionNoXml()
{
    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
}


//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgdGwsFeatureReader::IsNull(CREFSTRING propertyName)
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.IsNull");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgdGwsFeatureReader::GetBoolean(CREFSTRING propertyName)
{
    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retreive the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetBoolean");

    try
    {
        retVal = gwsFeatureIter->GetBoolean(parsedPropertyName.c_str());
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetBoolean",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetBoolean");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgdGwsFeatureReader::GetByte(CREFSTRING propertyName)
{
    BYTE retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetByte");

    try
    {
        retVal = (BYTE)gwsFeatureIter->GetByte(parsedPropertyName.c_str());
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetByte",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetByte");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgdGwsFeatureReader::GetDateTime(CREFSTRING propertyName)
{
    Ptr<MgDateTime> retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetDateTime");

    try
    {
        FdoDateTime val = gwsFeatureIter->GetDateTime(parsedPropertyName.c_str());
        retVal = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                                (INT8)val.hour, (INT8)val.minute, val.seconds);
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetDateTime",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetDateTime");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the single value.</returns>
float MgdGwsFeatureReader::GetSingle(CREFSTRING propertyName)
{
    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetSingle");

    try
    {
        retVal = gwsFeatureIter->GetSingle(parsedPropertyName.c_str());
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetSingle",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetSingle");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the double value.</returns>
double MgdGwsFeatureReader::GetDouble(CREFSTRING propertyName)
{
    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetDouble");

    try
    {
        retVal = gwsFeatureIter->GetDouble(parsedPropertyName.c_str());
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetDouble",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetDouble");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgdGwsFeatureReader::GetInt16(CREFSTRING propertyName)
{
    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetInt16");

    try
    {
        retVal = (INT16)gwsFeatureIter->GetInt16(parsedPropertyName.c_str());
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetInt16",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetInt16");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgdGwsFeatureReader::GetInt32(CREFSTRING propertyName)
{
    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetInt32");

    try
    {
        retVal = (INT32)gwsFeatureIter->GetInt32(parsedPropertyName.c_str());
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetInt32",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
        }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetInt32");

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
INT64 MgdGwsFeatureReader::GetInt64(CREFSTRING propertyName)
{
    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetInt64");

    try
    {
        retVal = (INT64)gwsFeatureIter->GetInt64(parsedPropertyName.c_str());
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetInt64",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetInt64");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
STRING MgdGwsFeatureReader::GetString(CREFSTRING propertyName)
{
    STRING retVal = L"";

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetString");

    try
    {
        FdoString* str = gwsFeatureIter->GetString(parsedPropertyName.c_str());
        if (str != NULL)
        {
            retVal = str;
        }
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetString",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetString");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgdGwsFeatureReader::GetBLOB(CREFSTRING propertyName)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetBLOB");

    try
    {
        FdoPtr<FdoLOBValue> fdoVal = gwsFeatureIter->GetLOB(parsedPropertyName.c_str());
        if (fdoVal != NULL)
        {
            FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
            if (byteArray != NULL)
            {
                FdoByte* bytes = byteArray->GetData();
                FdoInt32 len = byteArray->GetCount();
                Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
                byteReader = byteSource->GetReader();
            }
        }
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetBLOB",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetBLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgdGwsFeatureReader::GetCLOB(CREFSTRING propertyName)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetCLOB");

    try
    {
        FdoPtr<FdoLOBValue> fdoVal = gwsFeatureIter->GetLOB(parsedPropertyName.c_str());
        if (fdoVal != NULL)
        {
            FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
            if (byteArray != NULL)
            {
                FdoByte* bytes = byteArray->GetData();
                FdoInt32 len = byteArray->GetCount();
                Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
                byteReader = byteSource->GetReader();
            }
        }
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetCLOB",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetCLOB");

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
MgFeatureReader* MgdGwsFeatureReader::GetFeatureObject(CREFSTRING propertyName)
{
    // TODO: Figure out how to support object properties.
    throw new MgNotImplementedException(L"MgdGwsFeatureReader.GetFeatureObject", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgdGwsFeatureReader::GetGeometry(CREFSTRING propertyName)
{
    Ptr<MgByteReader> retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetGeometry");

    try
    {
        FdoInt32 len = 0;
        const FdoByte* data = gwsFeatureIter->GetGeometry(parsedPropertyName.c_str(), &len);

        if (data != NULL)
        {
            Ptr<MgByte> mgBytes = new MgByte((BYTE_ARRAY_IN)data, len);
            Ptr<MgByteSource> bSource = new MgByteSource(mgBytes);
            bSource->SetMimeType(MgMimeType::Agf);
            retVal = bSource->GetReader();
        }
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetGeometry",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetGeometry");

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
MgRaster* MgdGwsFeatureReader::GetRaster(CREFSTRING propertyName)
{
    Ptr<MgRaster> retVal;
    STRING gwsFeatureReader = L"";

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetRaster");

    // TODO: The IsNull() check is returning true for WFS/WMS FDO providers when there is valid data.
    //       In this case it should really be returning false so that the data can be retrieved.
    //       For now the check will be commented out until this can be resolved in FDO.
    if(false) // TODO: Force the exception throwing code to be skipped until issue is resolved.
//    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgdGwsFeatureReader.GetRaster",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoIRaster> raster = gwsFeatureIter->GetRaster(parsedPropertyName.c_str());
        CHECKNULL((FdoIRaster*)raster, L"MgdGwsFeatureReader.GetRaster");

        retVal = MgdFeatureUtil::GetMgRaster(raster, parsedPropertyName);
        CHECKNULL((MgRaster*)retVal, L"MgdGwsFeatureReader.GetRaster");

        // Get the service from service manager
        Ptr<MgFeatureService> rasterHelp = new MgdRasterHelper(this);
        retVal->SetMgService(rasterHelp);
        //MgRaster demands a handle
        STRING handle;
        MgUtil::GenerateUuid(handle);
        retVal->SetHandle(handle);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetRaster");

    return retVal.Detach();
}

MgByteReader* MgdGwsFeatureReader::GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // If there is no raster property, GetRaster should not be called
    if (!m_classDef->HasRasterProperty())
    {
        // TODO: specify which argument and message, once we have the mechanism
        STRING message = MgdFeatureUtil::GetMessage(L"MgMissingRasterProperty");
        throw new MgInvalidOperationException(L"MgdFeatureReader::GetRaster", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // There can be more than one Raster property
    if (rasterPropName.empty())
    {
        rasterPropName = m_classDef->GetRasterPropertyName();
    }

    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(rasterPropName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetRaster");

    // If this property is requested then we fetch the raster data
    byteReader = MgdFeatureUtil::GetRaster(gwsFeatureIter, parsedPropertyName, xSize, ySize);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetRaster")

    return byteReader.Detach();
}

// getter by index
BYTE_ARRAY_OUT MgdGwsFeatureReader::GetGeometry(INT32 index, INT32& length)
{
    STRING propertyName = GetPropertyName(index);
    return GetGeometry(propertyName, length);
}

MgByteReader* MgdGwsFeatureReader::GetGeometry(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetGeometry(propertyName);
}

MgFeatureReader*  MgdGwsFeatureReader::GetFeatureObject(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetFeatureObject(propertyName);
}

bool MgdGwsFeatureReader::GetBoolean(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetBoolean(propertyName);
}

BYTE MgdGwsFeatureReader::GetByte(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetByte(propertyName);
}

MgDateTime* MgdGwsFeatureReader::GetDateTime(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetDateTime(propertyName);
}

double MgdGwsFeatureReader::GetDouble(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetDouble(propertyName);
}

INT16 MgdGwsFeatureReader::GetInt16(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetInt16(propertyName);
}

INT32 MgdGwsFeatureReader::GetInt32(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetInt32(propertyName);
}

INT64 MgdGwsFeatureReader::GetInt64(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetInt64(propertyName);
}

float MgdGwsFeatureReader::GetSingle(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetSingle(propertyName);
}

STRING MgdGwsFeatureReader::GetString(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetString(propertyName);
}

MgByteReader* MgdGwsFeatureReader::GetBLOB(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetBLOB(propertyName);
}

MgByteReader* MgdGwsFeatureReader::GetCLOB(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetCLOB(propertyName);
}

bool MgdGwsFeatureReader::IsNull(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return IsNull(propertyName);
}

 MgRaster* MgdGwsFeatureReader::GetRaster(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetRaster(propertyName);
}

void MgdGwsFeatureReader::Serialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgdGwsFeatureReader::Serialize");
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgdGwsFeatureReader::Deserialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgdGwsFeatureReader::Deserialize");
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes all features into an XML.
/// XML is serialized from the current position of feature reader in the order
/// data are retrieved.
/// <returns>MgByteReader holding XML.</returns>
MgByteReader* MgdGwsFeatureReader::ToXml()
{
    throw new MgInvalidOperationException(L"MgdGwsFeatureReader.ToXml",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

string MgdGwsFeatureReader::GetResponseElementName() { NOT_IMPLEMENTED(L"MgdGwsFeatureReader::GetResponseElementName"); }

string MgdGwsFeatureReader::GetBodyElementName() { NOT_IMPLEMENTED(L"MgdGwsFeatureReader::GetResponseElementName"); }

void MgdGwsFeatureReader::ResponseStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdGwsFeatureReader::GetResponseElementName"); }

void MgdGwsFeatureReader::ResponseEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdGwsFeatureReader::GetResponseElementName"); }

void MgdGwsFeatureReader::BodyStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdGwsFeatureReader::GetResponseElementName"); }

void MgdGwsFeatureReader::BodyEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdGwsFeatureReader::GetResponseElementName"); }

void MgdGwsFeatureReader::CurrentToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdGwsFeatureReader::CurrentToStringUtf8"); }

void MgdGwsFeatureReader::HeaderToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdGwsFeatureReader::HeaderToStringUtf8"); }

//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgdGwsFeatureReader::Close()
{
    MG_FEATURE_SERVICE_TRY()

    /*
    if (m_removeFromPoolOnDestruction)
    {
        MgServerFeatureReaderPool* featPool = MgServerFeatureReaderPool::GetInstance();
        if(NULL != featPool)
        {
            STRING featureReader = featPool->GetReaderId(this);
            if (L"" != featureReader)
                featPool->Remove(featureReader);
        }
    }*/

    if (!m_secondaryGwsFeatureIteratorMap.empty())
    {
        for (GwsFeatureIteratorMap::iterator iter = m_secondaryGwsFeatureIteratorMap.begin();
             iter != m_secondaryGwsFeatureIteratorMap.end();
             iter++)
        {
            IGWSFeatureIterator* pSecondaryFeatureIter = iter->second.Iterator();
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
    m_primaryExtendedFeatureDescription = NULL;

    // Force the expression engine to clean up
    m_filter = NULL;
    m_expressionEngine = NULL;
    m_joinReader = NULL;

    /*
    // Let the FDO Connection Manager know that we are no longer using a FDO provider connection.
    MgFdoConnectionManager* fdoConnectionManager = MgFdoConnectionManager::GetInstance();
    ACE_ASSERT(NULL != fdoConnectionManager);

    if (NULL != fdoConnectionManager)
    {
        // Loop through all of the FDO connections
        MgGwsConnectionMap* connections = m_pool->GetConnections();
        for (MgGwsConnectionIter iter = connections->begin();
             iter != connections->end();
             iter++)
        {
            MgServerFeatureConnection* conn = (*iter).second;
            FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();
            fdoConnectionManager->Close(fdoConn);
        }
    }*/

    // Loop through all of the FDO connections
    MgGwsConnectionMap* connections = m_pool->GetConnections();
    for (MgGwsConnectionIter iter = connections->begin();
         iter != connections->end();
         iter++)
    {
        MgdFeatureConnection* conn = (*iter).second;
        conn->Close();
        //FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();
        //fdoConn->Close();
    }

    m_pool = NULL;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.Close");
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgdGwsFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length)
{
    const FdoByte* data = NULL;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader.GetGeometry");

    try
    {
        FdoInt32 len = 0;
        data = gwsFeatureIter->GetGeometry(parsedPropertyName.c_str(), &len);
        length = len;
    }
    catch(...)
    {
        if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdGwsFeatureReader::GetGeometry",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdJoinFeatureReader::GetGeometry");

    return (BYTE_ARRAY_OUT)data;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgdGwsFeatureReader::GetString(CREFSTRING propertyName, INT32& length)
{
    FdoString* retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgdGwsFeatureReader::GetString");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgdGwsFeatureReader::GetString",
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader::GetString");

    return ((const wchar_t*)retVal);
}

void MgdGwsFeatureReader::DeterminePropertyFeatureSource(CREFSTRING inputPropName,
                                                              IGWSFeatureIterator** gwsFeatureIter,
                                                              STRING& relationName,
                                                              STRING& className,
                                                              STRING& parsedPropName)
{
    CHECKNULL(m_gwsFeatureIterator, L"MgdGwsFeatureReader.DeterminePropertyFeatureSource");

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

        throw new MgInvalidArgumentException(L"MgdGwsFeatureReader.DeterminePropertyFeatureSource",
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
            secondaryFeatureIter = iter->second.Iterator();
            CHECKNULL(secondaryFeatureIter, L"MgdGwsFeatureReader.DeterminePropertyFeatureSource");

            FdoPtr<IGWSExtendedFeatureDescription> secondaryDesc;
            secondaryFeatureIter->DescribeFeature(&secondaryDesc);

            GWSQualifiedName secQualifiedClassName = secondaryDesc->ClassName();

            FdoString* featureSource = secQualifiedClassName.FeatureSource();
            FdoString* fclassName = secQualifiedClassName.Name();

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
                    *gwsFeatureIter = iter->second.HasData() ? secondaryFeatureIter : NULL;
                    className = (wchar_t*)fclassName;
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

    if(className.empty())
    {
        // No match was found so this is a computed property (eg: extents)
        GWSQualifiedName primQualifiedClassName = m_primaryExtendedFeatureDescription->ClassName();
        FdoString* fclassName = primQualifiedClassName.Name();
        className = (wchar_t*)fclassName;
    }
}

void MgdGwsFeatureReader::DeterminePropertyFeatureSource(CREFSTRING inputPropName, IGWSFeatureIterator** gwsFeatureIter, STRING& parsedPropName)
{
    STRING relationName;
    STRING className;
    this->DeterminePropertyFeatureSource(inputPropName, gwsFeatureIter, relationName, className, parsedPropName);
}

GwsFeatureIteratorMap MgdGwsFeatureReader::GetSecondaryGwsFeatureIteratorMap()
{
    return m_secondaryGwsFeatureIteratorMap;
}

void MgdGwsFeatureReader::SetAdvancePrimaryIterator(bool bAdvancePrimary)
{
    m_bAdvancePrimaryIterator = bAdvancePrimary;
}

void MgdGwsFeatureReader::SetGwsFeatureIteratorMap(GwsFeatureIteratorMap featureIterMap)
{
    m_secondaryGwsFeatureIteratorMap = featureIterMap;
}

MgStringCollection* MgdGwsFeatureReader::GetAttributeNameDelimiters()
{
    Ptr<MgStringCollection> stringCol = new MgStringCollection();

    for (int i = 0; i < m_attributeNameDelimiters->GetCount(); i++)
    {
        stringCol->Add( m_attributeNameDelimiters->GetItem(i) );
    }

    return stringCol.Detach();
}

bool MgdGwsFeatureReader::IsForceOneToOne()
{
    return m_bForceOneToOne;
}

IGWSFeatureIterator* MgdGwsFeatureReader::GetFeatureIterator()
{
    return FDO_SAFE_ADDREF(m_gwsFeatureIterator.p);
}

void MgdGwsFeatureReader::ParseSecondaryPropertyName(CREFSTRING inputPropName, CREFSTRING delimiter, CREFSTRING secondaryProp, STRING& relationName, STRING& parsedPropName)
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

FdoIFeatureReader* MgdGwsFeatureReader::GetJoinFeatureReader()
{
    return FDO_SAFE_ADDREF(m_joinReader.p);
}

MgFeatureSet* MgdGwsFeatureReader::GetFeatures(INT32 count)
{
    CHECKNULL(m_gwsFeatureIterator, L"MgdGwsFeatureReader.GetFeatures");

    MG_FEATURE_SERVICE_TRY()

    INT32 featCount = count;

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Get MgClassDefinition
        m_classDef = GetMgClassDefinition(true);
        CHECKNULL(m_classDef.p, L"MgdGwsFeatureReader.GetFeatures");
    }

    if (NULL == (MgFeatureSet*)m_featureSet)
    {
        // Create a feature set for a pool of features
        m_featureSet = new MgFeatureSet();
        CHECKNULL((MgFeatureSet*)m_featureSet, L"MgdGwsFeatureReader.GetFeatures");

        // Assign feature class definition to Feature Set
        m_featureSet->SetClassDefinition(m_classDef);
    }
    else
    {
        m_featureSet->ClearFeatures();
    }

    // If class definition contains raster property
    // we can only retrieve one feature at a time
    // because, user would need to supply x,y and extents before
    // they could fetch the raster data. Therefore we can not
    // advance FdoIReader.
    if (m_classDef->HasRasterProperty())
    {
        featCount = 1;
    }

    // Add all features to feature set
    AddFeatures(featCount);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetFeatures")

    return SAFE_ADDREF((MgFeatureSet*)m_featureSet);
}

void MgdGwsFeatureReader::AddFeatures(INT32 count)
{
    CHECKNULL(m_featureSet, L"MgdGwsFeatureReader.AddFeatures");

    INT32 desiredFeatures = 0;

    // Access the class definition
    Ptr<MgClassDefinition> classDef = m_featureSet->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgdGwsFeatureReader.AddFeatures");

    // Access the property definition collection
    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetPropertiesIncludingBase();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgdGwsFeatureReader.AddFeatures");

    // How many properties are we fetching, it should be atleast one
    INT32 cnt = propDefCol->GetCount();

    // We only read if there is atleast one property requested
    if (cnt > 0)
    {
        bool found = false;

        do
        {
            // Advance the reader
            // Read next throws exception which it should not (therefore we just ignore it)
            try
            {
                found = ReadNext();
            }
            catch (FdoException* e)
            {
                // Note: VB 05/10/06 The assert has been commented out as
                // Linux does not remove them from a release build. The assert
                // will cause the server to crash on Linux. The Oracle provider will throw
                // an exception if the ReadNext() method is called after it returns false.
                // This is a known problem and it is safe to ignore the exception.
                //assert(false);
                e->Release();
                found = false;
            }
            catch(...)
            {
                found = false;
            }

            if(found)
            {
                AddFeature((MgPropertyDefinitionCollection*)propDefCol);
                if (count > 0)
                {
                    desiredFeatures++;
                    // Collected required features therefore do not process more
                    if (desiredFeatures == count)
                    {
                        break;
                    }
                }
            }
        } while (found);
    }
}

//////////////////////////////////////////////////////////////////
void MgdGwsFeatureReader::AddFeature(MgPropertyDefinitionCollection* propDefCol)
{
    CHECKNULL(propDefCol, L"MgdGwsFeatureReader.AddFeature");

    Ptr<MgPropertyCollection> propCol = new MgPropertyCollection();
    INT32 cnt = propDefCol->GetCount();

    for (INT32 i=0; i < cnt; i++)
    {
        // Access the property definition
        Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(i);

        // Get the name of property
        STRING propName = propDef->GetName();

        INT16 type = MgdFeatureUtil::GetMgPropertyType(propDef);
        Ptr<MgProperty> prop = MgdFeatureUtil::GetMgProperty(this, propName, type);
        if (prop != NULL)
        {
            propCol->Add(prop);
        }
    }

    m_featureSet->AddFeature(propCol);
}

MgClassDefinition* MgdGwsFeatureReader::GetMgClassDefinition(bool bSerialize)
{
    CHECKNULL(m_gwsFeatureIteratorCopy, L"MgdGwsFeatureReader.GetMgClassDefinition");

    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Retrieve FdoClassDefinition
        FdoPtr<FdoClassDefinition> fdoClassDefinition = m_gwsFeatureIteratorCopy->GetClassDefinition();
        CHECKNULL((FdoClassDefinition*)fdoClassDefinition, L"MgdGwsFeatureReader.GetMgClassDefinition");

        // Convert FdoClassDefinition to MgClassDefinition
        Ptr<MgClassDefinition> mgClassDef = MgdFeatureUtil::GetMgClassDefinition(fdoClassDefinition, bSerialize);
        CHECKNULL((MgClassDefinition*)mgClassDef, L"MgdGwsFeatureReader.GetMgClassDefinition");

        // Advance the primary feature source iterator
        if (m_gwsFeatureIteratorCopy->ReadNext())
        {
            // Retrieve the secondary feature source iterators, get the property definitions and add to class definition
            FdoPtr<IGWSExtendedFeatureDescription> desc;
            m_gwsFeatureIteratorCopy->DescribeFeature(&desc);

            int nSecondaryFeatures = desc->GetCount();
            if (nSecondaryFeatures > 0)
            {
                if (m_attributeNameDelimiters->GetCount() != nSecondaryFeatures)
                {
                    // Should never get here
                    throw new MgInvalidArgumentException(L"MgdGwsFeatureReader.GetMgClassDefinition", __LINE__, __WFILE__, NULL, L"MgInvalidCollectionSize", NULL);
                }
            }

            for (int i = 0; i < nSecondaryFeatures; i++)
            {
                try
                {
                    FdoPtr<IGWSFeatureIterator> featureIter = m_gwsFeatureIteratorCopy->GetJoinedFeatures(i);
                    if (featureIter)
                    {
                        STRING attributeNameDelimiter = m_attributeNameDelimiters->GetItem(i);

                        // Retrieve the secondary class definitions
                        FdoPtr<FdoClassDefinition> secFdoClassDefinition;

                        if (featureIter->ReadNext())
                        {
                            m_secondaryGwsFeatureIteratorMap.insert(GwsFeatureIteratorPair(attributeNameDelimiter, GwsRightSideIterator(featureIter,false)));
                            secFdoClassDefinition = featureIter->GetClassDefinition();
                        }
                        else
                        {
                            CGwsQueryResultDescriptors * primaryDescriptors = dynamic_cast<CGwsQueryResultDescriptors *> (desc.p);
                            int descriptorCount = primaryDescriptors->GetCount();
                            for (int dcIndex = 0; dcIndex < descriptorCount; dcIndex++)
                            {
                                CGwsQueryResultDescriptors* secondaryDescriptors = dynamic_cast<CGwsQueryResultDescriptors *> ( primaryDescriptors->GetItem(dcIndex) );
                                secFdoClassDefinition = secondaryDescriptors->ClassDefinition();
                            }
                        }
                        CHECKNULL((FdoClassDefinition*)secFdoClassDefinition, L"MgdGwsFeatureReader.GetMgClassDefinition");

                        FdoStringP qname = secFdoClassDefinition->GetQualifiedName();

                        // Convert FdoClassDefinition to MgClassDefinition
                        Ptr<MgClassDefinition> secMgClassDef = MgdFeatureUtil::GetMgClassDefinition(secFdoClassDefinition, bSerialize);
                        CHECKNULL((MgClassDefinition*)secMgClassDef, L"MgdGwsFeatureReader.GetMgClassDefinition");

                        // retrieve the secondary properites and prefix them with the relation name
                        Ptr<MgPropertyDefinitionCollection> mpdc2 = secMgClassDef->GetProperties();
                        INT32 mpdc2Count = mpdc2->GetCount();

                        // Add the secondary properties to the classDefinition
                        Ptr<MgPropertyDefinitionCollection> mpdc = mgClassDef->GetProperties();
                        for (INT32 secPropIndex = 0; secPropIndex < mpdc2Count; secPropIndex++)
                        {
                            Ptr<MgPropertyDefinition> propDef = mpdc2->GetItem(secPropIndex);
                            STRING secPropName = propDef->GetName();
                            STRING relationName = (wchar_t*)m_relationNames->GetString(i + 1);
                            secPropName = relationName + attributeNameDelimiter + secPropName;
                            propDef->SetName(secPropName);
                            mpdc->Add(propDef);
                        }
                    }

                    mgClassDef->SetName(m_extensionName);
                }
                catch (FdoException* e)
                {
                    STRING messageId;
                    MgStringCollection arguments;
                    wchar_t* buf = (wchar_t*)e->GetExceptionMessage();
                    INT64 nativeErrorCode = e->GetNativeErrorCode();

                    if (NULL != buf)
                    {
                        messageId = L"MgFormatInnerExceptionMessage";
                        arguments.Add(buf);
                    }

                    FDO_SAFE_RELEASE(e);

                    throw new MgFdoException(L"MgdGwsFeatureReader.GetMgClassDefinition",
                        __LINE__, __WFILE__, NULL, messageId, &arguments, nativeErrorCode);
                }
            }

            // Convert MgClassDefinition to FdoClassDefinition.
            Ptr<MgClassDefinitionCollection> mcdc = new MgClassDefinitionCollection();
            mcdc->Add(mgClassDef);
            FdoPtr<FdoClassCollection> fcc;
            fcc = FdoClassCollection::Create(NULL);
            MgdFeatureUtil::GetFdoClassCollection(fcc, mcdc);
            int nFccCount = fcc->GetCount();

            // Get the FdoClassDefinition
            FdoPtr<FdoClassDefinition> fdc = MgdFeatureUtil::GetFdoClassDefinition(mgClassDef, fcc);

            // Pass the FdoClassDefinition to SerializeToXml
            if (bSerialize)
            {
                STRING str;
                Ptr<MgByteReader> byteReader = SerializeToXml(fdc);
                str = byteReader->ToString();

                STRING str1 = L"";

                size_t idx = str.find(L"?>");

                if (idx >= 0)
                {
                    str1 = str.substr(idx+2);
                }
                else if (idx < 0)
                {
                    idx = str.find(L"<Class");
                    if (idx >= 0)
                    {
                        str1 = str.substr(idx);
                    }
                }

                // Set the serialized feature join result xml for the MgClassDefinition.
                mgClassDef->SetSerializedXml(str1);
            }
        }

        // Store the it for future use
        m_classDef = SAFE_ADDREF((MgClassDefinition*)mgClassDef);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGwsFeatureReader.GetMgClassDefinition")

    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
}

MgByteReader* MgdGwsFeatureReader::SerializeToXml(FdoClassDefinition* classDef)
{
    CHECKNULL(classDef, L"MgdGwsFeatureReader.SerializeToXml");

    FdoString* className = classDef->GetName();
    FdoFeatureSchemaP pSchema = classDef->GetFeatureSchema();
    FdoFeatureSchemaP tempSchema;
    FdoClassDefinitionP featClass;
    FdoInt32 index = 0;

    if (pSchema != NULL)
    {
        //Get the position of the class in the collecion
        FdoPtr<FdoClassCollection> fcc = pSchema->GetClasses();
        index = fcc->IndexOf( className );

        // Move class of interest to its own schema
        tempSchema = FdoFeatureSchema::Create( pSchema->GetName(), L"" );
        featClass = FdoClassesP(pSchema->GetClasses())->GetItem( className );
        FdoClassesP(pSchema->GetClasses())->Remove(featClass);
        FdoClassesP(tempSchema->GetClasses())->Add(featClass);
    }
    else
    {
        tempSchema = FdoFeatureSchema::Create(L"TempSchema", L"" );
        FdoClassesP(tempSchema->GetClasses())->Add(classDef);
    }

    FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
    tempSchema->WriteXml(fmis);
    fmis->Reset();

    FdoInt64 len = fmis->GetLength();
    FdoByte *bytes = new FdoByte[(size_t)len];
    CHECKNULL(bytes, L"MgdGwsFeatureReader::SerializeToXml");

    fmis->Read(bytes, (FdoSize)len);

    // Get byte reader from memory stream
    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    // Cleanup the above addition/deletion of class definition
    if (pSchema != NULL)
    {
        if (featClass != NULL)
        {
            FdoClassesP(tempSchema->GetClasses())->Remove(featClass);
            FdoClassesP(pSchema->GetClasses())->Insert(index, featClass);
        }
    }
    else
    {
        FdoClassesP(tempSchema->GetClasses())->Remove(classDef);
    }

    delete[] bytes;

    return byteReader.Detach();
}

void MgdGwsFeatureReader::OwnsConnections()
{
    MgGwsConnectionMap* connections = m_pool->GetConnections();
    for (MgGwsConnectionIter iter = connections->begin();
         iter != connections->end();
         iter++)
    {
        MgdFeatureConnection* conn = (*iter).second;
        if(conn)
        {
            conn->OwnReader();
        }
    }
}