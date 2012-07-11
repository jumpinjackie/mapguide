using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer
{
    public abstract class MgTransientMapState : IDisposable
    {
        private Stack<MgdMapStateTransition> _states;

        protected MgMapDisplayParameters _origState;

        protected MgTransientMapState()
        {
            _states = new Stack<MgdMapStateTransition>();
        }

        protected abstract MgMapDisplayParameters GetCurrentState();

        protected abstract void ApplyViewCenter(double x, double y);
        protected abstract void ApplyViewScale(double scale);
        protected abstract void ApplyDisplaySize(int width, int height);
        protected abstract void ApplyDPI(int dpi);

        private void ApplyState(MgMapDisplayParameters state)
        {
            ApplyViewCenter(state.X, state.Y);
            ApplyViewScale(state.Scale);
            ApplyDisplaySize(state.Width, state.Height);
            if (state.DPI.HasValue)
                ApplyDPI(state.DPI.Value);
        }

        public int Depth { get { return _states.Count; } }

        /// <summary>
        /// Pushes the given state onto the map state stack. The map takes on the display parameters specified
        /// in this given state.
        /// </summary>
        /// <param name="state"></param>
        public void PushState(MgMapDisplayParameters state)
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
        public MgMapDisplayParameters PopState()
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

    public abstract class MgTransientMapState<T> : MgTransientMapState where T : MgMapBase
    {
        protected T _map;

        protected MgTransientMapState(T map)
        {
            _map = map;
            _origState = GetCurrentState();
        }

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
        public double X { get; private set; }
        public double Y { get; private set; }
        public double Scale { get; private set; }
        public int Width { get; private set; }
        public int Height { get; private set; }
        public int? DPI { get; private set; }

        public MgMapDisplayParameters(double x, double y, double scale, int width, int height)
        {
            this.X = x;
            this.Y = y;
            this.Scale = scale;
            this.Width = width;
            this.Height = height;
        }

        public MgMapDisplayParameters(double x, double y, double scale, int width, int height, int dpi)
            : this(x, y, scale, width, height)
        {
            this.DPI = dpi;
        }
    }
}
