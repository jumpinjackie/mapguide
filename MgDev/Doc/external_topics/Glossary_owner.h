/**
\addtogroup owner
<p>
  By default, the <i>owner</i> of a resource is the user who
  created the resource. The owner has complete control over
  permissions on a resource, regardless of permission settings.
</p>
<p>
  Only the owner of a resource (and the Administrator) can:
</p>
<ol>
  <li>Transfer his/her ownership of a resource to another user
    (via
    \link MgResourceService::ChangeResourceOwner ChangeResourceOwner \endlink
    ). </li>
  <li>Assign the permissions on his/her owned resource to other
    users (via the
    \link MgResourceService::InheritPermissionsFrom InheritPermissionsFrom \endlink
    or
    \link MgResourceService::SetResource SetResource \endlink
    ). </li>
</ol>
<p>
  You can use
  \link MgResourceService::EnumerateResources EnumerateResources \endlink
  to determine the current owner of a resource.
</p>
**/ 