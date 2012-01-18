#ifndef DESKTOP_RESOURCE_PACKAGE_HANDLER_H
#define DESKTOP_RESOURCE_PACKAGE_HANDLER_H

class MgResourcePackageHandler
{
/// Constructors/Destructor

public:

    explicit MgResourcePackageHandler(MgResourceService& repositoryManager);
    virtual ~MgResourcePackageHandler();

private:

    // Unimplemented Methods

    MgResourcePackageHandler();
    MgResourcePackageHandler(const MgResourcePackageHandler&);
    MgResourcePackageHandler& operator=(const MgResourcePackageHandler&);

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