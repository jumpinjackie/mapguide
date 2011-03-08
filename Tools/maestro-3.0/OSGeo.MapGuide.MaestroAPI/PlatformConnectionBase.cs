#region Disclaimer / License
// Copyright (C) 2009, Kenneth Skovhede
// http://www.hexad.dk, opensource@hexad.dk
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
// 
#endregion
using System;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using System.Text;
using System.IO;

using ObjCommon = OSGeo.MapGuide.ObjectModels.Common;
using OSGeo.MapGuide.ObjectModels.Capabilities;
using OSGeo.MapGuide.MaestroAPI.Resource;
using OSGeo.MapGuide.MaestroAPI.Commands;
using OSGeo.MapGuide.MaestroAPI.Mapping;
using OSGeo.MapGuide.MaestroAPI.Serialization;
using OSGeo.MapGuide.MaestroAPI.CoordinateSystem;
using OSGeo.MapGuide.ObjectModels.MapDefinition;
using OSGeo.MapGuide.ObjectModels.Common;
using System.Collections.Specialized;
using OSGeo.MapGuide.MaestroAPI.Schema;
using OSGeo.MapGuide.MaestroAPI.Feature;

namespace OSGeo.MapGuide.MaestroAPI
{
	/// <summary>
	/// Base class of all connection classes. Covers functionality encompassed by
    /// the MapGuide Geospatial Platform API (ie. Feature Service and Resource Service)
	/// </summary>
	public abstract class PlatformConnectionBase
	{
		/// <summary>
		/// A list of cached serializers
		/// </summary>
		protected Hashtable m_serializers;

		/// <summary>
		/// The current XML validator
		/// </summary>
		protected XMLValidator m_validator;

		/// <summary>
		/// The path of Xsd schemas 
		/// </summary>
		protected string m_schemasPath;

		/// <summary>
		/// A lookup table for Xsd Schemas
		/// </summary>
		protected Hashtable m_cachedSchemas;

		/// <summary>
		/// A flag indicating if Xsd validation is perfomed
		/// </summary>
		protected bool m_disableValidation = false;

        /// <summary>
        /// Initializes a new instance of the <see cref="PlatformConnectionBase"/> class.
        /// </summary>
		protected PlatformConnectionBase()
		{
            ResourceTypeRegistry.Init();
            
            m_serializers = new Hashtable();
            m_validator = new XMLValidator();
            m_cachedSchemas = new Hashtable();
            m_schemasPath = System.IO.Path.Combine(System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location), "Schemas");
		}

        #region Serialization plumbing

        /// <summary>
        /// Deserializes an object from a stream.
        /// </summary>
        /// <typeparam name="T">The expected object type</typeparam>
        /// <param name="data">The stream containing the object</param>
        /// <returns>The deserialized object</returns>
        virtual public T DeserializeObject<T>(System.IO.Stream data)
        {
            return (T)DeserializeObject(typeof(T), data);
        }

		/// <summary>
		/// Deserializes an object from a stream.
		/// </summary>
		/// <param name="type">The expected object type</param>
		/// <param name="data">The stream containing the object</param>
		/// <returns>The deserialized object</returns>
		virtual public object DeserializeObject(Type type, System.IO.Stream data)
		{
            //HACK: MGOS 2.2 outputs different capabilities xml (because it's actually the correct one!), so 
            //without breaking support against 2.1 and older servers, we transform the xml to its pre-2.2 form
            if (type == typeof(FdoProviderCapabilities) && this.SiteVersion < new Version(2, 2))
            {
                StringBuilder sb = null;
                using (StreamReader reader = new StreamReader(data))
                {
                    sb = new StringBuilder(reader.ReadToEnd());
                }

                //Pre-2.2 the elements were suffixed with Collection, change the suffix to List

                sb.Replace("<FunctionDefinitionCollection>", "<FunctionDefinitionList>");
                sb.Replace("</FunctionDefinitionCollection>", "</FunctionDefinitionList>");
                sb.Replace("<FunctionDefinitionCollection/>", "<FunctionDefinitionList/>");
                sb.Replace("<ArgumentDefinitionCollection>", "<ArgumentDefinitionList>");
                sb.Replace("</ArgumentDefinitionCollection>", "</ArgumentDefinitionList>");
                sb.Replace("<ArgumentDefinitionCollection/>", "<ArgumentDefinitionList/>");

                byte[] bytes = Encoding.UTF8.GetBytes(sb.ToString());

                //Replace the original input stream
                data = new MemoryStream(bytes);
            }

			//Must copy stream, because we will be reading it twice :(
			//Once for validation, and once for deserialization
			System.IO.MemoryStream ms = new System.IO.MemoryStream();
			Utility.CopyStream(data, ms);
			ms.Position = 0;

#if DEBUG_LASTMESSAGE
			//Save us a copy for later investigation
			using (System.IO.FileStream fs = System.IO.File.Open("lastResponse.xml", System.IO.FileMode.Create, System.IO.FileAccess.ReadWrite, System.IO.FileShare.None))
				Utility.CopyStream(ms, fs);

			ms.Position = 0;
#endif
			//TODO: Find out why the "xs:include" doesn't work with validator
			//Validation is quite important, as we otherwise may end up injecting malicious code
			//			if (!m_disableValidation)
			//			{
			//				m_validator.Validate(ms, GetSchema(type));
			//				ms.Position = 0;
			//			}

			try
			{
				return GetSerializer(type).Deserialize(ms);
			}
			catch (Exception ex)
			{
				string s = ex.Message;
				throw;
			}
		}

		/// <summary>
		/// Serialize an object into a new memory stream.
		/// </summary>
		/// <param name="o">The object to serialize</param>
		/// <returns>A memorystream with the serialized object</returns>
		virtual public System.IO.MemoryStream SerializeObject(object o)
		{
			System.IO.MemoryStream ms = new System.IO.MemoryStream();
            GetSerializer(o.GetType()).Serialize(new Utf8XmlWriter(ms), o);
            return Utility.RemoveUTF8BOM(ms);
		}

		/// <summary>
		/// Serializes an object into a stream
		/// </summary>
		/// <param name="o">The object to serialize</param>
		/// <param name="stream">The stream to serialize into</param>
		virtual public void SerializeObject(object o, System.IO.Stream stream)
		{
			//The Utf8 writer makes sure the Utf8 tag is in place + sets encoding to Utf8
			//This is needed because the server fails when rendering maps using non utf8 xml documents
			//And the XmlSerializer sytem in .Net does not have a method to set the encoding attribute

            //This does not remove the utf8 BOM marker :(
            //GetSerializer(o.GetType()).Serialize(new Utf8XmlWriter(stream), o);

            SerializeObject(o).WriteTo(stream);
		}

		/// <summary>
		/// Returns an XmlSerializer for the given type
		/// </summary>
		/// <param name="type">The object type to serialize</param>
		/// <returns>An XmlSerializer for the given type</returns>
		virtual protected System.Xml.Serialization.XmlSerializer GetSerializer(Type type)
		{
			if (m_serializers[type] == null)
				m_serializers[type] = new System.Xml.Serialization.XmlSerializer(type);
			return (System.Xml.Serialization.XmlSerializer)m_serializers[type];
        }

        #endregion

        #region Validation

        /// <summary>
        /// Gets or sets a flag that indicates if the Xml resources are validated before leaving and entering the server.
        /// </summary>
        public bool DisableValidation
        {
            get { return m_disableValidation; }
            set { m_disableValidation = value; }
        }

        /// <summary>
		/// Validates the current server version against the highest tested version.
		/// </summary>
		/// <param name="version">The version to validate</param>
        virtual protected void ValidateVersion(ObjCommon.SiteVersion version)
		{
			ValidateVersion(new Version(version.Version));
		}

		/// <summary>
		/// Validates the current server version against the highest tested version.
		/// </summary>
		/// <param name="version">The version to validate</param>
		virtual protected void ValidateVersion(Version version)
		{
			if (version > this.MaxTestedVersion)
				throw new Exception("Untested with MapGuide Build > " + this.MaxTestedVersion.ToString());
        }

        #endregion

        /// <summary>
        /// Gets the name of the provider of this implementation
        /// </summary>
        public abstract string ProviderName { get; }

        /// <summary>
        /// Gets a collection of name-value parameters required to create another copy
        /// of this connection via the <see cref="T:OSGeo.MapGuide.MaestroAPI.ConnectionProviderRegistry"/>
        /// </summary>
        /// <returns></returns>
        public abstract NameValueCollection CloneParameters { get; }

        /// <summary>
        /// Gets the current SessionID.
        /// </summary>
        abstract public string SessionID { get; }

        /// <summary>
        /// Gets the interface of this connection
        /// </summary>
        /// <returns></returns>
        protected abstract IServerConnection GetInterface();

		/// <summary>
		/// Removes the version numbers from a providername
		/// </summary>
		/// <param name="providername">The name of the provider, with or without version numbers</param>
		/// <returns>The provider name without version numbers</returns>
		virtual public string RemoveVersionFromProviderName(string providername)
		{
            return Utility.StripVersionFromProviderName(providername);
		}

		/// <summary>
		/// Gets the Xsd schema for a given type.
		/// </summary>
		/// <param name="type">The type to get the schema for</param>
		/// <returns>The schema for the given type</returns>
		virtual protected System.Xml.Schema.XmlSchema GetSchema(Type type)
		{
			if (m_cachedSchemas[type] == null)
			{
				System.Reflection.FieldInfo fi = type.GetField("SchemaName", System.Reflection.BindingFlags.Static | System.Reflection.BindingFlags.Public );
				if (fi == null)
					throw new Exception("Type " + type + ", does not contain Schema Info");

				string xsd = (string)fi.GetValue(null);

				using (System.IO.FileStream fs = System.IO.File.Open(System.IO.Path.Combine(m_schemasPath, xsd), System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read))
					m_cachedSchemas.Add(type, System.Xml.Schema.XmlSchema.Read(fs, null));
			}

			return (System.Xml.Schema.XmlSchema)m_cachedSchemas[type];
		
		}

        /// <summary>
        /// Raised when a resource is added
        /// </summary>
        public event ResourceEventHandler ResourceAdded;
        /// <summary>
        /// Raised when a resource is deleted. Note if a folder is deleted, this will
        /// only be raised for the folder and not its children. Also note that this is
        /// raised on any move operations as the original source is for all intents and
        /// purposes, deleted.
        /// </summary>
        public event ResourceEventHandler ResourceDeleted;
        /// <summary>
        /// Raised when a resource is updated
        /// </summary>
        public event ResourceEventHandler ResourceUpdated;

        /// <summary>
        /// Raises the <see cref="ResourceAdded"/> event
        /// </summary>
        /// <param name="resId"></param>
        protected void OnResourceAdded(string resId)
        {
            var handler = this.ResourceAdded;
            if (handler != null)
                handler(this, new ResourceEventArgs(resId));
        }

        /// <summary>
        /// Raises the <see cref="ResourceDeleted"/> event
        /// </summary>
        /// <param name="resId"></param>
        protected void OnResourceDeleted(string resId)
        {
            var handler = this.ResourceDeleted;
            if (handler != null)
                handler(this, new ResourceEventArgs(resId));
        }

        /// <summary>
        /// Raises the <see cref="ResourceUpdated"/> event
        /// </summary>
        /// <param name="resId"></param>
        protected void OnResourceUpdated(string resId)
        {
            var handler = this.ResourceUpdated;
            if (handler != null)
                handler(this, new ResourceEventArgs(resId));
        }

		/// <summary>
		/// Gets or sets the collection of cached schemas. Use the object type for key, and an XmlSchema instance for value.
		/// </summary>
		virtual public Hashtable CachedSchemas
		{
			get { return m_cachedSchemas; }
			set { m_cachedSchemas = value; }
		}

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources
        /// </summary>
        abstract public void Dispose();

        /// <summary>
        /// Clones this instance.
        /// </summary>
        /// <returns></returns>
        public abstract IServerConnection Clone();

        /// <summary>
        /// Raised when a outbound request has been dispatched
        /// </summary>
        public event RequestEventHandler RequestDispatched;

        /// <summary>
        /// Called when [request dispatched].
        /// </summary>
        /// <param name="data">The data.</param>
        protected void OnRequestDispatched(string data)
        {
            var handler = this.RequestDispatched;
            if (handler != null)
                handler(this, new RequestEventArgs(data));
        }

        #region Resource Service

        /// <summary>
        /// Returns raw data from the server a byte array
        /// </summary>
        /// <param name="resourceID">The full resourceID to get data from</param>
        /// <returns>Raw data from the given resource</returns>
        public abstract Stream GetResourceXmlData(string resourceID);

        /// <summary>
        /// Returns an object deserialized from server data.
        /// Uses the ResourceID to infer the object type.
        /// </summary>
        /// <param name="resourceID">The full resourceID of the item to retrieve.</param>
        /// <returns>A deserialized object.</returns>
        virtual public IResource GetResource(string resourceID)
        {
            var stream = GetResourceXmlData(resourceID);
            var rt = ResourceIdentifier.GetResourceType(resourceID);

            IResource o = ResourceTypeRegistry.Deserialize(rt, stream);
            o.CurrentConnection = GetInterface();
            o.ResourceID = resourceID;

            return o;
        }

        /// <summary>
        /// Deletes the resource.
        /// </summary>
        /// <param name="resourceid">The resourceid.</param>
        public abstract void DeleteResource(string resourceid);

        /// <summary>
        /// Writes an object into a resourceID
        /// </summary>
        /// <param name="resourceid">The resource to write into</param>
        /// <param name="resource">The resourcec to write</param>
        virtual public void WriteResource(string resourceid, object resource)
        {
            System.IO.MemoryStream ms = SerializeObject(resource);
            ms.Position = 0;

            //Validate that our data is correctly formated
            /*if (!m_disableValidation)
            {
                m_validator.Validate(ms, GetSchema(resource.GetType()));
                ms.Position = 0;
            }*/

#if DEBUG_LASTMESSAGE
			using (System.IO.Stream s = System.IO.File.Open("lastSave.xml", System.IO.FileMode.Create, System.IO.FileAccess.Write, System.IO.FileShare.None))
				Utility.CopyStream(ms, s);
			ms.Position = 0;
#endif

            SetResourceXmlData(resourceid, ms);
        }

        /// <summary>
        /// Writes raw data into a resource.
        /// </summary>
        /// <param name="resourceid">The resourceID to write into</param>
        /// <param name="stream">The stream containing the data to write.</param>
        public virtual void SetResourceXmlData(string resourceid, System.IO.Stream stream)
        {
            SetResourceXmlData(resourceid, stream, null);
        }

        /// <summary>
        /// Gets a full list of resources in the permanent server repository (Library).
        /// This method returns the full catalog and should be used sparringly.
        /// </summary>
        /// <returns>A list of contained resources</returns>
        virtual public ObjCommon.ResourceList GetRepositoryResources()
        {
            return GetRepositoryResources("Library://", null, -1, true);
        }

        /// <summary>
        /// Gets a list of resources in the permanent server repository (Library).
        /// This method limits folder recursion to the specified depth.
        /// </summary>
        /// <param name="depth">The max depth to recurse. Use -1 for no limit.</param>
        /// <returns>A list of contained resources</returns>
        virtual public ObjCommon.ResourceList GetRepositoryResources(int depth)
        {
            return GetRepositoryResources("Library://", null, depth, true);
        }

        /// <summary>
        /// Gets a list of resources in the permanent server repository (Library).
        /// This method limits folder recursion to the specified depth.
        /// </summary>
        /// <param name="startingpoint">The folder from which to return items. Use null for &quot;Library://&quot;</param>
        /// <param name="depth">The max depth to recurse. Use -1 for no limit.</param>
        /// <returns>A list of contained resources</returns>
        virtual public ObjCommon.ResourceList GetRepositoryResources(string startingpoint, int depth)
        {
            return GetRepositoryResources(startingpoint, null, depth, true);
        }

        /// <summary>
        /// Gets a list of resources in the permanent server repository (Library).
        /// This method limits folder recursion to the specified depth.
        /// </summary>
        /// <param name="startingpoint">The folder from which to return items. Use null for &quot;Library://&quot;</param>
        /// <returns>A list of contained resources</returns>
        virtual public ObjCommon.ResourceList GetRepositoryResources(string startingpoint)
        {
            return GetRepositoryResources(startingpoint, null, -1, true);
        }

        /// <summary>
        /// Gets a list of resources in the permanent server repository (Library).
        /// This method limits folder recursion to the specified depth.
        /// </summary>
        /// <param name="startingpoint">The folder from which to return items. Use null for &quot;Library://&quot;</param>
        /// <param name="type">The type of resource to look for. Basically this is the resource extension, like &quot;.MapDefinition&quot;. Use null for all resources.</param>
        /// <returns>A list of contained resources</returns>
        virtual public ObjCommon.ResourceList GetRepositoryResources(string startingpoint, string type)
        {
            return GetRepositoryResources(startingpoint, type, -1, true);
        }

        /// <summary>
        /// Gets a list of resources in the permanent server repository (Library).
        /// This method limits folder recursion to the specified depth.
        /// </summary>
        /// <param name="startingpoint">The folder from which to return items. Use null for &quot;Library://&quot;</param>
        /// <param name="type">The type of resource to look for. Basically this is the resource extension, like &quot;.MapDefinition&quot;. Use null for all resources.</param>
        /// <param name="depth">The max depth to recurse. Use -1 for no limit.</param>
        /// <returns>A list of contained resources</returns>
        public ObjCommon.ResourceList GetRepositoryResources(string startingpoint, string type, int depth)
        {
            return GetRepositoryResources(startingpoint, type, depth, true);
        }

        /// <summary>
        /// Gets a list of resources in the permanent server repository (Library).
        /// </summary>
        /// <param name="startingpoint">The folder from which to return items. Use null for &quot;Library://&quot;</param>
        /// <param name="type">The type of resource to look for. Basically this is the resource extension, like &quot;.MapDefinition&quot;. Use null for all resources.</param>
        /// <param name="depth">The max depth to recurse. Use -1 for no limit.</param>
        /// <param name="computeChildren">A flag indicating if the count of subfolders and resources should be calculated for leaf nodes</param>
        /// <returns>A list of contained resources</returns>
        abstract public ObjCommon.ResourceList GetRepositoryResources(string startingpoint, string type, int depth, bool computeChildren);

        /// <summary>
        /// Forces a timestamp update of the specified resource. This is akin to 
        /// setting the resource's content using its existing content.
        /// </summary>
        /// <param name="resourceId"></param>
        public virtual void Touch(string resourceId)
        {
            if (!ResourceIdentifier.IsFolderResource(resourceId))
            {
                SetResourceXmlData(resourceId, GetResourceXmlData(resourceId));
            }
        }

        /// <summary>
        /// Returns a boolean indicating if a given resource exists
        /// </summary>
        /// <param name="resourceid">The resource to look for</param>
        /// <returns>True if the resource exists false otherwise. Also returns false on error.</returns>
        public virtual bool ResourceExists(string resourceid)
        {
            try
            {
                string sourcefolder;
                if (resourceid.EndsWith("/"))
                    sourcefolder = resourceid.Substring(0, resourceid.Substring(0, resourceid.Length - 1).LastIndexOf("/") + 1);
                else
                    sourcefolder = resourceid.Substring(0, resourceid.LastIndexOf("/") + 1);

                ObjCommon.ResourceList lst = GetRepositoryResources(sourcefolder, 1);
                foreach (object o in lst.Items)
                    if (o.GetType() == typeof(ObjCommon.ResourceListResourceFolder) && ((ObjCommon.ResourceListResourceFolder)o).ResourceId == resourceid)
                        return true;
                    else if (o.GetType() == typeof(ObjCommon.ResourceListResourceDocument) && ((ObjCommon.ResourceListResourceDocument)o).ResourceId == resourceid)
                        return true;

                return false;
            }
            catch
            {
                return false;
            }
        }

        /// <summary>
        /// Updates all resource references inside an object.
        /// </summary>
        /// <param name="o">The object in which the resource references are to be updated</param>
        /// <param name="oldresourcepath">The current resource path, the one updating from</param>
        /// <param name="newresourcepath">The new resource path, the one updating to</param>
        /// <param name="folderupdates">True if the old and new resource path identifiers are folders, false otherwise</param>
        public virtual void UpdateResourceReferences(object o, string oldresourcepath, string newresourcepath, bool folderupdates)
        {
            UpdateResourceReferences(o, oldresourcepath, newresourcepath, folderupdates, new Hashtable());
        }

        /// <summary>
        /// Updates all resource references inside an object.
        /// </summary>
        /// <param name="o">The object in which the resource references are to be updated</param>
        /// <param name="oldresourcepath">The current resource path, the one updating from</param>
        /// <param name="newresourcepath">The new resource path, the one updating to</param>
        /// <param name="folderupdates">True if the old and new resource path identifiers are folders, false otherwise</param>
        /// <param name="visited">A hashtable with objects previously visited. Used for recursion, leave as null when calling from outside the API.</param>
        protected void UpdateResourceReferences(object o, string oldresourcepath, string newresourcepath, bool folderupdates, Hashtable visited)
        {
            if (o == null)
                return;

            if (visited == null)
                visited = new Hashtable();

            //Prevent infinite recursion
            if (o as string == null && !o.GetType().IsPrimitive)
            {
                if (visited.ContainsKey(o))
                    return;
                else
                    visited.Add(o, null);
            }

            if (folderupdates)
            {
                if (!oldresourcepath.EndsWith("/"))
                    oldresourcepath += "/";
                if (!newresourcepath.EndsWith("/"))
                    newresourcepath += "/";
            }

            //If the value is a document or fragment of a document, we still wan't to repoint it
            if (o as System.Xml.XmlDocument != null || o as System.Xml.XmlNode != null)
            {
                Queue<System.Xml.XmlNode> lst = new Queue<System.Xml.XmlNode>();
                if (o as System.Xml.XmlDocument != null)
                {
                    foreach (System.Xml.XmlNode n in (o as System.Xml.XmlDocument).ChildNodes)
                        if (n.NodeType == System.Xml.XmlNodeType.Element)
                            lst.Enqueue(n);
                }
                else
                    lst.Enqueue(o as System.Xml.XmlNode);

                while (lst.Count > 0)
                {
                    System.Xml.XmlNode n = lst.Dequeue();

                    foreach (System.Xml.XmlNode nx in n.ChildNodes)
                        if (nx.NodeType == System.Xml.XmlNodeType.Element)
                            lst.Enqueue(nx);

                    if (n.Name == "ResourceId")
                    {
                        string current = n.InnerXml;
                        if (folderupdates && current.StartsWith(oldresourcepath))
                            n.InnerXml = newresourcepath + current.Substring(oldresourcepath.Length);
                        else if (current == oldresourcepath)
                            n.InnerXml = newresourcepath;
                    }

                    foreach (System.Xml.XmlAttribute a in n.Attributes)
                        if (a.Name == "ResourceId")
                        {
                            string current = a.Value;
                            if (folderupdates && current.StartsWith(oldresourcepath))
                                n.Value = newresourcepath + current.Substring(oldresourcepath.Length);
                            else if (current == oldresourcepath)
                                n.Value = newresourcepath;
                        }
                }

                //There can be no objects in an xml document or node, so just return immediately
                return;
            }

            //Try to find the object properties
            foreach (System.Reflection.PropertyInfo pi in o.GetType().GetProperties())
            {
                //Only index free read-write properties are taken into account
                if (!pi.CanRead || !pi.CanWrite || pi.GetIndexParameters().Length != 0 || pi.GetValue(o, null) == null)
                    continue;

                object v = pi.GetValue(o, null);
                if (v == null)
                    continue;

                if (v is string)

                    //If we are at a ResourceId property, update it as needed
                    if (v is string)
                    {
                        bool isResId = pi.Name == "ResourceId";
                        if (!isResId)
                        {
                            //Search for attributes
                            object[] xmlAttrs = pi.GetCustomAttributes(typeof(System.Xml.Serialization.XmlElementAttribute), false);
                            if (xmlAttrs != null)
                                foreach (System.Xml.Serialization.XmlElementAttribute attr in xmlAttrs)
                                    if (attr.Type == typeof(string) && attr.ElementName == "ResourceId")
                                        if (pi.Name == "ResourceId")
                                        {
                                            isResId = true;
                                            break;
                                        }
                        }

                        if (isResId)
                        {
                            string current = v as string;

                            if (current != null)
                            {
                                if (folderupdates && current.StartsWith(oldresourcepath))
                                    pi.SetValue(o, newresourcepath + current.Substring(oldresourcepath.Length), null);
                                else if (current == oldresourcepath)
                                    pi.SetValue(o, newresourcepath, null);
                            }
                        }
                    }
                    else if (v is IEnumerable)
                    {
                        //Handle collections
                        System.Collections.IEnumerable srcList = (System.Collections.IEnumerable)v;
                        foreach (object ox in srcList)
                            UpdateResourceReferences(ox, oldresourcepath, newresourcepath, folderupdates, visited);
                    }
                    else if (v.GetType().IsArray)
                    {
                        //Handle arrays
                        System.Array sourceArr = (System.Array)v;
                        for (int i = 0; i < sourceArr.Length; i++)
                            UpdateResourceReferences(sourceArr.GetValue(i), oldresourcepath, newresourcepath, folderupdates, visited);
                    }
                    else if (v.GetType().IsClass)
                    {
                        //Handle subobjects
                        UpdateResourceReferences(v, oldresourcepath, newresourcepath, folderupdates, visited);
                    }
            }

        }


        /// <summary>
        /// Moves a resource, and subsequently updates all resources pointing to the old resource path
        /// </summary>
        /// <param name="oldpath">The current resource path, the one moving from</param>
        /// <param name="newpath">The new resource path, the one moving to</param>
        /// <param name="callback">A callback delegate, being called for non progress reporting events.</param>
        /// <param name="progress">A callback delegate, being called for progress reporting events.</param>
        /// <returns></returns>
        public virtual bool MoveResourceWithReferences(string oldpath, string newpath, LengthyOperationCallBack callback, LengthyOperationProgressCallBack progress)
        {
            LengthyOperationProgressArgs la = new LengthyOperationProgressArgs(Properties.Resources.MovingResource, -1);

            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;
            MoveResource(oldpath, newpath, true);
            la.Progress = 100;
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;

            la.Progress = -1;
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;

            ArrayList items = new ArrayList();
            Hashtable paths = new Hashtable();

            //The old path does not exist, but luckily the call works anyway
            ObjCommon.ResourceReferenceList rlf = EnumerateResourceReferences(oldpath);

            foreach (string s in rlf.ResourceId)
                if (!paths.ContainsKey(s))
                {
                    items.Add(new LengthyOperationCallbackArgs.LengthyOperationItem(s));
                    paths.Add(s, null);
                }

            la.Progress = 100;
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;

            LengthyOperationCallbackArgs args = new LengthyOperationCallbackArgs((LengthyOperationCallbackArgs.LengthyOperationItem[])items.ToArray(typeof(LengthyOperationCallbackArgs.LengthyOperationItem)));

            if (callback != null)
                callback(this, args);

            if (args.Cancel)
                return false;

            if (args.Index > args.Items.Length)
                return true;

            if (args.Items.Length == 0)
                return true;

            do
            {
                LengthyOperationCallbackArgs.LengthyOperationItem item = args.Items[args.Index];
                item.Status = LengthyOperationCallbackArgs.LengthyOperationItem.OperationStatus.Pending;

                if (callback != null)
                {
                    callback(this, args);
                    if (args.Cancel) return false;
                }

                try
                {
                    System.Xml.XmlDocument d = new System.Xml.XmlDocument();
                    using (var ms = GetResourceXmlData(item.Itempath))
                        d.Load(ms);

                    UpdateResourceReferences(d, oldpath, newpath, false);
                    using (System.IO.MemoryStream ms = new System.IO.MemoryStream())
                    {
                        d.Save(ms);
                        ms.Position = 0;

                        SetResourceXmlData(item.Itempath, ms);
                    }
                    item.Status = LengthyOperationCallbackArgs.LengthyOperationItem.OperationStatus.Success;
                }
                catch (Exception ex)
                {
                    string s = ex.Message;
                    item.Status = LengthyOperationCallbackArgs.LengthyOperationItem.OperationStatus.Failure;
                }

                if (callback != null)
                {
                    callback(this, args);
                    if (args.Cancel) return false;
                }

                args.Index++;
            } while (!args.Cancel && args.Index < args.Items.Length);

            return !args.Cancel;
        }

        /// <summary>
        /// Moves a folder, and subsequently updates all resources pointing to the old resource path
        /// </summary>
        /// <param name="oldpath">The current folder path, the one moving from</param>
        /// <param name="newpath">The new folder path, the one moving to</param>
        /// <param name="callback">A callback delegate, being called for non progress reporting events.</param>
        /// <param name="progress">A callback delegate, being called for progress reporting events.</param>
        /// <returns></returns>
        public virtual bool MoveFolderWithReferences(string oldpath, string newpath, LengthyOperationCallBack callback, LengthyOperationProgressCallBack progress)
        {
            oldpath = FixAndValidateFolderPath(oldpath);
            newpath = FixAndValidateFolderPath(newpath);

            LengthyOperationProgressArgs la = new LengthyOperationProgressArgs("Moving folder...", -1);

            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;

            MoveFolder(oldpath, newpath, true);
            la.Progress = 100;
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;

            int pg = 0;
            la.Progress = 0;
            la.StatusMessage = "Finding folder references...";
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;

            ObjCommon.ResourceList lst = GetRepositoryResources(newpath);

            Hashtable items = new Hashtable();
            foreach (object o in lst.Items)
            {
                if (o.GetType() == typeof(ObjCommon.ResourceListResourceDocument))
                {
                    //The old path does not exist, but we need to enumerate references at the old location
                    string resource_oldpath = ((ObjCommon.ResourceListResourceDocument)o).ResourceId;
                    resource_oldpath = oldpath + resource_oldpath.Substring(newpath.Length);

                    ObjCommon.ResourceReferenceList rlf = EnumerateResourceReferences(resource_oldpath);
                    foreach (string s in rlf.ResourceId)
                        if (!items.Contains(s))
                            items.Add(s, new LengthyOperationCallbackArgs.LengthyOperationItem(s));
                }

                pg++;
                la.Progress = Math.Max(Math.Min(99, (int)(((double)pg / (double)lst.Items.Count) * (double)100)), 0);

                if (progress != null)
                    progress(this, la);
                if (la.Cancel)
                    return false;
            }

            la.Progress = 100;
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;

            LengthyOperationCallbackArgs.LengthyOperationItem[] vi = new LengthyOperationCallbackArgs.LengthyOperationItem[items.Values.Count];
            items.Values.CopyTo(vi, 0);
            LengthyOperationCallbackArgs args = new LengthyOperationCallbackArgs(vi);

            if (callback != null)
                callback(this, args);

            if (args.Cancel)
                return false;

            if (args.Index > args.Items.Length)
                return true;

            if (args.Items.Length == 0)
                return true;

            do
            {
                LengthyOperationCallbackArgs.LengthyOperationItem item = args.Items[args.Index];
                item.Status = LengthyOperationCallbackArgs.LengthyOperationItem.OperationStatus.Pending;

                if (callback != null)
                {
                    callback(this, args);
                    if (args.Cancel)
                        return false;
                }

                try
                {
                    System.Xml.XmlDocument d = new System.Xml.XmlDocument();
                    using (var ms = GetResourceXmlData(item.Itempath))
                        d.Load(ms);

                    UpdateResourceReferences(d, oldpath, newpath, true);
                    using (System.IO.MemoryStream ms = new System.IO.MemoryStream())
                    {
                        d.Save(ms);
                        ms.Position = 0;

                        SetResourceXmlData(item.Itempath, ms);
                    }
                    item.Status = LengthyOperationCallbackArgs.LengthyOperationItem.OperationStatus.Success;
                }
                catch (Exception ex)
                {
                    string s = ex.Message;
                    item.Status = LengthyOperationCallbackArgs.LengthyOperationItem.OperationStatus.Failure;
                }

                if (callback != null)
                {
                    callback(this, args);
                    if (args.Cancel)
                        return false;
                }

                args.Index++;
            } while (!args.Cancel && args.Index < args.Items.Length);

            return !args.Cancel;
        }

        /// <summary>
        /// Copies folder, and subsequently updates all resources within the folder to use the new folder path instead of the originating one.
        /// </summary>
        /// <param name="oldpath">The current folder path, the one copying from</param>
        /// <param name="newpath">The new folder path, the one copying to</param>
        /// <param name="callback">A callback delegate, being called for non progress reporting events.</param>
        /// <param name="progress">A callback delegate, being called for progress reporting events.</param>
        /// <returns></returns>
        public bool CopyFolderWithReferences(string oldpath, string newpath, LengthyOperationCallBack callback, LengthyOperationProgressCallBack progress)
        {
            oldpath = FixAndValidateFolderPath(oldpath);
            newpath = FixAndValidateFolderPath(newpath);
            ObjCommon.ResourceList lst = GetRepositoryResources(oldpath);

            LengthyOperationProgressArgs la = new LengthyOperationProgressArgs("Copying folder...", -1);
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;
            CopyFolder(oldpath, newpath, true);
            la.Progress = 100;
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;


            la.Progress = 0;
            la.StatusMessage = "Finding folder references...";
            int pg = 0;
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;
            ArrayList items = new ArrayList();
            Hashtable paths = new Hashtable();
            foreach (object o in lst.Items)
            {
                if (o.GetType() == typeof(ObjCommon.ResourceListResourceDocument))
                {
                    ObjCommon.ResourceReferenceList rlf = EnumerateResourceReferences(((ObjCommon.ResourceListResourceDocument)o).ResourceId);
                    foreach (string s in rlf.ResourceId)
                        if (s.StartsWith(oldpath))
                        {
                            string dest = newpath + s.Substring(oldpath.Length);
                            if (!paths.ContainsKey(dest))
                            {
                                items.Add(new LengthyOperationCallbackArgs.LengthyOperationItem(dest));
                                paths.Add(dest, null);
                            }
                        }
                }
                pg++;
                la.Progress = Math.Max(Math.Min(99, (int)(((double)pg / (double)lst.Items.Count) * (double)100)), 0);

                if (progress != null)
                    progress(this, la);
                if (la.Cancel)
                    return false;
            }

            la.Progress = 100;
            if (progress != null)
                progress(this, la);
            if (la.Cancel)
                return false;

            LengthyOperationCallbackArgs args = new LengthyOperationCallbackArgs((LengthyOperationCallbackArgs.LengthyOperationItem[])items.ToArray(typeof(LengthyOperationCallbackArgs.LengthyOperationItem)));

            if (callback != null)
                callback(this, args);

            if (args.Cancel)
                return false;

            if (args.Index > args.Items.Length)
                return true;

            if (args.Items.Length == 0)
                return true;

            do
            {
                LengthyOperationCallbackArgs.LengthyOperationItem item = args.Items[args.Index];
                item.Status = LengthyOperationCallbackArgs.LengthyOperationItem.OperationStatus.Pending;

                if (callback != null)
                {
                    callback(this, args);
                    if (args.Cancel) return false;
                }

                try
                {
                    System.Xml.XmlDocument d = new System.Xml.XmlDocument();
                    using (var ms = GetResourceXmlData(item.Itempath))
                        d.Load(ms);

                    UpdateResourceReferences(d, oldpath, newpath, true);
                    using (System.IO.MemoryStream ms = new System.IO.MemoryStream())
                    {
                        d.Save(ms);
                        ms.Position = 0;

                        SetResourceXmlData(item.Itempath, ms);
                    }
                    item.Status = LengthyOperationCallbackArgs.LengthyOperationItem.OperationStatus.Success;
                }
                catch (Exception ex)
                {
                    string s = ex.Message;
                    item.Status = LengthyOperationCallbackArgs.LengthyOperationItem.OperationStatus.Failure;
                }

                if (callback != null)
                {
                    callback(this, args);
                    if (args.Cancel) return false;
                }

                args.Index++;
            } while (!args.Cancel && args.Index < args.Items.Length);

            return !args.Cancel;
        }

        /// <summary>
        /// Validates the origin of the folder, and ensures the folder path has a trailing slash.
        /// </summary>
        /// <param name="folderpath">The path to validate and fix</param>
        /// <returns>The fixed path</returns>
        virtual protected string FixAndValidateFolderPath(string folderpath)
        {
            if (!folderpath.StartsWith("Library://") && !folderpath.StartsWith("Session:" + this.SessionID + "//"))
                throw new Exception("Invalid folder path, must be either library or session");

            if (!folderpath.EndsWith("/"))
                folderpath += "/";

            return folderpath;
        }

        /// <summary>
        /// Creates a folder on the server
        /// </summary>
        /// <param name="resourceID">The path of the folder to create</param>
        virtual public void CreateFolder(string resourceID)
        {
            resourceID = FixAndValidateFolderPath(resourceID);
            SetResourceXmlData(resourceID, new System.IO.MemoryStream());
        }

        /// <summary>
        /// Returns a value indicating if a given folder exists
        /// </summary>
        /// <param name="folderpath">The path of the folder</param>
        /// <returns>True if the folder exists, false otherwise. Also returns false on error.</returns>
        virtual public bool HasFolder(string folderpath)
        {
            folderpath = FixAndValidateFolderPath(folderpath);

            try
            {
                ObjCommon.ResourceList l = this.GetRepositoryResources(folderpath, 1);
                return true;
            }
            catch
            {
                return false;
            }
        }

        /// <summary>
        /// Enumereates all references to a given resource
        /// </summary>
        /// <param name="resourceid">The resource to enumerate references for</param>
        /// <returns>A list of resources that reference the given resourceID</returns>
        abstract public ObjCommon.ResourceReferenceList EnumerateResourceReferences(string resourceid);

        /// <summary>
        /// Copies a resource from one location to another. This does not update any references.
        /// </summary>
        /// <param name="oldpath">The current resource path, the one copying from</param>
        /// <param name="newpath">The new resource path, the one copying to</param>
        /// <param name="overwrite">True if the copy can overwrite an existing resource, false otherwise</param>
        abstract public void CopyResource(string oldpath, string newpath, bool overwrite);

        /// <summary>
        /// Copies a folder and all its content. This does not update any references.
        /// </summary>
        /// <param name="oldpath">The current folder path, the one copying from</param>
        /// <param name="newpath">The new folder path, the one copying to</param>
        /// <param name="overwrite">True if the copy can overwrite an existing folder, false otherwise</param>
        abstract public void CopyFolder(string oldpath, string newpath, bool overwrite);

        /// <summary>
        /// Moves a resource from one location to another. This does not update any references.
        /// </summary>
        /// <param name="oldpath">The current resource path, the one moving from</param>
        /// <param name="newpath">The new resource path, the one moving to</param>
        /// <param name="overwrite">True if the move can overwrite an existing resource, false otherwise</param>
        abstract public void MoveResource(string oldpath, string newpath, bool overwrite);

        /// <summary>
        /// Moves a folder and its content from one location to another. This does not update any references.
        /// </summary>
        /// <param name="oldpath">The current folder path, the one moving from</param>
        /// <param name="newpath">The new folder path, the one moving to</param>
        /// <param name="overwrite">True if the move can overwrite an existing folder, false otherwise</param>
        abstract public void MoveFolder(string oldpath, string newpath, bool overwrite);


        /// <summary>
        /// Returns data from a resource as a memorystream
        /// </summary>
        /// <param name="resourceID">The id of the resource to fetch data from</param>
        /// <param name="dataname">The name of the associated data item</param>
        /// <returns>A stream containing the references resource data</returns>
        abstract public System.IO.Stream GetResourceData(string resourceID, string dataname);

        /// <summary>
        /// Uploads data to a resource
        /// </summary>
        /// <param name="resourceid">The id of the resource to update</param>
        /// <param name="dataname">The name of the data to update or create</param>
        /// <param name="datatype">The type of data</param>
        /// <param name="stream">A stream containing the new content of the resource data</param>
        virtual public void SetResourceData(string resourceid, string dataname, ObjCommon.ResourceDataType datatype, System.IO.Stream stream)
        {
            SetResourceData(resourceid, dataname, datatype, stream, null);
        }

        /// <summary>
        /// Uploads data to a resource
        /// </summary>
        /// <param name="resourceid">The id of the resource to update</param>
        /// <param name="dataname">The name of the data to update or create</param>
        /// <param name="datatype">The type of data</param>
        /// <param name="stream">A stream containing the new content of the resource data</param>
        /// <param name="callback">The callback.</param>
        abstract public void SetResourceData(string resourceid, string dataname, ObjCommon.ResourceDataType datatype, System.IO.Stream stream, Utility.StreamCopyProgressDelegate callback);

        /// <summary>
        /// Saves the resource.
        /// </summary>
        /// <param name="resource">The resource.</param>
        public void SaveResource(OSGeo.MapGuide.MaestroAPI.Resource.IResource resource)
        {
            SaveResourceAs(resource, resource.ResourceID);
        }

        /// <summary>
        /// Saves the resource with the specified resource ID
        /// </summary>
        /// <param name="resource">The resource.</param>
        /// <param name="resourceid">The resourceid.</param>
        public void SaveResourceAs(OSGeo.MapGuide.MaestroAPI.Resource.IResource resource, string resourceid)
        {
            var stream = resource.SerializeToStream();
            SetResourceXmlData(resourceid, stream);
        }

        /// <summary>
        /// Upload a MapGuide Package file to the server
        /// </summary>
        /// <param name="filename">Name of the file to upload</param>
        /// <param name="callback">A callback argument used to display progress. May be null.</param>
        abstract public void UploadPackage(string filename, Utility.StreamCopyProgressDelegate callback);

        /// <summary>
        /// Updates the repository.
        /// </summary>
        /// <param name="resourceId">The resource id.</param>
        /// <param name="header">The header.</param>
        abstract public void UpdateRepository(string resourceId, ObjCommon.ResourceFolderHeaderType header);

        /// <summary>
        /// Gets the folder or resource header.
        /// </summary>
        /// <param name="resourceId">The resource id.</param>
        /// <returns></returns>
        abstract public object GetFolderOrResourceHeader(string resourceId);

        /// <summary>
        /// Sets the resource XML data.
        /// </summary>
        /// <param name="resourceId">The resource id.</param>
        /// <param name="content">The content.</param>
        /// <param name="header">The header.</param>
        abstract public void SetResourceXmlData(string resourceId, System.IO.Stream content, System.IO.Stream header);

        /// <summary>
        /// Gets the resource header.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <returns></returns>
        public virtual ObjCommon.ResourceDocumentHeaderType GetResourceHeader(string resourceID)
        {
            return (ObjCommon.ResourceDocumentHeaderType)this.GetFolderOrResourceHeader(resourceID);
        }

        /// <summary>
        /// Gets the folder header.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <returns></returns>
        public virtual ObjCommon.ResourceFolderHeaderType GetFolderHeader(string resourceID)
        {
            if (resourceID.EndsWith("//"))
            {
                ObjCommon.ResourceList lst = this.GetRepositoryResources(resourceID, 0);
                ObjCommon.ResourceListResourceFolder fld = lst.Items[0] as ObjCommon.ResourceListResourceFolder;
                return fld.ResourceFolderHeader;
            }
            else
                return (ObjCommon.ResourceFolderHeaderType)this.GetFolderOrResourceHeader(resourceID);
        }

        /// <summary>
        /// Sets the folder header.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <param name="header">The header.</param>
        public virtual void SetFolderHeader(string resourceID, ObjCommon.ResourceFolderHeaderType header)
        {
            SetFolderOrResourceHeader(resourceID, header);
        }

        /// <summary>
        /// Sets the resource header.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <param name="header">The header.</param>
        public virtual void SetResourceHeader(string resourceID, ObjCommon.ResourceDocumentHeaderType header)
        {
            SetFolderOrResourceHeader(resourceID, header);
        }

        /// <summary>
        /// Sets the folder or resource header.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <param name="header">The header.</param>
        public virtual void SetFolderOrResourceHeader(string resourceID, object header)
        {
            if (header == null)
                throw new ArgumentNullException("header");

            ObjCommon.ResourceSecurityType sec;
            if (header as ObjCommon.ResourceFolderHeaderType != null)
                sec = (header as ObjCommon.ResourceFolderHeaderType).Security;
            else if (header as ObjCommon.ResourceDocumentHeaderType != null)
                sec = (header as ObjCommon.ResourceDocumentHeaderType).Security;
            else
                throw new ArgumentException("Header must be either ResourceFolderHeaderType or ResourceDocumentHeaderType", "header");

            if (sec.Users != null && sec.Users.User != null && sec.Users.User.Count == 0)
                sec.Users = null;

            if (sec.Groups != null && sec.Groups.Group != null && sec.Groups.Group.Count == 0)
                sec.Groups = null;

            if (resourceID.EndsWith("//"))
            {
                if (header as ObjCommon.ResourceFolderHeaderType == null)
                    throw new Exception("The resourceId: " + resourceID + " must be updated with a folder header");
                UpdateRepository(resourceID, header as ObjCommon.ResourceFolderHeaderType);
            }
            else
                this.SetResourceXmlData(resourceID, null, this.SerializeObject(header));
        }


        /// <summary>
        /// Enumerates all unmanaged folders, meaning alias'ed folders
        /// </summary>
        /// <param name="type">The type of data to return</param>
        /// <param name="filter">A filter applied to the items</param>
        /// <param name="recursive">True if the list should contains recursive results</param>
        /// <param name="startpath">The path to retrieve the data from</param>
        /// <returns>A list of unmanaged data</returns>
        abstract public ObjCommon.UnmanagedDataList EnumerateUnmanagedData(string startpath, string filter, bool recursive, UnmanagedDataTypes type);

        #endregion

        #region Feature Service

        /// <summary>
        /// Returns an installed provider, given the name of the provider
        /// </summary>
        /// <param name="providername">The name of the provider</param>
        /// <returns>The first matching provider or null</returns>
        virtual public ObjCommon.FeatureProviderRegistryFeatureProvider GetFeatureProvider(string providername)
        {
            string pname = RemoveVersionFromProviderName(providername).ToLower();
            foreach (ObjCommon.FeatureProviderRegistryFeatureProvider p in this.FeatureProviders)
                if (RemoveVersionFromProviderName(p.Name).ToLower().Equals(pname.ToLower()))
                    return p;

            return null;
        }

        /// <summary>
        /// Tests the connection.
        /// </summary>
        /// <param name="featuresource">The featuresource.</param>
        /// <returns></returns>
        public abstract string TestConnection(string featuresource);

        /// <summary>
        /// Gets a list of installed feature providers
        /// </summary>
        abstract public ObjCommon.FeatureProviderRegistryFeatureProvider[] FeatureProviders { get; }


        /// <summary>
        /// Returns the spatial info for a given featuresource
        /// </summary>
        /// <param name="resourceID">The ID of the resource to query</param>
        /// <param name="activeOnly">Query only active items</param>
        /// <returns>A list of spatial contexts</returns>
        abstract public ObjCommon.FdoSpatialContextList GetSpatialContextInfo(string resourceID, bool activeOnly);

        /// <summary>
        /// Gets the names of the identity properties from a feature
        /// </summary>
        /// <param name="resourceID">The resourceID for the FeatureSource</param>
        /// <param name="classname">The classname of the feature, including schema</param>
        /// <returns>A string array with the found identities</returns>
        abstract public string[] GetIdentityProperties(string resourceID, string classname);

        /// <summary>
        /// Describes the feature source.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <returns></returns>
        abstract public FeatureSourceDescription DescribeFeatureSource(string resourceID);

        /// <summary>
        /// Describes the feature source.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <param name="schema">The schema.</param>
        /// <returns></returns>
        abstract public FeatureSchema DescribeFeatureSource(string resourceID, string schema);

        /// <summary>
        /// feature source description cache
        /// </summary>
        protected Dictionary<string, FeatureSourceDescription> m_featureSchemaCache = new Dictionary<string, FeatureSourceDescription>();
        /// <summary>
        /// a class definition cache
        /// </summary>
        protected Dictionary<string, ClassDefinition> m_featureSchemaNameCache = new Dictionary<string, ClassDefinition>();

        /// <summary>
        /// Gets the feature source description.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <returns></returns>
        public virtual FeatureSourceDescription GetFeatureSourceDescription(string resourceID)
        {
            if (!m_featureSchemaCache.ContainsKey(resourceID))
            {
                try
                {
                    m_featureSchemaCache[resourceID] = this.DescribeFeatureSource(resourceID);
                    foreach (ClassDefinition scm in m_featureSchemaCache[resourceID].AllClasses)
                        m_featureSchemaNameCache[resourceID + "!" + scm.QualifiedName] = scm;
                }
                catch
                {
                    m_featureSchemaCache[resourceID] = null;
                }
            }

            return m_featureSchemaCache[resourceID];

        }


        /// <summary>
        /// Gets the class definition.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <param name="schema">The schema.</param>
        /// <returns></returns>
        public virtual ClassDefinition GetClassDefinition(string resourceID, string schema)
        {
            /*if (schema != null && schema.IndexOf(":") > 0)
                schema = schema.Substring(0, schema.IndexOf(":"));*/

            //If it is missing, just get the entire schema, and hope that we will need the others
            //Some providers actually return the entire list even when asked for a particular schema
            if (!m_featureSchemaCache.ContainsKey(resourceID + "!" + schema))
                GetFeatureSourceDescription(resourceID);
            if (!m_featureSchemaNameCache.ContainsKey(resourceID + "!" + schema))
                m_featureSchemaNameCache[resourceID + "!" + schema] = null;

            return m_featureSchemaNameCache[resourceID + "!" + schema];
        }

        /// <summary>
        /// Resets the feature source schema cache.
        /// </summary>
        public virtual void ResetFeatureSourceSchemaCache()
        {
            m_featureSchemaCache = new Dictionary<string, FeatureSourceDescription>();
            m_featureSchemaNameCache = new Dictionary<string, ClassDefinition>();
        }

        /// <summary>
        /// Performs an aggregate query on all columns in the datasource
        /// </summary>
        /// <param name="resourceID">The resourceID of the FeatureSource to query</param>
        /// <param name="schema">The schema name</param>
        /// <param name="filter">The filter to apply to the </param>
        /// <returns>A FeatureSetReader with the aggregated values</returns>
        public virtual IReader AggregateQueryFeatureSource(string resourceID, string schema, string filter)
        {
            return AggregateQueryFeatureSource(resourceID, schema, filter, (string[])null);
        }

        /// <summary>
        /// Performs an aggregate query on columns in the datasource
        /// </summary>
        /// <param name="resourceID">The resourceID of the FeatureSource to query</param>
        /// <param name="schema">The schema name</param>
        /// <param name="filter">The filter to apply to the </param>
        /// <param name="columns">The columns to aggregate</param>
        /// <returns>A IFeatureReader with the aggregated values</returns>
        public abstract IReader AggregateQueryFeatureSource(string resourceID, string schema, string filter, string[] columns);

        /// <summary>
        /// Performs an aggregate query on computed resources
        /// </summary>
        /// <param name="resourceID">The resourceID of the FeatureSource to query</param>
        /// <param name="schema">The schema name</param>
        /// <param name="filter">The filter to apply to the </param>
        /// <param name="aggregateFunctions">A collection of column name and aggregate functions</param>
        /// <returns>A FeatureSetReader with the aggregated values</returns>
        public abstract IReader AggregateQueryFeatureSource(string resourceID, string schema, string filter, System.Collections.Specialized.NameValueCollection aggregateFunctions);

        /// <summary>
        /// Gets the spatial extent.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <param name="schema">The schema.</param>
        /// <param name="geometry">The geometry.</param>
        /// <returns></returns>
        public virtual OSGeo.MapGuide.ObjectModels.Common.IEnvelope GetSpatialExtent(string resourceID, string schema, string geometry)
        {
            return GetSpatialExtent(resourceID, schema, geometry, null, false);
        }

        /// <summary>
        /// Gets the spatial extent.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <param name="schema">The schema.</param>
        /// <param name="geometry">The geometry.</param>
        /// <param name="filter">The filter.</param>
        /// <returns></returns>
        public virtual OSGeo.MapGuide.ObjectModels.Common.IEnvelope GetSpatialExtent(string resourceID, string schema, string geometry, string filter)
        {
            return GetSpatialExtent(resourceID, schema, geometry, filter, false);
        }

        /// <summary>
        /// Gets the spatial extent.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <param name="schema">The schema.</param>
        /// <param name="geometry">The geometry.</param>
        /// <param name="allowFallbackToContextInformation">if set to <c>true</c> [allow fallback to context information].</param>
        /// <returns></returns>
        public virtual OSGeo.MapGuide.ObjectModels.Common.IEnvelope GetSpatialExtent(string resourceID, string schema, string geometry, bool allowFallbackToContextInformation)
        {
            return GetSpatialExtent(resourceID, schema, geometry, null, allowFallbackToContextInformation);
        }

        /// <summary>
        /// Gets the spatial extent.
        /// </summary>
        /// <param name="resourceID">The resource ID.</param>
        /// <param name="schema">The schema.</param>
        /// <param name="geometry">The geometry.</param>
        /// <param name="filter">The filter.</param>
        /// <param name="allowFallbackToContextInformation">if set to <c>true</c> [allow fallback to context information].</param>
        /// <returns></returns>
        protected virtual OSGeo.MapGuide.ObjectModels.Common.IEnvelope GetSpatialExtent(string resourceID, string schema, string geometry, string filter, bool allowFallbackToContextInformation)
        {
            Check.NotEmpty(schema, "schema");
            Check.NotEmpty(geometry, "geometry");
            try
            {
                System.Collections.Specialized.NameValueCollection fun = new System.Collections.Specialized.NameValueCollection();
                fun.Add("extent", "SpatialExtents(\"" + geometry + "\")");
                using (IReader fsr = AggregateQueryFeatureSource(resourceID, schema, filter, fun))
                {
                    if (fsr.ReadNext())
                    {
                        Topology.Geometries.IGeometry geom = fsr["extent"] as Topology.Geometries.IGeometry;
                        if (geom == null)
                        {
                            throw new Exception("No data found in resource: " + resourceID);
                        }
                        else
                        {
                            var env = geom.EnvelopeInternal;
                            return OSGeo.MapGuide.ObjectModels.ObjectFactory.CreateEnvelope(
                                env.MinX,
                                env.MinY,
                                env.MaxX,
                                env.MaxY);
                        }
                    }
                    else
                        throw new Exception("No data found in resource: " + resourceID);
                }
            }
            catch
            {
                if (allowFallbackToContextInformation)
                    try
                    {
                        ObjCommon.FdoSpatialContextList lst = this.GetSpatialContextInfo(resourceID, false);
                        if (lst.SpatialContext != null && lst.SpatialContext.Count >= 1)
                        {
                            return OSGeo.MapGuide.ObjectModels.ObjectFactory.CreateEnvelope(
                                double.Parse(lst.SpatialContext[0].Extent.LowerLeftCoordinate.X, System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture),
                                double.Parse(lst.SpatialContext[0].Extent.UpperRightCoordinate.X, System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture),
                                double.Parse(lst.SpatialContext[0].Extent.LowerLeftCoordinate.Y, System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture),
                                double.Parse(lst.SpatialContext[0].Extent.UpperRightCoordinate.Y, System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture)
                            );
                        }
                    }
                    catch
                    {
                    }

                throw;
            }
        }

        /// <summary>
        /// Enumerates the data stores.
        /// </summary>
        /// <param name="providerName">Name of the provider.</param>
        /// <param name="partialConnString">The partial conn string.</param>
        /// <returns></returns>
        public abstract OSGeo.MapGuide.ObjectModels.Common.DataStoreList EnumerateDataStores(string providerName, string partialConnString);

        /// <summary>
        /// Gets the schemas.
        /// </summary>
        /// <param name="resourceId">The resource id.</param>
        /// <returns></returns>
        public abstract string[] GetSchemas(string resourceId);

        /// <summary>
        /// Gets the class names.
        /// </summary>
        /// <param name="resourceId">The resource id.</param>
        /// <param name="schemaName">Name of the schema.</param>
        /// <returns></returns>
        public abstract string[] GetClassNames(string resourceId, string schemaName);

        #endregion

        #region Feature/Capability Discovery

        /// <summary>
        /// Gets the highest version the API is currently tested againts
        /// </summary>
        virtual public Version MaxTestedVersion
        {
            get { return SiteVersions.GetVersion(KnownSiteVersions.MapGuideOS2_1); }
        }

        /// <summary>
        /// Gets the site version.
        /// </summary>
        /// <value>The site version.</value>
        abstract public Version SiteVersion { get; }

        /// <summary>
        /// Gets the custom property names.
        /// </summary>
        /// <returns></returns>
        public abstract string[] GetCustomPropertyNames();

        /// <summary>
        /// Gets the type of the custom property.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <returns></returns>
        public abstract Type GetCustomPropertyType(string name);

        /// <summary>
        /// Sets the custom property.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <param name="value">The value.</param>
        public abstract void SetCustomProperty(string name, object value);

        /// <summary>
        /// Gets the custom property.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <returns></returns>
        public abstract object GetCustomProperty(string name);

        /// <summary>
        /// Creates the command.
        /// </summary>
        /// <param name="cmdType">Type of the CMD.</param>
        /// <returns></returns>
        public virtual ICommand CreateCommand(int cmdType)
        {
            CommandType ct = (CommandType)cmdType;
            switch (ct)
            {
                default:
                    return null;
            }
        }

        #endregion

        #region runtime map

        /// <summary>
        /// Infers the meters per unit value from the specified coordinate system
        /// </summary>
        /// <param name="csWkt"></param>
        /// <param name="units"></param>
        /// <returns></returns>
        protected virtual double InferMPU(string csWkt, double units)
        {
            var cs = CoordinateSystemBase.Create(csWkt);
            return cs.MetersPerUnitX * units;
        }

        /// <summary>
        /// Creates a new runtime map instance from an existing map definition. Meters per unit
        /// is calculated from the Coordinate System WKT of the map definition.
        /// </summary>
        /// <remarks>
        /// Calculation of meters-per-unit may differ between implementations. This may have an adverse
        /// effect on things such as rendering and measuring depending on the underlying implementation
        /// 
        /// If you are certain of the meters-per-unit value required, use the overloaded method that 
        /// accepts a metersPerUnit parameter.
        /// </remarks>
        /// <param name="runtimeMapResourceId"></param>
        /// <param name="baseMapDefinitionId"></param>
        /// <returns></returns>
        public RuntimeMap CreateMap(string runtimeMapResourceId, string baseMapDefinitionId)
        {
            var mdf = (IMapDefinition)GetResource(baseMapDefinitionId);
            var mpu = InferMPU(mdf.CoordinateSystem, 1.0);
            return CreateMap(runtimeMapResourceId, mdf, mpu);
        }

        /// <summary>
        /// Creates a new runtime map instance from an existing map definition
        /// </summary>
        /// <param name="runtimeMapResourceId"></param>
        /// <param name="baseMapDefinitionId"></param>
        /// <param name="metersPerUnit"></param>
        /// <returns></returns>
        public virtual RuntimeMap CreateMap(string runtimeMapResourceId, string baseMapDefinitionId, double metersPerUnit)
        {
            var mdf = (IMapDefinition)GetResource(baseMapDefinitionId);
            return CreateMap(runtimeMapResourceId, mdf, metersPerUnit);
        }

        /// <summary>
        /// Creates a new runtime map instance from an existing map definition. Meters per unit
        /// is calculated from the Coordinate System WKT of the map definition.
        /// </summary>
        /// <remarks>
        /// Calculation of meters-per-unit may differ between implementations. This may have an adverse
        /// effect on things such as rendering and measuring depending on the underlying implementation
        /// 
        /// If you are certain of the meters-per-unit value required, use the overloaded method that 
        /// accepts a metersPerUnit parameter.
        /// </remarks>
        /// <param name="runtimeMapResourceId"></param>
        /// <param name="mdf"></param>
        /// <returns></returns>
        public RuntimeMap CreateMap(string runtimeMapResourceId, IMapDefinition mdf)
        {
            var mpu = InferMPU(mdf.CoordinateSystem, 1.0);
            return CreateMap(runtimeMapResourceId, mdf, mpu);
        }

        /// <summary>
        /// Creates a new runtime map instance from an existing map definition
        /// </summary>
        /// <param name="runtimeMapResourceId"></param>
        /// <param name="mdf"></param>
        /// <param name="metersPerUnit"></param>
        /// <returns></returns>
        public virtual RuntimeMap CreateMap(string runtimeMapResourceId, IMapDefinition mdf, double metersPerUnit)
        {
            var map = new RuntimeMap(mdf, metersPerUnit);
            map.ResourceID = runtimeMapResourceId;
            return map;
        }

        /// <summary>
        /// Opens the specified runtime map
        /// </summary>
        /// <param name="runtimeMapResourceId"></param>
        /// <returns></returns>
        public virtual RuntimeMap OpenMap(string runtimeMapResourceId)
        {
            if (!runtimeMapResourceId.StartsWith("Session:" + this.SessionID + "//") || !runtimeMapResourceId.EndsWith(".Map"))
                throw new ArgumentException("Runtime maps must be in the current session repository");

            var map = new RuntimeMap(GetInterface());
            map.Deserialize(new MgBinaryDeserializer(this.GetResourceData(runtimeMapResourceId, "RuntimeData"), this.SiteVersion));
            if (this.SiteVersion >= SiteVersions.GetVersion(KnownSiteVersions.MapGuideOS1_2))
                map.DeserializeLayerData(new MgBinaryDeserializer(this.GetResourceData(runtimeMapResourceId, "LayerGroupData"), this.SiteVersion));

            return map;
        }
        #endregion
    }
}
