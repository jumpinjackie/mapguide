﻿#region Disclaimer / License
// Copyright (C) 2010, Jackie Ng
// http://trac.osgeo.org/mapguide/wiki/maestro, jumpinjackie@gmail.com
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
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO;
using System.Text;
using OSGeo.MapGuide.MaestroAPI.CoordinateSystem;
using OSGeo.MapGuide.MaestroAPI.Mapping;
using OSGeo.MapGuide.MaestroAPI.Resource;
using OSGeo.MapGuide.MaestroAPI.Serialization;
using OSGeo.MapGuide.MaestroAPI.Services;
using OSGeo.MapGuide.ObjectModels.ApplicationDefinition;
using OSGeo.MapGuide.ObjectModels.Capabilities;
using OSGeo.MapGuide.ObjectModels.Common;
using OSGeo.MapGuide.ObjectModels.MapDefinition;
using OSGeo.MapGuide.MaestroAPI.Exceptions;
using System.Diagnostics;
using OSGeo.MapGuide.MaestroAPI.Commands;
using OSGeo.MapGuide.MaestroAPI.Native.Commands;
using OSGeo.MapGuide.MaestroAPI.Schema;
using OSGeo.MapGuide.MaestroAPI.Feature;

namespace OSGeo.MapGuide.MaestroAPI.Native
{
    public class LocalNativeConnection : MgServerConnectionBase, 
                                         IServerConnection,
                                         IFeatureService,
                                         IResourceService,
                                         ITileService,
                                         IMappingService,
                                         IDrawingService,
                                         ISiteService
    {
        private OSGeo.MapGuide.MgSiteConnection m_con;
        private string m_locale;
        private string m_sessionId;
        private Version m_siteVersion = null;

        /// <summary>
        /// The web config file
        /// </summary>
        protected string m_webconfig;

        public const string PARAM_SESSION = "SessionId";
        public const string PARAM_CONFIG = "ConfigFile";
        public const string PARAM_USERNAME = "Username";
        public const string PARAM_PASSWORD = "Password";
        public const string PARAM_LOCALE = "Locale";

        private LocalNativeConnection()
            : base()
        {
        }

        //This is the constructor used by ConnectionProviderRegistry.CreateConnection

        internal LocalNativeConnection(NameValueCollection initParams)
            : this()
        {
            if (initParams[PARAM_SESSION] != null)
            {
                string sessionid = initParams[PARAM_SESSION];

                InitConnection(sessionid);
            }
            else
            {
                if (initParams[PARAM_CONFIG] == null)
                    throw new ArgumentException("Missing connection parameter: " + PARAM_CONFIG);
                if (initParams[PARAM_PASSWORD] == null)
                    throw new ArgumentException("Missing connection parameter: " + PARAM_PASSWORD);
                if (initParams[PARAM_USERNAME] == null)
                    throw new ArgumentException("Missing connection parameter: " + PARAM_USERNAME);

                string configFile = initParams[PARAM_CONFIG];
                string password = initParams[PARAM_PASSWORD];
                string username = initParams[PARAM_USERNAME];
                string locale = null;
                if (initParams[PARAM_LOCALE] != null)
                    locale = initParams[PARAM_LOCALE];

                InitConnection(configFile, username, password, locale);
            }
        }

        public override string ProviderName
        {
            get { return "Maestro.LocalNative"; }
        }

        public override NameValueCollection CloneParameters
        {
            get
            {
                var nvc = new NameValueCollection();
                nvc[PARAM_CONFIG] = m_webconfig;
                nvc[CommandLineArguments.Provider] = this.ProviderName;
                nvc[CommandLineArguments.Session] = this.SessionID;
                return nvc;
            }
        }

        private void InitConnection(string sessionid)
        {
            MgUserInformation mgui = new MgUserInformation(sessionid);
            m_con = new MgSiteConnection();
            m_con.Open(mgui);
            m_sessionId = sessionid;
        }

        private void InitConnection(string configFile, string username, string password, string locale)
        {
            m_webconfig = configFile;
            m_username = username;
            m_password = password;
            m_locale = locale;

            OSGeo.MapGuide.MapGuideApi.MgInitializeWebTier(configFile);
            //Throws an exception if it fails
            RestartSession();
        }

        /// <summary>
        /// Returns a working copy of the site connection.
        /// </summary>
        internal MgSiteConnection Connection
        {
            get
            {
                //It seems that the connection 'forgets' that it is logged in.
                if (string.IsNullOrEmpty(m_con.GetSite().GetCurrentSession()))
                    m_con.Open(new MgUserInformation(this.SessionID));
                return m_con;
            }
        }

        public override string SessionID
		{
			get
			{
				return m_sessionId;
			}
		}

        protected override double InferMPU(string csWkt, double units)
        {
            try
            {
                MgCoordinateSystemFactory csFact = new MgCoordinateSystemFactory();
                MgCoordinateSystem cs = csFact.Create(csWkt);
                return cs.ConvertMetersToCoordinateSystemUnits(units);
            }
            catch (MgException ex) //Not implemented due to wrapping Proj.4?
            {
                ex.Dispose();
                return base.InferMPU(csWkt, units);
            }
            catch (Exception ex) //Binding to assembly with pre-refactored CS API?
            {
                return base.InferMPU(csWkt, units);
            }
        }

        private void LogMethodCall(string method, bool success, params string[] values)
        {
            OnRequestDispatched(method + "(" + string.Join(", ", values) + ") " + ((success) ? "Success" : "Failure"));
        }

		public override ResourceList GetRepositoryResources(string startingpoint, string type, int depth, bool computeChildren)
		{
			if (type == null)
				type = "";
			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			System.Reflection.MethodInfo mi = res.GetType().GetMethod("EnumerateResources", new Type[] { typeof(MgResourceIdentifier), typeof(int), typeof(string), typeof(bool) });
			var result = (ResourceList) base.DeserializeObject(typeof(ResourceList), Utility.MgStreamToNetStream(res, mi, new object[] {new MgResourceIdentifier(startingpoint), depth, type, computeChildren }));
            LogMethodCall("MgResourceService::EnumerateResources", true, startingpoint, depth.ToString(), type, computeChildren.ToString());
            return result;
		}

		public override FeatureProviderRegistryFeatureProvider[] FeatureProviders
		{
			get
			{
				MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
                var reg = (FeatureProviderRegistry)base.DeserializeObject(typeof(FeatureProviderRegistry), Utility.MgStreamToNetStream(fes, fes.GetType().GetMethod("GetFeatureProviders"), new object[] { }));
                LogMethodCall("MgFeatureService::GetFeatureProviders", true);
                return reg.FeatureProvider.ToArray();
			}
		}

		public string TestConnection(string providername, System.Collections.Specialized.NameValueCollection parameters)
		{
			MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
			System.Text.StringBuilder sb = new System.Text.StringBuilder();
			if (parameters != null)
			{
				foreach(System.Collections.DictionaryEntry de in parameters)
					sb.Append((string)de.Key + "=" + (string)de.Value + "\t");
				if (sb.Length > 0)
					sb.Length--;
			}
			var res = fes.TestConnection(providername, sb.ToString()) ? "True" : "Unspecified errors";
            LogMethodCall("MgFeatureService::TestConnection", true, providername, sb.ToString());
            return res;
		}

		public override string TestConnection(string featuresource)
		{
			MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
			var res = fes.TestConnection(new MgResourceIdentifier(featuresource)) ? "True" : "Unspecified errors";
            LogMethodCall("MgFeatureService::TestConnection", true, featuresource);
            return res;
		}

		public FdoProviderCapabilities GetProviderCapabilities(string provider)
		{
			MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
			var res = (FdoProviderCapabilities) base.DeserializeObject(typeof(FdoProviderCapabilities), Utility.MgStreamToNetStream(fes, fes.GetType().GetMethod("GetCapabilities"), new object[] { provider }));
            LogMethodCall("MgFeatureService::GetProviderCapabilities", true, provider);
            return res;
		}

		public override System.IO.Stream GetResourceData(string resourceID, string dataname)
		{
			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			var result = Utility.MgStreamToNetStream(res, res.GetType().GetMethod("GetResourceData"), new object[] { new MgResourceIdentifier(resourceID), dataname });
            LogMethodCall("MgResourceService::GetResourceData", true, resourceID, dataname);
            return result;
		}

		public override Stream GetResourceXmlData(string resourceID)
		{
			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
            var result = Utility.MgStreamToNetStream(res, res.GetType().GetMethod("GetResourceContent"), new object[] { new MgResourceIdentifier(resourceID) });
            LogMethodCall("MgResourceService::GetResourceContent", true, resourceID);
            return result;
		}

		public override void SetResourceXmlData(string resourceid, System.IO.Stream content, System.IO.Stream header)
		{
            bool exists = ResourceExists(resourceid);

			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;

            byte[] bufHeader = header == null ? new byte[0] : Utility.StreamAsArray(header);
            byte[] bufContent = content == null ? new byte[0] : Utility.StreamAsArray(content);
            MgByteReader rH = bufHeader.Length == 0 ? null : new MgByteReader(bufHeader, bufHeader.Length, "text/xml");
            MgByteReader rC = bufContent.Length == 0 ? null : new MgByteReader(bufContent, bufContent.Length, "text/xml");
            res.SetResource(new MgResourceIdentifier(resourceid), rC, rH);
            LogMethodCall("MgResourceService::SetResource", true, resourceid, "MgByteReader", "MgByteReader");
            if (exists)
                OnResourceUpdated(resourceid);
            else
                OnResourceAdded(resourceid);
		}

        public IReader ExecuteSqlQuery(string featureSourceID, string sql)
        {
            MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
            MgSqlDataReader reader = fes.ExecuteSqlQuery(new MgResourceIdentifier(featureSourceID), sql);
            LogMethodCall("MgFeatureService::ExecuteSqlQuery", true, featureSourceID, sql);
            return new LocalNativeSqlReader(reader);
        }

		public IFeatureReader QueryFeatureSource(string resourceID, string schema, string query)
		{
			return QueryFeatureSource(resourceID, schema, query, null);
		}

		public IFeatureReader QueryFeatureSource(string resourceID, string schema)
		{
			return QueryFeatureSource(resourceID, schema, null, null);
		}

        public IFeatureReader QueryFeatureSource(string resourceID, string schema, string query, string[] columns)
        {
            return QueryFeatureSource(resourceID, schema, query, columns, null);
        }

		public IFeatureReader QueryFeatureSource(string resourceID, string schema, string query, string[] columns, System.Collections.Specialized.NameValueCollection computedProperties)
		{
			MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
			MgFeatureQueryOptions mgf = new MgFeatureQueryOptions();
			if (query != null)
				mgf.SetFilter(query);

			if (columns != null && columns.Length != 0)
				foreach(string s in columns)
					mgf.AddFeatureProperty(s);

            if (computedProperties != null && computedProperties.Count > 0)
                foreach (string s in computedProperties.Keys)
                    mgf.AddComputedProperty(s, computedProperties[s]);

            MgFeatureReader mr = fes.SelectFeatures(new MgResourceIdentifier(resourceID), schema, mgf);

            LogMethodCall("MgFeatureService::SelectFeatures", true, resourceID, schema, "MgFeatureQueryOptions");

   			return new LocalNativeFeatureReader(mr);
		}

        private IReader AggregateQueryFeatureSourceCore(string resourceID, string schema, string query, string[] columns, System.Collections.Specialized.NameValueCollection computedProperties)
        {
            MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
            MgFeatureAggregateOptions mgf = new MgFeatureAggregateOptions();
            if (query != null)
                mgf.SetFilter(query);

            if (columns != null && columns.Length != 0)
                foreach (string s in columns)
                    mgf.AddFeatureProperty(s);

            if (computedProperties != null && computedProperties.Count > 0)
                foreach (string s in computedProperties.Keys)
                    mgf.AddComputedProperty(s, computedProperties[s]);

            var reader = fes.SelectAggregate(new MgResourceIdentifier(resourceID), schema, mgf);

            LogMethodCall("MgFeatureService::SelectAggregate", true, resourceID, schema, "MgFeatureAggregateOptions");

            return new LocalNativeDataReader(reader);
        }

        public override IReader AggregateQueryFeatureSource(string resourceID, string schema, string filter, string[] columns)
        {
            return AggregateQueryFeatureSourceCore(resourceID, schema, filter, columns, null);
        }

        public override IReader AggregateQueryFeatureSource(string resourceID, string schema, string filter, System.Collections.Specialized.NameValueCollection aggregateFunctions)
        {
            return AggregateQueryFeatureSourceCore(resourceID, schema, filter, null, aggregateFunctions);
        }


		public override FeatureSourceDescription DescribeFeatureSource(string resourceID)
		{
			MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
			System.IO.MemoryStream ms = new System.IO.MemoryStream(System.Text.Encoding.UTF8.GetBytes(fes.DescribeSchemaAsXml(new MgResourceIdentifier(resourceID), "")));

            LogMethodCall("MgFeatureService::DescribeSchemaAsXml", true, resourceID, "");

			return new FeatureSourceDescription(ms);
		}

        public override FeatureSchema DescribeFeatureSource(string resourceID, string schema)
		{
			MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
            if (schema != null && schema.IndexOf(":") > 0)
                schema = schema.Split(':')[0];
			System.IO.MemoryStream ms = new System.IO.MemoryStream(System.Text.Encoding.UTF8.GetBytes(fes.DescribeSchemaAsXml(new MgResourceIdentifier(resourceID), schema)));

            LogMethodCall("MgFeatureService::DescribeSchemaAsXml", true, resourceID, schema);

            return new FeatureSourceDescription(ms).Schemas[0];
		}
        
		public void DeleteResourceData(string resourceID, string dataname)
		{
			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			res.DeleteResourceData(new MgResourceIdentifier(resourceID), dataname);

            LogMethodCall("MgResourceService::DeleteResourceData", true, resourceID, dataname);
		}

		public ResourceDataList EnumerateResourceData(string resourceID)
		{
			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			System.IO.Stream ms = Utility.MgStreamToNetStream(res, res.GetType().GetMethod("EnumerateResourceData"), new object[] { new MgResourceIdentifier(resourceID) });

            LogMethodCall("MgResourceService::EnumerateResourceData", true, resourceID);
            
            return (ResourceDataList)DeserializeObject(typeof(ResourceDataList), ms);
		}

		public override void DeleteResource(string resourceID)
		{
			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			res.DeleteResource(new MgResourceIdentifier(resourceID));

            LogMethodCall("MgResourceService::DeleteResource", true, resourceID);

            OnResourceDeleted(resourceID);
		}

		public override Version SiteVersion
		{
			get
			{
                if (m_siteVersion != null)
                    return m_siteVersion;

                try
                {
                    MgSite site = m_con.GetSite();

                    MgServerAdmin amd = new MgServerAdmin();
                    amd.Open(new MgUserInformation(m_sessionId));

                    MgPropertyCollection col = amd.GetInformationProperties();

                    LogMethodCall("MgServerAdmin::GetInformationProperties", true);

                    for (int i = 0; i < col.Count; i++)
                    {
                        if (col[i].Name == "ServerVersion")
                        {
                            m_siteVersion = new Version(((MgStringProperty)col[i]).GetValue());
                            break;
                        }
                    }
                }
                catch
                {
                }


                //Default
                if (m_siteVersion == null)
                    m_siteVersion = SiteVersions.GetVersion(KnownSiteVersions.MapGuideOS1_2);

                return m_siteVersion;
            }
		}


        private ICoordinateSystemCatalog m_coordsys = null;
        //TODO: Figure out a strategy for cache invalidation 
        
        public ICoordinateSystemCatalog CoordinateSystemCatalog
		{
            get
            {
                if (this.SiteVersion < OSGeo.MapGuide.MaestroAPI.SiteVersions.GetVersion(OSGeo.MapGuide.MaestroAPI.KnownSiteVersions.MapGuideOS1_2))
                    return null;
                else
                {
                    if (m_coordsys == null)
                        m_coordsys = new LocalNativeCoordinateSystemCatalog(this);
                    return m_coordsys;
                }
            }
        }

		public string DisplayName
		{
			get
			{
				return this.Connection.GetSite().GetCurrentSiteAddress();
			}
		}

		public override ResourceReferenceList EnumerateResourceReferences(string resourceid)
		{
			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
            System.IO.Stream ms = Utility.MgStreamToNetStream(res, res.GetType().GetMethod("EnumerateReferences"), new object[] { new MgResourceIdentifier(resourceid) });

            LogMethodCall("MgResourceService::EnumerateReferences", true, resourceid);
            return (ResourceReferenceList)DeserializeObject(typeof(ResourceReferenceList), ms);
		}

		public override void CopyResource(string oldpath, string newpath, bool overwrite)
		{
            bool exists = ResourceExists(newpath);

			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			res.CopyResource(new MgResourceIdentifier(oldpath), new MgResourceIdentifier(newpath), overwrite);

            LogMethodCall("MgResourceService::CopyResource", true, oldpath, newpath, overwrite.ToString());

            if (exists)
                OnResourceUpdated(newpath);
            else
                OnResourceAdded(newpath);
		}

		public override void CopyFolder(string oldpath, string newpath, bool overwrite)
		{
            bool exists = ResourceExists(newpath);

			if (!oldpath.EndsWith("/"))
				oldpath += "/";
			if (!newpath.EndsWith("/"))
				newpath += "/";

			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			res.CopyResource(new MgResourceIdentifier(oldpath), new MgResourceIdentifier(newpath), overwrite);
            
            LogMethodCall("MgResourceService::CopyResource", true, oldpath, newpath, overwrite.ToString());

            if (exists)
                OnResourceUpdated(newpath);
            else
                OnResourceAdded(newpath);
		}

		public override void MoveResource(string oldpath, string newpath, bool overwrite)
		{
            bool exists = ResourceExists(newpath);

			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			res.MoveResource(new MgResourceIdentifier(oldpath), new MgResourceIdentifier(newpath), overwrite);

            LogMethodCall("MgResourceService::MoveResource", true, oldpath, newpath, overwrite.ToString());

            OnResourceDeleted(oldpath);
            if (exists)
                OnResourceUpdated(newpath);
            else
                OnResourceAdded(newpath);
		}

		public override void MoveFolder(string oldpath, string newpath, bool overwrite)
		{
            bool exists = ResourceExists(newpath);

			if (!oldpath.EndsWith("/"))
				oldpath += "/";
			if (!newpath.EndsWith("/"))
				newpath += "/";

			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			res.MoveResource(new MgResourceIdentifier(oldpath), new MgResourceIdentifier(newpath), overwrite);

            LogMethodCall("MgResourceService::MoveResource", true, oldpath, newpath, overwrite.ToString());

            OnResourceDeleted(oldpath);
            if (exists)
                OnResourceUpdated(newpath);
            else
                OnResourceAdded(newpath);
		}

		public override System.IO.Stream RenderRuntimeMap(string resourceId, double x, double y, double scale, int width, int height, int dpi, string format, bool clip)
		{
			MgRenderingService rnd = this.Connection.CreateService(MgServiceType.RenderingService) as MgRenderingService;
			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
			MgGeometryFactory gf = new MgGeometryFactory();

			string mapname = new ResourceIdentifier(resourceId).Path;

			MgMap map = new MgMap();
			map.Open(res, mapname);
			MgSelection sel = new MgSelection(map);
            //The color accepted by MgColor has alpha as the last value, but the returned has alpha first
			MgColor color = new MgColor(Utility.ParseHTMLColor(map.GetBackgroundColor()));

            //TODO: The render method is missing the overload for the Clip parameter
            object[] args = new object[] { map, sel, gf.CreateCoordinateXY(x, y), scale, width, height, color, format, true };
            Type[] types = new Type[] { args[0].GetType(), args[1].GetType(), args[2].GetType(), args[3].GetType(), args[4].GetType(), args[5].GetType(), args[6].GetType(), args[7].GetType(), args[8].GetType() };
			var result = Utility.MgStreamToNetStream(rnd, rnd.GetType().GetMethod("RenderMap", types), args);

            LogMethodCall("MgRenderingService::RenderMap", true, "MgMap", "MgSelection", "MgPoint("+ x + "," + y + ")", scale.ToString(), width.ToString(), height.ToString(), "MgColor", format, true.ToString());

            return result;
		}

        public override System.IO.Stream RenderRuntimeMap(string resourceId, double x1, double y1, double x2, double y2, int width, int height, int dpi, string format, bool clip)
        {
            MgRenderingService rnd = this.Connection.CreateService(MgServiceType.RenderingService) as MgRenderingService;
            MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
            MgGeometryFactory gf = new MgGeometryFactory();

            string mapname = new ResourceIdentifier(resourceId).Path;

            MgMap map = new MgMap();
            map.Open(res, mapname);
            MgSelection sel = new MgSelection(map);
            //The color accepted by MgColor has alpha as the last value, but the returned has alpha first
            MgColor color = new MgColor(Utility.ParseHTMLColor(map.GetBackgroundColor()));
            MgEnvelope env = new MgEnvelope(gf.CreateCoordinateXY(x1, y1), gf.CreateCoordinateXY(x2, y2));

            //TODO: The render is missing the clip param for the extent override method

            object[] args = new object[] { map, sel, env, width, height, color, format };
            Type[] types = new Type[] { args[0].GetType(), args[1].GetType(), args[2].GetType(), args[3].GetType(), args[4].GetType(), args[5].GetType(), args[6].GetType() };
            var result = Utility.MgStreamToNetStream(rnd, rnd.GetType().GetMethod("RenderMap", types), args);

            LogMethodCall("MgRenderingService::RenderMap", true, "MgMap", "MgSelection", "MgEnvelope", width.ToString(), height.ToString(), "MgColor", format);

            return result;
        }

        public override Stream RenderDynamicOverlay(RuntimeMap map, MapSelection selection, string format, bool keepSelection)
        {
            MgRenderingService rnd = this.Connection.CreateService(MgServiceType.RenderingService) as MgRenderingService;
            MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;

            MgMap mmap = new MgMap();
            mmap.Open(res, map.Name);
            MgSelection sel = new MgSelection(mmap);
            if (selection != null)
                sel.FromXml(selection.ToXml());

            object[] args = new object[] { mmap, sel, format, keepSelection };
            Type[] types = new Type[] { args[0].GetType(), args[1].GetType(), args[2].GetType(), args[3].GetType() };
            var result = Utility.MgStreamToNetStream(rnd, rnd.GetType().GetMethod("RenderDynamicOverlay", types), args);

            LogMethodCall("MgRenderingService::RenderDynamicOverlay", true, "MgMap", "MgSelection", format, keepSelection.ToString());

            return result;
        }

		public override bool IsSessionExpiredException(Exception ex)
		{
			return ex != null && ex.GetType() == typeof(OSGeo.MapGuide.MgSessionExpiredException) ||  ex.GetType() == typeof(OSGeo.MapGuide.MgSessionNotFoundException);
		}

		/// <summary>
		/// Returns the spatial info for a given featuresource
		/// </summary>
		/// <param name="resourceID">The ID of the resource to query</param>
		/// <param name="activeOnly">Query only active items</param>
		/// <returns>A list of spatial contexts</returns>
		public override FdoSpatialContextList GetSpatialContextInfo(string resourceID, bool activeOnly)
		{
			MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
			MgSpatialContextReader rd = fes.GetSpatialContexts(new MgResourceIdentifier(resourceID), activeOnly);

            LogMethodCall("MgFeatureService::GetSpatialContexts", true, resourceID, activeOnly.ToString());

            return this.DeserializeObject(typeof(FdoSpatialContextList), Utility.MgStreamToNetStream(rd, rd.GetType().GetMethod("ToXml"), null)) as FdoSpatialContextList;
		}

		/// <summary>
		/// Gets the names of the identity properties from a feature
		/// </summary>
		/// <param name="resourceID">The resourceID for the FeatureSource</param>
		/// <param name="classname">The classname of the feature, including schema</param>
		/// <returns>A string array with the found identities</returns>
		public override string[] GetIdentityProperties(string resourceID, string classname)
		{
			MgFeatureService fes = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
			string[] parts = classname.Split(':');
            MgResourceIdentifier resId = new MgResourceIdentifier(resourceID);
			MgPropertyDefinitionCollection props;
		    if (parts.Length == 1)
				parts = new string[] { classname };
			else if (parts.Length != 2)
				throw new Exception("Unable to parse classname into class and schema: " + classname);

            var classes = fes.DescribeSchema(resId, parts[0])[0].GetClasses();

            LogMethodCall("MgFeatureService::DescribeSchema", true, resourceID, parts[0]);

            foreach (MgClassDefinition cdef in classes)
            {
                if (parts.Length == 1 || cdef.Name.ToLower().Trim().Equals(parts[1].ToLower().Trim()))
                {
                    props = cdef.GetIdentityProperties();

                    string[] res = new string[props.Count];
                    for (int i = 0; i < props.Count; i++)
                        res[i] = (props[i] as MgProperty).Name;

                    return res;
                }
            }

            throw new Exception("Unable to find class: " + parts[1] + " in schema " + parts[0]);
		}

		/// <summary>
		/// Restarts the server session, and creates a new session ID
		/// </summary>
		/// <param name="throwException">If set to true, the call throws an exception if the call failed</param>
		/// <returns>True if the creation succeed, false otherwise</returns>
		public override bool RestartSession(bool throwException)
		{
			if (m_username == null || m_password == null)
				if (throwException)
					throw new Exception("Cannot recreate session, because connection was not opened with username and password");
				else
					return false;

			try
			{
				MgUserInformation mgui = new MgUserInformation(m_username, m_password);
				if (m_locale != null)
					mgui.SetLocale(m_locale);
				else
					mgui.SetLocale("en");
				MgSiteConnection con = new MgSiteConnection(); 
				con.Open(mgui);
				string s = con.GetSite().CreateSession();
				if (s == null || s.Trim().Length == 0)
					throw new Exception("Failed to retrieve new session identifier");

				m_sessionId = s;
				m_con = con;
				return true;
			}
			catch
			{
				if (throwException)
					throw;
				else
					return false;
			}
		}

		/// <summary>
		/// Enumerates all unmanaged folders, meaning alias'ed folders
		/// </summary>
		/// <param name="type">The type of data to return</param>
		/// <param name="filter">A filter applied to the items</param>
		/// <param name="recursive">True if the list should contains recursive results</param>
		/// <param name="startpath">The path to retrieve the data from</param>
		/// <returns>A list of unmanaged data</returns>
		public override UnmanagedDataList EnumerateUnmanagedData(string startpath, string filter, bool recursive, UnmanagedDataTypes type)
		{
			throw new MissingMethodException();
		}

        public override void UpdateRepository(string resourceId, ResourceFolderHeaderType header)
        {
            MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;

            if (header == null)
            {
                res.UpdateRepository(new MgResourceIdentifier(resourceId), null, null);
                LogMethodCall("MgResourceService::UpdateRepository", true, resourceId, "null", "null");
            }
            else
            {
                byte[] data = this.SerializeObject(header).ToArray();
                MgByteReader rd = new MgByteReader(data, data.Length, "text/xml");
                res.UpdateRepository(new MgResourceIdentifier(resourceId), null, rd);

                LogMethodCall("MgResourceService::UpdateRepository", true, resourceId, "null", "MgByteReader");
            }
        }

        public override object GetFolderOrResourceHeader(string resourceID)
        {
			MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
            if (ResourceIdentifier.IsFolderResource(resourceID))
                return this.DeserializeObject<ResourceFolderHeaderType>(Utility.MgStreamToNetStream(res, res.GetType().GetMethod("GetResourceHeader"), new object[] { new MgResourceIdentifier(resourceID) }));
            else
                return this.DeserializeObject<ResourceDocumentHeaderType>(Utility.MgStreamToNetStream(res, res.GetType().GetMethod("GetResourceHeader"), new object[] { new MgResourceIdentifier(resourceID) }));
        }


        /// <summary>
        /// Gets a list of users in a group
        /// </summary>
        /// <param name="group">The group to retrieve the users from</param>
        /// <returns>The list of users</returns>
        public override UserList EnumerateUsers(string group)
        {
            if (m_cachedUserList == null)
            {
                m_cachedUserList = this.DeserializeObject<UserList>(Utility.MgStreamToNetStream(this.Connection.GetSite(), this.Connection.GetSite().GetType().GetMethod("EnumerateUsers"), new object[] { group }));
                LogMethodCall("MgSite::EnumerateUsers", true, group);
            }
            return m_cachedUserList;
        }

        /// <summary>
        /// Gets a list of all groups on the server
        /// </summary>
        /// <returns>The list of groups</returns>
        public override GroupList EnumerateGroups()
        {
            if (m_cachedGroupList == null)
            {
                m_cachedGroupList = this.DeserializeObject<GroupList>(Utility.MgStreamToNetStream(this.Connection.GetSite(), this.Connection.GetSite().GetType().GetMethod("EnumerateGroups"), null));
                LogMethodCall("MgSite::EnumerateGroups", true);
            }
            return m_cachedGroupList;
        }

        public override System.IO.Stream GetTile(string mapdefinition, string baselayergroup, int col, int row, int scaleindex, string format)
        {
            MgTileService ts = this.Connection.CreateService(MgServiceType.TileService) as MgTileService;

            Type[] types = new Type[] { typeof(MgResourceIdentifier), typeof(string), typeof(int), typeof(int), typeof(int) };

            var result = Utility.MgStreamToNetStream(ts, ts.GetType().GetMethod("GetTile", types), new object[] { new MgResourceIdentifier(mapdefinition), baselayergroup, col, row, scaleindex });
            LogMethodCall("MgTileService::GetTile", true, mapdefinition, baselayergroup, col.ToString(), row.ToString(), scaleindex.ToString());
            return result;
            //ts.GetTile(new MgResourceIdentifier(mapdefinition), baselayergroup, col, row, scaleindex)
        }

        public override bool ResourceExists(string resourceid)
        {
            //API is safe to call in MG 2.1 and newer
            if (this.SiteVersion >= new Version(2, 1))
            {
                MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
                var result = res.ResourceExists(new MgResourceIdentifier(resourceid));
                LogMethodCall("MgResourceService::ResourceExists", true, resourceid);
                return result;
            }
            else
            {
                return base.ResourceExists(resourceid);
            }
        }

        public string[] GetConnectionPropertyValues(string providerName, string propertyName, string partialConnectionString)
        {
            MgFeatureService featSvc = this.Connection.CreateService(MgServiceType.FeatureService) as MgFeatureService;
            MgStringCollection result = featSvc.GetConnectionPropertyValues(providerName, propertyName, partialConnectionString);
            LogMethodCall("MgFeatureService::GetConnectionPropertyValues", true, providerName, propertyName, partialConnectionString);
            string[] values = new string[result.GetCount()];
            for (int i = 0; i < result.GetCount(); i++)
            {
                values[i] = result.GetItem(i);
            }
            return values;
        }

        public bool SupportsResourcePreviews
        {
            get { return false; }
        }

        public override void Dispose()
        {
            if (m_con != null)
            {
                m_con.Dispose();
                m_con = null;
            }
        }

        /// <summary>
        /// Renders a minature bitmap of the layers style
        /// </summary>
        /// <param name="scale">The scale for the bitmap to match</param>
        /// <param name="layerdefinition">The layer the image should represent</param>
        /// <param name="themeIndex">If the layer is themed, this gives the theme index, otherwise set to 0</param>
        /// <param name="type">The geometry type, 1 for point, 2 for line, 3 for area, 4 for composite</param>
        /// <returns>The minature bitmap</returns>
        public override System.Drawing.Image GetLegendImage(double scale, string layerdefinition, int themeIndex, int type)
        {
            MgMappingService ms = this.Connection.CreateService(MgServiceType.MappingService) as MgMappingService;
            MgResourceIdentifier ldef = new MgResourceIdentifier(layerdefinition);

            var bmp = new System.Drawing.Bitmap(Utility.MgStreamToNetStream(ms, ms.GetType().GetMethod("GenerateLegendImage"), new object[] { ldef, scale, 16, 16, "PNG", type, themeIndex }));
            LogMethodCall("MgMappingService::GetLegendImage", true, scale.ToString(), layerdefinition, themeIndex.ToString(), type.ToString());
            return bmp;
        }

        public OSGeo.MapGuide.MaestroAPI.Services.IFeatureService FeatureService
        {
            get { return this; }
        }

        public OSGeo.MapGuide.MaestroAPI.Services.IResourceService ResourceService
        {
            get { return this; }
        }

        public override OSGeo.MapGuide.MaestroAPI.Commands.ICommand CreateCommand(int cmdType)
        {
            CommandType ct = (CommandType)cmdType;
            if (ct == CommandType.GetResourceContents)
                return new LocalGetResourceContents(this);
            return base.CreateCommand(cmdType);
        }

        private IConnectionCapabilities _caps;

        public IConnectionCapabilities Capabilities
        {
            get 
            {
                if (_caps == null)
                {
                    _caps = new LocalNativeCapabilities(this);
                }
                return _caps;
            }
        }

        public OSGeo.MapGuide.MaestroAPI.Services.IService GetService(int serviceType)
        {
            ServiceType st = (ServiceType)serviceType;
            switch (st)
            {
                case ServiceType.Drawing:
                case ServiceType.Feature:
                case ServiceType.Mapping:
                case ServiceType.Resource:
                case ServiceType.Tile:
                case ServiceType.Site:
                    return this;
                case ServiceType.Fusion:
                    if (this.SiteVersion >= new Version(2, 0))
                        return this;
                    break;
            }
            throw new UnsupportedServiceTypeException(st);
        }

        protected override IServerConnection GetInterface()
        {
            return this;
        }

        public override void SetResourceData(string resourceid, string dataname, ResourceDataType datatype, Stream stream, OSGeo.MapGuide.MaestroAPI.Utility.StreamCopyProgressDelegate callback)
        {
            byte[] data = Utility.StreamAsArray(stream);
            if (callback != null)
                callback(0, data.Length, data.Length);
            MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
            MgByteReader reader = new MgByteReader(data, data.Length, "binary/octet-stream");
            res.SetResourceData(new MgResourceIdentifier(resourceid), dataname, datatype.ToString(), reader);
            LogMethodCall("MgResourceService::SetResourceData", true, resourceid, dataname, datatype.ToString(), "MgByteReader");
            if (callback != null)
                callback(data.Length, 0, data.Length);
        }

        public override void UploadPackage(string filename, OSGeo.MapGuide.MaestroAPI.Utility.StreamCopyProgressDelegate callback)
        {
            System.IO.FileInfo fi = new System.IO.FileInfo(filename);

            if (callback != null)
                callback(0, fi.Length, fi.Length);

            MgResourceService res = this.Connection.CreateService(MgServiceType.ResourceService) as MgResourceService;
            MgByteSource pkgSource = new MgByteSource(filename);
            MgByteReader rd = pkgSource.GetReader();
            res.ApplyResourcePackage(rd);
            rd.Dispose();
            LogMethodCall("MgResourceService::ApplyResourcePackage", true, "MgByteReader");

            if (callback != null)
                callback(fi.Length, 0, fi.Length);
        }

        public override string[] GetCustomPropertyNames()
        {
            return new string[] { };
        }

        public override Type GetCustomPropertyType(string name)
        {
            throw new CustomPropertyNotFoundException();
        }

        public override void SetCustomProperty(string name, object value)
        {
            throw new CustomPropertyNotFoundException();
        }

        public override object GetCustomProperty(string name)
        {
            throw new CustomPropertyNotFoundException();
        }

        public override DataStoreList EnumerateDataStores(string providerName, string partialConnString)
        {
            var fes = (MgFeatureService)this.Connection.CreateService(MgServiceType.FeatureService);
            var list = (DataStoreList)base.DeserializeObject(typeof(DataStoreList), Utility.MgStreamToNetStream(fes, fes.GetType().GetMethod("EnumerateDataStores"), new object[] { providerName, partialConnString }));
            LogMethodCall("MgFeatureService::EnumerateDataStores", true, providerName, partialConnString);
            return list;
        }

        public override string[] GetSchemas(string resourceId)
        {
            List<string> names = new List<string>();
            var fsvc = (MgFeatureService)this.Connection.CreateService(MgServiceType.FeatureService);
            var schemaNames = fsvc.GetSchemas(new MgResourceIdentifier(resourceId));
            LogMethodCall("MgFeatureService::GetSchemas", true, resourceId);
            for (int i = 0; i < schemaNames.GetCount(); i++)
            {
                names.Add(schemaNames.GetItem(i));
            }
            return names.ToArray();
        }

        public override string[] GetClassNames(string resourceId, string schemaName)
        {
            List<string> names = new List<string>();
            var fsvc = (MgFeatureService)this.Connection.CreateService(MgServiceType.FeatureService);
            var classNames = fsvc.GetClasses(new MgResourceIdentifier(resourceId), schemaName);
            LogMethodCall("MgFeatureService::GetClasses", true, resourceId, schemaName);
            for (int i = 0; i < classNames.GetCount(); i++)
            {
                names.Add(classNames.GetItem(i));
            }
            return names.ToArray();
        }

        public override IServerConnection Clone()
        {
            var initP = new NameValueCollection();
            initP[PARAM_SESSION] = this.SessionID;
            return new LocalNativeConnection(initP);
        }

        private MgServerAdmin _admin;

        internal MgServerAdmin ServerAdmin
        {
            get
            {
                if (_admin == null)
                {
                    _admin = new MgServerAdmin();
                    _admin.Open(new MgUserInformation(this.SessionID));
                }

                return _admin;
            }
        }

        public override SiteInformation GetSiteInfo()
        {
            var info = new SiteInformation();
            info.SiteServer = new SiteInformationSiteServer();
            info.SiteServer.OperatingSystem = new SiteInformationSiteServerOperatingSystem();
            info.Statistics = new SiteInformationStatistics();

            var props = this.ServerAdmin.GetInformationProperties();
            LogMethodCall("MgServerAdmin::GetInformationProperties", true);

            var prop = props.GetItem(MgServerInformationProperties.DisplayName);
            info.SiteServer.DisplayName = ((MgStringProperty)prop).GetValue();

            prop = props.GetItem(MgServerInformationProperties.OperatingSystemVersion);
            info.SiteServer.OperatingSystem.Version = ((MgStringProperty)prop).GetValue();

            prop = props.GetItem(MgServerInformationProperties.TotalPhysicalMemory);
            info.SiteServer.OperatingSystem.TotalPhysicalMemory = ((MgInt64Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.TotalVirtualMemory);
            info.SiteServer.OperatingSystem.TotalVirtualMemory = ((MgInt64Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.AvailablePhysicalMemory);
            info.SiteServer.OperatingSystem.AvailablePhysicalMemory = ((MgInt64Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.AvailableVirtualMemory);
            info.SiteServer.OperatingSystem.AvailableVirtualMemory = ((MgInt64Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.Status);
            info.SiteServer.Status = ((MgBooleanProperty)prop).GetValue() ? "Online" : "Offline";

            prop = props.GetItem(MgServerInformationProperties.ServerVersion);
            info.SiteServer.Version = ((MgStringProperty)prop).GetValue();

            prop = props.GetItem(MgServerInformationProperties.TotalActiveConnections);
            info.Statistics.ActiveConnections = ((MgInt32Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.AdminOperationsQueueCount);
            info.Statistics.AdminOperationsQueueCount = ((MgInt32Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.AverageOperationTime);
            info.Statistics.AverageOperationTime = ((MgInt64Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.ClientOperationsQueueCount);
            info.Statistics.ClientOperationsQueueCount = ((MgInt32Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.CpuUtilization);
            info.Statistics.CpuUtilization = ((MgInt32Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.SiteOperationsQueueCount);
            info.Statistics.SiteOperationsQueueCount = ((MgInt32Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.TotalConnections);
            info.Statistics.TotalConnections = ((MgInt32Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.TotalProcessedOperations);
            info.Statistics.TotalOperationsProcessed = ((MgInt32Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.TotalReceivedOperations);
            info.Statistics.TotalOperationsReceived = ((MgInt32Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.TotalOperationTime);
            info.Statistics.TotalOperationTime = ((MgInt64Property)prop).GetValue().ToString();

            prop = props.GetItem(MgServerInformationProperties.Uptime);
            info.Statistics.Uptime = ((MgInt64Property)prop).GetValue().ToString();

            return info;
        }

        public Stream DescribeDrawing(string resourceID)
        {
            var dwSvc = (MgDrawingService)this.Connection.CreateService(MgServiceType.DrawingService);
            var result = Utility.MgStreamToNetStream(dwSvc, dwSvc.GetType().GetMethod("DescribeDrawing"), new object[] { new MgResourceIdentifier(resourceID) });
            LogMethodCall("MgDrawingService::DescribeDrawing", true, resourceID);
            return result;
        }

        public string[] EnumerateDrawingLayers(string resourceID, string sectionName)
        {
            var dwSvc = (MgDrawingService)this.Connection.CreateService(MgServiceType.DrawingService);
            var layers = dwSvc.EnumerateLayers(new MgResourceIdentifier(resourceID), sectionName);
            LogMethodCall("MgDrawingService::EnumerateLayers", true, resourceID, sectionName);
            var layerNames = new List<string>();
            for (int i = 0; i < layers.GetCount(); i++)
            {
                layerNames.Add(layers.GetItem(i));
            }
            return layerNames.ToArray();
        }

        public DrawingSectionResourceList EnumerateDrawingSectionResources(string resourceID, string sectionName)
        {
            var dwSvc = (MgDrawingService)this.Connection.CreateService(MgServiceType.DrawingService);
            var list = base.DeserializeObject<DrawingSectionResourceList>(Utility.MgStreamToNetStream(dwSvc, dwSvc.GetType().GetMethod("EnumerateDrawingSectionResources"), new object[] { new MgResourceIdentifier(resourceID), sectionName }));
            LogMethodCall("MgDrawingService::EnumerateDrawingSectionResources", true, resourceID, sectionName);
            return list;
        }

        public DrawingSectionList EnumerateDrawingSections(string resourceID)
        {
            var dwSvc = (MgDrawingService)this.Connection.CreateService(MgServiceType.DrawingService);
            var list = base.DeserializeObject<DrawingSectionList>(Utility.MgStreamToNetStream(dwSvc, dwSvc.GetType().GetMethod("EnumerateDrawingSections"), new object[] { new MgResourceIdentifier(resourceID) }));
            LogMethodCall("MgDrawingService::EnumerateDrawingSections", true, resourceID);
            return list;
        }

        public string GetDrawingCoordinateSpace(string resourceID)
        {
            var dwSvc = (MgDrawingService)this.Connection.CreateService(MgServiceType.DrawingService);
            var res = dwSvc.GetCoordinateSpace(new MgResourceIdentifier(resourceID));
            LogMethodCall("MgDrawingService::GetCoordinateSpace", true, resourceID);
            return res;
        }

        public Stream GetDrawing(string resourceID)
        {
            var dwSvc = (MgDrawingService)this.Connection.CreateService(MgServiceType.DrawingService);
            var res = Utility.MgStreamToNetStream(dwSvc, dwSvc.GetType().GetMethod("GetDrawing"), new object[] { new MgResourceIdentifier(resourceID) });
            LogMethodCall("MgDrawingService::GetDrawing", true, resourceID);
            return res;
        }

        public Stream GetLayer(string resourceID, string sectionName, string layerName)
        {
            var dwSvc = (MgDrawingService)this.Connection.CreateService(MgServiceType.DrawingService);
            var res = Utility.MgStreamToNetStream(dwSvc, dwSvc.GetType().GetMethod("GetLayer"), new object[] { new MgResourceIdentifier(resourceID), sectionName, layerName });
            LogMethodCall("MgDrawingService::GetLayer", true, resourceID, sectionName, layerName);
            return res;
        }

        public Stream GetSection(string resourceID, string sectionName)
        {
            var dwSvc = (MgDrawingService)this.Connection.CreateService(MgServiceType.DrawingService);
            var res = Utility.MgStreamToNetStream(dwSvc, dwSvc.GetType().GetMethod("GetSection"), new object[] { new MgResourceIdentifier(resourceID), sectionName });
            LogMethodCall("MgDrawingService::GetSection", true, resourceID, sectionName);
            return res;
        }

        public Stream GetSectionResource(string resourceID, string resourceName)
        {
            var dwSvc = (MgDrawingService)this.Connection.CreateService(MgServiceType.DrawingService);
            var res = Utility.MgStreamToNetStream(dwSvc, dwSvc.GetType().GetMethod("GetSectionResource"), new object[] { new MgResourceIdentifier(resourceID), resourceName });
            LogMethodCall("MgDrawingService::GetSectionResource", true, resourceID, resourceName);
            return res;
        }
    }
}
