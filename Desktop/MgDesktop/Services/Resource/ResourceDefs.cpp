#include "ResourceDefs.h"

///////////////////////////////////////////////////////////////////////////////
/// Package API names
///
const STRING MgPackageApiName::LoadPackage  = L"LoadPackage";
const STRING MgPackageApiName::MakePackage  = L"MakePackage";


///////////////////////////////////////////////////////////////////////////////
/// Status codes for packages
///
const STRING MgPackageStatusCode::Succeeded = L"Succeeded";
const STRING MgPackageStatusCode::Failed    = L"Failed";
const STRING MgPackageStatusCode::InProgress= L"InProgress";
const STRING MgPackageStatusCode::NotStarted= L"NotStarted";
const STRING MgPackageStatusCode::Unknown   = L"Unknown";