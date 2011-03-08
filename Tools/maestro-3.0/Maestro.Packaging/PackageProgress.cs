#region Disclaimer / License
// Copyright (C) 2009, Kenneth Skovhede
// http://www.hexad.dk, opensource@hexad.dk
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
// 
#endregion
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.MaestroAPI.Services;
using OSGeo.MapGuide.MaestroAPI;

namespace Maestro.Packaging
{
    /// <summary>
    /// Progress dialog for packaging operations
    /// </summary>
    public partial class PackageProgress : Form
    {
        /// <summary>
        /// The stages required to build a package
        /// </summary>
        private readonly static ProgressType[] BUILD_STAGES = null;

        static PackageProgress()
        {
            BUILD_STAGES = 
            new ProgressType[] {
                ProgressType.ReadingFileList,
                ProgressType.PreparingFolder,
                ProgressType.MovingResources,
                ProgressType.Compressing
            };
        }

        private bool m_allowClose = true;
        private volatile System.Threading.Thread m_thread;

        private PackageBuilder m_invokeObj = null;
        private object[] m_invokeArgs = null;
        private System.Reflection.MethodInfo m_invokeMethod = null;
        private object m_invokeResult = null;

        /// <summary>
        /// Lets the user choose a file to upload, and then transfers it
        /// </summary>
        /// <param name="owner">The owner form</param>
        /// <param name="con">The connection to use</param>
        /// <returns>A DialogResult object that indicates the upload result</returns>
        public static DialogResult UploadPackage(Form owner, IServerConnection con)
        {
            OpenFileDialog dlg = Maestro.Shared.UI.DialogFactory.OpenFile();
            //Mono does NOT like this one
            //dlg.AutoUpgradeEnabled = true;
            dlg.CheckFileExists = true;
            dlg.CheckPathExists = true;
            dlg.DefaultExt = ".mgp";
            dlg.Filter = Properties.Resources.UploadPackageDialogFilter;
            dlg.FilterIndex = 0;
            dlg.Multiselect = false;
            dlg.ValidateNames = true;
            dlg.Title = Properties.Resources.UploadPackageDialogTitle;

            if (dlg.ShowDialog(owner) == DialogResult.OK)
            {
                return UploadPackage(owner, con, dlg.FileName);
            }
            else
                return DialogResult.Cancel;
        }

        /// <summary>
        /// Lists the contents of a package
        /// </summary>
        /// <param name="owner">The owner form</param>
        /// <param name="connection">The connection used to deserialize the items</param>
        /// <param name="packageFile">The package file to read</param>
        /// <returns>A list of resources in the package</returns>
        public static Dictionary<string, ResourceItem> ListPackageContents(Form owner, IServerConnection connection, string packageFile)
        {
            PackageProgress pkgp = new PackageProgress();
            pkgp.m_invokeArgs = new object[] { packageFile };
            pkgp.m_invokeObj = new PackageBuilder(connection);
            pkgp.m_invokeMethod = pkgp.m_invokeObj.GetType().GetMethod("ListPackageContents");

            if (pkgp.ShowDialog(owner) == DialogResult.OK)
                return (Dictionary<string, ResourceItem>)pkgp.m_invokeResult;
            else
                return null;
        }

        /// <summary>
        /// Uploads a package file to the server
        /// </summary>
        /// <param name="owner">The owner form</param>
        /// <param name="connection">The connection used to upload the package</param>
        /// <param name="packageFile">The package file to upload</param>
        /// <returns>A DialogResult object that indicates the result of the operation</returns>
        public static DialogResult UploadPackage(Form owner, IServerConnection connection, string packageFile)
        {
            PackageProgress pkgp = new PackageProgress();
            pkgp.Text = Properties.Resources.TitleUploading;
            pkgp.m_invokeArgs = new object[] { packageFile };
            pkgp.m_invokeObj = new PackageBuilder(connection);
            pkgp.m_invokeMethod = pkgp.m_invokeObj.GetType().GetMethod("UploadPackage");

            return pkgp.ShowDialog(owner);
        }

        /// <summary>
        /// Rebuilds a package
        /// </summary>
        /// <param name="owner">The owner form</param>
        /// <param name="connection">The connection used to serialize the items</param>
        /// <param name="sourcePackageFile">The package file that the new file is based on</param>
        /// <param name="items">The items that describes the layout of the new package</param>
        /// <param name="targetfile">The output package file</param>
        /// <param name="insertEraseCommands">True if each resource should have a delete operation inserted before the actual operation, false otherwise</param>
        /// <returns>A DialogResult object that indicates the result of the operation</returns>
        public static DialogResult RebuildPackage(Form owner, IServerConnection connection, string sourcePackageFile, List<ResourceItem> items, string targetfile, bool insertEraseCommands)
        {
            PackageProgress pkgp = new PackageProgress();
            pkgp.m_invokeArgs = new object[] { sourcePackageFile, items, targetfile, insertEraseCommands };
            pkgp.m_invokeObj = new PackageBuilder(connection);
            pkgp.m_invokeMethod = pkgp.m_invokeObj.GetType().GetMethod("RebuildPackage");

            return pkgp.ShowDialog(owner);
        }

        /// <summary>
        /// Creates a new package from a server folder
        /// </summary>
        /// <param name="owner">The owner form</param>
        /// <param name="connection">The connection used to retrieve the resources</param>
        /// <param name="folderResourceId">The folder to create the package from</param>
        /// <param name="zipfilename">The name of the output package file</param>
        /// <param name="allowedExtensions">A list of allowed extensions, set to null for all types. The special value &quot;*&quot; matches all unknown extensions.</param>
        /// <param name="removeExistingFiles">A flag indicating if the package should contain a delete instruction to delete the target area before restore</param>
        /// <param name="alternateTargetResourceId">The folder path where the package should be restore, set to null or empty string to use the source path</param>
        /// <returns></returns>
        public static DialogResult CreatePackage(Form owner, IServerConnection connection, string folderResourceId, string zipfilename, IEnumerable<ResourceTypes> allowedExtensions, bool removeExistingFiles, string alternateTargetResourceId)
        {
            PackageProgress pkgp = new PackageProgress();
            pkgp.m_invokeArgs = new object[] {folderResourceId, zipfilename, allowedExtensions, removeExistingFiles, alternateTargetResourceId };
            pkgp.m_invokeObj = new PackageBuilder(connection);
            pkgp.m_invokeMethod = pkgp.m_invokeObj.GetType().GetMethod("CreatePackage");

            return pkgp.ShowDialog(owner);
        }

        private PackageProgress()
        {
            InitializeComponent();
        }

        private void PackageProgress_Load(object sender, EventArgs e)
        {
            m_allowClose = false;
            backgroundWorker.RunWorkerAsync();
            m_invokeObj.Progress += new ProgressDelegate(SetCurrentProgress);
        }

        private void PackageProgress_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (m_thread != null && !m_allowClose && e.CloseReason == CloseReason.UserClosing)
            {
                if (MessageBox.Show(this, Properties.Resources.CancelConfirmation, Application.ProductName, MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question, MessageBoxDefaultButton.Button3) == DialogResult.Yes)
                {
                    try { m_thread.Abort(); }
                    catch { }
                }
                else
                    e.Cancel = true;
            }
        }

        private delegate void SetCurrentProgressDelegate(ProgressType type, string resource, int total, int pg);
        private void SetCurrentProgress(ProgressType type, string resource, int total, int pg)
        {
            if (this.InvokeRequired)
                this.Invoke(new SetCurrentProgressDelegate(SetCurrentProgress), new object[] { type, resource, total, pg });
            else
            {
                int span = this.CurrentProgress.Maximum - this.CurrentProgress.Minimum;
                double v = (((double)pg / total) * span) + this.CurrentProgress.Minimum;
                this.CurrentProgress.Value = (int)Math.Max(Math.Min(v, this.CurrentProgress.Maximum), this.CurrentProgress.Minimum);
                OperationLabel.Text = resource;

                int largePg = (int)Array.IndexOf(BUILD_STAGES, type);
                if (largePg >= 0)
                {
                    this.TotalProgress.Maximum = 100;
                    this.TotalProgress.Minimum = 0;

                    double tick = 100 / BUILD_STAGES.Length;
                    double lv = (tick * largePg) + (tick * ((double)pg / total));

                    this.TotalProgress.Value = (int)Math.Max(Math.Min((int)lv, this.TotalProgress.Maximum), this.TotalProgress.Minimum);
                }
                
                if (type == ProgressType.Uploading)
                {
                    HideTotal();
                    if (pg == total)
                    {
                        OperationLabel.Text = Properties.Resources.ProgressWaitingForServer;
                        CurrentProgress.Style = ProgressBarStyle.Marquee;
                    }
                    else
                    {
                        OperationLabel.Text = string.Format(Properties.Resources.ProgressUploading, ((double)pg / 1024), ((double)total / 1024));
                        this.Refresh();
                    }
                }
                else if (type == ProgressType.ListingFiles)
                {
                    HideTotal();
                    OperationLabel.Text = Properties.Resources.ProgressReadingPackage;
                    CurrentProgress.Style = ProgressBarStyle.Marquee;
                }
                else if (type == ProgressType.Compressing)
                {
                    OperationLabel.Text = Properties.Resources.ProgressCompressing;
                }
                else if (type == ProgressType.MovingResources)
                {
                    OperationLabel.Text = Properties.Resources.ProgressUpdatingReferences;
                }
                else if (type == ProgressType.PreparingFolder)
                {
                    OperationLabel.Text = resource;
                }

            }
        }

        private void HideTotal()
        {
            if (TotalProgress.Visible)
            {
                this.Height -= TotalProgress.Height;
                TotalProgress.Visible = false;
                TotalLabel.Visible = false;
            }
        }

        private void backgroundWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                m_thread = System.Threading.Thread.CurrentThread;
                e.Result = m_invokeMethod.Invoke(m_invokeObj, m_invokeArgs);
            }
            catch (System.Threading.ThreadAbortException)
            {
                System.Threading.Thread.ResetAbort();
                e.Cancel = true;
            }
            catch (System.Reflection.TargetInvocationException tai)
            {
                throw tai.InnerException;
            }
            finally
            {
                m_thread = null;
            }
        }

        private void backgroundWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            m_allowClose = true;
            if (e.Cancelled)
            {
                this.DialogResult = DialogResult.Cancel;
                this.Close();
                return;
            }
            else if (e.Error != null)
                throw e.Error;

            m_invokeResult = e.Result;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void CancelBtn_Click(object sender, EventArgs e)
        {
            FormClosingEventArgs ev = new FormClosingEventArgs(CloseReason.UserClosing, false);
            PackageProgress_FormClosing(sender, ev);
            if (!ev.Cancel)
            {
                try { m_thread.Abort(); }
                catch { }
            }
        }
    }
}