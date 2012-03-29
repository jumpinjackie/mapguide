using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.IO;

namespace DotNetHarness
{
    public partial class ImageResponseDialog : Form
    {
        private ImageResponseDialog()
        {
            InitializeComponent();
        }

        public ImageResponseDialog(MgByteReader reader)
            : this()
        {
            byte[] b = new byte[reader.GetLength()];
            reader.Read(b, b.Length);
            using (var ms = new MemoryStream(b))
            {
                var img = Image.FromStream(ms);
                picResult.Image = img;

                this.Width = img.Width;
                this.Height = img.Height;
            }
        }
    }
}
