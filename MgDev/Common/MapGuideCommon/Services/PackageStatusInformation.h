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


#ifndef MGPACKAGESTATUSINFORMATION_H_
#define MGPACKAGESTATUSINFORMATION_H_

class MgPackageStatusInformation;
template class MG_MAPGUIDE_API Ptr<MgPackageStatusInformation>;

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// This class manages resource package status information.
///
class MG_MAPGUIDE_API MgPackageStatusInformation : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgPackageStatusInformation)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs the object.
    ///
    MgPackageStatusInformation();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the object.
    ///
    virtual ~MgPackageStatusInformation();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the package status message.
    ///
    /// \return
    /// The package status message.
    ///
    STRING GetStatusMessage();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the status code of the package.
    ///
    /// \return
    /// The package status code
    ///
    STRING GetStatusCode();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the status code of the package.
    ///
    /// \param statusCode
    /// Status code of the package.
    ///
    /// \return
    /// Nothing.
    ///
    void SetStatusCode(CREFSTRING statusCode);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the package API.
    ///
    /// \return
    /// The package API name.
    ///
    STRING GetApiName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the package API.
    ///
    /// \param apiName
    /// Name of the package API.
    ///
    /// \return
    /// Nothing.
    ///
    void SetApiName(CREFSTRING apiName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the package.
    ///
    /// \return
    /// The package name.
    ///
    STRING GetPackageName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the package.
    ///
    /// \param packageName
    /// Name of the package.
    ///
    /// \return
    /// Nothing.
    ///
    void SetPackageName(CREFSTRING packageName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the description of the package.
    ///
    /// \return
    /// The package description.
    ///
    STRING GetPackageDescription();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the description of the package.
    ///
    /// \param packageDescription
    /// Description of the package.
    ///
    /// \return
    /// Nothing.
    ///
    void SetPackageDescription(CREFSTRING packageDescription);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the modification date of the package.
    ///
    /// \return
    /// The package's modification date.
    ///
    MgDateTime* GetPackageDate();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the modification date of the package.
    ///
    /// \param packageDate
    /// Modification date of the package.
    ///
    /// \return
    /// Nothing.
    ///
    void SetPackageDate(MgDateTime* packageDate);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the size (in bytes) of the package.
    ///
    /// \return
    /// The package size.
    ///
    INT64 GetPackageSize();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the size (in bytes) of the package.
    ///
    /// \param packageSize
    /// Size of the package.
    ///
    /// \return
    /// Nothing.
    ///
    void SetPackageSize(INT64 packageSize);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the user who performed the package API.
    ///
    /// \return
    /// The user name.
    ///
    STRING GetUserName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the user who performed the package API.
    ///
    /// \param userName
    /// Name of the user.
    ///
    /// \return
    /// Nothing.
    ///
    void SetUserName(CREFSTRING userName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the Site server on which the package API is performed.
    ///
    /// \return
    /// The server name.
    ///
    STRING GetServerName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the Site server on which the package API is performed.
    ///
    /// \param serverName
    /// Name of the server.
    ///
    /// \return
    /// Nothing.
    ///
    void SetServerName(CREFSTRING serverName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the IP address of the Site server on which the package API is performed.
    ///
    /// \return
    /// The server name.
    ///
    STRING GetServerAddress();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the IP address of the Site server on which the package API is performed.
    ///
    /// \param serverAddress
    /// IP address of the server.
    ///
    /// \return
    /// Nothing.
    ///
    void SetServerAddress(CREFSTRING serverAddress);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the start time of the package.
    ///
    /// \return
    /// The package's start time.
    ///
    MgDateTime* GetStartTime();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the start time of the package.
    ///
    /// \param startTime
    /// Start time of the package.
    ///
    /// \return
    /// Nothing.
    ///
    void SetStartTime(MgDateTime* startTime);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the end time of the package.
    ///
    /// \return
    /// The package's end time.
    ///
    MgDateTime* GetEndTime();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the end time of the package.
    ///
    /// \param endTime
    /// End time of the package.
    ///
    /// \return
    /// Nothing.
    ///
    void SetEndTime(MgDateTime* endTime);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of failed operations encountered by the package API.
    ///
    /// \return
    /// The number of failed operations.
    ///
    INT32 GetOperationsFailed();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the number of failed operations encountered by the package API.
    ///
    /// \param opsFailed
    /// Number of failed operations.
    ///
    /// \return
    /// Nothing.
    ///
    void SetOperationsFailed(INT32 opsFailed);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of operations received from the package API.
    ///
    /// \return
    /// The total number of operations.
    ///
    INT32 GetOperationsReceived();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the number of operations received by the package API.
    ///
    /// \param opsReceived
    /// Total number of operations.
    ///
    /// \return
    /// Nothing.
    ///
    void SetOperationsReceived(INT32 opsReceived);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the average time (in milliseconds) per operation.
    ///
    /// \return
    /// The average operation time.
    ///
    double GetAverageOperationTime();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the error message.
    ///
    /// \return
    /// The error message.
    ///
    STRING GetErrorMessage();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the error message.
    ///
    /// \param errorMessage
    /// Error message.
    ///
    /// \return
    /// Nothing.
    ///
    void SetErrorMessage(CREFSTRING errorMessage);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the stack trace.
    ///
    /// \return
    /// The stack trace.
    ///
    STRING GetStackTrace();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the stack trace.
    ///
    /// \param stackTrace
    /// Stack trace.
    ///
    /// \return
    /// Nothing.
    ///
    void SetStackTrace(CREFSTRING stackTrace);

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Copy constructor.
    ///
    MgPackageStatusInformation(const MgPackageStatusInformation& packageStatusInfo);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Assignment operator.
    ///
    MgPackageStatusInformation& operator=(const MgPackageStatusInformation& packageStatusInfo);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to TCP/IP stream.
    ///
    /// \param stream
    /// Stream.
    ///
    virtual void Serialize(MgStream* stream);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from TCP/IP stream.
    ///
    /// \param stream
    /// Stream.
    ///
    virtual void Deserialize(MgStream* stream);

protected:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes the object.
    ///
    virtual void Dispose();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the class ID.
    ///
    /// \return
    /// Class ID.
    ///
    virtual INT32 GetClassId();

private:

    STRING m_apiName;
    STRING m_statusCode;

    STRING m_packageName;
    STRING m_packageDescription;
    Ptr<MgDateTime> m_packageDate;
    INT64 m_packageSize;

    STRING m_userName;
    STRING m_serverName;
    STRING m_serverAddress;

    Ptr<MgDateTime> m_startTime;
    Ptr<MgDateTime> m_endTime;

    INT32 m_opsFailed;
    INT32 m_opsReceived;
    STRING m_errorMessage;
    STRING m_stackTrace;

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Service_PackageStatusInformation;
};
/// \endcond

#endif
