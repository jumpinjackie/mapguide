//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "ProxyFeatureService.h"
#include "ProxyFeatureReader.h"
#include "ProxySqlDataReader.h"
#include "ProxyDataReader.h"
#include "ProxyFeatureTransaction.h"
#include "Command.h"
#include "Services/SqlResult.h"

static const int Feature_Service = (int)MgPacketParser::msiFeature;

IMPLEMENT_CREATE_SERVICE(MgProxyFeatureService)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgProxyFeatureService object
/// </summary>
MgProxyFeatureService::MgProxyFeatureService() : MgFeatureService()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgProxyFeatureService::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the warnings if any
/// </summary>
void MgProxyFeatureService::SetWarning(MgWarnings* warning)
{
    if (warning)
    {
        Ptr<MgStringCollection> ptrCol = warning->GetMessages();
        this->m_warning->AddMessages(ptrCol);
        warning->Release();
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns list of all providers, their connection properties
/// and (default or enumerated) values for these properties, if any, in XML
/// format.
///
/// Schema Definition: FeatureProviders.xsd
/// Sample XML:        FeatureProviders.xml
/// </summary>
/// <returns>Byte array representing XML (or error status)
/// </returns>
MgByteReader* MgProxyFeatureService::GetFeatureProviders()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetFeatureProviders_Id,// Command Code
                       0,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns list of all values for the connection property
/// specified based on partial connection string. To understand this, let
/// us take an example:
///
/// Oracle Provider for Fdo has 3 services running on each Machine A, Machine B
/// and Machine C.
///
/// <table>
/// Machine     IP Address    ServiceName
/// ---------   -----------   ----------------------
/// Machine A   144.111.1.1   Canada, Europe, Russia
/// Machine B   192.111.1.2   Japan, China, India
/// Machine C   159.111.2.1   USA, Mexico, Australia
/// </table>
///
/// List of properties required for connection:
/// <table>
/// PropertyName    Enumerable      Values
/// ------------    ----------      -------------------------------------------------
/// UserName        No
/// Password        No
/// IPAddress       Yes             144.111.1.1
///                                 192.111.1.2
///                                 159.111.2.1
/// ServiceName     Yes             (This value is dependent on IPAddress
///                                  and therefore value is unknown)
///
/// DatabaseName    Yes             (This value is dependent on IPAddress+ServiceName
///                                  and therefore value is unknown)
/// </table>
///
/// User builds partial connection string as
/// UserName=test{ } Password=test{ } IPAddress=144.111.1.1
///
/// Using this partial connection string user requests values for ServiceName property.
///
/// Internally Fdo provider, if it has some dependency on a certain property, it needs
/// to look into property dictionary and find out if a dependent property values are already
/// set. In this case it is an IPAddress only but there can be more than one property.
///
/// If all dependent values are set, it will return the list of values for the dependent
/// property values. The result for the above connectionString would be
///
/// -        Canada,
/// -        Europe,
/// -        Russia
///
/// In case of SQLServer, we might have more than one database per service. Therefore
/// once the list of services are retrieved, user builds another connectionString as
///
/// UserName=test{ } Password=test{ } IPAddress=144.111.1.1{ } ServiceName=Canada
///
/// and requests property values for "DatabaseName". This would result in list of
/// databases available to the users e.g.
///
/// -       Alberta
/// -       British Columbia
/// -       Manitoba
///
/// The final connectionString for Open would be
///
/// UserName=test{ } Password=test{ } IPAddress=144.111.1.1{ } ServiceName=Canada{ } DatabaseName=Alberta
/// </summary>
///
/// <param name="propertyName">Input
/// Name of property for which values need to be retrieved. (Can not be NULL)
/// </param>
///
/// <param name="partialConnString">Input
/// Partial connection string on which property request is dependent.
/// NULL value indicates no dependency.
/// </param>
///
/// <returns>
/// String Collection or NULL if nothing is found
/// </returns>
MgStringCollection* MgProxyFeatureService::GetConnectionPropertyValues(CREFSTRING providerName,
                                                                       CREFSTRING propertyName,
                                                                       CREFSTRING partialConnString)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetConnectionPropertyValues_Id,    // Command Code
                       3,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &providerName,          // Argument#1
                       MgCommand::knString, &propertyName,          // Argument#1
                       MgCommand::knString, &partialConnString,     // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgStringCollection*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Attempts to connect to the Feature Provider given by the connection string
/// </summary>
/// <param name="connectionString">
/// Connection string for Feature Provider
/// </param>
/// <returns>
/// True if connection was successful
/// </returns>
bool MgProxyFeatureService::TestConnection(CREFSTRING providerName, CREFSTRING connectionString)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knInt8,                           // Return type expected
                       MgFeatureServiceOpId::TestConnection_Id,     // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &providerName,          // Argument#1
                       MgCommand::knString, &connectionString,      // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Attempts to connect to the Feature Provider given by the connection string
/// </summary>
/// <param name="connectionString">
/// Connection string for Feature Provider
/// </param>
/// <returns>
/// True if connection was successful
/// </returns>
bool MgProxyFeatureService::TestConnection(MgResourceIdentifier* resource)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knInt8,                           // Return type expected
                       MgFeatureServiceOpId::TestConnectionWithResource_Id, // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns capabilities of the provider applicable for the
/// connection. This would provide details on the following
/// capabilities:
/// 1. Connection
/// 2. Schema
/// 3. Command
/// 4. Filter
/// 5. Expression
///
/// Schema Definition: FeatureProviderCapabilities.xsd
/// Sample XML:        FeatureProviderCapabilities.xml
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <returns>
/// Byte array representing XML (or NULL)
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// NOTE:
/// Subject to change with FDO R2
MgByteReader* MgProxyFeatureService::GetCapabilities(CREFSTRING providerName)
{
    STRING connectionString = L"";
    return GetCapabilities(providerName, connectionString);
}

MgByteReader* MgProxyFeatureService::GetCapabilities(CREFSTRING providerName, CREFSTRING connectionString)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetCapabilities_Id,    // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &providerName,          // Argument#1
                       MgCommand::knString, &connectionString,      // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates or updates a feature schema within the specified feature source.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to a feature source.
/// </param>
/// <param name="schema">Input
/// Input schema to be created or updated.
/// </param>
/// <returns>
/// Returns nothing.
/// </returns>
void MgProxyFeatureService::ApplySchema(MgResourceIdentifier* resource, MgFeatureSchema* schema)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knVoid,                           // Return type expected
                       MgFeatureServiceOpId::ApplySchema_Id,        // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knObject, schema,                 // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the definitions of one or more schemas contained in the
/// feature source for particular classes. If the specified schema name or
/// a class name does not exist, this method will throw an exception.
///
MgFeatureSchemaCollection* MgProxyFeatureService::DescribeSchema(MgResourceIdentifier* resource,
    CREFSTRING schemaName, MgStringCollection* classNames)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::DescribeSchema_Id,     // Command Code
                       3,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &schemaName,            // Argument#2
                       MgCommand::knObject, classNames,             // Argument#3
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgFeatureSchemaCollection*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////
/// This method has been deprecated. Use the above method.
///
MgFeatureSchemaCollection* MgProxyFeatureService::DescribeSchema(MgResourceIdentifier* resource,
    CREFSTRING schemaName)
{
    return DescribeSchema(resource, schemaName, NULL);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the definitions in XML format of one or more schemas contained in the
/// feature source for particular classes. If the specified schema name or
/// a class name does not exist, this method will throw an exception.
///
STRING MgProxyFeatureService::DescribeSchemaAsXml(MgResourceIdentifier* resource,
    CREFSTRING schemaName, MgStringCollection* classNames)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knString,                         // Return type expected
                       MgFeatureServiceOpId::DescribeSchemaAsXml_Id,// Command Code
                       3,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &schemaName,            // Argument#2
                       MgCommand::knObject, classNames,             // Argument#3
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    STRING retVal = *(cmd.GetReturnValue().val.m_str);
    delete cmd.GetReturnValue().val.m_str;

    return retVal;
}

///////////////////////////////////////////////////////////////////////////////
/// This method has been deprecated. Use the above method.
///
STRING MgProxyFeatureService::DescribeSchemaAsXml(MgResourceIdentifier* resource,
    CREFSTRING schemaName)
{
    return DescribeSchemaAsXml(resource, schemaName, NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns the Fdo schema information in XML format
/// for the MgFeatureSchemaCollection object provided
/// </summary>
/// <param name="schema">Input
/// A resource identifier referring to connection string
/// </param>
/// <returns>
/// STRING representing XML
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
STRING MgProxyFeatureService::SchemaToXml(MgFeatureSchemaCollection* schema)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knString,                         // Return type expected
                       MgFeatureServiceOpId::SchemaToXml_Id,        // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, schema,                 // Argument#1
                       MgCommand::knNone);                          // End of argument


    SetWarning(cmd.GetWarningObject());

    ///KNN: is this correct?
    STRING retVal = *(cmd.GetReturnValue().val.m_str);
    delete cmd.GetReturnValue().val.m_str;

    return retVal;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method creates MgFeatureSchemaCollection using
/// XML schema provided
/// </summary>
/// <param name="xml">Input
/// A string containg xml
/// </param>
/// <returns>
/// MgFeatureSchemaCollection
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
MgFeatureSchemaCollection* MgProxyFeatureService::XmlToSchema(CREFSTRING xml)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::XmlToSchema_Id,        // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &xml,                   // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgFeatureSchemaCollection*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Select features returns feature information from an Fdo Provider.  An MgFeatureReader
/// object provides one way, read-only iteration over the features.  The various properties
/// of a feature class are accessed by name using methods of the MgFeatureReader.
///
/// The desired feature class is specified by name. A subset of the feature properties
/// can be specified using the properties string collection.  Futhermore, an Fdo filter text
/// can be applied to limit the result set.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to feature source
/// </param>
/// <param name="className">Input
/// A valid class name on which select operation needs to be executed. It should not be NULL.
/// </param>
/// <param name="options">Input
/// MgFeatureQueryOptions instance containing all required filters for this select operation
/// </param>
/// <returns>
/// MgFeatureReader which operates on the instance of actual reader returned from the
/// FdoProvider.
/// </returns>
/// EXCEPTIONS:
/// MgFeatureSourceException
/// MgInvalidArgumentException
/// MgFdoException
/// NOTE:
/// 1. Please refer to Overview section of this document or Fdo provider documents
///    for details on connection properties, schema, classes, class proeprties and
///    filter text.
/// 2. Interrogation of class definition would allow to determine properties of classes
///    which can be used for filter text.
MgFeatureReader*  MgProxyFeatureService::SelectFeatures(MgResourceIdentifier* resource,
                                                        CREFSTRING className,
                                                        MgFeatureQueryOptions* options)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::SelectFeatures_Id,     // Command Code
                       3,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &className,             // Argument#2
                       MgCommand::knObject, options,                // Argument#3
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    Ptr<MgProxyFeatureReader> featReader = (MgProxyFeatureReader*)cmd.GetReturnValue().val.m_obj;

    if (featReader != NULL)
        featReader->SetService(this); // Feature reader on proxy side would store proxy service to call GetFeatures()

    return SAFE_ADDREF((MgProxyFeatureReader*)featReader);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Select features returns feature information from an Fdo Provider.  An MgFeatureReader
/// object provides one way, read-only iteration over the features.  The various properties
/// of a feature class are accessed by name using methods of the MgFeatureReader.
///
/// The desired feature class is specified by name. A subset of the feature properties
/// can be specified using the properties string collection.  Futhermore, an Fdo filter text
/// can be applied to limit the result set. The expected expected coordinate system name can
/// be specified.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to feature source
/// </param>
/// <param name="className">Input
/// A valid class name on which select operation needs to be executed. It should not be NULL.
/// </param>
/// <param name="options">Input
/// MgFeatureQueryOptions instance containing all required filters for this select operation
/// </param>
/// <param name="coordinateSystem">Input
/// A valid coordinate system name. Retrieved features will be in the specified coordinate system.
/// </param>
/// <returns>
/// MgFeatureReader which operates on the instance of actual reader returned from the
/// FdoProvider.
/// </returns>
/// EXCEPTIONS:
/// MgFeatureSourceException
/// MgInvalidArgumentException
/// MgFdoException
/// NOTE:
/// 1. Please refer to Overview section of this document or Fdo provider documents
///    for details on connection properties, schema, classes, class proeprties and
///    filter text.
/// 2. Interrogation of class definition would allow to determine properties of classes
///    which can be used for filter text.
MgFeatureReader*  MgProxyFeatureService::SelectFeatures(MgResourceIdentifier* resource,
                                                        CREFSTRING className,
                                                        MgFeatureQueryOptions* options,
                                                        CREFSTRING coordinateSystem)
{
    throw new MgNotImplementedException(
        L"MgProxyFeatureService::SelectFeatures",
        __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL; // to make some compiler happy
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This operation allows user to select features and then combine them into a group (means aggregate them).
/// Once the features are grouped together into a single set of data, they do not represent features, in other
/// words, they do not corresponds to underlying database schema. Therefore, this operation returns MgDataReader.
/// Computed properties applied in SelectFeatures would only be applicable per feature. For example, a polygon feature
/// can compute its area while retriveal. The area would be computed property for a specific feature instance.
/// In SelectAggregate operation, user might want to group all area for a particular zone and sum them to
/// find total area based on certain criteria. The total area of a particular zone does not represent a feature instance (row).
///
/// To find about aggregate functionalities available, please refer to GetCapabilities document returned by GetCapabilities
/// operation.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to feature source
/// </param>
/// <param name="className">Input
/// A valid class name on which select operation needs to be executed. It should not be NULL.
/// </param>
/// <param name="options">Input
/// MgFeatureAggregateOptions instance containing all required filters for this select operation
/// </param>
/// <returns>
/// MgDataReader pointer which operates on the instance of actual reader returned from the
/// FdoProvider ( OR NULL ).
/// </returns>
/// EXCEPTIONS:
/// MgFeatureSourceException
/// MgInvalidArgumentException
/// MgFdoException
///
/// NOTE:
/// 1. Please refer to Overview section of this document or Fdo provider documents
///    for details on connection properties, schema, classes, class proeprties,
///    filter text and spatial operations.
/// 2. Please refer to Geometry API documentation for Geometry details.
/// 3. Interrogation of provider capabilities will inform list of operations supported
/// 4. Interrogation of class definition would allow to determine properties of classes
///    which can be used for filter text.
MgDataReader* MgProxyFeatureService::SelectAggregate(MgResourceIdentifier* resource,
                                                     CREFSTRING className,
                                                     MgFeatureAggregateOptions* options)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::SelectAggregate_Id,    // Command Code
                       3,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &className,             // Argument#2
                       MgCommand::knObject, options,                // Argument#3
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    Ptr<MgProxyDataReader> dataReader = (MgProxyDataReader*)cmd.GetReturnValue().val.m_obj;
    if (dataReader != NULL)
        dataReader->SetService(this); // Data reader on proxy side would store proxy service to call GetFeatures()

    return SAFE_ADDREF((MgProxyDataReader*)dataReader);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// It executes all the commands specified in command collection.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="commands">Input
/// Collection of commands to be executed
/// </param>
/// <param name="useTransaction">Input
/// true  -  all commands will be executed in transaction
/// false -  not to use transaction
/// </param>
/// <returns>
/// Integer collection referring to result for each command
/// Index of commandCollection and index of IntCollection would match the result.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
MgPropertyCollection* MgProxyFeatureService::UpdateFeatures(MgResourceIdentifier* resource,
                                                            MgFeatureCommandCollection* commands,
                                                            bool useTransaction)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::UpdateFeatures_Id,     // Command Code
                       3,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knObject, commands,               // Argument#2
                       MgCommand::knInt8, (INT8)useTransaction,     // Argument#3
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    Ptr<MgPropertyCollection> propCol = (MgPropertyCollection*)cmd.GetReturnValue().val.m_obj;
    if (propCol != NULL)
    {
        INT32 cnt = propCol->GetCount();
        for (INT32 i=0; i < cnt; i++)
        {
            Ptr<MgProperty> prop = propCol->GetItem(i);
            INT32 propType = prop->GetPropertyType();
            if (propType == MgPropertyType::Feature)
            {
                MgFeatureProperty* featProp = ((MgFeatureProperty*)((MgProperty*)prop));
                Ptr<MgProxyFeatureReader> reader = (MgProxyFeatureReader*)featProp->GetValue();
                if (reader != NULL)
                {
                    reader->SetService(this);
                }
            }
        }
    }

    return SAFE_ADDREF((MgPropertyCollection*)propCol);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// It executes all the commands specified in command collection
/// within the given transaction.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="commands">Input
/// Collection of commands to be executed
/// </param>
/// <param name="transaction">Input
/// The MgTransaction instance on which the commands
/// will be executed.
/// </param>
/// <returns>
/// Integer collection referring to result for each command
/// Index of commandCollection and index of IntCollection would match the result.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
MgPropertyCollection* MgProxyFeatureService::UpdateFeatures(MgResourceIdentifier* resource,
                                                            MgFeatureCommandCollection* commands,
                                                            MgTransaction* transaction)
{
    STRING transactionId = L"";
    MgProxyFeatureTransaction* proxyTransaction = dynamic_cast<MgProxyFeatureTransaction*>(transaction);
    if (NULL != proxyTransaction)
    {
        transactionId = proxyTransaction->GetTransactionId();
    }

    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
        MgCommand::knObject,                                        // Return type expected
        MgFeatureServiceOpId::UpdateFeaturesWithTransaction_Id,     // Command Code
        3,                                                          // No of arguments
        Feature_Service,                                            // Service Id
        BUILD_VERSION(1,0,0),                                       // Operation version
        MgCommand::knObject, resource,                              // Argument#1
        MgCommand::knObject, commands,                              // Argument#2
        MgCommand::knString, &transactionId,                         // Argument#3
        MgCommand::knNone);                                         // End of argument

    SetWarning(cmd.GetWarningObject());

    Ptr<MgPropertyCollection> propCol = (MgPropertyCollection*)cmd.GetReturnValue().val.m_obj;
    if (propCol != NULL)
    {
        INT32 cnt = propCol->GetCount();
        for (INT32 i=0; i < cnt; i++)
        {
            Ptr<MgProperty> prop = propCol->GetItem(i);
            INT32 propType = prop->GetPropertyType();
            if (propType == MgPropertyType::Feature)
            {
                MgFeatureProperty* featProp = ((MgFeatureProperty*)((MgProperty*)prop));
                Ptr<MgProxyFeatureReader> reader = (MgProxyFeatureReader*)featProp->GetValue();
                if (reader != NULL)
                {
                    reader->SetService(this);
                }
            }
        }
    }

    return SAFE_ADDREF((MgPropertyCollection*)propCol);
}

////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the locked features
/// </summary>
///
/// <param name="resource">Input
/// A resource identifier for the feature source.
/// </param>
/// <param name="className">Input
/// The name of the feature class on which
/// the select operation is performed.
/// </param>
/// <param name="options">Input
/// An MgFeatureAggregateOptions instance
/// containing all the criteria and filters
/// required for this select operation.
/// <param>
/// <returns>
/// Returns an MgFeatureReader containing the locked features.
/// </returns>
///
/// EXCEPTIONS:
/// MgFeatureServiceException
/// MgInvalidArgumentException
MgFeatureReader* MgProxyFeatureService::GetLockedFeatures( MgResourceIdentifier* resource,
                                                CREFSTRING className,
                                                MgFeatureQueryOptions* options )
{
    throw new MgNotImplementedException(
        L"MgProxyFeatureService::GetLockedFeatures",
        __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL; // to make some compiler happy
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Starts a transaction on the specified feature source
///
/// NOTE:
/// The returned MgTransaction instance has to be used along with ExecuteSqlQuery(),
/// ExecuteSqlNonQuery() or UpdateFeatures() taking MgTransaction as a parameter.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <returns>
/// Returns an MgTransaction instance (or NULL).
/// </returns>
///
/// EXCEPTIONS:
/// MgFeatureServiceException
/// MgInvalidArgumentException
/// MgInvalidOperationException
/// MgFdoException
MgTransaction* MgProxyFeatureService::BeginTransaction( MgResourceIdentifier* resource )
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::BeginFeatureTransaction_Id,   // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    Ptr<MgProxyFeatureTransaction> featTransaction = (MgProxyFeatureTransaction*)cmd.GetReturnValue().val.m_obj;
    if (featTransaction != NULL)
        featTransaction->SetService(this); // Feature transaction on proxy side would store proxy service to call Commit() or Rollback()

    return SAFE_ADDREF((MgProxyFeatureTransaction*)featTransaction);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT SQL statement specified and returns a pointer to
/// SqlDataReader instance. This instance can be used to retrieve column information
/// and related values.
///
/// NOTE:
/// Serialize() method of SqlDataReader would be able to convert data returned
/// to AWKFF or XML stream.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="sqlStatement">Input
/// This would allow users to specify free format SQL SELECT statement like
/// SELECT * FROM CLASSNAME WHERE COLOR = RED. This would return all rows
/// from "CLASSNAME" where COLOR column has value RED.
/// </param>
/// <returns>
/// SqlDataReader pointer, an instance of reader pointing to the actual reader
/// from FdoProvider (or NULL).
///
/// If any statement other than SELECT is passed to this method, it would return failure.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// MgInvalidSqlStatement
/// MgSqlNotSupported
MgSqlDataReader* MgProxyFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                        CREFSTRING sqlStatement)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::ExecuteSqlQuery_Id,    // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &sqlStatement,          // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    Ptr<MgProxySqlDataReader> sqlReader = (MgProxySqlDataReader*)cmd.GetReturnValue().val.m_obj;
    if (sqlReader != NULL)
        sqlReader->SetService(this); // Feature reader on proxy side would store proxy service to call GetFeatures()

    return SAFE_ADDREF((MgProxySqlDataReader*)sqlReader);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT SQL statement specified within a given transaction
/// and returns a pointer to SqlDataReader instance. This instance can be used to
/// retrieve column information and related values.
///
/// NOTE:
/// Serialize() method of SqlDataReader would be able to convert data returned
/// to AWKFF or XML stream.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="sqlStatement">Input
/// This would allow users to specify free format SQL SELECT statement like
/// SELECT * FROM CLASSNAME WHERE COLOR = RED. This would return all rows
/// from "CLASSNAME" where COLOR column has value RED.
/// </param>
/// <param name="params">Input&Output
/// Parameters binded to the SQL statement.
/// </param>
/// <param name="transaction">Input
/// The transaction object on which the sql statement will be executed.
/// </param>
/// <returns>
/// SqlDataReader pointer, an instance of reader pointing to the actual reader
/// from FdoProvider (or NULL).
///
/// If any statement other than SELECT is passed to this method, it would return failure.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// MgInvalidSqlStatement
/// MgSqlNotSupported
MgSqlDataReader* MgProxyFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                        CREFSTRING sqlStatement,
                                                        MgParameterCollection* params,
                                                        MgTransaction* transaction )
{
    STRING transactionId = L"";
    MgProxyFeatureTransaction* proxyTransaction = dynamic_cast<MgProxyFeatureTransaction*>(transaction);
    if (NULL != proxyTransaction)
    {
        transactionId = proxyTransaction->GetTransactionId();
    }

    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::ExecuteSqlQueryWithTransaction_Id,    // Command Code
                       4,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &sqlStatement,          // Argument#2
                       MgCommand::knObject, params,                 // Argument#3
                       MgCommand::knString, &transactionId,         // Argument#4
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    Ptr<MgProxySqlDataReader> sqlReader = (MgProxySqlDataReader*)cmd.GetReturnValue().val.m_obj;
    if (sqlReader != NULL)
        sqlReader->SetService(this); // Feature reader on proxy side would store proxy service to call GetFeatures()

    return SAFE_ADDREF((MgProxySqlDataReader*)sqlReader);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT SQL statement specified within a given transaction
/// and returns a pointer to SqlDataReader instance. This instance can be used to
/// retrieve column information and related values.
///
/// NOTE:
/// Serialize() method of SqlDataReader would be able to convert data returned
/// to AWKFF or XML stream.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="sqlStatement">Input
/// This would allow users to specify free format SQL SELECT statement like
/// SELECT * FROM CLASSNAME WHERE COLOR = RED. This would return all rows
/// from "CLASSNAME" where COLOR column has value RED.
/// </param>
/// <param name="params">Input&Output
/// Parameters binded to the SQL statement.
/// </param>
/// <param name="transaction">Input
/// The transaction object on which the sql statement will be executed.
/// </param>
/// <param name="fetchSize">Input
/// The fetch size of query. This method returns all data of query if
/// setting the fetch size to 0.
/// </param>
/// <returns>
/// SqlDataReader pointer, an instance of reader pointing to the actual reader
/// from FdoProvider (or NULL).
///
/// If any statement other than SELECT is passed to this method, it would return failure.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// MgInvalidSqlStatement
/// MgSqlNotSupported
MgSqlDataReader* MgProxyFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                        CREFSTRING sqlStatement,
                                                        MgParameterCollection* params,
                                                        MgTransaction* transaction,
                                                        INT32 fetchSize)
{
    STRING transactionId = L"";
    MgProxyFeatureTransaction* proxyTransaction = dynamic_cast<MgProxyFeatureTransaction*>(transaction);
    if (NULL != proxyTransaction)
    {
        transactionId = proxyTransaction->GetTransactionId();
    }

    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::ExecuteSqlQueryWithTransaction_Id,    // Command Code
                       5,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &sqlStatement,          // Argument#2
                       MgCommand::knObject, params,                 // Argument#3
                       MgCommand::knString, &transactionId,         // Argument#4
                       MgCommand::knInt32, fetchSize,               // Argument#5
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    MgSqlResult * pResult = (MgSqlResult *)cmd.GetReturnValue().val.m_obj;
    if(params != NULL)
    {
        Ptr<MgParameterCollection> retParams = pResult->GetParameters();
        for(int i = 0; i < retParams->GetCount(); i++)
        {
            Ptr<MgParameter> para = params->GetItem(i);
            Ptr<MgParameter> newpara = retParams->GetItem(i);
            Ptr<MgNullableProperty> prop = newpara->GetProperty();
            para->SetProperty(prop);
        }
    }

    Ptr<MgProxySqlDataReader> sqlReader = (MgProxySqlDataReader *)pResult->GetReader();
    if (sqlReader != NULL)
        sqlReader->SetService(this); // Feature reader on proxy side would store proxy service to call GetFeatures()

    return SAFE_ADDREF((MgProxySqlDataReader*)sqlReader);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes all SQL statements supported by providers except SELECT.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="sqlNonSelectStatement">Input
/// This would allow users to specify free format SQL statement like INSERT/UPDATE/DELETE/CREATE
/// </param>
/// <returns>
/// An positive integer value indicating how many instances (rows) have been affected.
///
/// NOTE: It is possible that only few rows were affected than actually expected.
/// In this scenario, warning object will contain the details on the failed records.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
INT32 MgProxyFeatureService::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                                CREFSTRING sqlNonSelectStatement)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knInt32,                          // Return type expected
                       MgFeatureServiceOpId::ExecuteSqlNonQuery_Id, // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &sqlNonSelectStatement, // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return cmd.GetReturnValue().val.m_i32;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes all SQL statements supported by providers except SELECT
/// within a given transaction.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="sqlNonSelectStatement">Input
/// This would allow users to specify free format SQL statement like INSERT/UPDATE/DELETE/CREATE
/// </param>
/// <param name="params">Input&Output
/// Parameters binded to the SQL statement.
/// </param>
/// <param name="transaction">Input
/// The transaction object on which the sql statement will be executed.
/// </param>
/// <returns>
/// An positive integer value indicating how many instances (rows) have been affected.
///
/// NOTE: It is possible that only few rows were affected than actually expected.
/// In this scenario, warning object will contain the details on the failed records.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
INT32 MgProxyFeatureService::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                                CREFSTRING sqlNonSelectStatement,
                                                MgParameterCollection* params,
                                                MgTransaction* transaction)
{
    STRING transactionId = L"";
    MgProxyFeatureTransaction* proxyTransaction = dynamic_cast<MgProxyFeatureTransaction*>(transaction);
    if (NULL != proxyTransaction)
    {
        transactionId = proxyTransaction->GetTransactionId();
    }

    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
        MgCommand::knObject,                                         // Return type expected
        MgFeatureServiceOpId::ExecuteSqlNonQueryWithTransaction_Id, // Command Code
        4,                                                          // No of arguments
        Feature_Service,                                            // Service Id
        BUILD_VERSION(1,0,0),                                       // Operation version
        MgCommand::knObject, resource,                              // Argument#1
        MgCommand::knString, &sqlNonSelectStatement,                // Argument#2
        MgCommand::knObject, params,                                // Argument#3
        MgCommand::knString, &transactionId,                        // Argument#4
        MgCommand::knNone);                                         // End of argument

    SetWarning(cmd.GetWarningObject());

    MgSqlResult * pResult = (MgSqlResult *)cmd.GetReturnValue().val.m_obj;
    if(params != NULL)
    {
        Ptr<MgParameterCollection> retParams = pResult->GetParameters();
        for(int i = 0; i < retParams->GetCount(); i++)
        {
            Ptr<MgParameter> para = params->GetItem(i);
            Ptr<MgParameter> newpara = retParams->GetItem(i);
            Ptr<MgNullableProperty> prop = newpara->GetProperty();
            para->SetProperty(prop);
        }
    }

    return pResult->GetRowAffected();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT statement to return all available spatial context
/// for the provider. The information for spatial contexts will be retrieved via
/// SpatialContextReader instance returned.
/// NOTE:
/// Serialize() method of SpatialContextReader would be able to convert data returned from
/// SpatialContextReader to binary stream for transmission or XML.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="bActiveOnly">Input
/// This flag is obsolete and no longer used.
/// </param>
/// <returns>
/// SpatialContextReader pointer, a pointer to the actual instance reader returned from the
/// FdoProvider (or NULL).
///
/// This reader can be utilized to retrieve the following information
/// Name -             Spatial Context name
/// Description -      Brief information about spatial context
/// CoordinateSystem -
/// ExtentType -       Static/Dynamic
/// Extent -           AWKB/AWKT byte/text array representing the boundary
///                    of the extent.
/// XYTolerance -      Double
/// ZTolerance -       Double
/// IsActive           Boolean
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// NOTE:
/// Subject to change in FDO R2
MgSpatialContextReader* MgProxyFeatureService::GetSpatialContexts(MgResourceIdentifier* resource,
                                                                  bool bActiveOnly)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetSpatialContexts_Id, // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knInt8, (INT8)bActiveOnly,        // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgSpatialContextReader*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT statement to return all available Long transactions
/// for the provider. The information for long transactions will be retrieved via
/// MgLongTransactionReader instance returned.
/// NOTE:
/// Serialize() method of SpatialContextReader would be able to convert data returned from
/// SpatialContextReader to XML.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="bActiveOnly">Input
/// This flag would decide whether it should return all or active long transaction.
/// True - Only active spatial context
/// False - All spatial contexts
/// </param>
/// <returns>
/// MgLongTransactionReader pointer, a pointer to the actual instance reader returned from the
/// FdoProvider (or NULL).
///
/// This reader can be utilized to retrieve the following information
/// Name -             Spatial Context name
/// Description -      Brief information about spatial context
/// Owner       -      Owner of this long transaction
/// IsActive           Boolean
/// IsFrozen    -
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// NOTE:
/// Subject to change in FDO R2
MgLongTransactionReader* MgProxyFeatureService::GetLongTransactions(MgResourceIdentifier* resource, bool bActiveOnly)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetLongTransactions_Id,// Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knInt8, (INT8)bActiveOnly,        // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgLongTransactionReader*)cmd.GetReturnValue().val.m_obj;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Set the active long transaction name for a feature source.
///
/// The long transaction name is associated with the caller's session.  If
/// no session is set then the method throws an MgSessionNotFoundException.
/// </summary>
/// <param name="featureSourceId">Input
/// A resource identifier identifying a feature source in the repository.
/// </param>
/// <param name="longTransactionName">Input
/// The long transaction name to set.
/// </param>
/// <returns>
/// Returns true if the name was successfully set; otherwise
/// returns false.
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgInvalidResourceTypeException
/// MgSessionNotFoundException
bool MgProxyFeatureService::SetLongTransaction( MgResourceIdentifier* featureSourceId,
                                                CREFSTRING longTransactionName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knInt8,                           // Return type expected
                       MgFeatureServiceOpId::SetLongTransaction_Id, // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, featureSourceId,        // Argument#1
                       MgCommand::knString, &longTransactionName,   // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}


//////////////////////////////////////////////////////////////////
// Retrieves WFS schema information for the specified feature classes
MgByteReader* MgProxyFeatureService::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                                            MgStringCollection* featureClasses)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::DescribeWfsFeatureType_Id, // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, featureSourceId,        // Argument#1
                       MgCommand::knObject, featureClasses,         // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
// Retrieves feature information in WFS format, based on the specified criteria
MgByteReader* MgProxyFeatureService::GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                                   CREFSTRING featureClass,
                                                   MgStringCollection* requiredProperties,
                                                   CREFSTRING srs,
                                                   CREFSTRING filter,
                                                   INT32 maxFeatures)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetWfsFeature_Id,      // Command Code
                       6,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, featureSourceId,        // Argument#1
                       MgCommand::knString, &featureClass,          // Argument#2
                       MgCommand::knObject, requiredProperties,     // Argument#3
                       MgCommand::knString, &srs,                   // Argument#4
                       MgCommand::knString, &filter,                // Argument#5
                       MgCommand::knInt32,  maxFeatures,            // Argument#6
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgProxyFeatureService::GetFeatures(CREFSTRING featureReader)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetFeatures_Id,        // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &featureReader,         // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgBatchPropertyCollection*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
bool MgProxyFeatureService::CloseFeatureReader(CREFSTRING featureReader)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knInt8,                           // Return type expected
                       MgFeatureServiceOpId::CloseFeatureReader_Id, // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &featureReader,         // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}


//////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgProxyFeatureService::GetSqlRows(CREFSTRING sqlReader)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetSqlRows_Id,         // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &sqlReader,             // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgBatchPropertyCollection*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
bool MgProxyFeatureService::CloseSqlReader(CREFSTRING sqlReader)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knInt8,                           // Return type expected
                       MgFeatureServiceOpId::CloseSqlReader_Id,     // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &sqlReader,             // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}


//////////////////////////////////////////////////////////////////
MgStringCollection* MgProxyFeatureService::GetSchemas(MgResourceIdentifier* resource)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetSchemas_Id,         // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgStringCollection*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
MgStringCollection* MgProxyFeatureService::GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetClasses_Id,         // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &schemaName,            // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgStringCollection*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
MgClassDefinition* MgProxyFeatureService::GetClassDefinition(MgResourceIdentifier* resource,
                                                             CREFSTRING schemaName,
                                                             CREFSTRING className)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetClassDefinition_Id, // Command Code
                       3,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &schemaName,            // Argument#2
                       MgCommand::knString, &className,             // Argument#3
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgClassDefinition*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
MgByteReader* MgProxyFeatureService::GetRaster(CREFSTRING reader, INT32 xSize, INT32 ySize, STRING propName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetRaster_Id,          // Command Code
                       4,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &reader,                // Argument#1
                       MgCommand::knInt32, xSize,                   // Argument#2
                       MgCommand::knInt32, ySize,                   // Argument#3
                       MgCommand::knString, &propName,              // Argument#4
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgProxyFeatureService::GetDataRows(CREFSTRING dataReader)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetDataRows_Id,        // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &dataReader,            // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgBatchPropertyCollection*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
bool MgProxyFeatureService::CloseDataReader(CREFSTRING dataReader)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knInt8,                           // Return type expected
                       MgFeatureServiceOpId::CloseDataReader_Id,    // Command Code
                       1,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &dataReader,            // Argument#1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}


//////////////////////////////////////////////////////////////////
void MgProxyFeatureService::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knVoid,                           // Return type expected
                       MgFeatureServiceOpId::CreateFeatureSource_Id,// Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knObject, sourceParams,           // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());
}


//////////////////////////////////////////////////////////////////
// Returns the collection of identity properties for the specified class.
// If schemaName is empty, then className needs to be fully qualified.
MgClassDefinitionCollection* MgProxyFeatureService::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                             CREFSTRING schemaName,
                                                                             MgStringCollection* classNames)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetIdentityProperties_Id,  // Command Code
                       3,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(2,1,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &schemaName,            // Argument#2
                       MgCommand::knObject, classNames,             // Argument#3
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgClassDefinitionCollection*)cmd.GetReturnValue().val.m_obj;
}

MgByteReader* MgProxyFeatureService::EnumerateDataStores(CREFSTRING providerName,
                                                         CREFSTRING partialConnString)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::EnumerateDataStores_Id,    // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &providerName,          // Argument#1
                       MgCommand::knString, &partialConnString,     // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

MgByteReader* MgProxyFeatureService::GetSchemaMapping(CREFSTRING providerName,
                                                      CREFSTRING partialConnString)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetSchemaMapping_Id,   // Command Code
                       2,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knString, &providerName,          // Argument#1
                       MgCommand::knString, &partialConnString,     // Argument#2
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Sets the connection properties for the Proxy Service.  This
/// information tells the proxy object where to connect.
///
/// \param connProp
/// Connection properties for server
///
void MgProxyFeatureService::SetConnectionProperties(MgConnectionProperties* connProp)
{
    m_connProp = SAFE_ADDREF(connProp);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the FDO cache information.
/// </summary>
/// <returns>
/// The FDO cache information.
/// </returns>
STRING MgProxyFeatureService::GetFdoCacheInfo()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knString,                           // Return type expected
                       MgFeatureServiceOpId::GetFdoCacheInfo_Id,    // Command Code
                       0,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    STRING retVal = *(cmd.GetReturnValue().val.m_str);
    delete cmd.GetReturnValue().val.m_str;

    return retVal;
}

//////////////////////////////////////////////////////////////////
MgClassDefinition* MgProxyFeatureService::GetClassDefinition(MgResourceIdentifier* resource,
                                                             CREFSTRING schemaName,
                                                             CREFSTRING className,
                                                             bool serialize)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgFeatureServiceOpId::GetClassDefinition2_Id, // Command Code
                       4,                                           // No of arguments
                       Feature_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &schemaName,            // Argument#2
                       MgCommand::knString, &className,             // Argument#3
                       MgCommand::knInt8, (INT8)serialize,          // Argument#4
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (MgClassDefinition*)cmd.GetReturnValue().val.m_obj;
}

bool MgProxyFeatureService::CommitTransaction(CREFSTRING transactionId)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                        // Connection
                       MgCommand::knInt8,                                 // Return type expected
                       MgFeatureServiceOpId::CommitFeatureTransaction_Id, // Command Code
                       1,                                                 // No of arguments
                       Feature_Service,                                   // Service Id
                       BUILD_VERSION(1,0,0),                              // Operation version
                       MgCommand::knString, &transactionId,               // Argument#1
                       MgCommand::knNone);                                // End of argument

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}

bool MgProxyFeatureService::RollbackTransaction(CREFSTRING transactionId)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                          // Connection
                       MgCommand::knInt8,                                   // Return type expected
                       MgFeatureServiceOpId::RollbackFeatureTransaction_Id, // Command Code
                       1,                                                   // No of arguments
                       Feature_Service,                                     // Service Id
                       BUILD_VERSION(1,0,0),                                // Operation version
                       MgCommand::knString, &transactionId,                 // Argument#1
                       MgCommand::knNone);                                  // End of argument

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}
