using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SampleExtension
{
    public partial class ParcelQueryResultWindow : Form
    {
        public ParcelQueryResultWindow(List<string> results)
        {
            InitializeComponent();
            lstResults.DataSource = results;
        }
    }
}
