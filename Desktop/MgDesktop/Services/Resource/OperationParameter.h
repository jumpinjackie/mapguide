#ifndef DESKTOP_OPERATION_PARAMETER_H
#define DESKTOP_OPERATION_PARAMETER_H

class MgOperationParameter
{
/// Constructors/Destructor

public:
    MgOperationParameter();
    MgOperationParameter(const MgOperationParameter& opParam);
    virtual ~MgOperationParameter();

/// Methods

public:
    MgOperationParameter& operator=(const MgOperationParameter& opParam);

    STRING GetValue() const;
    void SetValue(CREFSTRING value);

    STRING GetContentType() const;
    void SetContentType(CREFSTRING contentType);

/// Data Members

INTERNAL_API:
    static const STRING Type;
    static const STRING Depth;
    static const STRING ResourceId;
    static const STRING ResourceContent;
    static const STRING ResourceHeader;
    static const STRING SourceResourceId;
    static const STRING DestinationResourceId;
    static const STRING DataName;
    static const STRING DataType;
    static const STRING DataLength;
    static const STRING Data;
    static const STRING OldDataName;
    static const STRING NewDataName;
    static const STRING Overwrite;
    static const STRING Package;
    static const STRING Owner;
    static const STRING IncludeDescendants;

private:
    STRING m_value;
    STRING m_contentType;
};


/// Inline Methods

inline STRING MgOperationParameter::GetValue() const
{
    return m_value;
}


inline STRING MgOperationParameter::GetContentType() const
{
    return m_contentType;
}

#endif