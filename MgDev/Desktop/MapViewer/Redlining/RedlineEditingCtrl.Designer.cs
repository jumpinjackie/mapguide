namespace OSGeo.MapGuide.Viewer.Redlining
{
    partial class RedlineEditingCtrl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RedlineEditingCtrl));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.chkPromptForLabel = new System.Windows.Forms.CheckBox();
            this.lblRedlineLayerName = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.lblDrawingStatus = new System.Windows.Forms.Label();
            this.lblRedlineCount = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.grdActiveRedlines = new System.Windows.Forms.DataGridView();
            this.ID = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Text = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.btnEditRedlineText = new System.Windows.Forms.ToolStripButton();
            this.btnDelete = new System.Windows.Forms.ToolStripButton();
            this.btnSelectRedlines = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.btnRefresh = new System.Windows.Forms.ToolStripButton();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.btnDrawLine = new System.Windows.Forms.Button();
            this.btnDrawCircle = new System.Windows.Forms.Button();
            this.btnDrawPoint = new System.Windows.Forms.Button();
            this.btnDrawPolygon = new System.Windows.Forms.Button();
            this.btnDrawRect = new System.Windows.Forms.Button();
            this.btnDrawLineString = new System.Windows.Forms.Button();
            this.drawTooltip = new System.Windows.Forms.ToolTip(this.components);
            this.groupBox1.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.grdActiveRedlines)).BeginInit();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.chkPromptForLabel);
            this.groupBox1.Controls.Add(this.lblRedlineLayerName);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.lblDrawingStatus);
            this.groupBox1.Controls.Add(this.lblRedlineCount);
            this.groupBox1.Controls.Add(this.panel1);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.btnDrawLine);
            this.groupBox1.Controls.Add(this.btnDrawCircle);
            this.groupBox1.Controls.Add(this.btnDrawPoint);
            this.groupBox1.Controls.Add(this.btnDrawPolygon);
            this.groupBox1.Controls.Add(this.btnDrawRect);
            this.groupBox1.Controls.Add(this.btnDrawLineString);
            resources.ApplyResources(this.groupBox1, "groupBox1");
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.TabStop = false;
            // 
            // chkPromptForLabel
            // 
            resources.ApplyResources(this.chkPromptForLabel, "chkPromptForLabel");
            this.chkPromptForLabel.Name = "chkPromptForLabel";
            this.chkPromptForLabel.UseVisualStyleBackColor = true;
            // 
            // lblRedlineLayerName
            // 
            resources.ApplyResources(this.lblRedlineLayerName, "lblRedlineLayerName");
            this.lblRedlineLayerName.ForeColor = System.Drawing.SystemColors.ControlText;
            this.lblRedlineLayerName.Name = "lblRedlineLayerName";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // lblDrawingStatus
            // 
            resources.ApplyResources(this.lblDrawingStatus, "lblDrawingStatus");
            this.lblDrawingStatus.ForeColor = System.Drawing.Color.Red;
            this.lblDrawingStatus.Name = "lblDrawingStatus";
            // 
            // lblRedlineCount
            // 
            resources.ApplyResources(this.lblRedlineCount, "lblRedlineCount");
            this.lblRedlineCount.Name = "lblRedlineCount";
            // 
            // panel1
            // 
            resources.ApplyResources(this.panel1, "panel1");
            this.panel1.Controls.Add(this.grdActiveRedlines);
            this.panel1.Controls.Add(this.toolStrip1);
            this.panel1.Name = "panel1";
            // 
            // grdActiveRedlines
            // 
            this.grdActiveRedlines.AllowUserToAddRows = false;
            this.grdActiveRedlines.AllowUserToDeleteRows = false;
            this.grdActiveRedlines.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.grdActiveRedlines.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ID,
            this.Text});
            resources.ApplyResources(this.grdActiveRedlines, "grdActiveRedlines");
            this.grdActiveRedlines.Name = "grdActiveRedlines";
            this.grdActiveRedlines.ReadOnly = true;
            this.grdActiveRedlines.SelectionChanged += new System.EventHandler(this.grdActiveRedlines_SelectionChanged);
            // 
            // ID
            // 
            this.ID.DataPropertyName = "ID";
            resources.ApplyResources(this.ID, "ID");
            this.ID.Name = "ID";
            this.ID.ReadOnly = true;
            // 
            // Text
            // 
            this.Text.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Text.DataPropertyName = "Text";
            resources.ApplyResources(this.Text, "Text");
            this.Text.Name = "Text";
            this.Text.ReadOnly = true;
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnEditRedlineText,
            this.btnDelete,
            this.btnSelectRedlines,
            this.toolStripSeparator1,
            this.btnRefresh});
            resources.ApplyResources(this.toolStrip1, "toolStrip1");
            this.toolStrip1.Name = "toolStrip1";
            // 
            // btnEditRedlineText
            // 
            this.btnEditRedlineText.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            resources.ApplyResources(this.btnEditRedlineText, "btnEditRedlineText");
            this.btnEditRedlineText.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.edit_xml;
            this.btnEditRedlineText.Name = "btnEditRedlineText";
            this.btnEditRedlineText.Click += new System.EventHandler(this.btnEditRedlineText_Click);
            // 
            // btnDelete
            // 
            this.btnDelete.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            resources.ApplyResources(this.btnDelete, "btnDelete");
            this.btnDelete.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.cross_script;
            this.btnDelete.Name = "btnDelete";
            this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
            // 
            // btnSelectRedlines
            // 
            this.btnSelectRedlines.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            resources.ApplyResources(this.btnSelectRedlines, "btnSelectRedlines");
            this.btnSelectRedlines.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.select_features;
            this.btnSelectRedlines.Name = "btnSelectRedlines";
            this.btnSelectRedlines.Click += new System.EventHandler(this.btnSelectRedlines_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            resources.ApplyResources(this.toolStripSeparator1, "toolStripSeparator1");
            // 
            // btnRefresh
            // 
            this.btnRefresh.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnRefresh.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.view_refresh;
            resources.ApplyResources(this.btnRefresh, "btnRefresh");
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // btnDrawLine
            // 
            resources.ApplyResources(this.btnDrawLine, "btnDrawLine");
            this.btnDrawLine.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.layer_shape_line;
            this.btnDrawLine.Name = "btnDrawLine";
            this.drawTooltip.SetToolTip(this.btnDrawLine, resources.GetString("btnDrawLine.ToolTip"));
            this.btnDrawLine.UseVisualStyleBackColor = true;
            this.btnDrawLine.Click += new System.EventHandler(this.btnDrawLine_Click);
            // 
            // btnDrawCircle
            // 
            resources.ApplyResources(this.btnDrawCircle, "btnDrawCircle");
            this.btnDrawCircle.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.layer_shape_ellipse;
            this.btnDrawCircle.Name = "btnDrawCircle";
            this.drawTooltip.SetToolTip(this.btnDrawCircle, resources.GetString("btnDrawCircle.ToolTip"));
            this.btnDrawCircle.UseVisualStyleBackColor = true;
            this.btnDrawCircle.Click += new System.EventHandler(this.btnDrawCircle_Click);
            // 
            // btnDrawPoint
            // 
            resources.ApplyResources(this.btnDrawPoint, "btnDrawPoint");
            this.btnDrawPoint.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.layer_small;
            this.btnDrawPoint.Name = "btnDrawPoint";
            this.drawTooltip.SetToolTip(this.btnDrawPoint, resources.GetString("btnDrawPoint.ToolTip"));
            this.btnDrawPoint.UseVisualStyleBackColor = true;
            this.btnDrawPoint.Click += new System.EventHandler(this.btnDrawPoint_Click);
            // 
            // btnDrawPolygon
            // 
            resources.ApplyResources(this.btnDrawPolygon, "btnDrawPolygon");
            this.btnDrawPolygon.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.layer_shape_polygon;
            this.btnDrawPolygon.Name = "btnDrawPolygon";
            this.drawTooltip.SetToolTip(this.btnDrawPolygon, resources.GetString("btnDrawPolygon.ToolTip"));
            this.btnDrawPolygon.UseVisualStyleBackColor = true;
            this.btnDrawPolygon.Click += new System.EventHandler(this.btnDrawPolygon_Click);
            // 
            // btnDrawRect
            // 
            resources.ApplyResources(this.btnDrawRect, "btnDrawRect");
            this.btnDrawRect.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.layer_shape;
            this.btnDrawRect.Name = "btnDrawRect";
            this.drawTooltip.SetToolTip(this.btnDrawRect, resources.GetString("btnDrawRect.ToolTip"));
            this.btnDrawRect.UseVisualStyleBackColor = true;
            this.btnDrawRect.Click += new System.EventHandler(this.btnDrawRect_Click);
            // 
            // btnDrawLineString
            // 
            resources.ApplyResources(this.btnDrawLineString, "btnDrawLineString");
            this.btnDrawLineString.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.layer_shape_polyline;
            this.btnDrawLineString.Name = "btnDrawLineString";
            this.drawTooltip.SetToolTip(this.btnDrawLineString, resources.GetString("btnDrawLineString.ToolTip"));
            this.btnDrawLineString.UseVisualStyleBackColor = true;
            this.btnDrawLineString.Click += new System.EventHandler(this.btnDrawLineString_Click);
            // 
            // RedlineEditingCtrl
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox1);
            this.Name = "RedlineEditingCtrl";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.grdActiveRedlines)).EndInit();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnDrawCircle;
        private System.Windows.Forms.Button btnDrawRect;
        private System.Windows.Forms.Button btnDrawPolygon;
        private System.Windows.Forms.Button btnDrawLineString;
        private System.Windows.Forms.Button btnDrawLine;
        private System.Windows.Forms.Button btnDrawPoint;
        private System.Windows.Forms.Label lblRedlineCount;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.DataGridView grdActiveRedlines;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton btnEditRedlineText;
        private System.Windows.Forms.ToolStripButton btnDelete;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton btnRefresh;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ToolTip drawTooltip;
        private System.Windows.Forms.Label lblDrawingStatus;
        private System.Windows.Forms.DataGridViewTextBoxColumn ID;
        private System.Windows.Forms.DataGridViewTextBoxColumn Text;
        private System.Windows.Forms.Label lblRedlineLayerName;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ToolStripButton btnSelectRedlines;
        private System.Windows.Forms.CheckBox chkPromptForLabel;
    }
}
