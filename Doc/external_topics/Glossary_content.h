/**
\addtogroup content
<p>
  For a resource, the content is an XML file which defines all
  the attributes that MapGuide needs to use that resource.
  It may refer to other resources or to
  \link resource_data resource data \endlink.
</p>
<p>
  For a repository, the content is an XML file that can contain
  an optional description of the repository.
<TABLE class="RuledTable">
  <tr>
    <th>Item</th>
    <th>Schema For Content</th>
  </tr>
  <tr>
    <td>Repository</td>
    <td>RepositoryContent</td>
  </tr>
  <tr>
    <td>Drawing source resource</td>
    <td>DrawingSource</td>
  </tr>
  <tr>
    <td>Feature source resource</td>
    <td>FeatureSource</td>
  </tr>
  <tr>
    <td>Import rules resource</td>
    <td>[[?]]</td>
  </tr>
  <tr>
    <td>Layer definition resource</td>
    <td>LayerDefinition</td>
  </tr>
  <tr>
    <td>Load procedure resource</td>
    <td>LoadProcedure</td>
  </tr>
  <tr>
    <td>Map resource</td>
    <td>[[?]]</td>
  </tr>
  <tr>
    <td>Map definition resource</td>
    <td>MapDefinition</td>
  </tr>
  <tr>
    <td>Print layout resource</td>
    <td>PrintLayout</td>
  </tr>
  <tr>
    <td>Raster import rules resource</td>
    <td>[[?]]</td>
  </tr>
  <tr>
    <td>Symbol library resource</td>
    <td>SymbolLibrary</td>
  </tr>
  <tr>
    <td>Web layout resource</td>
    <td>WebLayout</td>
  </tr>
</TABLE>
<h3>See Also</h3>
<p>
<ul>
  <li>
    \link MgResourceService::SetResource SetResource \endlink
  </li>
  <li>
    \link MgResourceService::GetRepositoryContent GetRepositoryContent \endlink
  </li>
  <li>
    \link MgResourceService::GetResourceContent GetResourceContent \endlink
  </li>
  <li>
    \link MgResourceService::UpdateRepository UpdateRepository \endlink
  </li>
  <li>
    \link RepositoryContent_schema RepositoryContent \endlink schema</li>
    
</ul>

</p>
**/ 