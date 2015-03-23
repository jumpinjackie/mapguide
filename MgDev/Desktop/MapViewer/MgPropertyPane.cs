using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Collections;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A control that displays properties of selected features
    /// </summary>
    [ToolboxItem(true)]
    public partial class MgPropertyPane : UserControl, IPropertyPane
    {
        private string _countFmt;

        /// <summary>
        /// Initializes a new instance of the <see cref="MgPropertyPane"/> class.
        /// </summary>
        public MgPropertyPane()
        {
            InitializeComponent();
            _countFmt = lblCount.Text;

            cmbLayer.SelectedIndexChanged += new EventHandler(OnSelectionLayerChanged);
        }

        private MgSelectionSet _sset;

        /// <summary>
        /// Initializes the property pane from the given selection set
        /// </summary>
        /// <param name="set"></param>
        public void Init(MgSelectionSet set)
        {
            propGrid.SelectedObject = null;
            featureToolStrip.Visible = false;

            if (_sset != null)
            {
                _sset.Dispose();
                _sset = null;

                cmbLayer.Items.Clear();
                _currentLayerFeatures.Clear();
            }

            _sset = set;

            var layerNames = _sset.LayerNames;
            cmbLayer.Items.AddRange(layerNames);
            if (layerNames.Length > 0)
                cmbLayer.SelectedIndex = 0;

            controlToolStrip.Visible = (layerNames.Length > 0);
        }

        /// <summary>
        /// Gets the current feature displayed on the property pane
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public MgFeature CurrentFeature
        {
            get
            {
                var idx = this.CurrentSelectedFeatureIndex;
                if (idx >= 0)
                    return _currentLayerFeatures[idx];

                return null;
            }
        }

        void OnSelectedFeatureChanged()
        {
            var feat = this.CurrentFeature;
            if (feat != null)
            {
                propGrid.SelectedObject = new MgDictionaryPropertyGridAdapter(feat.Properties);
            }
        }

        private List<MgFeature> _currentLayerFeatures = new List<MgFeature>();

        void OnSelectionLayerChanged(object sender, EventArgs e)
        {
            if (cmbLayer.SelectedItem != null)
            {
                var layerName = cmbLayer.SelectedItem.ToString();
                _currentLayerFeatures.Clear();

                foreach (var feat in _sset.GetFeaturesForLayer(layerName))
                {
                    _currentLayerFeatures.Add(feat);
                }

                if (_currentLayerFeatures.Count > 0)
                {
                    lblCount.Text = string.Format(_countFmt, _currentLayerFeatures.Count);
                    featureToolStrip.Visible = true;
                    this.CurrentSelectedFeatureIndex = 0;
                }
                else
                {
                    featureToolStrip.Visible = false;
                }
            }
        }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        internal int CurrentSelectedFeatureIndex
        {
            get
            {
                int idx;
                if (int.TryParse(txtPosition.Text, out idx))
                    return idx - 1;

                return 0;
            }
            set
            {
                txtPosition.Text = (value + 1).ToString();
                OnSelectedFeatureChanged();
            }
        }

        /// <summary>
        /// Raised when a request to zoom to the current selected feature has been made
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Raised when the zoom to current feature toolbar button is clicked")] //NOXLATE
        public event EventHandler RequestZoomToCurrentFeature;

        private void btnZoomCurrent_Click(object sender, EventArgs e)
        {
            if (this.CurrentFeature != null)
            {
                var handler = this.RequestZoomToCurrentFeature;
                if (handler != null)
                    handler(this, EventArgs.Empty);
            }
        }

        private void btnPrevFeature_Click(object sender, EventArgs e)
        {
            var idx = this.CurrentSelectedFeatureIndex;
            if (idx > 0)
            {
                idx--;
            }
            else
            {
                idx = 0;
            }
            this.CurrentSelectedFeatureIndex = idx;
        }

        private void btnNextFeature_Click(object sender, EventArgs e)
        {
            var idx = this.CurrentSelectedFeatureIndex;
            if (idx < 0)
            {
                idx = 0;
            }
            else
            {
                if (idx < _currentLayerFeatures.Count - 1)
                    idx++;
                else
                    idx = _currentLayerFeatures.Count - 1;
            }
            this.CurrentSelectedFeatureIndex = idx;
        }

        internal void SetLanguage(System.Globalization.CultureInfo lang)
        {
            ComponentResourceManager resources = new ComponentResourceManager(this.GetType());
            ApplyResourceToControl(resources, this, lang);
            resources.ApplyResources(this, "$this", lang);

            //NOTE: These will slip under the radar of the above call because they aren't direct children
            resources.ApplyResources(btnNextFeature, btnNextFeature.Name, lang);
            resources.ApplyResources(btnPrevFeature, btnPrevFeature.Name, lang);
            resources.ApplyResources(lblCount, lblCount.Name, lang);
            resources.ApplyResources(btnZoomSelectedFeature, btnZoomSelectedFeature.Name, lang);
        }

        private static void ApplyResourceToControl(ComponentResourceManager resources, Control control, System.Globalization.CultureInfo lang)
        {
            foreach (Control c in control.Controls)
            {
                ApplyResourceToControl(resources, c, lang);
                resources.ApplyResources(c, c.Name, lang);
            }
        }
    }

    /// <summary>
    /// An adapter that allows an <see cref="T:System.Collections.IDictionary"/> to be presented
    /// in a <see cref="T:System.Windows.Forms.PropertyGrid"/>
    /// </summary>
    public class MgDictionaryPropertyGridAdapter : ICustomTypeDescriptor
    {
        IDictionary _dictionary;

        public MgDictionaryPropertyGridAdapter(IDictionary d)
        {
            _dictionary = d;
        }

        public string GetComponentName()
        {
            return TypeDescriptor.GetComponentName(this, true);
        }

        public EventDescriptor GetDefaultEvent()
        {
            return TypeDescriptor.GetDefaultEvent(this, true);
        }

        public string GetClassName()
        {
            return TypeDescriptor.GetClassName(this, true);
        }

        public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
        {
            ArrayList properties = new ArrayList();
            foreach (DictionaryEntry e in _dictionary)
            {
                properties.Add(new DictionaryPropertyDescriptor(_dictionary, e.Key));
            }

            PropertyDescriptor[] props =
                (PropertyDescriptor[])properties.ToArray(typeof(PropertyDescriptor));

            return new PropertyDescriptorCollection(props);
        }

        public EventDescriptorCollection GetEvents(Attribute[] attributes)
        {
            return TypeDescriptor.GetEvents(this, attributes, true);
        }

        EventDescriptorCollection System.ComponentModel.ICustomTypeDescriptor.GetEvents()
        {
            return TypeDescriptor.GetEvents(this, true);
        }

        public TypeConverter GetConverter()
        {
            return TypeDescriptor.GetConverter(this, true);
        }

        public object GetPropertyOwner(PropertyDescriptor pd)
        {
            return _dictionary;
        }

        public AttributeCollection GetAttributes()
        {
            return TypeDescriptor.GetAttributes(this, true);
        }

        public object GetEditor(Type editorBaseType)
        {
            return TypeDescriptor.GetEditor(this, editorBaseType, true);
        }

        public PropertyDescriptor GetDefaultProperty()
        {
            return null;
        }

        PropertyDescriptorCollection
            System.ComponentModel.ICustomTypeDescriptor.GetProperties()
        {
            return ((ICustomTypeDescriptor)this).GetProperties(new Attribute[0]);
        }
    }

    internal class DictionaryPropertyDescriptor : PropertyDescriptor
    {
        IDictionary _dictionary;
        object _key;

        internal DictionaryPropertyDescriptor(IDictionary d, object key)
            : base(key.ToString(), null)
        {
            _dictionary = d;
            _key = key;
        }

        public override Type PropertyType
        {
            get
            {
                if (_dictionary[_key] != null)
                {
                    return _dictionary[_key].GetType();
                }
                else
                {
                    return typeof(object);
                }
            }
        }

        public override void SetValue(object component, object value)
        {
            _dictionary[_key] = value;
        }

        public override object GetValue(object component)
        {
            return _dictionary[_key];
        }

        public override bool IsReadOnly
        {
            get { return true; } //Property Pane values are not editable
        }

        public override Type ComponentType
        {
            get { return null; }
        }

        public override bool CanResetValue(object component)
        {
            return false;
        }

        public override void ResetValue(object component)
        {
        }

        public override bool ShouldSerializeValue(object component)
        {
            return false;
        }
    }
}
