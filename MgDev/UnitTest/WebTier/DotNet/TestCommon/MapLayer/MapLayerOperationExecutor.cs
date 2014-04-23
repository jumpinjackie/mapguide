using OSGeo.MapGuide.Test.Common;
using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Operations
{
    public interface IMapCreator
    {
        MgMapBase CreateMap(MgResourceIdentifier resId);
        MgLayerBase CreateLayer(MgResourceIdentifier resId);
    }

    /// <summary>
    /// The base class of all test executors operating with the runtime map
    /// </summary>
    /// <typeparam name="T">The test executor subclass type</typeparam>
    public abstract class MapLayerOperationExecutor<T> : PlatformApiTestExecutor
    {
        protected MgMapBase _map;
        protected MgResourceService _resourceService;
        protected IMapCreator _creator;

        protected MapLayerOperationExecutor(MgResourceService resSvc, string unitTestVm, IMapCreator creator)
            : base(typeof(T).Name.ToUpper(), ApiTypes.Platform, unitTestVm)
        {
            _resourceService = resSvc;
            _creator = creator;
        }

        protected void CreateMapFromResource(int paramSetId)
        {
            if (_map != null)
                return;

            _unitTestVm.Execute("Select ParamValue from Params WHERE ParamSet={0} AND ParamName=\"MAPDEFINITION\"", paramSetId);
            string mapName = _unitTestVm.GetString("ParamValue");
            if (string.IsNullOrEmpty(mapName))
            {
                mapName = "Library://maplayertest/World.MapDefinition";
            }

            MgResourceIdentifier mdfId = new MgResourceIdentifier(mapName);
            _map = _creator.CreateMap(mdfId);
        }

        protected MgLayerBase FindLayer(string name)
        {
            if (_map != null)
            {
                var layers = _map.GetLayers();
                var lidx = layers.IndexOf(name);
                if (lidx >= 0)
                    return layers.GetItem(lidx);
            }
            return null;
        }
    }
}
