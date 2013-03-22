using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Xml.Serialization;
using System.IO;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    public partial class MgViewerOptionsControlImpl : MgControlView
    {
        private IMapViewer _viewer;
        private string _saveDirectory;

        internal MgViewerOptionsControlImpl(IMapViewer viewer, string saveDirectory)
        {
            InitializeComponent();
            this.Title = Strings.TitleViewerOptions;
            this.ModalWindow = true;
            _viewer = viewer;
            _saveDirectory = saveDirectory;

            lblSelectionColor.BackColor = _viewer.SelectionColor;
            chkShowVertexCoordinates.Checked = _viewer.ShowVertexCoordinatesWhenDigitizing;
            chkConvertTiledToNonTiled.Checked = _viewer.ConvertTiledGroupsToNonTiled;
            numZoomInFactor.Value = Convert.ToDecimal(_viewer.ZoomInFactor);
            numZoomOutFactor.Value = Convert.ToDecimal(_viewer.ZoomOutFactor);
        }

        private void btnPickSelectionColor_Click(object sender, EventArgs e)
        {
            if (colorDialog.ShowDialog() == DialogResult.OK)
            {
                lblSelectionColor.BackColor = colorDialog.Color;
            }
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            var options = new MgViewerOptions();
            _viewer.SelectionColor = lblSelectionColor.BackColor;
            options.SelectionColor = Util.ToHtmlColorWithAlpha(_viewer.SelectionColor);
            options.ShowVertexCoordinates = _viewer.ShowVertexCoordinatesWhenDigitizing = chkShowVertexCoordinates.Checked;
            options.ConvertTiledLayersToNonTiled = _viewer.ConvertTiledGroupsToNonTiled = chkConvertTiledToNonTiled.Checked;
            options.ZoomInFactor = _viewer.ZoomInFactor = Convert.ToDouble(numZoomInFactor.Value);
            options.ZoomOutFactor = _viewer.ZoomOutFactor = Convert.ToDouble(numZoomOutFactor.Value);

            var ser = new XmlSerializer(typeof(MgViewerOptions));
            using (var stream = File.OpenWrite(Path.Combine(_saveDirectory, MgViewerOptions.FILENAME)))
            {
                ser.Serialize(stream, options);
            }
            MessageBox.Show(Strings.TextOptionsSaved);
            base.Close();
        }
    }
    
    [Serializable]
    public class MgViewerOptions
    {
        public const string FILENAME = "MgViewerOptions.xml"; //NOXLATE

        [XmlElement]
        public string SelectionColor { get; set; }

        [XmlElement]
        public bool ShowVertexCoordinates { get; set; }

        [XmlElement]
        public bool ConvertTiledLayersToNonTiled { get; set; }

        [XmlElement]
        public double ZoomInFactor { get; set; }

        [XmlElement]
        public double ZoomOutFactor { get; set; }
    }
}
