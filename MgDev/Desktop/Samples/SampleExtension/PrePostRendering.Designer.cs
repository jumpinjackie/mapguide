namespace SampleExtension
{
    partial class PrePostRendering
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PrePostRendering));
            this.label1 = new System.Windows.Forms.Label();
            this.chkEnablePreRender = new System.Windows.Forms.CheckBox();
            this.chkEnablePostRender = new System.Windows.Forms.CheckBox();
            this.txtMessages = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(12, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(227, 285);
            this.label1.TabIndex = 1;
            this.label1.Text = resources.GetString("label1.Text");
            // 
            // chkEnablePreRender
            // 
            this.chkEnablePreRender.AutoSize = true;
            this.chkEnablePreRender.Location = new System.Drawing.Point(15, 314);
            this.chkEnablePreRender.Name = "chkEnablePreRender";
            this.chkEnablePreRender.Size = new System.Drawing.Size(110, 17);
            this.chkEnablePreRender.TabIndex = 2;
            this.chkEnablePreRender.Text = "Enable pre-render";
            this.chkEnablePreRender.UseVisualStyleBackColor = true;
            this.chkEnablePreRender.CheckedChanged += new System.EventHandler(this.chkEnablePreRender_CheckedChanged);
            // 
            // chkEnablePostRender
            // 
            this.chkEnablePostRender.AutoSize = true;
            this.chkEnablePostRender.Location = new System.Drawing.Point(131, 314);
            this.chkEnablePostRender.Name = "chkEnablePostRender";
            this.chkEnablePostRender.Size = new System.Drawing.Size(115, 17);
            this.chkEnablePostRender.TabIndex = 3;
            this.chkEnablePostRender.Text = "Enable post-render";
            this.chkEnablePostRender.UseVisualStyleBackColor = true;
            this.chkEnablePostRender.CheckedChanged += new System.EventHandler(this.chkEnablePostRender_CheckedChanged);
            // 
            // txtMessages
            // 
            this.txtMessages.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtMessages.Location = new System.Drawing.Point(15, 348);
            this.txtMessages.Multiline = true;
            this.txtMessages.Name = "txtMessages";
            this.txtMessages.ReadOnly = true;
            this.txtMessages.Size = new System.Drawing.Size(224, 68);
            this.txtMessages.TabIndex = 4;
            // 
            // PrePostRendering
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.txtMessages);
            this.Controls.Add(this.chkEnablePostRender);
            this.Controls.Add(this.chkEnablePreRender);
            this.Controls.Add(this.label1);
            this.Name = "PrePostRendering";
            this.Size = new System.Drawing.Size(255, 442);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox chkEnablePreRender;
        private System.Windows.Forms.CheckBox chkEnablePostRender;
        private System.Windows.Forms.TextBox txtMessages;
    }
}
