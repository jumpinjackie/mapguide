using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace OSGeo.MapGuide.Viewer.Redlining
{
    public partial class TextPrompt : Form
    {
        private TextPrompt()
        {
            InitializeComponent();
        }

        public static string GetValue(string title, string prompt)
        {
            return GetValue(title, prompt, null, null);
        }

        public static string GetValue(string title, string prompt, string okLabel, string cancelLabel)
        {
            using (var diag = new TextPrompt())
            {
                diag.Text = title;
                diag.lblPrompt.Text = prompt;
                if (!string.IsNullOrEmpty(okLabel))
                    diag.btnOK.Text = okLabel;
                if (!string.IsNullOrEmpty(cancelLabel))
                    diag.btnCancel.Text = cancelLabel;

                if (diag.ShowDialog() == DialogResult.OK)
                    return diag.txtValue.Text;
                else
                    return null;
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
        }
    }
}
