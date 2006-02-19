using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using SqliteDotNet;
using System.Data;
using System.Data.SqlTypes;

namespace SqliteGUI
{
    /// <summary>
    /// Summary description for TableEditForm.
    /// </summary>
    public class TableEditForm : System.Windows.Forms.Form
    {
        private System.Windows.Forms.DataGrid dataGrid1;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        private SqliteDb database;
        private SqliteVm vm = null;
        private DataTable dataTable = null;

        public TableEditForm()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            //
            // TODO: Add any constructor code after InitializeComponent call
            //
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if ((null != database) && (!Form1.IsReadOnly(database.GetName())))
                {
                    WriteBack();
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
            this.dataGrid1 = new System.Windows.Forms.DataGrid();
            ((System.ComponentModel.ISupportInitialize)(this.dataGrid1)).BeginInit();
            this.SuspendLayout();
            // 
            // dataGrid1
            // 
            this.dataGrid1.DataMember = "";
            this.dataGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGrid1.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.dataGrid1.Location = new System.Drawing.Point(0, 0);
            this.dataGrid1.Name = "dataGrid1";
            this.dataGrid1.Size = new System.Drawing.Size(492, 473);
            this.dataGrid1.TabIndex = 0;
            // 
            // TableEditForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(492, 473);
            this.Controls.Add(this.dataGrid1);
            this.Name = "TableEditForm";
            this.Text = "TableEditForm";
            ((System.ComponentModel.ISupportInitialize)(this.dataGrid1)).EndInit();
            this.ResumeLayout(false);

        }
        #endregion

        public void LoadTable(SqliteDb newDatabase, string tableName)
        {
            this.Name = tableName;
            this.SetDatabase(newDatabase);

            DataGridTableStyle tableStyle = new DataGridTableStyle();
            tableStyle.MappingName = tableName;
            if (tableName == "TestCase")
            {
                tableStyle.PreferredRowHeight *= 3;
            }
            else if (tableName == "ApiTestResults" || tableName == "HttpTestResults")
            {
                tableStyle.PreferredRowHeight *= 2;
            }

            int gridWidth = 0;

            vm = new SqliteVm(database, true);
            if (vm != null)
            {
                dataTable = new DataTable(tableName);

                string sql = String.Format("select rowid, * from {0} ;", tableName);
                int sqErr = vm.Execute(sql);
                int nCols = vm.NumCols();

                for (int i = 0; i<nCols; i++)
                {
                    string colName = vm.ColumnName(i);
                    string colType = vm.ColumnType(colName);
                    DataGridTextBoxColumn colStyle = new DataGridTextBoxColumn();
                    colStyle.MappingName = colName;
                    colStyle.HeaderText = colName;

                    switch (colType.ToUpper())
                    {
                        case "INTEGER":
                            dataTable.Columns.Add(colName, typeof(int));
                            colStyle.Width = 60;
                            break;
                        case "REAL":
                            dataTable.Columns.Add(colName, typeof(double));
                            colStyle.Width = 60;
                            break;
                        case "TEXT":
                            dataTable.Columns.Add(colName, typeof(string));
                            colStyle.TextBox.Multiline = true;
                            if (colName == "ParamName" || colName == "ContentType" || colName == "TestType" || colName == "Prerequisite")
                            {
                                colStyle.Width = 100;
                            }
                            else if (colName == "ParamValue")
                            {
                                colStyle.Width = 275;
                            }
                            else
                            {
                                colStyle.Width = 200;
                            }
                            break;

                        case "BLOB":
                            DataColumn col = new DataColumn(colName, typeof(SqlBinary));
                            col.ReadOnly = true;
                            dataTable.Columns.Add(col);
                            colStyle.Width = 150;
                            break;
                        default:
                            colStyle = null;
                            break;
                    }

                    if (colStyle != null)
                    {
                        tableStyle.GridColumnStyles.Add(colStyle);
                        gridWidth += colStyle.Width;
                    }
                }


                //rowid should be readonly
                dataTable.Columns[0].ReadOnly = true;

                // update grid with new cols and width
                dataGrid1.TableStyles.Add(tableStyle);
                dataGrid1.Width = gridWidth;

                while (sqErr == Sqlite.Row)
                {
                    object [] cols = new object[nCols];

                    for (int i=0; i<nCols; i++)
                    {
                        string colName = vm.ColumnName(i);
                        string colType = vm.ColumnType(colName);
                        switch (colType.ToUpper())
                        {
                            case "INTEGER":
                                int colInt = vm.GetInt(colName);
                                cols[i] = colInt;
                                break;
                            case "REAL":
                                double colDbl = vm.GetDouble(colName);
                                cols[i] = colDbl;
                                break;
                            case "TEXT":
                                string colText = vm.GetString(colName);
                                cols[i] = colText;
                                break;
                            case "BLOB":
                                byte[] bytes = vm.GetBlob(colName).Read();
                                if (bytes != null)
                                {
                                    SqlBinary binBlob = new SqlBinary(bytes);
                                    cols[i] = binBlob;
                                }
                                else
                                {
                                    cols[i] = null;
                                }
                                break;
                            default:
                                break;
                        }
                    }

                    dataTable.Rows.Add(cols);
                    sqErr = vm.NextRow();
                }
            }

            vm.SqlFinalize();
            vm = null;
            dataTable.AcceptChanges();

            dataGrid1.DataSource = dataTable;
            this.Width = gridWidth+75;
        }


        public void WriteBack()
        {
            this.vm = new SqliteVm(this.database, true);
            for (int i=0; i<dataTable.Rows.Count; i++)
            {
                int sqErr = Sqlite.Ok;

                DataRow row = dataTable.Rows[i];

                switch (row.RowState)
                {
                    case DataRowState.Added:
                    {
                        string insertStmt = String.Format("INSERT INTO {0} (",dataTable.TableName);
                        for (int j=1; j<dataTable.Columns.Count; j++)
                        {
                            // ignore binary for now...
                            if (dataTable.Columns[j].DataType == typeof(SqlBinary))
                            {
                                continue;
                            }

                            if (j > 1) { insertStmt += ",";}

                            insertStmt += dataTable.Columns[j].ColumnName;
                        }

                        insertStmt += ") VALUES (";

                        for (int j=1; j<dataTable.Columns.Count; j++)
                        {
                            // ignore binary for now...
                            if (dataTable.Columns[j].DataType == typeof(SqlBinary))
                            {
                                continue;
                            }

                            if (j > 1) { insertStmt += ",";}

                            insertStmt += String.Format("\"{0}\"", row[j].ToString());
                        }

                        insertStmt += ");";
                        sqErr = vm.Execute(insertStmt);
                    }
                    break;

                    case DataRowState.Deleted:
                    {
                        // hack... rejectchanges to get the row back...
                        row.RejectChanges();
                        object obj = row[0];
                        if (obj != null)
                        {
                            string rowId = obj.ToString();
                            if (rowId != null && rowId.Length > 0)
                            {
                                string deleteStmt =
                                    String.Format("DELETE FROM {0} WHERE rowid = {1};",
                                    dataTable.TableName, rowId);
                                sqErr = vm.Execute(deleteStmt);
                            }
                        }
                        row.Delete();
                    }
                    break;

                    case DataRowState.Modified:
                    {
                        int nCols = dataTable.Columns.Count;

                        string updateStmt = String.Format("UPDATE {0} SET ", dataTable.TableName);

                        for (int j=1; j<nCols; j++)
                        {
                            // ignore binary for now...
                            if (dataTable.Columns[j].DataType == typeof(SqlBinary))
                            {
                                continue;
                            }

                            if (j > 1) { updateStmt += ",";}

                            updateStmt += String.Format("{0}=\"{1}\"",
                                dataTable.Columns[j].ColumnName,
                                row[j].ToString());
                        }

                        updateStmt += String.Format(" WHERE rowid = {0};", row[0].ToString());
                        sqErr = vm.Execute(updateStmt);
                    }
                    break;

                    case DataRowState.Unchanged:
                    {
                        // do nothing
                        ;
                        ;
                    }
                    break;

                    default:
                    break;
                }
            }

            vm.SqlFinalize();
            vm = null;
        }

        public void SetDatabase(SqliteDb newDatabase)
        {
            this.database = newDatabase;
        }
    }
}
