using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace OSGeo.MapGuide.Viewer.Redlining
{
    internal partial class NewRedlineLayerDialog : Form
    {
        private NewRedlineLayerDialog()
        {
            InitializeComponent();
        }

        private RedlineStyle _style;

        public NewRedlineLayerDialog(RedlineStyle style, RedlineDataStoreFormat defaultDataStore, int defaultGeometryTypes, string defaultName)
            : this()
        {
            _style = style;
            cmbDataStoreFormat.DataSource = Enum.GetValues(typeof(RedlineDataStoreFormat));
            
            txtName.Text = defaultName;
            cmbDataStoreFormat.SelectedItem = defaultDataStore;
            if ((defaultGeometryTypes & MgFeatureGeometricType.Point) == MgFeatureGeometricType.Point)
                chkPoints.Checked = true;
            if ((defaultGeometryTypes & MgFeatureGeometricType.Curve) == MgFeatureGeometricType.Curve)
                chkLines.Checked = true;
            if ((defaultGeometryTypes & MgFeatureGeometricType.Surface) == MgFeatureGeometricType.Surface)
                chkPolygons.Checked = true;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }

        private void btnEditLayerStyle_Click(object sender, EventArgs e)
        {
            using (var diag = new RedlineStyleDialog(_style))
            {
                if (diag.ShowDialog() == DialogResult.OK)
                {
                    _style = diag.GetUpdatedStyle();
                }
            }
        }

        private void chkPoints_CheckedChanged(object sender, EventArgs e)
        {
            if (chkPoints.Checked && this.Format == RedlineDataStoreFormat.SHP)
            {
                chkLines.Checked = false;
                chkPolygons.Checked = false;
            }
        }

        private void chkLines_CheckedChanged(object sender, EventArgs e)
        {
            if (chkLines.Checked && this.Format == RedlineDataStoreFormat.SHP)
            {
                chkPoints.Checked = false;
                chkPolygons.Checked = false;
            }
        }

        private void chkPolygons_CheckedChanged(object sender, EventArgs e)
        {
            if (chkPolygons.Checked && this.Format == RedlineDataStoreFormat.SHP)
            {
                chkPoints.Checked = false;
                chkLines.Checked = false;
            }
        }

        public RedlineStyle Style { get { return _style; } }

        public RedlineDataStoreFormat Format { get { return (RedlineDataStoreFormat)cmbDataStoreFormat.SelectedItem; } }

        public int GeometryTypes
        {
            get
            {
                int result = 0;
                if (chkPoints.Checked)
                    result |= MgFeatureGeometricType.Point;
                if (chkLines.Checked)
                    result |= MgFeatureGeometricType.Curve;
                if (chkPolygons.Checked)
                    result |= MgFeatureGeometricType.Surface;
                return result;
            }
        }

        public string LayerName { get { return txtName.Text; } }

        private void cmbDataStoreFormat_SelectedIndexChanged(object sender, EventArgs e)
        {
            errorProvider.Clear();
            if (this.Format == RedlineDataStoreFormat.SHP)
            {
                int gt = this.GeometryTypes;
                if (gt != MgFeatureGeometricType.Point &&
                    gt != MgFeatureGeometricType.Curve &&
                    gt != MgFeatureGeometricType.Surface)
                {
                    errorProvider.SetError(chkPoints, Strings.SHPGeometryLimitation);
                    errorProvider.SetError(chkLines, Strings.SHPGeometryLimitation);
                    errorProvider.SetError(chkPolygons, Strings.SHPGeometryLimitation);
                }
            }
        }
    }
}
