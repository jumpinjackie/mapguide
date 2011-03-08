﻿#region Disclaimer / License
// Copyright (C) 2011, Jackie Ng
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
using Maestro.Editors.Common;
using OSGeo.MapGuide.ObjectModels.FeatureSource;

namespace Maestro.Editors.FeatureSource.Providers.Gdal
{
    public partial class GdalProviderCtrl : EditorBindableCollapsiblePanel
    {
        public GdalProviderCtrl()
        {
            InitializeComponent();
            _comp = new CompositeFileCtrl();
            _sing = new SingleFileCtrl();
            _comp.Dock = DockStyle.Fill;
            _sing.Dock = DockStyle.Fill;
        }

        private IEditorService _service;
        private IFeatureSource _fs;

        private CompositeFileCtrl _comp;
        private SingleFileCtrl _sing;

        private bool _init = false;

        public override void Bind(IEditorService service)
        {
            try
            {
                _init = true;
                _service = service;
                _fs = (IFeatureSource)_service.GetEditedResource();
                _sing.Bind(service);
                _comp.Bind(service);
                if (!string.IsNullOrEmpty(_fs.GetConfigurationContent()))
                    rdComposite.Checked = true;
                else
                    OnTypeCheckedChanged(null, null); //It is already checked by default
            }
            finally
            {
                _init = false;
            }
        }

        private void OnTypeCheckedChanged(object sender, EventArgs e)
        {
            if (rdSingle.Checked)
            {
                panel1.Controls.Clear();
                panel1.Controls.Add(_sing);

                //_sing.InitDefaults();

                //if (!_init) //When switching modes, invalidate configuraton document as it is no longer valid
                //    _fs.SetConfigurationContent(null);
            }
            else if (rdComposite.Checked)
            {
                panel1.Controls.Clear();
                panel1.Controls.Add(_comp);

                //_comp.InitDefaults();

                //if (!_init) //When switching modes, invalidate configuraton document as it is no longer valid
                //    _fs.SetConfigurationContent(null);
            }
        }

        private void btnTest_Click(object sender, EventArgs e)
        {
            //Flush back to session before testing
            _service.SyncSessionCopy();
            string result = _fs.TestConnection();
            txtStatus.Text = string.Format(Properties.Resources.FdoConnectionStatus, result);
        }
    }
}
