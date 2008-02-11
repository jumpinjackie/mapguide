/**
\addtogroup Resource_Service_Module
<p>
	The Resource Service enables you to manipulate repositories
	and resources.
</p>
<p>
	A resource repository stores all the files and configuration
	information used by MapGuide. There are two types of
	repository. The <i>library</i> is used for persistent
	storage. There may also be <i>session</i> repositories. These hold data only for the user's current
	session, and are automatically deleted by the 
	MapGuide server if they have not been used for a certain
	length of time. MapGuide creates session
	repositories automatically as required. You can also create
	your own session repositories by using MgSite::CreateSession().
</p>
<p>
	A repository is structured like a directory, with documents,
	folders, and subfolders. Each resource, and any associated
	files required for that resource, is stored in one folder.
</p>
<p>
	The root level of each repository, each folder in a
	repository, and each resource in a repository, has a <i>header</i> that specifies the permissions to access it. These
	permissions control whether users and groups can read and/or
	write to a resource or folder. Permissions can either be set
	explicitly for each folder or resource, or they can be set to <i>inherit</i> the permissions from the parent folder.
</p>
<p>
	Resources may depend on other items:
</p>
<ul>
	<li>Some resources, such as print layouts, are self
		sufficient and do not refer to any other resources or files.</li>
	<li>Some resources <i>reference</i> other resources. For
		example, maps and layers are stored as separate resources,
		and the map refers to the layers that are included in the
		map.</li>
	<li>Some resources use associated <i>resource data</i>. For
		example, an ArcSDE feature source uses a file which holds
		the database credentials. Resource data can be stored as
		files, streams, or strings.
		<ul>
			<li>Files are used when the data is large, such as SHP files</li>
			<li>Streams are used for faster access to smaller pieces of binary data, such as symbols.</li>
			<li>Strings are used for small pieces of text data, such as database access credentials.</li>
		</ul>
	</li>
</ul>
\image html resources_and_data.gif
<h3> Methods For Working With Repositories </h3>
<ul>
	<li>MgResourceService::GetRepositoryContent</li>
	<li>MgResourceService::GetRepositoryHeader</li>
	<li>MgResourceService::UpdateRepository</li>
</ul>
<h3> Methods For Working With Resources </h3>
<ul>
	<li>MgResourceService::ChangeResourceOwner</li>
	<li>MgResourceService::CopyResource</li>
	<li>MgResourceService::DeleteResource</li>
	<li>MgResourceService::EnumerateReferences</li>
	<li>MgResourceService::EnumerateResources</li>
	<li>MgResourceService::EnumerateUnmanagedData</li>
	<li>MgResourceService::GetResourceContent</li>
	<li>MgResourceService::GetResourceHeader</li>
	<li>MgResourceService::GetResourceMetadata</li>
	<li>MgResourceService::InheritPermissionsFrom</li>
	<li>MgResourceService::MoveResource</li>
	<li>MgResourceService::ResourceExists</li>
	<li>MgResourceService::SetResource</li>
	<li>MgResourceService::SetResourceMetadata </li>
</ul>
<h3> Methods For Working With Resource Data </h3>
<ul>
	<li>MgResourceService::DeleteResourceData</li>
	<li>MgResourceService::EnumerateResourceData</li>
	<li>MgResourceService::GetResourceData</li>
	<li>MgResourceService::RenameResourceData </li>
	<li>MgResourceService::SetResourceData</li>
</ul>
<h3> Methods For Working With Resource Packages </h3>
<ul>
	<li>MgResourceService::ApplyResourcePackage</li>
</ul>
**/
