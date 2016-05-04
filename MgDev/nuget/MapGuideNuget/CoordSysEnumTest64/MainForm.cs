using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;

namespace CoordSysEnumTest
{
    public partial class MainForm : Form
    {
        private MainForm()
        {
            InitializeComponent();
        }

        private MgCoordinateSystemFactory _fact;

        public MainForm(MgCoordinateSystemFactory fact) : this()
        {
            _fact = fact;
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            MgStringCollection categories = _fact.EnumerateCategories();
            var items = new List<string>();
            for (int i = 0; i < categories.GetCount(); i++)
                items.Add(categories.GetItem(i));

            lstCoordSysCategories.DataSource = items;
        }
    }
}
