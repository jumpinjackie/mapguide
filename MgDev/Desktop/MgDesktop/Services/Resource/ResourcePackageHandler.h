#ifndef DESKTOP_RESOURCE_PACKAGE_HANDLER_H
#define DESKTOP_RESOURCE_PACKAGE_HANDLER_H

class MgdResourcePackageHandler
{
/// Constructors/Destructor

public:

    explicit MgdResourcePackageHandler(MgResourceService& repositoryManager);
    virtual ~MgdResourcePackageHandler();

private:

    // Unimplemented Methods

    MgdResourcePackageHandler();
    MgdResourcePackageHandler(const MgdResourcePackageHandler&);
    MgdResourcePackageHandler& operator=(const MgdResourcePackageHandler&);

/// Methods

public:

    virtual void End(MgException* except) = 0;

protected:

    void InitializeStatus(CREFSTRING packageApiName,
        CREFSTRING packagePathname, bool logActivities);
    void UpdateStatus(MgException* except);

/// Data Members

protected:

    MgResourceService& m_repositoryManager;
    //Ptr<MgPackageLogWriter> m_packageLogWriter;

    STRING m_packagePathname;

    INT32 m_opsSucceeded;
    INT32 m_opsReceived;
};

/// Inline Methods

#endif