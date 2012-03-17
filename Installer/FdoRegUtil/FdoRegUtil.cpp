// FdoRegUtil.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// usage: FdoRegUtil.exe [-r] <provider dll>
//        FdoRegUtil.exe [-r] <provider name> <display name> <provider description> <version> <fdo version> <provider dll path>
//        FdoRegUtil.exe [-u] <provider name>

int _tmain(int argc, _TCHAR* argv[])
{
	int ret = 0;
    if (argc == 8)
    {
        if (wcscmp(argv[1], L"-r") != 0)
            Usage();
        ret = Register(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    }
	else if (argc == 3)
	{
		if(wcscmp(argv[1], L"-u") == 0)
			ret = Unregister(argv[2]);
        else if(wcscmp(argv[1], L"-r") == 0)
			ret = Register(argv[2]);
        else
            Usage();
	}
	return ret;
}

void Usage()
{
	printf("usage\n");
	printf("registering (method 1): FdoRegUtil.exe -r <provider dll path>\n");
    printf("registering (method 2): FdoRegUtil.exe -r <provider name> <display name> <provider description> <version> <fdo version> <provider dll path>\n");
	printf("unregistering: FdoRegUtil.exe -u <provider name>\n");
}

int Register(_TCHAR* path)
{
	int ret = 0;
	FdoPtr<IProviderRegistry> provReg = FdoFeatureAccessManager::GetProviderRegistry();
    FdoPtr<IConnectionManager> connMgr = FdoFeatureAccessManager::GetConnectionManager();
	try 
	{
        //As of FDO 3.6 this supports FDO dll paths
        FdoPtr<FdoIConnection> conn = connMgr->CreateConnection(path);
        FdoPtr<FdoIConnectionInfo> connInfo = conn->GetConnectionInfo();

        //FDO connections already have this information!
        provReg->RegisterProvider(connInfo->GetProviderName(),
                                  connInfo->GetProviderDisplayName(),
                                  connInfo->GetProviderDescription(),
                                  connInfo->GetProviderVersion(),
                                  connInfo->GetFeatureDataObjectsVersion(),
                                  path,
                                  false);
	}
	catch(FdoException* ex)
	{
		ex->Release();
		ret = -1;
	}
	return ret;
}

int Register(_TCHAR* name, _TCHAR* displayName, _TCHAR* description, _TCHAR* version, _TCHAR* fdoVersion, _TCHAR* path)
{
    int ret = 0;
    FdoPtr<IProviderRegistry> prvReg = FdoFeatureAccessManager::GetProviderRegistry();
    try 
    {
        prvReg->RegisterProvider(name, displayName, description, version, fdoVersion, path, false);
    }
    catch(FdoException* ex)
    {
        ex->Release();
        ret = -1;
    }
    return ret;
}

int Unregister(_TCHAR* name)
{
	int ret = 0;
	FdoPtr<IProviderRegistry> prvReg = FdoFeatureAccessManager::GetProviderRegistry();
	try 
	{
		prvReg->UnregisterProvider(name);
	}
	catch(FdoException* ex)
	{
		ex->Release();
		ret = -1;
	}
	return ret;
}