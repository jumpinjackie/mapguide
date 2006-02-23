using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;

using System.Data;
using System.Data.SqlTypes;

using SqliteDotNet;

namespace SqliteGUI
{
    /// <summary>
    /// Summary description for NewAddTestForm.
    /// </summary>
    public class NewAddTestForm : System.Windows.Forms.Form
    {
        private string testName = "";
        private string paramSet = "";
        private SqliteDb database;
        private DataTable dataTable = null;
        private SqliteVm vm;
        private System.Windows.Forms.Label newTestNameLabel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button addTestButton;
        private System.Windows.Forms.Button deleteTestButton;
        private System.Windows.Forms.TextBox addTestBox;
        private System.Windows.Forms.TextBox prerequisite;
        private System.Windows.Forms.TextBox executeSequence;
        private System.Windows.Forms.Label prerequisiteLabel;
        private System.Windows.Forms.Label testDescriptionLabel;
        private System.Windows.Forms.Label executeSequenceLabel;
        private System.Windows.Forms.ComboBox testNameList;
        private System.Windows.Forms.Label testNameLabel;
        private System.Windows.Forms.Button DeleteParamSet;
        private System.Windows.Forms.Button AddParamSet;
        private System.Windows.Forms.DataGrid dataGrid1;
        private System.Windows.Forms.Label sampleResultLabel;
        private System.Windows.Forms.Label contentTypeLabel;
        private System.Windows.Forms.Label resultDescriptionLabel;
        private System.Windows.Forms.TextBox contentType;
        private System.Windows.Forms.RichTextBox sampleResult;
        private System.Windows.Forms.RichTextBox resultDescription;
        private System.Windows.Forms.TextBox paramSetBox;
        private System.Windows.Forms.ListBox paramSetsList;
        private System.Windows.Forms.Label testTypeLabel;
        private System.Windows.Forms.TextBox testType;
        private System.Windows.Forms.TextBox testDescription;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog2;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public NewAddTestForm()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            //
            // TODO: Add any constructor code after InitializeComponent call
            //
            //Set up the table for the params
            this.SetTable();
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            //If the database is writeable save the data else continue with disposing
            if ((null != this.database) && (!Form1.IsReadOnly(this.database.GetName())))
            {
                this.SaveData(true);
            }

            this.dataTable.Dispose();
            if( disposing )
            {
                //Finalize the virtual machine
                if (this.vm != null)
                {
                    this.vm.SqlFinalize();
                }

                if(components != null)
                {
                    components.Dispose();
                }
            }

            base.Dispose( disposing );
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.newTestNameLabel = new System.Windows.Forms.Label();
            this.DeleteParamSet = new System.Windows.Forms.Button();
            this.AddParamSet = new System.Windows.Forms.Button();
            this.paramSetBox = new System.Windows.Forms.TextBox();
            this.paramSetsList = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.addTestButton = new System.Windows.Forms.Button();
            this.deleteTestButton = new System.Windows.Forms.Button();
            this.addTestBox = new System.Windows.Forms.TextBox();
            this.prerequisite = new System.Windows.Forms.TextBox();
            this.executeSequence = new System.Windows.Forms.TextBox();
            this.prerequisiteLabel = new System.Windows.Forms.Label();
            this.testDescriptionLabel = new System.Windows.Forms.Label();
            this.executeSequenceLabel = new System.Windows.Forms.Label();
            this.testNameList = new System.Windows.Forms.ComboBox();
            this.testNameLabel = new System.Windows.Forms.Label();
            this.dataGrid1 = new System.Windows.Forms.DataGrid();
            this.sampleResultLabel = new System.Windows.Forms.Label();
            this.contentTypeLabel = new System.Windows.Forms.Label();
            this.resultDescriptionLabel = new System.Windows.Forms.Label();
            this.contentType = new System.Windows.Forms.TextBox();
            this.sampleResult = new System.Windows.Forms.RichTextBox();
            this.resultDescription = new System.Windows.Forms.RichTextBox();
            this.testTypeLabel = new System.Windows.Forms.Label();
            this.testType = new System.Windows.Forms.TextBox();
            this.testDescription = new System.Windows.Forms.TextBox();
            this.saveButton = new System.Windows.Forms.Button();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.saveFileDialog2 = new System.Windows.Forms.SaveFileDialog();
            ((System.ComponentModel.ISupportInitialize)(this.dataGrid1)).BeginInit();
            this.SuspendLayout();
            // 
            // newTestNameLabel
            // 
            this.newTestNameLabel.Location = new System.Drawing.Point(8, 56);
            this.newTestNameLabel.Name = "newTestNameLabel";
            this.newTestNameLabel.Size = new System.Drawing.Size(88, 23);
            this.newTestNameLabel.TabIndex = 17;
            this.newTestNameLabel.Text = "New Test Name";
            this.newTestNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // DeleteParamSet
            // 
            this.DeleteParamSet.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.DeleteParamSet.Location = new System.Drawing.Point(232, 336);
            this.DeleteParamSet.Name = "DeleteParamSet";
            this.DeleteParamSet.TabIndex = 10;
            this.DeleteParamSet.Text = "Delete";
            this.DeleteParamSet.Click += new System.EventHandler(this.DeleteParamSet_Click);
            // 
            // AddParamSet
            // 
            this.AddParamSet.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.AddParamSet.Location = new System.Drawing.Point(232, 448);
            this.AddParamSet.Name = "AddParamSet";
            this.AddParamSet.TabIndex = 9;
            this.AddParamSet.Text = "Add";
            this.AddParamSet.Click += new System.EventHandler(this.AddParamSet_Click);
            // 
            // paramSetBox
            // 
            this.paramSetBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.paramSetBox.Location = new System.Drawing.Point(120, 448);
            this.paramSetBox.Name = "paramSetBox";
            this.paramSetBox.TabIndex = 8;
            this.paramSetBox.Text = "";
            // 
            // paramSetsList
            // 
            this.paramSetsList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left)));
            this.paramSetsList.Location = new System.Drawing.Point(120, 280);
            this.paramSetsList.Name = "paramSetsList";
            this.paramSetsList.Size = new System.Drawing.Size(104, 147);
            this.paramSetsList.TabIndex = 85;
            this.paramSetsList.SelectedIndexChanged += new System.EventHandler(this.paramSetsList_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(32, 280);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(80, 23);
            this.label1.TabIndex = 12;
            this.label1.Text = "Param Sets";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // addTestButton
            // 
            this.addTestButton.Location = new System.Drawing.Point(432, 56);
            this.addTestButton.Name = "addTestButton";
            this.addTestButton.Size = new System.Drawing.Size(80, 23);
            this.addTestButton.TabIndex = 3;
            this.addTestButton.Text = "Add Test";
            this.addTestButton.Click += new System.EventHandler(this.addTestButton_Click);
            // 
            // deleteTestButton
            // 
            this.deleteTestButton.Location = new System.Drawing.Point(432, 16);
            this.deleteTestButton.Name = "deleteTestButton";
            this.deleteTestButton.Size = new System.Drawing.Size(80, 24);
            this.deleteTestButton.TabIndex = 19;
            this.deleteTestButton.Text = "DeleteTest";
            this.deleteTestButton.Click += new System.EventHandler(this.deleteTestButton_Click);
            // 
            // addTestBox
            // 
            this.addTestBox.Location = new System.Drawing.Point(120, 56);
            this.addTestBox.Name = "addTestBox";
            this.addTestBox.Size = new System.Drawing.Size(288, 20);
            this.addTestBox.TabIndex = 2;
            this.addTestBox.Text = "";
            // 
            // prerequisite
            // 
            this.prerequisite.Location = new System.Drawing.Point(120, 208);
            this.prerequisite.Name = "prerequisite";
            this.prerequisite.Size = new System.Drawing.Size(280, 20);
            this.prerequisite.TabIndex = 6;
            this.prerequisite.Text = "";
            // 
            // executeSequence
            // 
            this.executeSequence.Location = new System.Drawing.Point(120, 96);
            this.executeSequence.Name = "executeSequence";
            this.executeSequence.TabIndex = 4;
            this.executeSequence.Text = "";
            // 
            // prerequisiteLabel
            // 
            this.prerequisiteLabel.Location = new System.Drawing.Point(16, 208);
            this.prerequisiteLabel.Name = "prerequisiteLabel";
            this.prerequisiteLabel.TabIndex = 5;
            this.prerequisiteLabel.Text = "Prerequisite";
            this.prerequisiteLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // testDescriptionLabel
            // 
            this.testDescriptionLabel.Location = new System.Drawing.Point(8, 136);
            this.testDescriptionLabel.Name = "testDescriptionLabel";
            this.testDescriptionLabel.Size = new System.Drawing.Size(88, 16);
            this.testDescriptionLabel.TabIndex = 4;
            this.testDescriptionLabel.Text = "Test Description";
            this.testDescriptionLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // executeSequenceLabel
            // 
            this.executeSequenceLabel.Location = new System.Drawing.Point(8, 96);
            this.executeSequenceLabel.Name = "executeSequenceLabel";
            this.executeSequenceLabel.TabIndex = 3;
            this.executeSequenceLabel.Text = "Execute Sequence";
            this.executeSequenceLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // testNameList
            // 
            this.testNameList.ItemHeight = 13;
            this.testNameList.Location = new System.Drawing.Point(120, 16);
            this.testNameList.Name = "testNameList";
            this.testNameList.Size = new System.Drawing.Size(288, 21);
            this.testNameList.TabIndex = 1;
            this.testNameList.SelectedIndexChanged += new System.EventHandler(this.testNameList_SelectedIndexChanged);
            // 
            // testNameLabel
            // 
            this.testNameLabel.Location = new System.Drawing.Point(24, 16);
            this.testNameLabel.Name = "testNameLabel";
            this.testNameLabel.Size = new System.Drawing.Size(72, 24);
            this.testNameLabel.TabIndex = 22;
            this.testNameLabel.Text = "Test Name";
            this.testNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // dataGrid1
            // 
            this.dataGrid1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGrid1.DataMember = "";
            this.dataGrid1.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.dataGrid1.Location = new System.Drawing.Point(464, 144);
            this.dataGrid1.Name = "dataGrid1";
            this.dataGrid1.Size = new System.Drawing.Size(408, 224);
            this.dataGrid1.TabIndex = 11;
            // 
            // sampleResultLabel
            // 
            this.sampleResultLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.sampleResultLabel.Location = new System.Drawing.Point(368, 464);
            this.sampleResultLabel.Name = "sampleResultLabel";
            this.sampleResultLabel.Size = new System.Drawing.Size(80, 23);
            this.sampleResultLabel.TabIndex = 81;
            this.sampleResultLabel.Text = "Sample Result";
            this.sampleResultLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // contentTypeLabel
            // 
            this.contentTypeLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.contentTypeLabel.Location = new System.Drawing.Point(376, 432);
            this.contentTypeLabel.Name = "contentTypeLabel";
            this.contentTypeLabel.Size = new System.Drawing.Size(72, 16);
            this.contentTypeLabel.TabIndex = 80;
            this.contentTypeLabel.Text = "Content Type";
            this.contentTypeLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // resultDescriptionLabel
            // 
            this.resultDescriptionLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.resultDescriptionLabel.Location = new System.Drawing.Point(384, 384);
            this.resultDescriptionLabel.Name = "resultDescriptionLabel";
            this.resultDescriptionLabel.Size = new System.Drawing.Size(72, 23);
            this.resultDescriptionLabel.TabIndex = 79;
            this.resultDescriptionLabel.Text = "Description";
            this.resultDescriptionLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // contentType
            // 
            this.contentType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.contentType.Location = new System.Drawing.Point(464, 432);
            this.contentType.Name = "contentType";
            this.contentType.Size = new System.Drawing.Size(120, 20);
            this.contentType.TabIndex = 13;
            this.contentType.Text = "";
            // 
            // sampleResult
            // 
            this.sampleResult.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.sampleResult.Location = new System.Drawing.Point(464, 464);
            this.sampleResult.Name = "sampleResult";
            this.sampleResult.Size = new System.Drawing.Size(408, 104);
            this.sampleResult.TabIndex = 14;
            this.sampleResult.Text = "";
            // 
            // resultDescription
            // 
            this.resultDescription.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.resultDescription.Location = new System.Drawing.Point(464, 384);
            this.resultDescription.Name = "resultDescription";
            this.resultDescription.Size = new System.Drawing.Size(408, 32);
            this.resultDescription.TabIndex = 12;
            this.resultDescription.Text = "";
            // 
            // testTypeLabel
            // 
            this.testTypeLabel.Location = new System.Drawing.Point(16, 240);
            this.testTypeLabel.Name = "testTypeLabel";
            this.testTypeLabel.TabIndex = 82;
            this.testTypeLabel.Text = "Test Type";
            this.testTypeLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // testType
            // 
            this.testType.Location = new System.Drawing.Point(120, 240);
            this.testType.Name = "testType";
            this.testType.Size = new System.Drawing.Size(104, 20);
            this.testType.TabIndex = 7;
            this.testType.Text = "";
            // 
            // testDescription
            // 
            this.testDescription.Location = new System.Drawing.Point(120, 128);
            this.testDescription.Multiline = true;
            this.testDescription.Name = "testDescription";
            this.testDescription.Size = new System.Drawing.Size(280, 56);
            this.testDescription.TabIndex = 5;
            this.testDescription.Text = "";
            // 
            // saveButton
            // 
            this.saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.saveButton.Location = new System.Drawing.Point(768, 584);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(96, 23);
            this.saveButton.TabIndex = 15;
            this.saveButton.Text = "Save Changes";
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // NewAddTestForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(888, 618);
            this.Controls.Add(this.saveButton);
            this.Controls.Add(this.testType);
            this.Controls.Add(this.testTypeLabel);
            this.Controls.Add(this.sampleResultLabel);
            this.Controls.Add(this.contentTypeLabel);
            this.Controls.Add(this.resultDescriptionLabel);
            this.Controls.Add(this.contentType);
            this.Controls.Add(this.sampleResult);
            this.Controls.Add(this.resultDescription);
            this.Controls.Add(this.dataGrid1);
            this.Controls.Add(this.newTestNameLabel);
            this.Controls.Add(this.DeleteParamSet);
            this.Controls.Add(this.AddParamSet);
            this.Controls.Add(this.paramSetBox);
            this.Controls.Add(this.paramSetsList);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.addTestButton);
            this.Controls.Add(this.deleteTestButton);
            this.Controls.Add(this.addTestBox);
            this.Controls.Add(this.prerequisite);
            this.Controls.Add(this.testDescription);
            this.Controls.Add(this.executeSequence);
            this.Controls.Add(this.prerequisiteLabel);
            this.Controls.Add(this.testDescriptionLabel);
            this.Controls.Add(this.executeSequenceLabel);
            this.Controls.Add(this.testNameList);
            this.Controls.Add(this.testNameLabel);
            this.Name = "NewAddTestForm";
            this.Text = "NewAddTestForm";
            ((System.ComponentModel.ISupportInitialize)(this.dataGrid1)).EndInit();
            this.ResumeLayout(false);

        }
        #endregion

        //Loads the form
        public void LoadForm(SqliteDb newDatabase)
        {
            //Create a new virtual machine
            this.database = newDatabase;
            int status = Sqlite.Ok;
            this.vm = new SqliteVm(newDatabase, true);

            //Get all tests from the test case table and add the in the dropdown box
            status = this.vm.Execute("Select TestName from TestCase");
            while (Sqlite.Row == status)
            {
                this.testNameList.Items.Add(this.vm.GetString("TestName"));
                status = this.vm.NextRow();
            }
            //If we have any tests in the list select the first one by default
            if (this.testNameList.Items.Count > 0)
            {
                this.testNameList.SelectedIndex = 0;
                this.testName = this.testNameList.SelectedItem.ToString();
            }
        }

        //Updates fields related to test case table
        private void UpdateTestCaseFields(string testName)
        {
            //Get all fields for the given test name
            int status = Sqlite.Ok;
            status = this.vm.Execute("Select * from TestCase where TestName=\""+testName+"\"");

            //If that test is in the database update the corresponding fields
            //Note: we need to keep track on the current test name to be able to save the data after a new selection
            if ((Sqlite.Done == status) || (Sqlite.Row == status))
            {
                this.executeSequence.Text = this.vm.GetString("ExecuteSequence");
                this.testDescription.Text = this.vm.GetString("Description");
                this.prerequisite.Text = this.vm.GetString("Prerequisite");
                this.testType.Text = this.vm.GetString("TestType");
                this.UpdateParamSetsList(this.vm.GetString("ParamSets"));
                this.testName = testName;
            }
        }

        //Clears all fields related to the TestCase table
        private void ClearTestCaseFields()
        {
            this.executeSequence.Clear();
            this.testDescription.Clear();
            this.prerequisite.Clear();
            this.testType.Clear();
            this.paramSetsList.Items.Clear();
            this.paramSet = "";
        }

        //Called when new test name is selected from the list
        private void testNameList_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            //If the database is writeable save the current form or discard it silently
            if ((null != this.database) && (!Form1.IsReadOnly(this.database.GetName())))
            {
                this.SaveData(true);
            }

            //Clears the form
            this.ClearTestCaseFields();
            this.ClearSampleResultFields();
            this.ClearDataTableFields();

            //If there is a test currently selected update the fields related to the TestCase tables
            if (-1 != this.testNameList.SelectedIndex)
            {
                this.UpdateTestCaseFields(this.testNameList.SelectedItem.ToString());
            }
        }

        //Updates the paramSets list
        private void UpdateParamSetsList(string paramSets)
        {
            string[] setsList;
            int i = 0;
            this.paramSetsList.Items.Clear();

            //Split the string into array
            setsList = paramSets.Split(',');

            //Do not add blank paramSets
            while (i<setsList.Length && "" != setsList[i])
            {
                this.paramSetsList.Items.Add(setsList[i]);
                i++;
            }
        }

        //Sets the field in the table (there is not effect right now)
        private void SetTable()
        {
            this.dataTable = new DataTable("Parameters");

            DataGridTableStyle tableStyle = new DataGridTableStyle();
            tableStyle.MappingName = "Parameters";

            DataGridTextBoxColumn colStyle = new DataGridTextBoxColumn();

            tableStyle.PreferredColumnWidth *= 2;
            tableStyle.PreferredRowHeight *= 2;

            this.dataGrid1.Width = 400;

            colStyle.MappingName = "ParamName";
            colStyle.HeaderText = "ParamName";
            this.dataTable.Columns.Add("ParamName");
            colStyle.Width = (int) (0.3*this.dataGrid1.Width);
            tableStyle.GridColumnStyles.Add(colStyle);
            colStyle.Dispose();

            colStyle = new DataGridTextBoxColumn();
            colStyle.MappingName = "ParamValue";
            colStyle.HeaderText = "ParamValue";
            this.dataTable.Columns.Add("ParamValue");
            colStyle.Width = (int) (0.6*this.dataGrid1.Width);
            tableStyle.GridColumnStyles.Add(colStyle);

            this.dataGrid1.TableStyles.Add(tableStyle);

            this.dataGrid1.DataSource = this.dataTable;
        }

        //Called when new paramSet is selected from the list
        private void paramSetsList_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            //If the database is writeable then save the current form else silently discard it
            if ((null != this.database) && (!Form1.IsReadOnly(this.database.GetName())))
            {
                this.SaveData(true);
            }

            //Clears fields that are associated with the old paramSet
            this.ClearDataTableFields();
            this.ClearSampleResultFields();

            //If there is param set selected update appropriate fields
            ////Note: we need to keep track on the current param set to be able to save the data after a new selection
            if (-1 != this.paramSetsList.SelectedIndex)
            {
                this.paramSet = this.paramSetsList.SelectedItem.ToString();
                this.UpdateTableFields(this.paramSet);

                this.UpdateSampleResultFields(this.paramSet);
            }
        }

        private void ClearDataTableFields()
        {
            this.dataTable.Clear();
        }

        //Update the table containing the param name and param value
        private void UpdateTableFields(string paramSet)
        {
            int status;
            string [] cols = new string[2];

            //Empty paramSet forms an invalid SQL query that causes a crash. Need to check for it
            if ("" != paramSet)
            {
                //For each pair of param name and value add a row in the table
                status = this.vm.Execute("Select ParamName, ParamValue from Params where ParamSet="+paramSet);
                while (Sqlite.Row == status)
                {
                    cols[0] = this.vm.GetString("ParamName");
                    cols[1] = this.vm.GetString("ParamValue");

                    this.dataTable.Rows.Add(cols);

                    status = this.vm.NextRow();
                }
                this.dataTable.AcceptChanges();
            }
        }

        //Clear fields
        private void ClearSampleResultFields()
        {
            this.resultDescription.Clear();
            this.contentType.Clear();
            this.sampleResult.Clear();
        }

        //Update fields related to sample results(either ApiTestResults or HttpTestResults table)
        private void UpdateSampleResultFields(string paramSet)
        {
            int status = Sqlite.Done;

            //Empty paramSet forms an invalid SQL query that causes a crash. Need to check for it
            if ("" != paramSet)
            {
                //Check which table is going to be modified
                if ("Api" == this.testType.Text)
                {
                    status = this.vm.Execute("Select * from ApiTestResults where ParamSet="+paramSet);
                }
                else if("Http" == this.testType.Text)
                {
                    status = this.vm.Execute("Select * from HttpTestResults where ParamSet="+paramSet);
                }

                //Update the fields if there is any data in the database
                if (Sqlite.Row == status)
                {
                    this.resultDescription.Text = this.vm.GetString("Description");
                    this.contentType.Text = this.vm.GetString("ContentType");
                    this.sampleResult.Text = this.vm.GetString("Result");
                }
            }
        }

        //Called when adding new param set in the list
        private void AddParamSet_Click(object sender, System.EventArgs e)
        {
            //Check for empty and duplicate param sets and display error message
            if ("" == this.paramSetBox.Text)
            {
                System.Windows.Forms.MessageBox.Show("ParamSet cannot be blank",
                    "Error",
                    System.Windows.Forms.MessageBoxButtons.OK,
                    System.Windows.Forms.MessageBoxIcon.Error);
            }
            else if(this.paramSetsList.Items.Contains(this.paramSetBox.Text))
            {
                System.Windows.Forms.MessageBox.Show("ParamSet already exists",
                    "Error",
                    System.Windows.Forms.MessageBoxButtons.OK,
                    System.Windows.Forms.MessageBoxIcon.Error);
            }
            else
            {
                //Select the newly added param set and clear the text box
                this.paramSetsList.SetSelected(this.paramSetsList.Items.Add(this.paramSetBox.Text), true);
                this.paramSetBox.Clear();
            }
        }

        //Deletes param set from the list
        private void DeleteParamSet_Click(object sender, System.EventArgs e)
        {
            //If we have something selected, remove that item and clear the related fields and variables
            if (-1 != this.paramSetsList.SelectedIndex)
            {
                string message = String.Format("Delete param set {0} and all associated data?",
                                                this.paramSetsList.SelectedItem.ToString());
                DialogResult result = System.Windows.Forms.MessageBox.Show(message,
                    "Deleting Param Set",
                    System.Windows.Forms.MessageBoxButtons.YesNo,
                    System.Windows.Forms.MessageBoxIcon.Question);

                if (DialogResult.Yes == result)
                {
                    this.DeleteParamSetData(this.paramSetsList.SelectedItem.ToString());
                    this.paramSetsList.Items.RemoveAt(this.paramSetsList.SelectedIndex);
                    this.ClearDataTableFields();
                    this.ClearSampleResultFields();
                    this.paramSet = "";
                }
            }
        }

        //Called when new test is added
        private void addTestButton_Click(object sender, System.EventArgs e)
        {
            //Check for empty or duplicate names
            if ("" == this.addTestBox.Text)
            {
                System.Windows.Forms.MessageBox.Show("Test name cannot be blank",
                    "Error",
                    System.Windows.Forms.MessageBoxButtons.OK,
                    System.Windows.Forms.MessageBoxIcon.Error);
            }
            else if(this.testNameList.Items.Contains(this.addTestBox.Text))
            {
                System.Windows.Forms.MessageBox.Show("The test already exists",
                    "Error",
                    System.Windows.Forms.MessageBoxButtons.OK,
                    System.Windows.Forms.MessageBoxIcon.Error);
            }
            else
            {
                //Select the newly added test
                this.testNameList.SelectedIndex = this.testNameList.Items.Add(this.addTestBox.Text);
                this.addTestBox.Clear();
            }
        }

        //Called when a test is deleted
        private void deleteTestButton_Click(object sender, System.EventArgs e)
        {
            //If we have a test selected delete it and clear the appropriate fields
            if (-1 != this.testNameList.SelectedIndex)
            {
                string message = String.Format("Delete {0} and all associated data?",
                    this.testNameList.SelectedItem.ToString());
                DialogResult result = System.Windows.Forms.MessageBox.Show(message,
                    "Deleting Param Set",
                    System.Windows.Forms.MessageBoxButtons.YesNo,
                    System.Windows.Forms.MessageBoxIcon.Question);

                if (DialogResult.Yes == result)
                {
                    this.DeleteTest(this.testNameList.SelectedItem.ToString(), this.paramSetsList.Items);
                    this.testNameList.Items.RemoveAt(this.testNameList.SelectedIndex);
                    this.ClearTestCaseFields();
                    this.ClearDataTableFields();
                    this.ClearSampleResultFields();
                    this.testName = "";
                }
            }
        }

        //Called when the save button on the form is pressed
        private void saveButton_Click(object sender, System.EventArgs e)
        {
            if (null != this.database)
            {
                string dbName = this.database.GetName();

                //If the file is writeable or the user wants to overwrite read only file save the data
                if (!Form1.IsReadOnly(dbName) || this.Overwrite())
                {
                    this.SaveData(false);
                }
            }
        }

        //Validates parameters before storing them in the database
        //Silent validation is done when the form is closing. If the parameters are valid they will be
        //stored else changes will be lost
        private bool Validate(bool silentValidation)
        {
            bool valid = true;

            string errMsg = "The following errors have occurred: \n";
            string msgBody = "";

            if (this.paramSetsList.Items.Count == 0)
            {
                msgBody += "ParamSet list cannot be empty \n";
            }
            else if (this.paramSetsList.SelectedIndex == -1)
            {
                msgBody += "ParamSet must be selected \n";
            }

            if (this.executeSequence.Text == "")
            {
                msgBody += "ExecuteSequence cannot be blank \n";
            }

            if (this.testNameList.Items.Count == 0)
            {
                msgBody += "TestName list cannot be empty\n";
            }
            else if (this.testNameList.SelectedIndex == -1)
            {
                msgBody += "TestName must be selected \n";
            }

            if ((this.dataTable != null) && (this.dataTable.Rows.Count == 0))
            {
                msgBody += "Parameter list cannot be empty \n";
            }
            if ( (this.testType.Text != "Api") && (this.testType.Text != "Http"))
            {
                msgBody += "Test type is invalid. \n";
            }

            if (msgBody != "")
            {
                valid = false;
                if (!silentValidation)
                {
                    errMsg += msgBody;
                    System.Windows.Forms.MessageBox.Show(errMsg,
                        "Invalid Data",
                        System.Windows.Forms.MessageBoxButtons.OK,
                        System.Windows.Forms.MessageBoxIcon.Error);
                }
            }
            return valid;
        }

        //Saves the data on the form
        public void SaveData(bool silentValidation)
        {
            if (this.Validate(silentValidation))
            {
                this.SaveParamsTable();
                this.SaveTestResultsTable();
                this.SaveTestCaseTable();
            }
        }

        //Save changes in TestCase table
        private void SaveTestCaseTable()
        {
            string sqlStmt;
            //If there is already an entry with the same test name then update the fields else insert a new row
            if (Sqlite.Row == this.vm.Execute("Select * From TestCase where TestName=\'"+this.testNameList.SelectedItem.ToString()+"\'"))
            {
                sqlStmt = String.Format("UPDATE TestCase SET ExecuteSequence={0}, ParamSets=\"{1}\", Description=\"{2}\", TestType=\"{3}\", Prerequisite=\"{4}\" WHERE TestName=\'{5}\'",
                    this.executeSequence.Text,
                    this.ParamSetsListToString(),
                    this.testDescription.Text,
                    this.testType.Text,
                    this.prerequisite.Text,
                    this.testName);
            }
            else
            {
                sqlStmt = String.Format("INSERT INTO TestCase VALUES({0},\'{1}\', \'{2}\', \'{3}\', \'{4}\', \'{5}\');",
                    this.executeSequence.Text,
                    this.testName,
                    this.ParamSetsListToString(),
                    this.testDescription.Text,
                    this.testType.Text,
                    this.prerequisite.Text);
            }
            this.vm.Execute(sqlStmt);
        }

        //Save changes in params table
        private void SaveParamsTable()
        {
            string sqlStmt;
            int numRows = this.dataTable.Rows.Count;
            int i = 0;

            //We need param set for the queries
            if ("" != this.paramSet)
            {
                for (i=0 ; i<numRows; i++)
                {
                    if (DataRowState.Deleted == this.dataTable.Rows[i].RowState)
                    {
                        this.dataTable.Rows[i].RejectChanges();
                        sqlStmt = String.Format("DELETE From Params where ParamSet={0} AND ParamName=\'{1}\'",
                            this.paramSet,
                            this.dataTable.Rows[i][0].ToString());
                    }
                    else
                    {
                        //If there the param name already exists then update the fields else insert new row
                        sqlStmt = String.Format("Select * From Params where ParamSet={0} AND ParamName=\'{1}\'",
                            this.paramSet,
                            this.dataTable.Rows[i][0].ToString());

                        if (Sqlite.Row == this.vm.Execute(sqlStmt))
                        {
                            sqlStmt = String.Format("UPDATE Params SET ParamValue=\'{0}\' WHERE ParamSet={1} AND ParamName=\'{2}\'",
                                this.dataTable.Rows[i][1].ToString(),
                                this.paramSet,
                                this.dataTable.Rows[i][0].ToString());
                        }
                        else
                        {
                            sqlStmt = String.Format("INSERT INTO Params VALUES({0}, \'{1}\', \'{2}\')",
                                this.paramSet,
                                this.dataTable.Rows[i][0].ToString(),
                                this.dataTable.Rows[i][1].ToString());
                        }
                    }
                    this.vm.Execute(sqlStmt);
                }
            }
        }

        //Saves sample results
        private void SaveTestResultsTable()
        {
            string sqlStmt;
            string tableName = "";

            if ("" != this.paramSet)
            {
                //Determine which table to modify based on the test type
                if ("Api" == this.testType.Text)
                {
                    tableName = "ApiTestResults";
                }
                else if ("Http" == this.testType.Text)
                {
                    tableName = "HttpTestResults";
                }

                //If the param set given is already in the table then update the fields else insert a new row
                sqlStmt = String.Format("Select * From {0} where ParamSet={1}",
                    tableName,
                    this.paramSet);
                if (Sqlite.Row == this.vm.Execute(sqlStmt))
                {
                    sqlStmt = String.Format("UPDATE {0} SET Description=\'{1}\', ContentType=\'{2}\', Result=\'{3}\' WHERE ParamSet = {4}",
                        tableName,
                        this.resultDescription.Text,
                        this.contentType.Text,
                        this.sampleResult.Text,
                        this.paramSet);
                }
                else
                {
                    sqlStmt = String.Format("INSERT INTO {0} VALUES(\'{1}\', {2}, \'{3}\', \'{4}\')",
                        tableName,
                        this.resultDescription.Text,
                        this.paramSet,
                        this.contentType.Text,
                        this.sampleResult.Text);
                }
                this.vm.Execute(sqlStmt);
            }
        }

        //Takes the param set list from the form and converts it to a comma separated string that is stored in the database
        private string ParamSetsListToString()
        {
            string paramSets = "";
            int i = 0;

            for (i=0; i<this.paramSetsList.Items.Count; i++)
            {
                paramSets += this.paramSetsList.Items[i]+",";
            }
            return paramSets;
        }

        private bool DeleteParamSetData(string paramSet)
        {
            bool deleted = true;

            if (null != this.database)
            {
                //If the database is writeable or the user wants to overwrite existing database
                //then delete the data related to this param set
                if ((!Form1.IsReadOnly(this.database.GetName())) || (this.Overwrite()))
                {
                    this.vm.Execute("DELETE FROM Params WHERE ParamSet="+paramSet);
                    //Check which result table to modify.
                    //If the test type is invalid nothing will be deleted. Shall we display error message?
                    if ("Api" == this.testType.Text)
                    {
                        this.vm.Execute("DELETE FROM ApiTestResults WHERE ParamSet="+paramSet);
                    }
                    else if ("Http" == this.testType.Text)
                    {
                        this.vm.Execute("DELETE FROM HttpTestResults WHERE ParamSet="+paramSet);
                    }
                }
            }
            else
            {
                System.Windows.Forms.MessageBox.Show("No database connection.\n Close and open the database file",
                    "Fatal Error",
                    System.Windows.Forms.MessageBoxButtons.OK,
                    System.Windows.Forms.MessageBoxIcon.Error);
                deleted = false;
            }
            return deleted;
        }

        private bool DeleteTest (string testName, System.Windows.Forms.ListBox.ObjectCollection paramSets)
        {
            bool deleted = true;
            int i=0;
            if (null != this.database)
            {
                if ((!Form1.IsReadOnly(this.database.GetName())) || (this.Overwrite()))
                {
                    for(i=0; i<paramSets.Count; i++)
                    {
                        this.DeleteParamSetData(paramSets[i].ToString());
                    }
                    this.vm.Execute("DELETE FROM TestCase WHERE TestName=\'"+testName+"\'");
                }
            }
            else
            {
                System.Windows.Forms.MessageBox.Show("No database connection.\n Close and open the database file",
                    "Fatal Error",
                    System.Windows.Forms.MessageBoxButtons.OK,
                    System.Windows.Forms.MessageBoxIcon.Error);
                deleted = false;
            }
            return deleted;
        }

        //Create a FileInfo object and overwrite the ReadOnly attribute
        //Create a new SqliteDb object as the old will not be aware of the change
        //Send the new object to the parent form
        private void MakeWriteable(string dbName)
        {
            FileInfo dbFile = new FileInfo(dbName);
            dbFile.Attributes = FileAttributes.Archive;

            if (null != this.vm)
            {
                this.vm.SqlFinalize();
            }
            this.database.Close();
            this.database = new SqliteDb();
            this.database.Open(dbName);

            Form1 form = this.ParentForm as Form1;
            if (form != null)
            {
                form.SetDatabase(database);
            }

            this.vm = new SqliteVm(this.database, true);
        }

        private bool Overwrite()
        {
            bool overwrite = false;
            string dbName = this.database.GetName();

            string errMsg = String.Format("File {0} is read only! \nOverwrite it?",
                dbName);
            DialogResult result = System.Windows.Forms.MessageBox.Show(errMsg,
                "Error",
                System.Windows.Forms.MessageBoxButtons.YesNo,
                System.Windows.Forms.MessageBoxIcon.Hand,
                System.Windows.Forms.MessageBoxDefaultButton.Button1);

            //Overwrite the database if the user wants
            if (DialogResult.Yes == result)
            {
                this.MakeWriteable(dbName);
                overwrite = true;
            }
            return overwrite;
        }
    }
}
