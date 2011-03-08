﻿using Maestro.Editors.Common;
namespace Maestro.Editors.LayerDefinition.Vector.Thematics
{
    partial class ThemeCreator
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ThemeCreator));
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.AggregateLabel = new System.Windows.Forms.Label();
            this.OverwriteRules = new System.Windows.Forms.RadioButton();
            this.AppendRules = new System.Windows.Forms.RadioButton();
            this.ColumnCombo = new System.Windows.Forms.ComboBox();
            this.RuleCount = new System.Windows.Forms.NumericUpDown();
            this.AggregateCombo = new System.Windows.Forms.ComboBox();
            this.DataGroup = new System.Windows.Forms.GroupBox();
            this.GroupPanel = new System.Windows.Forms.Panel();
            this.RuleCountPanel = new System.Windows.Forms.Panel();
            this.DisplayGroup = new System.Windows.Forms.GroupBox();
            this.ColorBrewerPanel = new System.Windows.Forms.Panel();
            this.label3 = new System.Windows.Forms.Label();
            this.ColorBrewerDataType = new System.Windows.Forms.ComboBox();
            this.ColorBrewerColorSet = new CustomCombo();
            this.ColorBrewerLabel = new System.Windows.Forms.LinkLabel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.GradientToColor = new ColorComboBox();
            this.GradientFromColor = new ColorComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.ChangeBaseStyleBtn = new System.Windows.Forms.Button();
            this.ColorBrewerColors = new System.Windows.Forms.RadioButton();
            this.GradientColors = new System.Windows.Forms.RadioButton();
            this.PreviewGroup = new System.Windows.Forms.GroupBox();
            this.PreviewPicture = new System.Windows.Forms.PictureBox();
            this.ButtonPanel = new System.Windows.Forms.Panel();
            this.CancelBtn = new System.Windows.Forms.Button();
            this.OKBtn = new System.Windows.Forms.Button();
            this.colorComboBox1 = new ColorComboBox();
            ((System.ComponentModel.ISupportInitialize)(this.RuleCount)).BeginInit();
            this.DataGroup.SuspendLayout();
            this.GroupPanel.SuspendLayout();
            this.RuleCountPanel.SuspendLayout();
            this.DisplayGroup.SuspendLayout();
            this.ColorBrewerPanel.SuspendLayout();
            this.panel2.SuspendLayout();
            this.PreviewGroup.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PreviewPicture)).BeginInit();
            this.ButtonPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // AggregateLabel
            // 
            resources.ApplyResources(this.AggregateLabel, "AggregateLabel");
            this.AggregateLabel.Name = "AggregateLabel";
            // 
            // OverwriteRules
            // 
            resources.ApplyResources(this.OverwriteRules, "OverwriteRules");
            this.OverwriteRules.Checked = true;
            this.OverwriteRules.Name = "OverwriteRules";
            this.OverwriteRules.TabStop = true;
            this.OverwriteRules.UseVisualStyleBackColor = true;
            // 
            // AppendRules
            // 
            resources.ApplyResources(this.AppendRules, "AppendRules");
            this.AppendRules.Name = "AppendRules";
            this.AppendRules.UseVisualStyleBackColor = true;
            // 
            // ColumnCombo
            // 
            resources.ApplyResources(this.ColumnCombo, "ColumnCombo");
            this.ColumnCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ColumnCombo.FormattingEnabled = true;
            this.ColumnCombo.Name = "ColumnCombo";
            this.ColumnCombo.SelectedIndexChanged += new System.EventHandler(this.ColumnCombo_SelectedIndexChanged);
            // 
            // RuleCount
            // 
            resources.ApplyResources(this.RuleCount, "RuleCount");
            this.RuleCount.Minimum = new decimal(new int[] {
            3,
            0,
            0,
            0});
            this.RuleCount.Name = "RuleCount";
            this.RuleCount.Value = new decimal(new int[] {
            3,
            0,
            0,
            0});
            this.RuleCount.ValueChanged += new System.EventHandler(this.RuleCount_ValueChanged);
            // 
            // AggregateCombo
            // 
            resources.ApplyResources(this.AggregateCombo, "AggregateCombo");
            this.AggregateCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.AggregateCombo.FormattingEnabled = true;
            this.AggregateCombo.Items.AddRange(new object[] {
            resources.GetString("AggregateCombo.Items"),
            resources.GetString("AggregateCombo.Items1"),
            resources.GetString("AggregateCombo.Items2"),
            resources.GetString("AggregateCombo.Items3")});
            this.AggregateCombo.Name = "AggregateCombo";
            this.AggregateCombo.SelectedIndexChanged += new System.EventHandler(this.AggregateCombo_SelectedIndexChanged);
            // 
            // DataGroup
            // 
            resources.ApplyResources(this.DataGroup, "DataGroup");
            this.DataGroup.Controls.Add(this.GroupPanel);
            this.DataGroup.Controls.Add(this.RuleCountPanel);
            this.DataGroup.Controls.Add(this.ColumnCombo);
            this.DataGroup.Controls.Add(this.AppendRules);
            this.DataGroup.Controls.Add(this.OverwriteRules);
            this.DataGroup.Controls.Add(this.label1);
            this.DataGroup.Name = "DataGroup";
            this.DataGroup.TabStop = false;
            // 
            // GroupPanel
            // 
            this.GroupPanel.Controls.Add(this.AggregateLabel);
            this.GroupPanel.Controls.Add(this.AggregateCombo);
            resources.ApplyResources(this.GroupPanel, "GroupPanel");
            this.GroupPanel.Name = "GroupPanel";
            // 
            // RuleCountPanel
            // 
            resources.ApplyResources(this.RuleCountPanel, "RuleCountPanel");
            this.RuleCountPanel.Controls.Add(this.RuleCount);
            this.RuleCountPanel.Controls.Add(this.label2);
            this.RuleCountPanel.Name = "RuleCountPanel";
            // 
            // DisplayGroup
            // 
            resources.ApplyResources(this.DisplayGroup, "DisplayGroup");
            this.DisplayGroup.Controls.Add(this.ColorBrewerPanel);
            this.DisplayGroup.Controls.Add(this.ColorBrewerLabel);
            this.DisplayGroup.Controls.Add(this.panel2);
            this.DisplayGroup.Controls.Add(this.ChangeBaseStyleBtn);
            this.DisplayGroup.Controls.Add(this.ColorBrewerColors);
            this.DisplayGroup.Controls.Add(this.GradientColors);
            this.DisplayGroup.Name = "DisplayGroup";
            this.DisplayGroup.TabStop = false;
            // 
            // ColorBrewerPanel
            // 
            this.ColorBrewerPanel.Controls.Add(this.label3);
            this.ColorBrewerPanel.Controls.Add(this.ColorBrewerDataType);
            this.ColorBrewerPanel.Controls.Add(this.ColorBrewerColorSet);
            resources.ApplyResources(this.ColorBrewerPanel, "ColorBrewerPanel");
            this.ColorBrewerPanel.Name = "ColorBrewerPanel";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // ColorBrewerDataType
            // 
            this.ColorBrewerDataType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ColorBrewerDataType.FormattingEnabled = true;
            resources.ApplyResources(this.ColorBrewerDataType, "ColorBrewerDataType");
            this.ColorBrewerDataType.Name = "ColorBrewerDataType";
            this.ColorBrewerDataType.SelectedIndexChanged += new System.EventHandler(this.ColorBrewerDataType_SelectedIndexChanged);
            // 
            // ColorBrewerColorSet
            // 
            this.ColorBrewerColorSet.DropDownWidth = 150;
            this.ColorBrewerColorSet.FormattingEnabled = true;
            resources.ApplyResources(this.ColorBrewerColorSet, "ColorBrewerColorSet");
            this.ColorBrewerColorSet.Name = "ColorBrewerColorSet";
            this.ColorBrewerColorSet.SelectedIndexChanged += new System.EventHandler(this.ColorBrewerColorSet_SelectedIndexChanged);
            // 
            // ColorBrewerLabel
            // 
            resources.ApplyResources(this.ColorBrewerLabel, "ColorBrewerLabel");
            this.ColorBrewerLabel.Name = "ColorBrewerLabel";
            this.ColorBrewerLabel.TabStop = true;
            this.ColorBrewerLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.ColorBrewerLabel_LinkClicked);
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.GradientToColor);
            this.panel2.Controls.Add(this.GradientFromColor);
            this.panel2.Controls.Add(this.label4);
            resources.ApplyResources(this.panel2, "panel2");
            this.panel2.Name = "panel2";
            // 
            // GradientToColor
            // 
            this.GradientToColor.FormattingEnabled = true;
            resources.ApplyResources(this.GradientToColor, "GradientToColor");
            this.GradientToColor.Name = "GradientToColor";
            this.GradientToColor.SelectedIndexChanged += new System.EventHandler(this.GradientToColor_SelectedIndexChanged);
            // 
            // GradientFromColor
            // 
            this.GradientFromColor.FormattingEnabled = true;
            resources.ApplyResources(this.GradientFromColor, "GradientFromColor");
            this.GradientFromColor.Name = "GradientFromColor";
            this.GradientFromColor.SelectedIndexChanged += new System.EventHandler(this.GradientFromColor_SelectedIndexChanged);
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // ChangeBaseStyleBtn
            // 
            resources.ApplyResources(this.ChangeBaseStyleBtn, "ChangeBaseStyleBtn");
            this.ChangeBaseStyleBtn.Name = "ChangeBaseStyleBtn";
            this.ChangeBaseStyleBtn.UseVisualStyleBackColor = true;
            this.ChangeBaseStyleBtn.Click += new System.EventHandler(this.ChangeBaseStyleBtn_Click);
            // 
            // ColorBrewerColors
            // 
            resources.ApplyResources(this.ColorBrewerColors, "ColorBrewerColors");
            this.ColorBrewerColors.Name = "ColorBrewerColors";
            this.ColorBrewerColors.TabStop = true;
            this.ColorBrewerColors.UseVisualStyleBackColor = true;
            this.ColorBrewerColors.CheckedChanged += new System.EventHandler(this.ColorBrewerColors_CheckedChanged);
            // 
            // GradientColors
            // 
            resources.ApplyResources(this.GradientColors, "GradientColors");
            this.GradientColors.Name = "GradientColors";
            this.GradientColors.TabStop = true;
            this.GradientColors.UseVisualStyleBackColor = true;
            this.GradientColors.CheckedChanged += new System.EventHandler(this.GradientColors_CheckedChanged);
            // 
            // PreviewGroup
            // 
            resources.ApplyResources(this.PreviewGroup, "PreviewGroup");
            this.PreviewGroup.Controls.Add(this.PreviewPicture);
            this.PreviewGroup.Name = "PreviewGroup";
            this.PreviewGroup.TabStop = false;
            // 
            // PreviewPicture
            // 
            resources.ApplyResources(this.PreviewPicture, "PreviewPicture");
            this.PreviewPicture.Name = "PreviewPicture";
            this.PreviewPicture.TabStop = false;
            // 
            // ButtonPanel
            // 
            this.ButtonPanel.Controls.Add(this.CancelBtn);
            this.ButtonPanel.Controls.Add(this.OKBtn);
            resources.ApplyResources(this.ButtonPanel, "ButtonPanel");
            this.ButtonPanel.Name = "ButtonPanel";
            // 
            // CancelBtn
            // 
            resources.ApplyResources(this.CancelBtn, "CancelBtn");
            this.CancelBtn.Name = "CancelBtn";
            this.CancelBtn.UseVisualStyleBackColor = true;
            this.CancelBtn.Click += new System.EventHandler(this.CancelBtn_Click);
            // 
            // OKBtn
            // 
            resources.ApplyResources(this.OKBtn, "OKBtn");
            this.OKBtn.Name = "OKBtn";
            this.OKBtn.UseVisualStyleBackColor = true;
            this.OKBtn.Click += new System.EventHandler(this.OKBtn_Click);
            // 
            // colorComboBox1
            // 
            resources.ApplyResources(this.colorComboBox1, "colorComboBox1");
            this.colorComboBox1.Name = "colorComboBox1";
            // 
            // ThemeCreator
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.ButtonPanel);
            this.Controls.Add(this.PreviewGroup);
            this.Controls.Add(this.DisplayGroup);
            this.Controls.Add(this.DataGroup);
            this.Name = "ThemeCreator";
            this.Load += new System.EventHandler(this.ThemeCreator_Load);
            ((System.ComponentModel.ISupportInitialize)(this.RuleCount)).EndInit();
            this.DataGroup.ResumeLayout(false);
            this.DataGroup.PerformLayout();
            this.GroupPanel.ResumeLayout(false);
            this.GroupPanel.PerformLayout();
            this.RuleCountPanel.ResumeLayout(false);
            this.RuleCountPanel.PerformLayout();
            this.DisplayGroup.ResumeLayout(false);
            this.DisplayGroup.PerformLayout();
            this.ColorBrewerPanel.ResumeLayout(false);
            this.ColorBrewerPanel.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.PreviewGroup.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.PreviewPicture)).EndInit();
            this.ButtonPanel.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label AggregateLabel;
        private System.Windows.Forms.RadioButton OverwriteRules;
        private System.Windows.Forms.RadioButton AppendRules;
        private System.Windows.Forms.ComboBox ColumnCombo;
        private System.Windows.Forms.NumericUpDown RuleCount;
        private System.Windows.Forms.ComboBox AggregateCombo;
        private System.Windows.Forms.GroupBox DataGroup;
        private System.Windows.Forms.GroupBox DisplayGroup;
        private System.Windows.Forms.GroupBox PreviewGroup;
        private System.Windows.Forms.Panel ButtonPanel;
        private System.Windows.Forms.Button CancelBtn;
        private System.Windows.Forms.Button OKBtn;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button ChangeBaseStyleBtn;
        private System.Windows.Forms.RadioButton ColorBrewerColors;
        private System.Windows.Forms.RadioButton GradientColors;
        private CustomCombo ColorBrewerColorSet;
        private System.Windows.Forms.Label label4;
        private ColorComboBox colorComboBox1;
        private System.Windows.Forms.Panel RuleCountPanel;
        private ColorComboBox GradientToColor;
        private ColorComboBox GradientFromColor;
        private System.Windows.Forms.PictureBox PreviewPicture;
        private System.Windows.Forms.Panel GroupPanel;
        private System.Windows.Forms.Panel ColorBrewerPanel;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox ColorBrewerDataType;
        private System.Windows.Forms.LinkLabel ColorBrewerLabel;
    }
}