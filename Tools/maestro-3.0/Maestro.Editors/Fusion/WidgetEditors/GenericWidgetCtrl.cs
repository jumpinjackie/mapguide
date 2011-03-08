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
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using OSGeo.MapGuide.ObjectModels.ApplicationDefinition;

namespace Maestro.Editors.Fusion.WidgetEditors
{
    /// <summary>
    /// A generic widget editor that edits the underlying XML content
    /// </summary>
    public partial class GenericWidgetCtrl : UserControl, IWidgetEditor
    {
        public GenericWidgetCtrl()
        {
            InitializeComponent();
        }

        private string _xml;
        private IWidget _widget;

        public void Setup(IWidget widget, FlexibleLayoutEditorContext context, IEditorService edsvc)
        {
            _widget = widget;
            _xml = _widget.ToXml();
            txtXmlContent.Text = _xml;
        }

        public Control Content
        {
            get { return this; }
        }

        private void txtXmlContent_TextChanged(object sender, EventArgs e)
        {
            btnSave.Enabled = !(txtXmlContent.Text.Equals(_xml));
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.LoadXml(txtXmlContent.Text);

                //Base widget properties
                XmlNode node = doc.SelectSingleNode("//WidgetType/Name");
                if (node != null)
                    _widget.Name = node.InnerText;

                node = doc.SelectSingleNode("//WidgetType/Type");
                if (node != null)
                    _widget.Type = node.InnerText;

                node = doc.SelectSingleNode("//WidgetType/Location");
                if (node != null)
                    _widget.Location = node.InnerText;

                //Extension elements
                node = doc.SelectSingleNode("//WidgetType/Extension");
                if (node != null)
                {
                    List<XmlElement> elements = new List<XmlElement>();
                    foreach (XmlNode child in node.ChildNodes)
                    {
                        elements.Add(node[child.Name]);
                    }
                    _widget.Extension.Content = elements.ToArray();
                }

                //If a UI widget, set its properties
                var uiw = _widget as IUIWidget;
                if (uiw != null)
                {
                    node = doc.SelectSingleNode("//WidgetType/StatusItem");
                    if (node != null)
                        uiw.StatusText = node.InnerText;

                    node = doc.SelectSingleNode("//WidgetType/ImageUrl");
                    if (node != null)
                        uiw.ImageUrl = node.InnerText;

                    node = doc.SelectSingleNode("//WidgetType/ImageClass");
                    if (node != null)
                        uiw.ImageClass = node.InnerText;

                    node = doc.SelectSingleNode("//WidgetType/Tooltip");
                    if (node != null)
                        uiw.Tooltip = node.InnerText;

                    node = doc.SelectSingleNode("//WidgetType/Label");
                    if (node != null)
                        uiw.Label = node.InnerText;

                    node = doc.SelectSingleNode("//Widget/Disabled");
                    if (node != null)
                        uiw.Disabled = node.InnerText;
                }

                MessageBox.Show(Properties.Resources.WidgetUpdated);

                _xml = _widget.ToXml();
                txtXmlContent.Text = _xml;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }
}
