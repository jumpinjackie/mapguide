// FdoRegUtil.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// usage: FdoRegUtil.exe [/r|/u] <provider name> <display name> <provider description> <version> <fdo version> <provider dll path>

int _tmain(int argc, _TCHAR* argv[])
{
	int ret = 0;
	if (argc == 3)
	{
		if(wcscmp(argv[1], L"-u") != 0)
			goto usage;

		ret = Unregister(argv[2]);
		goto exit;
	}
	else if (argc == 8)
	{
		if(wcscmp(argv[1], L"-r") != 0)
			goto usage;

		ret = Register(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
		goto exit;
	}
usage:
	Usage();
exit:
	return ret;
}

void Usage()
{
	printf("usage\n");
	printf("registering: FdoRegUtil.exe -r <provider name> <display name> <provider description> <version> <fdo version> <provider dll path>\n");
	printf("unregistering: FdoRegUtil.exe -u <provider name>\n");
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