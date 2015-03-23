#ifndef DESKTOP_OPERATION_INFO_H
#define DESKTOP_OPERATION_INFO_H

#include "OperationParameter.h"

typedef std::map<STRING, MgdOperationParameter> MgOpParamMap;

class MgdOperationName
{
INTERNAL_API:
//    static const STRING CreateRepository;
//    static const STRING DeleteRepository;
    static const STRING UpdateRepository;
//    static const STRING ApplyResourcePackage;

    static const STRING SetResource;
    static const STRING DeleteResource;
    static const STRING MoveResource;
    static const STRING CopyResource;
    static const STRING ChangeResourceOwner;
    static const STRING InheritPermissionsFrom;

    static const STRING SetResourceData;
    static const STRING DeleteResourceData;
    static const STRING RenameResourceData;
};


class MgOperationInfo
{
/// Constructors/Destructor

public:
    MgOperationInfo();
    explicit MgOperationInfo(CREFSTRING name);
    MgOperationInfo(const MgOperationInfo& opInfo);
    virtual ~MgOperationInfo();

/// Methods

public:
    MgOperationInfo& operator=(const MgOperationInfo& opInfo);

    STRING GetName() const;
    void SetName(CREFSTRING name);

    STRING GetVersion() const;
    void SetVersion(CREFSTRING version);

    const MgdOperationParameter& GetParameter(CREFSTRING name,
        bool required = true) const;
    void AddParameter(CREFSTRING name, const MgdOperationParameter& opParam);
    void RemoveParameter(CREFSTRING name);

    const MgOpParamMap& GetParameters() const;

/// Data Members

public:
    static const STRING sm_currentVersion;
    static const MgdOperationParameter sm_blankParameter;

private:
    STRING m_name;
    STRING m_version;
    MgOpParamMap m_parameters;
};


/// Inline Methods

inline STRING MgOperationInfo::GetName() const
{
    return m_name;
}


inline STRING MgOperationInfo::GetVersion() const
{
    return m_version;
}


inline const MgOpParamMap& MgOperationInfo::GetParameters() const
{
    return m_parameters;
}

#endif