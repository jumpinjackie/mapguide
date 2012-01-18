using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using DotNetHarness.Resource;
using DotNetHarness.Rendering;
using DotNetHarness.Feature;
using DotNetHarness.Tile;

namespace DotNetHarness
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void lstApiCategories_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lstApiCategories.SelectedItem == null)
                return;

            lstOperations.DataSource = GetApiOperationsForCategory(lstApiCategories.SelectedItem.ToString());
        }

        private string[] GetApiOperationsForCategory(string category)
        {
            string[] ops = new string[0];
            switch (category)
            {
                case "Resource":
                    ops = new string[] { "ApplyResourcePackage", "EnumerateResources", "GetResourceContent", "SetResource", "SetResourceData", "EnumerateResourceData", "GetResourceData", "ResourceExists", "DeleteResource", "CopyResource", "MoveResource", "DeleteResourceData", "RenameResourceData", "EnumerateUnmanagedData" };
                    break;
                case "Feature":
                    ops = new string[] { "CreateFeatureSource", "RegisterProvider", "SelectFeatures", "SelectAggregates", "GetFeatureProviders", "GetConnectPropertyValues", "GetCapabilities", "DescribeSchema", "GetSchemas", "GetClasses", "GetSpatialContexts", "ExecuteSql", "ExecuteSqlNonQuery", "EnumerateDataStores", "TestConnection", "GetSchemaMapping" };
                    break;
                case "Rendering":
                    ops = new string[] { "RenderMap", "RenderDynamicOverlay", "RenderTile" };
                    break;
                case "Tile":
                    ops = new string[] { "GetTile", "ClearTileCache" };
                    break;
            }
            return ops;
        }

        private void lstOperations_SelectedIndexChanged(object sender, EventArgs e)
        {
            string cat = lstApiCategories.SelectedItem.ToString();
            string op = lstOperations.SelectedItem.ToString();

            Control c = GetControlForOperation(cat, op);
            splitContainer1.Panel2.Controls.Clear();
            c.Dock = DockStyle.Fill;
            splitContainer1.Panel2.Controls.Add(c);
        }

        private Control GetControlForOperation(string cat, string op)
        {
            if (cat == "Resource")
            {
                switch (op)
                {
                    case "ApplyResourcePackage":
                        return new ApplyResourcePackageControl();
                    case "ResourceExists":
                        return new ResourceExistsControl();
                    case "EnumerateResourceData":
                        return new EnumerateResourceDataControl();
                    case "EnumerateResources":
                        return new EnumerateResourcesControl();
                    case "GetResourceContent":
                        return new GetResourceContentControl();
                    case "GetResourceContents":
                        return new MultiGetResourceContentControl();
                    case "SetResource":
                        return new SetResourceControl();
                    case "SetResourceData":
                        return new SetResourceDataControl();
                    case "GetResourceData":
                        return new GetResourceDataControl();
                    case "CopyResource":
                        return new CopyResourceControl();
                    case "MoveResource":
                        return new MoveResourceControl();
                    case "DeleteResource":
                        return new DeleteResourceControl();
                    case "EnumerateUnmanagedData":
                        return new EnumerateUnmanagedDataControl();
                }
            }
            else if (cat == "Feature")
            {
                switch (op)
                { 
                    case "CreateFeatureSource":
                        return new CreateFeatureSourceCtrl();
                    case "RegisterProvider":
                        return new RegisterProviderCtrl();
                    case "SelectFeatures":
                        return new SelectFeaturesControl();
                    case "SelectAggregates":
                        return new SelectAggregatesControl();
                    case "GetSpatialContexts":
                        return new GetSpatialContextsControl();
                    case "GetFeatureProviders":
                        return new GetFeatureProvidersControl();
                    case "GetCapabilities":
                        return new GetCapabilitiesControl();
                    case "GetSchemaMapping":
                        return new GetSchemaMappingCtrl();
                    case "GetSchemas":
                        return new GetSchemasControl();
                    case "GetClasses":
                        return new GetClassesControl();
                    case "ExecuteSql":
                        return new ExecuteSqlControl();
                    case "ExecuteSqlNonQuery":
                        return new ExecuteSqlNonQueryControl();
                    case "TestConnection":
                        return new TestConnectionControl();
                    case "EnumerateDataStores":
                        return new EnumerateDataStoresControl();
                }
            }
            else if (cat == "Rendering")
            {
                switch (op)
                { 
                    case "RenderMap":
                        return new RenderMapControl();
                    case "RenderDynamicOverlay":
                        return new RenderDynamicOverlayControl();
                    case "RenderTile":
                        return new RenderTileControl();
                }
            }
            else if (cat == "Tile")
            {
                switch (op)
                {
                    case "GetTile":
                        return new GetTileControl();
                }
            }

            return new Control();
        }
    }
}
