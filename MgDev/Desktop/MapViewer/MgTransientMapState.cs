using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// Represents a temporary change in map state. Used primarily to set the current in-memory viewer map
    /// into a temporary different display state for rendering and plotting.
    /// 
    /// Implements IDisposable semantics allowing for temporary map state to be explicity defined within a
    /// C# using block with auto-reversion
    /// </summary>
    public abstract class MgTransientMapState : IDisposable
    {
        private Stack<MgdMapStateTransition> _states;

        /// <summary>
        /// The original state
        /// </summary>
        protected MgMapDisplayParameters _origState;

        /// <summary>
        /// Constructor
        /// </summary>
        protected MgTransientMapState()
        {
            _states = new Stack<MgdMapStateTransition>();
        }

        /// <summary>
        /// Gets the current display state
        /// </summary>
        /// <returns></returns>
        protected abstract MgMapDisplayParameters GetCurrentState();

        /// <summary>
        /// Applies the view center
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        protected abstract void ApplyViewCenter(double x, double y);

        /// <summary>
        /// Applies the view scale
        /// </summary>
        /// <param name="scale"></param>
        protected abstract void ApplyViewScale(double scale);

        /// <summary>
        /// Applies the display size
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        protected abstract void ApplyDisplaySize(int width, int height);

        /// <summary>
        /// Applies the DPI
        /// </summary>
        /// <param name="dpi"></param>
        protected abstract void ApplyDPI(int dpi);

        private void ApplyState(MgMapDisplayParameters state)
        {
            ApplyViewCenter(state.X, state.Y);
            ApplyViewScale(state.Scale);
            ApplyDisplaySize(state.Width, state.Height);
            if (state.DPI.HasValue)
                ApplyDPI(state.DPI.Value);
        }
        
        /// <summary>
        /// Gets the size of temporary state transitions on the map state stack
        /// </summary>
        public int Depth { get { return _states.Count; } }

        /// <summary>
        /// Pushes the given state onto the map state stack. The map takes on the display parameters specified
        /// in this given state.
        /// </summary>
        /// <param name="state"></param>
        public void PushState(MgMapDisplayParameters state)
        {
            if (state == null)
                throw new ArgumentNullException("state"); //NOXLATE

            var oldState = GetCurrentState();
            ApplyState(state);

            _states.Push(new MgdMapStateTransition() { OldState = oldState, NewState = state });
        }

        /// <summary>
        /// Pops the latest state from the map state stack. The map state is restored to
        /// the previously applied state.
        /// </summary>
        /// <returns>The state that was previously applied</returns>
        public MgMapDisplayParameters PopState()
        {
            if (_states.Count == 0)
                return null;

            var trans = _states.Pop();
            ApplyState(trans.OldState);
            return trans.NewState;
        }

        /// <summary>
        /// Reverts the map to the original display state
        /// </summary>
        public void Dispose()
        {
            while (_states.Count > 0)
            {
                this.PopState();
            }
            ApplyState(_origState);
        }
    }

    /// <summary>
    /// Represents a temporary map display state
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public abstract class MgTransientMapState<T> : MgTransientMapState where T : MgMapBase
    {
        /// <summary>
        /// The map instance
        /// </summary>
        protected T _map;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="map"></param>
        protected MgTransientMapState(T map)
        {
            _map = map;
            _origState = GetCurrentState();
        }

        /// <summary>
        /// Gets the current display state
        /// </summary>
        /// <returns></returns>
        protected override MgMapDisplayParameters GetCurrentState()
        {
            var pt = _map.ViewCenter;
            var coord = pt.Coordinate;
            return new MgMapDisplayParameters(coord.X, coord.Y, _map.ViewScale, _map.DisplayWidth, _map.DisplayHeight, _map.DisplayDpi);
        }
    }

    internal class MgdMapStateTransition
    {
        public MgMapDisplayParameters NewState { get; set; }

        public MgMapDisplayParameters OldState { get; set; }
    }

    /// <summary>
    /// Represents display parameters for a map
    /// </summary>
    public class MgMapDisplayParameters
    {
        /// <summary>
        /// The view center X coordinate
        /// </summary>
        public double X { get; private set; }
        
        /// <summary>
        /// The view center Y coordinate
        /// </summary>
        public double Y { get; private set; }

        /// <summary>
        /// The view scale
        /// </summary>
        public double Scale { get; private set; }

        /// <summary>
        /// The display width
        /// </summary>
        public int Width { get; private set; }

        /// <summary>
        /// The display height
        /// </summary>
        public int Height { get; private set; }

        /// <summary>
        /// The display DPI
        /// </summary>
        public int? DPI { get; private set; }
        
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="scale"></param>
        /// <param name="width"></param>
        /// <param name="height"></param>
        public MgMapDisplayParameters(double x, double y, double scale, int width, int height)
        {
            this.X = x;
            this.Y = y;
            this.Scale = scale;
            this.Width = width;
            this.Height = height;
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="scale"></param>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <param name="dpi"></param>
        public MgMapDisplayParameters(double x, double y, double scale, int width, int height, int dpi)
            : this(x, y, scale, width, height)
        {
            this.DPI = dpi;
        }
    }
}
