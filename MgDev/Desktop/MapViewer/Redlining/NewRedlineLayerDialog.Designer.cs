namespace OSGeo.MapGuide.Viewer.Redlining
{
    partial class NewRedlineLayerDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(NewRedlineLayerDialog));
            this.label1 = new System.Windows.Forms.Label();
            this.errorProvider = new System.Windows.Forms.ErrorProvider(this.components);
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.txtName = new System.Windows.Forms.TextBox();
            this.cmbDataStoreFormat = new System.Windows.Forms.ComboBox();
            this.chkPoints = new System.Windows.Forms.CheckBox();
            this.chkLines = new System.Windows.Forms.CheckBox();
            this.chkPolygons = new System.Windows.Forms.CheckBox();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnEditLayerStyle = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // errorProvider
            // 
            this.errorProvider.ContainerControl = this;
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // txtName
            // 
            resources.ApplyResources(this.txtName, "txtName");
            this.txtName.Name = "txtName";
            // 
            // cmbDataStoreFormat
            // 
            this.cmbDataStoreFormat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbDataStoreFormat.FormattingEnabled = true;
            resources.ApplyResources(this.cmbDataStoreFormat, "cmbDataStoreFormat");
            this.cmbDataStoreFormat.Name = "cmbDataStoreFormat";
            this.cmbDataStoreFormat.SelectedIndexChanged += new System.EventHandler(this.cmbDataStoreFormat_SelectedIndexChanged);
            // 
            // chkPoints
            // 
            resources.ApplyResources(this.chkPoints, "chkPoints");
            this.chkPoints.Name = "chkPoints";
            this.chkPoints.UseVisualStyleBackColor = true;
            this.chkPoints.CheckedChanged += new System.EventHandler(this.chkPoints_CheckedChanged);
            // 
            // chkLines
            // 
            resources.ApplyResources(this.chkLines, "chkLines");
            this.chkLines.Name = "chkLines";
            this.chkLines.UseVisualStyleBackColor = true;
            this.chkLines.CheckedChanged += new System.EventHandler(this.chkLines_CheckedChanged);
            // 
            // chkPolygons
            // 
            resources.ApplyResources(this.chkPolygons, "chkPolygons");
            this.chkPolygons.Name = "chkPolygons";
            this.chkPolygons.UseVisualStyleBackColor = true;
            this.chkPolygons.CheckedChanged += new System.EventHandler(this.chkPolygons_CheckedChanged);
            // 
            // btnOK
            // 
            resources.ApplyResources(this.btnOK, "btnOK");
            this.btnOK.Name = "btnOK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            resources.ApplyResources(this.btnCancel, "btnCancel");
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnEditLayerStyle
            // 
            resources.ApplyResources(this.btnEditLayerStyle, "btnEditLayerStyle");
            this.btnEditLayerStyle.Name = "btnEditLayerStyle";
            this.btnEditLayerStyle.UseVisualStyleBackColor = true;
            this.btnEditLayerStyle.Click += new System.EventHandler(this.btnEditLayerStyle_Click);
            // 
            // NewRedlineLayerDialog
            // 
            this.AcceptButton = this.btnOK;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ControlBox = false;
            this.Controls.Add(this.btnEditLayerStyle);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.chkPolygons);
            this.Controls.Add(this.chkLines);
            this.Controls.Add(this.chkPoints);
            this.Controls.Add(this.cmbDataStoreFormat);
            this.Controls.Add(this.txtName);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "NewRedlineLayerDialog";
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ErrorProvider errorProvider;
        private System.Windows.Forms.TextBox txtName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.CheckBox chkPolygons;
        private System.Windows.Forms.CheckBox chkLines;
        private System.Windows.Forms.CheckBox chkPoints;
        private System.Windows.Forms.ComboBox cmbDataStoreFormat;
        private System.Windows.Forms.Button btnEditLayerStyle;
    }
}