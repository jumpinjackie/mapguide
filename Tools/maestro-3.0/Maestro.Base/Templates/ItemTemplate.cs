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
using System.Text;
using OSGeo.MapGuide.MaestroAPI.Services;
using Res = Maestro.Base.Properties.Resources;
using System.Drawing;
using OSGeo.MapGuide.MaestroAPI.Resource;
using OSGeo.MapGuide.MaestroAPI;

namespace Maestro.Base.Templates
{
    public abstract class ItemTemplate
    {
        public string Name { get; set; }

        public string Description { get; set; }

        public string Category { get; protected set; }

        public string ResourceType { get; protected set; }

        public Image Icon { get; protected set; }

        /// <summary>
        /// Gets the minimum site version this template is applicable to
        /// </summary>
        public virtual Version MinimumSiteVersion
        {
            get
            {
                return new Version(1, 0);
            }
        }

        /// <summary>
        /// Creates a new item from this template
        /// </summary>
        /// <param name="conn">The server connection</param>
        /// <returns>a new resource, null if the user cancelled during this process</returns>
        public abstract IResource CreateItem(IServerConnection conn);
    }
}
