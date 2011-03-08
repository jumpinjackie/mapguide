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
using ICSharpCode.Core;
using OSGeo.MapGuide.MaestroAPI;
using Maestro.Base.Services;

namespace Maestro.Base.Commands.SiteExplorer
{
    internal class OpenResourceCommand : AbstractMenuCommand
    {
        private IServerConnection _conn;

        public override void Run()
        {
            var wb = Workbench.Instance;
            var exp = wb.ActiveSiteExplorer;
            var items = wb.ActiveSiteExplorer.SelectedItems;
            var openMgr = ServiceRegistry.GetService<OpenResourceManager>();
            var connMgr = ServiceRegistry.GetService<ServerConnectionManager>();
            _conn = connMgr.GetConnection(wb.ActiveSiteExplorer.ConnectionName);

            if (items.Length > 0)
            {
                foreach (var item in items)
                {
                    if (!item.IsFolder)
                    {
                        if (openMgr.IsOpen(item.ResourceId))
                        {
                            openMgr.GetOpenEditor(item.ResourceId).Activate();
                        }
                        else
                        {
                            openMgr.Open(item.ResourceId, _conn, false, exp);
                        }
                    }
                }
            }
        }
    }
}
