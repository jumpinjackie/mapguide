/**
\addtogroup resource_data
<p>
  Data used by a resource but that is not stored in the
  resource itself. For example, a binary SDF file is stored
  separately from the XML used to store the SDF resource
  itself.
</p>
<p>
  Resource data can be stored as <i>files</i>, <i>streams</i>,
  or <i>strings</i>.
</p>
<ul>
  <li> Files are used when the data is large. For example, SDF
    files.</li>
  <li>Streams are used for faster access for smaller pieces of
    binary data. For example, symbols.</li>
  <li>Strings are used for small pieces of text data. For
    example, database access credentials.</li>
</ul>
<h3>See Also</h3>
<ul>
  <li>
    \link Resource_Service_Module Resource Service \endlink
  </li>
</ul>

**/ 