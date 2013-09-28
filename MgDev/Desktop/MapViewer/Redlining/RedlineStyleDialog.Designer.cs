namespace OSGeo.MapGuide.Viewer.Redlining
{
    partial class RedlineStyleDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RedlineStyleDialog));
            this.grpLabelStyle = new System.Windows.Forms.GroupBox();
            this.chkUnderline = new System.Windows.Forms.CheckBox();
            this.chkItalic = new System.Windows.Forms.CheckBox();
            this.chkBold = new System.Windows.Forms.CheckBox();
            this.cmbLabelStyle = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.cmbLabelSizeUnits = new System.Windows.Forms.ComboBox();
            this.clrLabelBackground = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.txtLabelSizeValue = new System.Windows.Forms.TextBox();
            this.clrLabelForeground = new System.Windows.Forms.Button();
            this.grpPolygonStyleBorder = new System.Windows.Forms.GroupBox();
            this.cmbPolyBorderPattern = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            this.cmbPolyBorderThicknessUnit = new System.Windows.Forms.ComboBox();
            this.clrPolyBorder = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.txtPolyBorderThicknessValue = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.grpPolygonStyleFill = new System.Windows.Forms.GroupBox();
            this.numFillTransparency = new System.Windows.Forms.NumericUpDown();
            this.lblPolyFillBackground = new System.Windows.Forms.Label();
            this.cmbPolyFillPattern = new System.Windows.Forms.ComboBox();
            this.clrPolyFillBackground = new System.Windows.Forms.Button();
            this.label14 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.clrPolyFillForeground = new System.Windows.Forms.Button();
            this.grpLineStyle = new System.Windows.Forms.GroupBox();
            this.cmbLinePattern = new System.Windows.Forms.ComboBox();
            this.label16 = new System.Windows.Forms.Label();
            this.cmbLineThicknessUnit = new System.Windows.Forms.ComboBox();
            this.clrLine = new System.Windows.Forms.Button();
            this.label18 = new System.Windows.Forms.Label();
            this.txtLineThicknessValue = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.grpPointStyle = new System.Windows.Forms.GroupBox();
            this.cmbPointMarkerType = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.cmbMarkerSizeUnit = new System.Windows.Forms.ComboBox();
            this.clrMarker = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.txtMarkerSizeValue = new System.Windows.Forms.TextBox();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.errorProvider = new System.Windows.Forms.ErrorProvider(this.components);
            this.colorDialog = new System.Windows.Forms.ColorDialog();
            this.chkTransparent = new System.Windows.Forms.CheckBox();
            this.grpLabelStyle.SuspendLayout();
            this.grpPolygonStyleBorder.SuspendLayout();
            this.grpPolygonStyleFill.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numFillTransparency)).BeginInit();
            this.grpLineStyle.SuspendLayout();
            this.grpPointStyle.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider)).BeginInit();
            this.SuspendLayout();
            // 
            // grpLabelStyle
            // 
            this.grpLabelStyle.Controls.Add(this.chkUnderline);
            this.grpLabelStyle.Controls.Add(this.chkItalic);
            this.grpLabelStyle.Controls.Add(this.chkBold);
            this.grpLabelStyle.Controls.Add(this.cmbLabelStyle);
            this.grpLabelStyle.Controls.Add(this.label4);
            this.grpLabelStyle.Controls.Add(this.label2);
            this.grpLabelStyle.Controls.Add(this.cmbLabelSizeUnits);
            this.grpLabelStyle.Controls.Add(this.clrLabelBackground);
            this.grpLabelStyle.Controls.Add(this.label1);
            this.grpLabelStyle.Controls.Add(this.label3);
            this.grpLabelStyle.Controls.Add(this.txtLabelSizeValue);
            this.grpLabelStyle.Controls.Add(this.clrLabelForeground);
            resources.ApplyResources(this.grpLabelStyle, "grpLabelStyle");
            this.grpLabelStyle.Name = "grpLabelStyle";
            this.grpLabelStyle.TabStop = false;
            // 
            // chkUnderline
            // 
            resources.ApplyResources(this.chkUnderline, "chkUnderline");
            this.chkUnderline.Name = "chkUnderline";
            this.chkUnderline.UseVisualStyleBackColor = true;
            // 
            // chkItalic
            // 
            resources.ApplyResources(this.chkItalic, "chkItalic");
            this.chkItalic.Name = "chkItalic";
            this.chkItalic.UseVisualStyleBackColor = true;
            // 
            // chkBold
            // 
            resources.ApplyResources(this.chkBold, "chkBold");
            this.chkBold.Name = "chkBold";
            this.chkBold.UseVisualStyleBackColor = true;
            // 
            // cmbLabelStyle
            // 
            resources.ApplyResources(this.cmbLabelStyle, "cmbLabelStyle");
            this.cmbLabelStyle.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbLabelStyle.FormattingEnabled = true;
            this.cmbLabelStyle.Name = "cmbLabelStyle";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // cmbLabelSizeUnits
            // 
            resources.ApplyResources(this.cmbLabelSizeUnits, "cmbLabelSizeUnits");
            this.cmbLabelSizeUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbLabelSizeUnits.FormattingEnabled = true;
            this.cmbLabelSizeUnits.Name = "cmbLabelSizeUnits";
            // 
            // clrLabelBackground
            // 
            this.clrLabelBackground.BackColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.clrLabelBackground, "clrLabelBackground");
            this.clrLabelBackground.Name = "clrLabelBackground";
            this.clrLabelBackground.UseVisualStyleBackColor = false;
            this.clrLabelBackground.Click += new System.EventHandler(this.OnPickColor);
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // txtLabelSizeValue
            // 
            resources.ApplyResources(this.txtLabelSizeValue, "txtLabelSizeValue");
            this.txtLabelSizeValue.Name = "txtLabelSizeValue";
            // 
            // clrLabelForeground
            // 
            this.clrLabelForeground.BackColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.clrLabelForeground, "clrLabelForeground");
            this.clrLabelForeground.Name = "clrLabelForeground";
            this.clrLabelForeground.UseVisualStyleBackColor = false;
            this.clrLabelForeground.Click += new System.EventHandler(this.OnPickColor);
            // 
            // grpPolygonStyleBorder
            // 
            this.grpPolygonStyleBorder.Controls.Add(this.cmbPolyBorderPattern);
            this.grpPolygonStyleBorder.Controls.Add(this.label8);
            this.grpPolygonStyleBorder.Controls.Add(this.cmbPolyBorderThicknessUnit);
            this.grpPolygonStyleBorder.Controls.Add(this.clrPolyBorder);
            this.grpPolygonStyleBorder.Controls.Add(this.label10);
            this.grpPolygonStyleBorder.Controls.Add(this.txtPolyBorderThicknessValue);
            this.grpPolygonStyleBorder.Controls.Add(this.label9);
            resources.ApplyResources(this.grpPolygonStyleBorder, "grpPolygonStyleBorder");
            this.grpPolygonStyleBorder.Name = "grpPolygonStyleBorder";
            this.grpPolygonStyleBorder.TabStop = false;
            // 
            // cmbPolyBorderPattern
            // 
            resources.ApplyResources(this.cmbPolyBorderPattern, "cmbPolyBorderPattern");
            this.cmbPolyBorderPattern.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbPolyBorderPattern.FormattingEnabled = true;
            this.cmbPolyBorderPattern.Name = "cmbPolyBorderPattern";
            // 
            // label8
            // 
            resources.ApplyResources(this.label8, "label8");
            this.label8.Name = "label8";
            // 
            // cmbPolyBorderThicknessUnit
            // 
            resources.ApplyResources(this.cmbPolyBorderThicknessUnit, "cmbPolyBorderThicknessUnit");
            this.cmbPolyBorderThicknessUnit.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbPolyBorderThicknessUnit.FormattingEnabled = true;
            this.cmbPolyBorderThicknessUnit.Name = "cmbPolyBorderThicknessUnit";
            // 
            // clrPolyBorder
            // 
            this.clrPolyBorder.BackColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.clrPolyBorder, "clrPolyBorder");
            this.clrPolyBorder.Name = "clrPolyBorder";
            this.clrPolyBorder.UseVisualStyleBackColor = false;
            this.clrPolyBorder.Click += new System.EventHandler(this.OnPickColor);
            // 
            // label10
            // 
            resources.ApplyResources(this.label10, "label10");
            this.label10.Name = "label10";
            // 
            // txtPolyBorderThicknessValue
            // 
            resources.ApplyResources(this.txtPolyBorderThicknessValue, "txtPolyBorderThicknessValue");
            this.txtPolyBorderThicknessValue.Name = "txtPolyBorderThicknessValue";
            // 
            // label9
            // 
            resources.ApplyResources(this.label9, "label9");
            this.label9.Name = "label9";
            // 
            // grpPolygonStyleFill
            // 
            this.grpPolygonStyleFill.Controls.Add(this.chkTransparent);
            this.grpPolygonStyleFill.Controls.Add(this.numFillTransparency);
            this.grpPolygonStyleFill.Controls.Add(this.lblPolyFillBackground);
            this.grpPolygonStyleFill.Controls.Add(this.cmbPolyFillPattern);
            this.grpPolygonStyleFill.Controls.Add(this.clrPolyFillBackground);
            this.grpPolygonStyleFill.Controls.Add(this.label14);
            this.grpPolygonStyleFill.Controls.Add(this.label13);
            this.grpPolygonStyleFill.Controls.Add(this.label12);
            this.grpPolygonStyleFill.Controls.Add(this.clrPolyFillForeground);
            resources.ApplyResources(this.grpPolygonStyleFill, "grpPolygonStyleFill");
            this.grpPolygonStyleFill.Name = "grpPolygonStyleFill";
            this.grpPolygonStyleFill.TabStop = false;
            // 
            // numFillTransparency
            // 
            resources.ApplyResources(this.numFillTransparency, "numFillTransparency");
            this.numFillTransparency.Name = "numFillTransparency";
            // 
            // lblPolyFillBackground
            // 
            resources.ApplyResources(this.lblPolyFillBackground, "lblPolyFillBackground");
            this.lblPolyFillBackground.Name = "lblPolyFillBackground";
            // 
            // cmbPolyFillPattern
            // 
            resources.ApplyResources(this.cmbPolyFillPattern, "cmbPolyFillPattern");
            this.cmbPolyFillPattern.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbPolyFillPattern.FormattingEnabled = true;
            this.cmbPolyFillPattern.Name = "cmbPolyFillPattern";
            this.cmbPolyFillPattern.SelectedIndexChanged += new System.EventHandler(this.cmbPolyFillPattern_SelectedIndexChanged);
            // 
            // clrPolyFillBackground
            // 
            this.clrPolyFillBackground.BackColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.clrPolyFillBackground, "clrPolyFillBackground");
            this.clrPolyFillBackground.Name = "clrPolyFillBackground";
            this.clrPolyFillBackground.UseVisualStyleBackColor = false;
            this.clrPolyFillBackground.Click += new System.EventHandler(this.OnPickColor);
            // 
            // label14
            // 
            resources.ApplyResources(this.label14, "label14");
            this.label14.Name = "label14";
            // 
            // label13
            // 
            resources.ApplyResources(this.label13, "label13");
            this.label13.Name = "label13";
            // 
            // label12
            // 
            resources.ApplyResources(this.label12, "label12");
            this.label12.Name = "label12";
            // 
            // clrPolyFillForeground
            // 
            this.clrPolyFillForeground.BackColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.clrPolyFillForeground, "clrPolyFillForeground");
            this.clrPolyFillForeground.Name = "clrPolyFillForeground";
            this.clrPolyFillForeground.UseVisualStyleBackColor = false;
            this.clrPolyFillForeground.Click += new System.EventHandler(this.OnPickColor);
            // 
            // grpLineStyle
            // 
            this.grpLineStyle.Controls.Add(this.cmbLinePattern);
            this.grpLineStyle.Controls.Add(this.label16);
            this.grpLineStyle.Controls.Add(this.cmbLineThicknessUnit);
            this.grpLineStyle.Controls.Add(this.clrLine);
            this.grpLineStyle.Controls.Add(this.label18);
            this.grpLineStyle.Controls.Add(this.txtLineThicknessValue);
            this.grpLineStyle.Controls.Add(this.label17);
            resources.ApplyResources(this.grpLineStyle, "grpLineStyle");
            this.grpLineStyle.Name = "grpLineStyle";
            this.grpLineStyle.TabStop = false;
            // 
            // cmbLinePattern
            // 
            resources.ApplyResources(this.cmbLinePattern, "cmbLinePattern");
            this.cmbLinePattern.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbLinePattern.FormattingEnabled = true;
            this.cmbLinePattern.Name = "cmbLinePattern";
            // 
            // label16
            // 
            resources.ApplyResources(this.label16, "label16");
            this.label16.Name = "label16";
            // 
            // cmbLineThicknessUnit
            // 
            resources.ApplyResources(this.cmbLineThicknessUnit, "cmbLineThicknessUnit");
            this.cmbLineThicknessUnit.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbLineThicknessUnit.FormattingEnabled = true;
            this.cmbLineThicknessUnit.Name = "cmbLineThicknessUnit";
            // 
            // clrLine
            // 
            this.clrLine.BackColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.clrLine, "clrLine");
            this.clrLine.Name = "clrLine";
            this.clrLine.UseVisualStyleBackColor = false;
            this.clrLine.Click += new System.EventHandler(this.OnPickColor);
            // 
            // label18
            // 
            resources.ApplyResources(this.label18, "label18");
            this.label18.Name = "label18";
            // 
            // txtLineThicknessValue
            // 
            resources.ApplyResources(this.txtLineThicknessValue, "txtLineThicknessValue");
            this.txtLineThicknessValue.Name = "txtLineThicknessValue";
            // 
            // label17
            // 
            resources.ApplyResources(this.label17, "label17");
            this.label17.Name = "label17";
            // 
            // grpPointStyle
            // 
            this.grpPointStyle.Controls.Add(this.cmbPointMarkerType);
            this.grpPointStyle.Controls.Add(this.label5);
            this.grpPointStyle.Controls.Add(this.label7);
            this.grpPointStyle.Controls.Add(this.cmbMarkerSizeUnit);
            this.grpPointStyle.Controls.Add(this.clrMarker);
            this.grpPointStyle.Controls.Add(this.label6);
            this.grpPointStyle.Controls.Add(this.txtMarkerSizeValue);
            resources.ApplyResources(this.grpPointStyle, "grpPointStyle");
            this.grpPointStyle.Name = "grpPointStyle";
            this.grpPointStyle.TabStop = false;
            // 
            // cmbPointMarkerType
            // 
            resources.ApplyResources(this.cmbPointMarkerType, "cmbPointMarkerType");
            this.cmbPointMarkerType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbPointMarkerType.FormattingEnabled = true;
            this.cmbPointMarkerType.Name = "cmbPointMarkerType";
            // 
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.Name = "label5";
            // 
            // label7
            // 
            resources.ApplyResources(this.label7, "label7");
            this.label7.Name = "label7";
            // 
            // cmbMarkerSizeUnit
            // 
            resources.ApplyResources(this.cmbMarkerSizeUnit, "cmbMarkerSizeUnit");
            this.cmbMarkerSizeUnit.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbMarkerSizeUnit.FormattingEnabled = true;
            this.cmbMarkerSizeUnit.Name = "cmbMarkerSizeUnit";
            // 
            // clrMarker
            // 
            this.clrMarker.BackColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.clrMarker, "clrMarker");
            this.clrMarker.Name = "clrMarker";
            this.clrMarker.UseVisualStyleBackColor = false;
            this.clrMarker.Click += new System.EventHandler(this.OnPickColor);
            // 
            // label6
            // 
            resources.ApplyResources(this.label6, "label6");
            this.label6.Name = "label6";
            // 
            // txtMarkerSizeValue
            // 
            resources.ApplyResources(this.txtMarkerSizeValue, "txtMarkerSizeValue");
            this.txtMarkerSizeValue.Name = "txtMarkerSizeValue";
            // 
            // btnSave
            // 
            resources.ApplyResources(this.btnSave, "btnSave");
            this.btnSave.Name = "btnSave";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnCancel
            // 
            resources.ApplyResources(this.btnCancel, "btnCancel");
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // errorProvider
            // 
            this.errorProvider.ContainerControl = this;
            // 
            // chkTransparent
            // 
            resources.ApplyResources(this.chkTransparent, "chkTransparent");
            this.chkTransparent.Name = "chkTransparent";
            this.chkTransparent.UseVisualStyleBackColor = true;
            // 
            // RedlineStyleDialog
            // 
            this.AcceptButton = this.btnSave;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ControlBox = false;
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.grpLabelStyle);
            this.Controls.Add(this.grpPolygonStyleBorder);
            this.Controls.Add(this.grpPolygonStyleFill);
            this.Controls.Add(this.grpLineStyle);
            this.Controls.Add(this.grpPointStyle);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "RedlineStyleDialog";
            this.grpLabelStyle.ResumeLayout(false);
            this.grpLabelStyle.PerformLayout();
            this.grpPolygonStyleBorder.ResumeLayout(false);
            this.grpPolygonStyleBorder.PerformLayout();
            this.grpPolygonStyleFill.ResumeLayout(false);
            this.grpPolygonStyleFill.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numFillTransparency)).EndInit();
            this.grpLineStyle.ResumeLayout(false);
            this.grpLineStyle.PerformLayout();
            this.grpPointStyle.ResumeLayout(false);
            this.grpPointStyle.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox grpLabelStyle;
        private System.Windows.Forms.CheckBox chkUnderline;
        private System.Windows.Forms.CheckBox chkItalic;
        private System.Windows.Forms.CheckBox chkBold;
        private System.Windows.Forms.ComboBox cmbLabelStyle;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmbLabelSizeUnits;
        private System.Windows.Forms.Button clrLabelBackground;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtLabelSizeValue;
        private System.Windows.Forms.Button clrLabelForeground;
        private System.Windows.Forms.GroupBox grpPolygonStyleBorder;
        private System.Windows.Forms.ComboBox cmbPolyBorderPattern;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox cmbPolyBorderThicknessUnit;
        private System.Windows.Forms.Button clrPolyBorder;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox txtPolyBorderThicknessValue;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.GroupBox grpPolygonStyleFill;
        private System.Windows.Forms.Label lblPolyFillBackground;
        private System.Windows.Forms.ComboBox cmbPolyFillPattern;
        private System.Windows.Forms.Button clrPolyFillBackground;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Button clrPolyFillForeground;
        private System.Windows.Forms.GroupBox grpLineStyle;
        private System.Windows.Forms.ComboBox cmbLinePattern;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.ComboBox cmbLineThicknessUnit;
        private System.Windows.Forms.Button clrLine;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.TextBox txtLineThicknessValue;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.GroupBox grpPointStyle;
        private System.Windows.Forms.ComboBox cmbPointMarkerType;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ComboBox cmbMarkerSizeUnit;
        private System.Windows.Forms.Button clrMarker;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtMarkerSizeValue;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.ErrorProvider errorProvider;
        private System.Windows.Forms.NumericUpDown numFillTransparency;
        private System.Windows.Forms.ColorDialog colorDialog;
        private System.Windows.Forms.CheckBox chkTransparent;
    }
}