#ifndef DESKTOP_PLATFORM_INIT_H
#define DESKTOP_PLATFORM_INIT_H

class MG_DESKTOP_API MgPlatform
{
PUBLISHED_API:
    static bool IsInitialized();
    static void Initialize(CREFSTRING configFile);
    static void Terminate();

private:
    static bool sm_init;
};

#endif