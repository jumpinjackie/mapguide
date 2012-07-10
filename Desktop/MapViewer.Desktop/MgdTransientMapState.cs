using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer.Desktop
{
    /// <summary>
    /// Represents a stack of transient map states. This is used to manipulate the display parameters
    /// of a <see cref="T:OSGeo.MapGuide.MgdMap"/> in a manner that can be undone upon disposal
    /// of this stack, allowing your map to be used for <see cref="T:OSGeo.MapGuide.MgdRenderingService.RenderMap()"/>
    /// calls without permanently altering state.
    /// </summary>
    public class MgdTransientMapState : IDisposable
    {
        private MgdMap _map;

        private Stack<MgdMapStateTransition> _states;

        private MgdMapDisplayParameters _origState;

        public MgdTransientMapState(MgdMap map)
        {
            _map = map;
            _origState = GetCurrentState();
            _states = new Stack<MgdMapStateTransition>();
        }

        private MgdMapDisplayParameters GetCurrentState()
        {
            var pt = _map.ViewCenter;
            var coord = pt.Coordinate;
            return new MgdMapDisplayParameters(coord.X, coord.Y, _map.ViewScale, _map.DisplayWidth, _map.DisplayHeight, _map.DisplayDpi);
        }

        private void ApplyState(MgdMapDisplayParameters state)
        {
            _map.SetViewCenterXY(state.X, state.Y);
            _map.SetViewScale(state.Scale);
            _map.SetDisplaySize(state.Width, state.Height);
            if (state.DPI.HasValue)
                _map.DisplayDpi = state.DPI.Value;
        }

        public int Depth { get { return _states.Count; } }

        /// <summary>
        /// Pushes the given state onto the map state stack. The map takes on the display parameters specified
        /// in this given state.
        /// </summary>
        /// <param name="state"></param>
        public void PushState(MgdMapDisplayParameters state)
        {
            if (state == null)
                throw new ArgumentNullException("state");

            var oldState = GetCurrentState();
            ApplyState(state);

            _states.Push(new MgdMapStateTransition() { OldState = oldState, NewState = state });
        }

        /// <summary>
        /// Pops the latest state from the map state stack. The map state is restored to
        /// the previously applied state.
        /// </summary>
        /// <returns>The state that was previously applied</returns>
        public MgdMapDisplayParameters PopState()
        {
            if (_states.Count == 0)
                return null;

            var trans = _states.Pop();
            ApplyState(trans.OldState);
            return trans.NewState;
        }

        public void Dispose()
        {
            while (_states.Count > 0)
            {
                this.PopState();
            }
            ApplyState(_origState);
        }
    }

    internal class MgdMapStateTransition
    {
        public MgdMapDisplayParameters NewState { get; set; }

        public MgdMapDisplayParameters OldState { get; set; }
    }

    /// <summary>
    /// Represents display parameters for a map
    /// </summary>
    public class MgdMapDisplayParameters
    {
        public double X { get; private set; }
        public double Y { get; private set; }
        public double Scale { get; private set; }
        public int Width { get; private set; }
        public int Height { get; private set; }
        public int? DPI { get; private set; }

        public MgdMapDisplayParameters(double x, double y, double scale, int width, int height)
        {
            this.X = x;
            this.Y = y;
            this.Scale = scale;
            this.Width = width;
            this.Height = height;
        }

        public MgdMapDisplayParameters(double x, double y, double scale, int width, int height, int dpi)
            : this(x, y, scale, width, height)
        {
            this.DPI = dpi;
        }
    }
}
