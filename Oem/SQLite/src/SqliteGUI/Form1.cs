using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;
using System.IO;

using SqliteDotNet;

namespace SqliteGUI
{
    /// <summary>
    /// Summary description for Form1.
    /// </summary>
    public class Form1 : System.Windows.Forms.Form
    {
        private SqliteGUI.NewAddTestForm addTestForm;
        private System.Windows.Forms.MainMenu mainMenu1;
        private System.Windows.Forms.MenuItem menuItem1;
        private System.Windows.Forms.MenuItem menuItem2;
        private System.Windows.Forms.MenuItem menuItem3;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;

        private SqliteDb database;
        private string databaseName;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.Splitter splitter1;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.Splitter splitter2;
        private System.Windows.Forms.MenuItem menuItem4;
        private System.Windows.Forms.MenuItem menuItem5;
        private System.Windows.Forms.MenuItem menuAddTest;
        private System.Windows.Forms.MenuItem newDbItem;
        private System.Windows.Forms.MenuItem newBlankDatabaseMenuItem;
        private System.Windows.Forms.MenuItem newTemplateDbMenuItem;
        private System.Windows.Forms.MenuItem menuItem10;
        private System.Windows.Forms.MenuItem saveDbMenuItem;
        private System.Windows.Forms.MenuItem saveAsMenuItem;
        private System.Windows.Forms.MenuItem menuItem13;
        private System.Windows.Forms.MenuItem menuItem14;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public Form1()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            //
            // TODO: Add any constructor code after InitializeComponent call
            //
            this.addTestForm = new SqliteGUI.NewAddTestForm();
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            //Dispose the addTestForm. This will save the current data on the form
            if (!this.addTestForm.IsDisposed)
            {
                this.addTestForm.Dispose();
            }
            if( disposing )
            {
                if (components != null)
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
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.newDbItem = new System.Windows.Forms.MenuItem();
            this.newBlankDatabaseMenuItem = new System.Windows.Forms.MenuItem();
            this.newTemplateDbMenuItem = new System.Windows.Forms.MenuItem();
            this.menuItem2 = new System.Windows.Forms.MenuItem();
            this.menuItem10 = new System.Windows.Forms.MenuItem();
            this.saveDbMenuItem = new System.Windows.Forms.MenuItem();
            this.saveAsMenuItem = new System.Windows.Forms.MenuItem();
            this.menuItem13 = new System.Windows.Forms.MenuItem();
            this.menuAddTest = new System.Windows.Forms.MenuItem();
            this.menuItem14 = new System.Windows.Forms.MenuItem();
            this.menuItem3 = new System.Windows.Forms.MenuItem();
            this.menuItem4 = new System.Windows.Forms.MenuItem();
            this.menuItem5 = new System.Windows.Forms.MenuItem();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.splitter2 = new System.Windows.Forms.Splitter();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.SuspendLayout();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                      this.menuItem1,
                                                                                      this.menuItem4});
            // 
            // menuItem1
            // 
            this.menuItem1.Index = 0;
            this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                      this.newDbItem,
                                                                                      this.menuItem2,
                                                                                      this.menuItem10,
                                                                                      this.saveDbMenuItem,
                                                                                      this.saveAsMenuItem,
                                                                                      this.menuItem13,
                                                                                      this.menuAddTest,
                                                                                      this.menuItem14,
                                                                                      this.menuItem3});
            this.menuItem1.Text = "Database";
            // 
            // newDbItem
            // 
            this.newDbItem.Index = 0;
            this.newDbItem.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                      this.newBlankDatabaseMenuItem,
                                                                                      this.newTemplateDbMenuItem});
            this.newDbItem.Text = "New";
            // 
            // newBlankDatabaseMenuItem
            // 
            this.newBlankDatabaseMenuItem.Index = 0;
            this.newBlankDatabaseMenuItem.Text = "Blank Database";
            this.newBlankDatabaseMenuItem.Click += new System.EventHandler(this.newBlankDatabaseMenuItem_Click);
            // 
            // newTemplateDbMenuItem
            // 
            this.newTemplateDbMenuItem.Index = 1;
            this.newTemplateDbMenuItem.Text = "Unit Test Template Db";
            this.newTemplateDbMenuItem.Click += new System.EventHandler(this.newTemplateDbMenuItem_Click);
            // 
            // menuItem2
            // 
            this.menuItem2.Index = 1;
            this.menuItem2.Shortcut = System.Windows.Forms.Shortcut.CtrlO;
            this.menuItem2.Text = "Open";
            this.menuItem2.Click += new System.EventHandler(this.DatabaseOpen_Click);
            // 
            // menuItem10
            // 
            this.menuItem10.Index = 2;
            this.menuItem10.Text = "-";
            // 
            // saveDbMenuItem
            // 
            this.saveDbMenuItem.Enabled = false;
            this.saveDbMenuItem.Index = 3;
            this.saveDbMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlS;
            this.saveDbMenuItem.Text = "Save";
            this.saveDbMenuItem.Click += new System.EventHandler(this.saveDbMenuItem_Click);
            // 
            // saveAsMenuItem
            // 
            this.saveAsMenuItem.Enabled = false;
            this.saveAsMenuItem.Index = 4;
            this.saveAsMenuItem.Text = "Save as";
            this.saveAsMenuItem.Click += new System.EventHandler(this.saveAsMenuItem_Click);
            // 
            // menuItem13
            // 
            this.menuItem13.Index = 5;
            this.menuItem13.Text = "-";
            // 
            // menuAddTest
            // 
            this.menuAddTest.Enabled = false;
            this.menuAddTest.Index = 6;
            this.menuAddTest.Text = "Add/Edit Tests";
            this.menuAddTest.Click += new System.EventHandler(this.menuAddTest_Click);
            // 
            // menuItem14
            // 
            this.menuItem14.Index = 7;
            this.menuItem14.Text = "-";
            // 
            // menuItem3
            // 
            this.menuItem3.Enabled = false;
            this.menuItem3.Index = 8;
            this.menuItem3.Text = "Close";
            this.menuItem3.Click += new System.EventHandler(this.DatabaseClose_Click);
            // 
            // menuItem4
            // 
            this.menuItem4.Index = 1;
            this.menuItem4.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                      this.menuItem5});
            this.menuItem4.Text = "Sql";
            // 
            // menuItem5
            // 
            this.menuItem5.Index = 0;
            this.menuItem5.Text = "Execute";
            this.menuItem5.Click += new System.EventHandler(this.SqlExecute_Click);
            // 
            // richTextBox1
            // 
            this.richTextBox1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.richTextBox1.Location = new System.Drawing.Point(0, 537);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(864, 88);
            this.richTextBox1.TabIndex = 7;
            this.richTextBox1.Text = "Sqlite statement...";
            // 
            // comboBox1
            // 
            this.comboBox1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.comboBox1.Items.AddRange(new object[] {
                                                           "CREATE TABLE name (col1 INTEGER, col2 REAL, col3 TEXT, col4 BLOB);",
                                                           "CREATE TABLE name AS SELECT ...;",
                                                           "VACUUM;",
                                                           "DROP TABLE name;"});
            this.comboBox1.Location = new System.Drawing.Point(0, 516);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(864, 21);
            this.comboBox1.TabIndex = 8;
            this.comboBox1.Text = "Sqlite constructs";
            this.comboBox1.SelectedIndexChanged += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
            // 
            // splitter1
            // 
            this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.splitter1.Location = new System.Drawing.Point(0, 513);
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(864, 3);
            this.splitter1.TabIndex = 9;
            this.splitter1.TabStop = false;
            // 
            // treeView1
            // 
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Left;
            this.treeView1.ImageIndex = -1;
            this.treeView1.Location = new System.Drawing.Point(0, 0);
            this.treeView1.Name = "treeView1";
            this.treeView1.SelectedImageIndex = -1;
            this.treeView1.Size = new System.Drawing.Size(176, 513);
            this.treeView1.TabIndex = 10;
            this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            // 
            // splitter2
            // 
            this.splitter2.Location = new System.Drawing.Point(176, 0);
            this.splitter2.Name = "splitter2";
            this.splitter2.Size = new System.Drawing.Size(3, 513);
            this.splitter2.TabIndex = 11;
            this.splitter2.TabStop = false;
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(864, 625);
            this.Controls.Add(this.splitter2);
            this.Controls.Add(this.treeView1);
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.comboBox1);
            this.Controls.Add(this.richTextBox1);
            this.IsMdiContainer = true;
            this.Menu = this.mainMenu1;
            this.Name = "Form1";
            this.Text = "Sqlite GUI";
            this.ResumeLayout(false);

        }
        #endregion

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.Run(new Form1());
        }

        private void OpenDatabase(string fName)
        {
            if (database != null)
            {
                CloseDatabase();
            }

            if (fName.Length > 0)
            {
                database = new SqliteDb();
                int err = database.Open(fName);
                if (err != 0)
                {
                    database = null;
                }
            }

            if (database != null)
            {
                databaseName = fName;

                treeView1.BeginUpdate();

                // Clear the TreeView each time the method is called.
                treeView1.Nodes.Clear();

                TreeNode databaseNode = new TreeNode(fName);

                SqliteVm vm = new SqliteVm(database, true);
                string tableList = "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;";

                int fini = vm.Execute(tableList);
                while (fini == Sqlite.Row)
                {
                    string tableName = vm.GetString("name");

                    databaseNode.Nodes.Add(tableName);

                    fini = vm.NextRow();
                }
                vm.SqlFinalize();
                vm = null;

                treeView1.Nodes.Add(databaseNode);

                treeView1.EndUpdate();
                //Enable menu items
                this.menuItem3.Enabled = true;
                this.menuAddTest.Enabled = true;
                this.saveAsMenuItem.Enabled = true;
                this.saveDbMenuItem.Enabled = true;
            }
        }

        private void CloseDatabase()
        {
            if (database != null)
            {
                //If the file is not read only save the data first else close all forms
                if (!IsReadOnly(database.GetName()))
                {
                    this.SaveDatabase();
                }
                else
                {
                    this.CloseForms();
                }

                database.Close();
                database = null;
                treeView1.BeginUpdate();
                treeView1.Nodes.Clear();
                treeView1.EndUpdate();
            }
            //Disable menu items
            this.menuItem3.Enabled = false;
            this.menuAddTest.Enabled = false;
            this.saveAsMenuItem.Enabled = false;
            this.saveDbMenuItem.Enabled = false;
        }

        private void DatabaseOpen_Click(object sender, System.EventArgs e)
        {

            openFileDialog1.Filter = "db files (*.db)|*.db|All files (*.*)|*.*";
            openFileDialog1.RestoreDirectory = true;

            string fName = "";

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                fName = openFileDialog1.FileName;
            }
            OpenDatabase(fName);
        }

        private void DatabaseClose_Click(object sender, System.EventArgs e)
        {
            CloseDatabase();
        }

        private void treeView1_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
        {
            if (e.Node.Parent != null)
            {
                TableEditForm tableForm = new TableEditForm();
                tableForm.Text = tableForm.Name = e.Node.Text;
                tableForm.LoadTable(database, tableForm.Text);
                tableForm.MdiParent = this;
                tableForm.Show();
            }
        }

        private void SqlExecute_Click(object sender, System.EventArgs e)
        {
            if (richTextBox1.Text.Length > 0 && database != null)
            {
                SqliteVm vm = new SqliteVm(database, true);
                string sql = richTextBox1.Text;
                int sqlErr = Sqlite.Error;
                try
                {
                    sqlErr = vm.Execute(sql);
                }
                catch (SEHException)
                {
                    // Do nothing.  Execution failed...
                }

                if (sqlErr != Sqlite.Done && sqlErr != Sqlite.Ok)
                {
                    string errMsg = vm.GetErrMsg();
                    errMsg += "\n";
                    errMsg += vm.GetQueryTail();
                    MessageBox.Show(errMsg, "Sqlite Execution Error");

                    vm.SqlFinalize();
                    vm = null;
                }
                else
                {
                    vm.SqlFinalize();
                    vm = null;

                    // reload database
                    OpenDatabase(databaseName);
                }
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            ComboBox comboBox = (ComboBox) sender;
            richTextBox1.Text = comboBox.SelectedItem.ToString();
        }

        public SqliteDb GetDb()
        {
            return database;
        }

        //Opens add test form
        private void menuAddTest_Click(object sender, System.EventArgs e)
        {
            if (null != this.database)
            {
                string sqlStmt = "SELECT name FROM sqlite_master WHERE type='table' AND name='TestCase';";

                SqliteVm vm = new SqliteVm(this.database, true);

                if (Sqlite.Row == vm.Execute(sqlStmt))
                {
                    //Hack...if the form is not visible then the user must have closed it->create a new one
                    //If it is visible bring it to focus
                    if (!this.addTestForm.Visible)
                    {
                        this.addTestForm = new SqliteGUI.NewAddTestForm();
                        this.addTestForm.LoadForm(database);
                        this.addTestForm.MdiParent = this;
                        this.addTestForm.Show();
                    }
                    else
                    {
                        this.addTestForm.Activate();
                    }
                }
                else
                {
                    System.Windows.Forms.MessageBox.Show("TestCase table does not exist!",
                        "Error",
                        System.Windows.Forms.MessageBoxButtons.OK,
                        System.Windows.Forms.MessageBoxIcon.Hand);
                }
                vm.SqlFinalize();
            }

        }

        private void newBlankDatabaseMenuItem_Click(object sender, System.EventArgs e)
        {
            this.NewDatabase(false);
        }

        private void newTemplateDbMenuItem_Click(object sender, System.EventArgs e)
        {
            this.NewDatabase(true);
        }

        private void saveDbMenuItem_Click(object sender, System.EventArgs e)
        {
            this.SaveDatabase();
        }

        private void saveAsMenuItem_Click(object sender, System.EventArgs e)
        {
            this.SaveAs();
        }

        private void NewDatabase(bool template)
        {
            saveFileDialog1.Filter = "db files (*.db)|*.db|All files (*.*)|*.*";
            saveFileDialog1.RestoreDirectory = true;
            SqliteDb db = null;

            string dbName = "";

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                dbName = saveFileDialog1.FileName;
                if (!IsReadOnly(dbName))
                {

                    //Close the existing database
                    if (null != this.database)
                    {
                        this.CloseDatabase();
                    }

                    //Delete the file
                    this.DeleteFile(dbName);

                    //Open the database. SQLite will create the file for us
                    db = new SqliteDb();
                    db.Open(dbName);

                    //Create a template database for the unit test infrastructure
                    if (template && (db != null))
                    {
                        SqliteVm templateVm = new SqliteVm(db, true);
                        templateVm.Execute("CREATE TABLE TestCase (ExecuteSequence INTEGER, TestName TEXT, ParamSets TEXT, Description TEXT, TestType TEXT, Prerequisite TEXT)");
                        templateVm.Execute("CREATE TABLE Params (ParamSet INTEGER, ParamName TEXT, ParamValue TEXT)");
                        templateVm.Execute("CREATE TABLE CommonParams (ParamName text, ParamValue text)");
                        templateVm.Execute("CREATE TABLE ApiTestResults (Description TEXT, ParamSet INTEGER, ContentType TEXT, Result TEXT)");
                        templateVm.Execute("CREATE TABLE HttpTestResults (Description TEXT, ParamSet INTEGER, ContentType TEXT, Result BLOB)");
                        templateVm.SqlFinalize();
                        templateVm = null;
                    }
                    db.Close();

                    //Open the new database
                    this.OpenDatabase(dbName);
                }
                else
                {
                    string errMsg;
                    errMsg = String.Format("File {0} is readOnly and cannot be replaced", dbName);
                    System.Windows.Forms.MessageBox.Show(errMsg,
                        "Error",
                        System.Windows.Forms.MessageBoxButtons.OK,
                        System.Windows.Forms.MessageBoxIcon.Hand);
                    this.NewDatabase(template);
                }
            }
        }

        private void SaveDatabase()
        {
            if (!this.addTestForm.IsDisposed)
            {
                this.addTestForm.Dispose();
            }

            if (database != null)
            {
                string dbName = database.GetName();
                if (!IsReadOnly(dbName))
                {
                    this.CloseForms();
                }
                else
                {
                    //If the file is read only show a warning message and if the user wants, make the file writeable
                    string errMsg = String.Format("File {0} is read only! \nOverwrite it?",
                                                  dbName);
                    DialogResult result = System.Windows.Forms.MessageBox.Show(errMsg,
                        "Error",
                        System.Windows.Forms.MessageBoxButtons.YesNo,
                        System.Windows.Forms.MessageBoxIcon.Hand,
                        System.Windows.Forms.MessageBoxDefaultButton.Button1);

                    if (DialogResult.Yes == result)
                    {
                        FileInfo dbFile = new FileInfo(dbName);
                        dbFile.Attributes = FileAttributes.Archive;

                        //Hack...if the database is not read only we can write to it.
                        //Nevertheles, if the ReadOnly property has changed after the database handler was created
                        //then, we need to re-create it or SQLite will throw exception
                        database.Close();
                        database = new SqliteDb();
                        database.Open(dbName);
                        this.CloseForms();
                    }
                }
            }
        }

        //Checks if a file is read only
        public static bool IsReadOnly(string fileName)
        {
            bool readOnly = false;
            int i=0;
            FileInfo file = new FileInfo(fileName);
            //Get all attributes and put them into array
            string [] attributes = file.Attributes.ToString().Split(',');

            //Parse the array
            while ((i<attributes.Length) && ("ReadOnly" != attributes[i]))
            {
                i++;
            }

            if (i<attributes.Length)
            {
                readOnly = true;
            }
            return readOnly;
        }

        //Deletes a file
        private void DeleteFile(string fileName)
        {
            FileInfo file = new FileInfo(fileName);
            if ((file.Exists) && (!IsReadOnly(fileName)))
            {
                file.Delete();
            }
        }

        //Closes all open forms
        //Closing the forms will also save them.
        //If the file is read only the changes will be lost and no warning will be displayed
        private void CloseForms()
        {
            if (!this.addTestForm.IsDisposed)
            {
                this.addTestForm.Dispose();
            }
            while ((this.MdiChildren.Length > 0) && this.addTestForm.IsDisposed)
            {
                if ("SqliteGUI.TableEditForm" == this.MdiChildren[0].GetType().ToString() )
                {
                    TableEditForm child = this.MdiChildren[0] as TableEditForm;
                    child.SetDatabase(database);
                    child.Close();
                    child = null;
                }
            }
        }

        //Implements SaveAs functionality
        private void SaveAs()
        {
            saveFileDialog1.Filter = "db files (*.db)|*.db|All files (*.*)|*.*";
            saveFileDialog1.RestoreDirectory = true;

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string newName = saveFileDialog1.FileName;
                //Should never be hit
                if (null == database)
                {
                    System.Windows.Forms.MessageBox.Show("No database is open",
                        "Error",
                        System.Windows.Forms.MessageBoxButtons.OK,
                        System.Windows.Forms.MessageBoxIcon.Hand,
                        System.Windows.Forms.MessageBoxDefaultButton.Button1);
                }

                else if (IsReadOnly(newName))
                {
                    string errMsg = String.Format("File {0} is read only! \nSave the file under different name?",
                        newName);
                    DialogResult result = System.Windows.Forms.MessageBox.Show(errMsg,
                        "Error",
                        System.Windows.Forms.MessageBoxButtons.YesNo,
                        System.Windows.Forms.MessageBoxIcon.Hand,
                        System.Windows.Forms.MessageBoxDefaultButton.Button1);

                    if (DialogResult.Yes == result)
                    {
                        this.SaveAs();
                    }
                }
                else
                {
                    //If the source file is read only the CopyTo method will also create a ReadOnly file
                    //Take the new file and remove the ReadOnly attribute in order to be able to save any new changes
                    System.IO.FileAttributes attribute = new System.IO.FileAttributes();
                    FileInfo oldDbFile = new FileInfo(database.GetName());
                    oldDbFile.CopyTo(newName, true);
                    if (IsReadOnly(database.GetName()))
                    {
                        FileInfo newDbFile = new FileInfo(newName);
                        newDbFile.Attributes = FileAttributes.Archive;
                    }

                    //Open the new file
                    database.Close();
                    database = new SqliteDb();
                    database.Open(newName);

                    this.SaveDatabase();
                    this.OpenDatabase(newName);
                }
            }
        }
        public void SetDatabase(SqliteDb newDatabase)
        {
            this.database = newDatabase;
        }
    }
}
