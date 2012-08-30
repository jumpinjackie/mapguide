#include "ResourceDefs.h"

///////////////////////////////////////////////////////////////////////////////
/// Package API names
///
const STRING MgdPackageApiName::LoadPackage  = L"LoadPackage";
const STRING MgdPackageApiName::MakePackage  = L"MakePackage";


///////////////////////////////////////////////////////////////////////////////
/// Status codes for packages
///
const STRING MgdPackageStatusCode::Succeeded = L"Succeeded";
const STRING MgdPackageStatusCode::Failed    = L"Failed";
const STRING MgdPackageStatusCode::InProgress= L"InProgress";
const STRING MgdPackageStatusCode::NotStarted= L"NotStarted";
const STRING MgdPackageStatusCode::Unknown   = L"Unknown";