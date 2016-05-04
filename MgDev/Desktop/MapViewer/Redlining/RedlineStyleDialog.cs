using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Globalization;

namespace OSGeo.MapGuide.Viewer.Redlining
{
    internal partial class RedlineStyleDialog : Form
    {
        private RedlineStyleDialog()
        {
            InitializeComponent();
        }

        public RedlineStyleDialog(RedlineStyle style)
            : this()
        {
            InitFromStyle(style);
        }

        private void InitFromStyle(RedlineStyle style)
        {
            cmbLabelSizeUnits.DataSource = Enum.GetValues(typeof(SizeUnit));
            cmbLineThicknessUnit.DataSource = Enum.GetValues(typeof(SizeUnit));
            cmbPolyBorderThicknessUnit.DataSource = Enum.GetValues(typeof(SizeUnit));
            cmbMarkerSizeUnit.DataSource = Enum.GetValues(typeof(SizeUnit));

            cmbLinePattern.DataSource = Enum.GetValues(typeof(LinePattern));
            cmbPolyBorderPattern.DataSource = Enum.GetValues(typeof(LinePattern));
            cmbPolyFillPattern.DataSource = Enum.GetValues(typeof(FillPattern));

            cmbPointMarkerType.DataSource = Enum.GetValues(typeof(MarkerType));
            cmbLabelStyle.DataSource = Enum.GetValues(typeof(LabelStyle));

            numFillTransparency.Value = Convert.ToDecimal(style.FillTransparency);

            cmbLabelSizeUnits.SelectedItem = style.LabelSizeUnits;
            cmbLineThicknessUnit.SelectedItem = style.LineSizeUnits;
            cmbPolyBorderThicknessUnit.SelectedItem = style.BorderSizeUnits;
            cmbMarkerSizeUnit.SelectedItem = style.MarkerSizeUnits;
            cmbLinePattern.SelectedItem = style.LinePattern;
            cmbPolyBorderPattern.SelectedItem = style.BorderPattern;
            cmbPolyFillPattern.SelectedItem = style.FillPattern;
            cmbPointMarkerType.SelectedItem = style.MarkerType;
            cmbLabelStyle.SelectedItem = style.LabelBackStyle;

            clrLabelBackground.BackColor = style.LabelBackColor;
            clrLabelForeground.BackColor = style.LabelForeColor;
            
            chkTransparent.Checked = style.FillBackTransparency;

            clrPolyFillBackground.BackColor = style.FillBackColor;
            clrPolyFillForeground.BackColor = style.FillForeColor;
            clrLine.BackColor = style.LineColor;
            clrMarker.BackColor = style.MarkerColor;
            clrPolyBorder.BackColor = style.BorderColor;

            txtLabelSizeValue.Text = style.LabelFontSize.ToString(CultureInfo.InvariantCulture);
            txtLineThicknessValue.Text = style.LineThickness.ToString(CultureInfo.InvariantCulture);
            txtMarkerSizeValue.Text = style.MarkerSize.ToString(CultureInfo.InvariantCulture);
            txtPolyBorderThicknessValue.Text = style.BorderThickness.ToString(CultureInfo.InvariantCulture);

            chkBold.Checked = style.LabelBold;
            chkItalic.Checked = style.LabelItalic;
            chkUnderline.Checked = style.LabelUnderline;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
        }

        internal RedlineStyle GetUpdatedStyle()
        {
            var style = RedlineStyle.CreateDefault();

            style.LabelSizeUnits = (SizeUnit)cmbLabelSizeUnits.SelectedItem;
            style.LineSizeUnits = (SizeUnit)cmbLineThicknessUnit.SelectedItem;
            style.BorderSizeUnits = (SizeUnit)cmbPolyBorderThicknessUnit.SelectedItem;
            style.MarkerSizeUnits = (SizeUnit)cmbMarkerSizeUnit.SelectedItem;
            style.LinePattern = (LinePattern)cmbLinePattern.SelectedItem;
            style.BorderPattern = (LinePattern)cmbPolyBorderPattern.SelectedItem;
            style.FillPattern = (FillPattern)cmbPolyFillPattern.SelectedItem;
            style.MarkerType = (MarkerType)cmbPointMarkerType.SelectedItem;
            style.LabelBackStyle = (LabelStyle)cmbLabelStyle.SelectedItem;

            style.LabelBackColor = clrLabelBackground.BackColor;
            style.LabelForeColor = clrLabelForeground.BackColor;
            style.FillBackColor = clrPolyFillBackground.BackColor;
            style.FillForeColor = clrPolyFillForeground.BackColor;
            style.LineColor = clrLine.BackColor;
            style.MarkerColor = clrMarker.BackColor;
            style.BorderColor = clrPolyBorder.BackColor;

            style.FillTransparency = Math.Min(100.0, Convert.ToDouble(numFillTransparency.Value));

            style.LabelFontSize = Convert.ToDouble(txtLabelSizeValue.Text);
            style.LineThickness = Convert.ToDouble(txtLineThicknessValue.Text);
            style.MarkerSize = Convert.ToDouble(txtMarkerSizeValue.Text);
            style.BorderThickness = Convert.ToDouble(txtPolyBorderThicknessValue.Text);

            style.LabelBold = chkBold.Checked;
            style.LabelItalic = chkItalic.Checked;
            style.LabelUnderline = chkUnderline.Checked;

            return style;
        }

        private void OnPickColor(object sender, EventArgs e)
        {
            var btn = (Button)sender;
            if (colorDialog.ShowDialog() == DialogResult.OK)
            {
                btn.BackColor = colorDialog.Color;
            }
        }

        private void cmbPolyFillPattern_SelectedIndexChanged(object sender, EventArgs e)
        {
            var pat = (FillPattern)cmbPolyFillPattern.SelectedItem;
            lblPolyFillBackground.Visible = clrPolyFillBackground.Visible = chkTransparent.Visible = (pat != FillPattern.Solid);
        }
    }
}
