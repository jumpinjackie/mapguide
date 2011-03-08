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
using OSGeo.MapGuide.MaestroAPI.Services;
using OSGeo.MapGuide.ObjectModels.Common;
using System.IO;
using Maestro.Shared.UI;
using OSGeo.MapGuide.MaestroAPI.Exceptions;
using OSGeo.MapGuide.MaestroAPI.Resource;
using OSGeo.MapGuide.MaestroAPI;

namespace Maestro.Editors.Common
{
    /// <summary>
    /// A control that manages attached resource data for a resource
    /// </summary>
    public partial class ResourceDataCtrl : UserControl
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="ResourceDataCtrl"/> class.
        /// </summary>
        public ResourceDataCtrl()
        {
            InitializeComponent();
            this.MarkEnabled = false;
        }

        /// <summary>
        /// Occurs when [data list changed].
        /// </summary>
        public event EventHandler DataListChanged;

        private void EvaluateCommands()
        {
            btnAdd.Enabled = (_edSvc != null);
            btnDelete.Enabled = (_edSvc != null && this.SelectedItem != null);
            btnDownload.Enabled = (_edSvc != null && this.SelectedItem != null);
            btnMark.Enabled = (this.SelectedItem != null);
        }

        private IEditorService _edSvc;

        private BindingList<ResourceDataListResourceData> _data;

        private ResourceDataListResourceData SelectedItem
        {
            get
            {
                if (lstDataFiles.SelectedItems.Count == 1)
                {
                    return lstDataFiles.SelectedItems[0].Tag as ResourceDataListResourceData;
                }
                return null;
            }
        }

        /// <summary>
        /// Raises the <see cref="E:System.Windows.Forms.UserControl.Load"/> event.
        /// </summary>
        /// <param name="e">An <see cref="T:System.EventArgs"/> that contains the event data.</param>
        protected override void OnLoad(EventArgs e)
        {
            EvaluateCommands();
        }

        private string _resourceID;

        /// <summary>
        /// Inits the specified service.
        /// </summary>
        /// <param name="service">The service.</param>
        public void Init(IEditorService service)
        {
            _edSvc = service;
            _resourceID = service.EditedResourceID;

            LoadResourceData();

            EvaluateCommands();
        }

        /// <summary>
        /// Gets or sets a value indicating whether [mark enabled].
        /// </summary>
        /// <value><c>true</c> if [mark enabled]; otherwise, <c>false</c>.</value>
        public bool MarkEnabled
        {
            get { return btnMark.Visible; }
            set { btnMark.Visible = value; }
        }

        /// <summary>
        /// Gets or sets the marked file.
        /// </summary>
        /// <value>The marked file.</value>
        public string MarkedFile
        {
            get
            {
                foreach (ListViewItem item in lstDataFiles.Items)
                {
                    if (item.Font.Bold)
                        return (item.Tag as ResourceDataListResourceData).Name;
                }
                return string.Empty;
            }
            set
            {
                MarkResourceDataAsSelected(value);
            }
        }

        private void LoadResourceData()
        {
            var list = _edSvc.ResourceService.EnumerateResourceData(_edSvc.EditedResourceID);
            _data = list.ResourceData;

            BindResourceList();
        }

        public delegate void ResourceUploadEventHandler(string dataName, string origPath);

        public event ResourceUploadEventHandler ResourceDataUploaded;

        private void btnAdd_Click(object sender, EventArgs e)
        {
            using (var open = DialogFactory.OpenFile())
            {
                open.Multiselect = true;
                if (open.ShowDialog() == DialogResult.OK)
                {
                    using (new WaitCursor(this))
                    {
                        try
                        {
                            foreach (var fileName in open.FileNames)
                            {
                                UploadFile(fileName);
                                var handler = this.ResourceDataUploaded;
                                if (handler != null)
                                    handler(Path.GetFileName(fileName), fileName);
                            }
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show(NestedExceptionMessageProcessor.GetFullMessage(ex), Properties.Resources.TitleError, MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }
                    }
                }
            }
        }

        public void UploadFile(string fileName)
        {
            using (var fs = new FileStream(fileName, FileMode.Open))
            {
                //_edSvc.AddResourceData(Path.GetFileName(open.FileName), ResourceDataType.File, fs);
                IResource res = _edSvc.GetEditedResource();
                res.SetResourceData(Path.GetFileName(fileName), ResourceDataType.File, fs);
                LoadResourceData();
                OnDataListChanged();
            }
        }

        private void OnDataListChanged()
        {
            var handler = this.DataListChanged;
            if (handler != null)
                handler(this, EventArgs.Empty);
        }

        private void btnDelete_Click(object sender, EventArgs e)
        {
            var item = this.SelectedItem;
            if (item != null)
            {
                if (MessageBox.Show(Properties.Resources.ConfirmDeleteResourceData, Properties.Resources.Confirm, MessageBoxButtons.YesNo) == DialogResult.No)
                    return;

                try
                {
                    using (new WaitCursor(this))
                    {
                        //_edSvc.RemoveResourceData(item.Name);
                        IResource res = _edSvc.GetEditedResource();
                        res.DeleteResourceData(item.Name);
                        _data.Remove(item);
                        BindResourceList();
                        OnDataListChanged();
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(NestedExceptionMessageProcessor.GetFullMessage(ex), Properties.Resources.TitleError, MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private Font _defaultFont;

        private void btnDownload_Click(object sender, EventArgs e)
        {
            var item = this.SelectedItem;
            if (item != null)
            {
                using (var save = DialogFactory.SaveFile())
                {
                    save.FileName = item.Name;
                    if (save.ShowDialog() == DialogResult.OK)
                    {
                        try
                        {
                            using (new WaitCursor(this))
                            {
                                //var stream = _edSvc.GetResourceData(item.Name);
                                IResource res = _edSvc.GetEditedResource();
                                var stream = res.GetResourceData(item.Name);
                                using (var fs = File.OpenWrite(save.FileName))
                                {
                                    Utility.CopyStream(stream, fs);
                                }
                            }
                            MessageBox.Show(string.Format(Properties.Resources.FileDownloaded, save.FileName));
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show(NestedExceptionMessageProcessor.GetFullMessage(ex), Properties.Resources.TitleError, MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }
                    }
                }
            }
        }

        private void btnMark_Click(object sender, EventArgs e)
        {
            if (lstDataFiles.SelectedItems.Count == 1)
            {
                MarkResourceDataAsSelected(lstDataFiles.SelectedItems[0].Name);
            }
        }

        private void MarkResourceDataAsSelected(string name)
        {
            ListViewItem item = null; 

            //Find matching item
            foreach (ListViewItem it in lstDataFiles.Items)
            {
                if (it.Name == name)
                {
                    item = it;
                    break;
                }
            }

            if (item != null)
            {
                //Restore original font
                foreach (ListViewItem it in lstDataFiles.Items)
                {
                    it.Font = new Font(_defaultFont, _defaultFont.Style);
                }

                //Bold the selected item
                var f = item.Font;
                item.Font = new Font(f, f.Style | FontStyle.Bold);

                OnResourceDataMarked(item.Name);
            }
        }

        private void BindResourceList()
        {
            lstDataFiles.Items.Clear();
            foreach (var f in _data)
            {
                var item = new ListViewItem();
                item.Tag = f;
                item.Text = f.Name;
                item.Name = f.Name;
                item.ToolTipText = f.Name;

                if (_defaultFont == null)
                    _defaultFont = item.Font;

                lstDataFiles.Items.Add(item);
            }
        }


        private void lstDataFiles_SelectedIndexChanged(object sender, EventArgs e)
        {
            EvaluateCommands();
        }

        /// <summary>
        /// Occurs when [resource data marked].
        /// </summary>
        public event ResourceDataSelectionEventHandler ResourceDataMarked;

        private void OnResourceDataMarked(string name)
        {
            var handler = this.ResourceDataMarked;
            if (handler != null)
                handler(this, name);
        }

        private void lstDataFiles_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                Array a = e.Data.GetData(DataFormats.FileDrop) as Array;
                if (a != null && a.Length > 0)
                {
                    e.Effect = DragDropEffects.Copy;
                }
                else
                    e.Effect = DragDropEffects.None;
            }
            else
                e.Effect = DragDropEffects.None;
        }

        private void lstDataFiles_DragDrop(object sender, DragEventArgs e)
        {
            Array a = e.Data.GetData(DataFormats.FileDrop) as Array;
            bool refresh = false;
            if (a != null && a.Length > 0)
            {
                for (int i = 0; i < a.Length; i++)
                {
                    string file = a.GetValue(i).ToString();
                    try
                    {
                        using (var fs = new FileStream(file, FileMode.Open))
                        {
                            IResource res = _edSvc.GetEditedResource();
                            res.SetResourceData(Path.GetFileName(file), ResourceDataType.File, fs);
                            refresh = true;
                        }
                    }
                    catch { }
                }
            }
            if (refresh)
            {
                LoadResourceData();
                OnDataListChanged();
            }
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public delegate void ResourceDataSelectionEventHandler(object sender, string dataName);
}
