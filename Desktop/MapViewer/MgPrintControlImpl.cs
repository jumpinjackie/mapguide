using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    public partial class MgPrintControlImpl : MgControlView
    {
        private IMapViewer _viewer;

        public MgPrintControlImpl(IMapViewer viewer)
        {
            InitializeComponent();
            this.Title = Strings.TitlePrintSettings;
            this.ModalWindow = true;
            _viewer = viewer;
        }

        private void btnPrintPreview_Click(object sender, EventArgs e)
        {
            if (this.printPreviewDialog.ShowDialog() == DialogResult.OK)
            {
                this.Close();
            }
        }

        private void btnPrint_Click(object sender, EventArgs e)
        {
            if (printDialog.ShowDialog() == DialogResult.OK)
            {
                printDocument.Print();
                this.Close();
            }
        }

        private void printDocument_BeginPrint(object sender, System.Drawing.Printing.PrintEventArgs e)
        {

        }

        private void printDocument_EndPrint(object sender, System.Drawing.Printing.PrintEventArgs e)
        {

        }

        private void printDocument_PrintPage(object sender, System.Drawing.Printing.PrintPageEventArgs e)
        {
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgSelectionBase selection = _viewer.GetSelection();
            MgMapBase map = _viewer.GetMap();
            MgPoint pt = map.ViewCenter;
            MgCoordinate coord = pt.Coordinate;
            float leftMargin = e.MarginBounds.Left;
            float topMargin = e.MarginBounds.Top;
            float yPos = topMargin;
            float pageHeight = e.MarginBounds.Height;

            //Write title
            var titleFont = new Font(FontFamily.GenericSansSerif, 18.0f, GraphicsUnit.Point);
            var sizeTitle = e.Graphics.MeasureString(txtTitle.Text, titleFont);
            e.Graphics.DrawString(txtTitle.Text, titleFont, Brushes.Black, new PointF(leftMargin, yPos));

            yPos += (sizeTitle.Height * 1.1f);

            var scaleFont = SystemFonts.CaptionFont;
            var scaleStr = string.Format("{0} 1 : {1}", Strings.TextScale, _viewer.GetMap().ViewScale); //NOXLATE
            var sizeScale = e.Graphics.MeasureString(scaleStr, scaleFont);

            //Scale to fit within this page
            var imgHeight = (int)(pageHeight - (sizeTitle.Height * 1.1f) - (sizeScale.Height * 1.1f));
            var imgWidth = e.MarginBounds.Width;

            using (var state = provider.CreateTransientState())
            {
                var tempState = new MgMapDisplayParameters(coord.X, coord.Y, map.ViewScale, imgWidth, imgHeight, (int)numDPI.Value);
                state.PushState(tempState);
                MgByteReader br = provider.RenderMap(selection, "PNG"); //NOXLATE
                using (MgReadOnlyStream stream = new MgReadOnlyStream(br))
                {
                    Image img = Image.FromStream(stream);
                    e.Graphics.DrawImage(img, leftMargin, yPos, imgWidth, imgHeight);
                    img.Dispose();
                }
            }

            yPos += imgHeight;

            //Render scale
            if (chkScale.Checked)
                e.Graphics.DrawString(scaleStr, scaleFont, Brushes.Black, new PointF(leftMargin, yPos));

            e.HasMorePages = false;
        }
    }
}
