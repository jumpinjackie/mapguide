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
using Maestro.Editors;
using OSGeo.MapGuide.MaestroAPI.Resource;
using Maestro.Editors.Fusion;

namespace Maestro.Base.Editor
{
    public partial class FusionEditor: EditorContentBase
    {
        public FusionEditor()
        {
            InitializeComponent();
        }

        private IResource _res;
        private IEditorService _edsvc;

        protected override void Bind(IEditorService service)
        {
            _edsvc = service;
            _res = _edsvc.GetEditedResource();

            flexEditor.Bind(service);
        }
    }
}
