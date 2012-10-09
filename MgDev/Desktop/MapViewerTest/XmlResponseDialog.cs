using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.Xml;
using System.IO;

namespace MapViewerTest
{
    public partial class XmlResponseDialog : Form
    {
        public XmlResponseDialog()
        {
            InitializeComponent();
        }

        public XmlResponseDialog(MgByteReader reader)
            : this()
        {
            this.Content = reader.ToString();
        }

        public string Content
        {
            get { return textBox1.Text; }
            set
            {
                textBox1.Text = FormatXml(value);
            }
        }

        private string FormatXml(string sUnformattedXml)
        {
            //load unformatted xml into a dom
            XmlDocument xd = new XmlDocument();
            xd.LoadXml(sUnformattedXml);

            //will hold formatted xml
            StringBuilder sb = new StringBuilder();

            //pumps the formatted xml into the StringBuilder above
            StringWriter sw = new StringWriter(sb);

            //does the formatting
            XmlTextWriter xtw = null;

            try
            {
                //point the xtw at the StringWriter
                xtw = new XmlTextWriter(sw);

                //we want the output formatted
                xtw.Formatting = Formatting.Indented;

                //get the dom to dump its contents into the xtw
                xd.WriteTo(xtw);
            }
            finally
            {
                //clean up even if error
                if (xtw != null)
                    xtw.Close();
            }

            //return the formatted xml
            return sb.ToString();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
