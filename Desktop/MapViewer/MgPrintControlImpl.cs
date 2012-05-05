using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace OSGeo.MapGuide.Viewer
{
    public partial class MgPrintControlImpl : MgControlImpl
    {
        private IMapViewer _viewer;

        public MgPrintControlImpl(IMapViewer viewer)
        {
            InitializeComponent();
            this.Title = Properties.Resources.TitlePrintSettings;
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
            var scaleStr = string.Format("{0} 1 : {1}", Properties.Resources.TextScale, _viewer.GetMap().ViewScale);
            var sizeScale = e.Graphics.MeasureString(scaleStr, scaleFont);

            //Draw map
            var img = _viewer.GetCurrentImage();
            var imgWidth = (float)img.Width;
            var imgHeight = (float)img.Height;

            //Scale to fit within this page
            //TODO: We should probably re-render at the desired size 
            var workableImgHeight = pageHeight - (sizeTitle.Height * 1.1f) - (sizeScale.Height * 1.1f);
            var workableImgWidth = e.MarginBounds.Width;
            var hratio = 1.0f;
            var wratio = 1.0f;
            if (imgHeight > workableImgHeight)
                hratio = imgHeight / workableImgHeight;
            if (imgWidth > workableImgWidth)
                wratio = imgWidth / workableImgWidth;

            //Multiply by this ratio
            imgWidth *= (1.0f / wratio);
            imgHeight *= (1.0f / hratio);

            e.Graphics.DrawImage(img, leftMargin, yPos, imgWidth, imgHeight);
            yPos += imgHeight;

            //Render scale
            if (chkScale.Checked)
                e.Graphics.DrawString(scaleStr, scaleFont, Brushes.Black, new PointF(leftMargin, yPos));

            e.HasMorePages = false;
        }
    }
}
