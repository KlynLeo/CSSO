using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MarketUI
{
    public partial class Form1 : Form
    {
        private const string ManagementExe = @"C:\Facultate\CSSO\tema4\Lab4\bin\management.exe";
        private const string LogsPath = @"C:\Facultate\CSSO\H4\Reports\logs.txt";
        private const string ErrorsPath = @"C:\Facultate\CSSO\H4\Reports\Summary\errors.txt";
        private const string SoldPath = @"C:\Facultate\CSSO\H4\Reports\Summary\sold.txt";
        private const string DonationsPath = @"C:\Facultate\CSSO\H4\Reports\Summary\donations.txt";


        private Process managementProcess;

        public Form1()
        {
            InitializeComponent();
            textLogs.ReadOnly = true;
            textErrors.ReadOnly = true;
            refreshTimer.Interval = 1000;
            refreshTimer.Tick += RefreshTimer_Tick;
        }

        private void btnBrowseDeposit_Click(object sender, EventArgs e)
        {
            using (var dlg = new FolderBrowserDialog())
            {
                if (dlg.ShowDialog() == DialogResult.OK)
                    textDeposit.Text = dlg.SelectedPath;
            }
        }

        private void btnBrowseSold_Click(object sender, EventArgs e)
        {
            using (var dlg = new FolderBrowserDialog())
            {
                if (dlg.ShowDialog() == DialogResult.OK)
                    textSold.Text = dlg.SelectedPath;
            }
        }

        private async void btnRun_Click(object sender, EventArgs e)
        {
            if (!File.Exists(ManagementExe))
            {
                MessageBox.Show("Nu găsesc management.exe.\nEditează calea în cod.", "Eroare", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (string.IsNullOrWhiteSpace(textDeposit.Text) || string.IsNullOrWhiteSpace(textSold.Text))
            {
                MessageBox.Show("Selectează ambele directoare (deposit și sold).", "Atenție", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            btnRun.Enabled = false;
            btnStop.Enabled = true;

            try
            {
                // reset loguri pe ecran
                textLogs.Clear();
                textErrors.Clear();
                labelSold.Text = "Sold total: 0";
                labelDonations.Text = "Donations total: 0";

                managementProcess = new Process();
                managementProcess.StartInfo.FileName = ManagementExe;
                managementProcess.StartInfo.WorkingDirectory = Path.GetDirectoryName(ManagementExe);
                managementProcess.StartInfo.UseShellExecute = true;
                managementProcess.StartInfo.CreateNoWindow = false;
                managementProcess.EnableRaisingEvents = true;
                managementProcess.Exited += (s, ev) =>
                {
                    this.BeginInvoke((Action)(() =>
                    {
                        refreshTimer.Stop();
                        btnRun.Enabled = true;
                        btnStop.Enabled = false;
                        MessageBox.Show("Procesul management s-a încheiat.");
                    }));
                };

                managementProcess.Start();
                refreshTimer.Start();

                await Task.Delay(500);
                RefreshNow();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Eroare la pornire: " + ex.Message, "Eroare", MessageBoxButtons.OK, MessageBoxIcon.Error);
                btnRun.Enabled = true;
                btnStop.Enabled = false;
            }
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            try
            {
                refreshTimer.Stop();
                var stopPath = Path.Combine(Path.GetDirectoryName(ManagementExe), "current_day.txt");
                File.WriteAllText(stopPath, "STOP");
                MessageBox.Show("Semnal STOP trimis către procese.");
                btnRun.Enabled = true;
                btnStop.Enabled = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Eroare la STOP: " + ex.Message);
            }
        }


        private void RefreshTimer_Tick(object sender, EventArgs e) => RefreshNow();

        private void RefreshNow()
        {
            textLogs.Text = SafeReadAllText(LogsPath);
            textErrors.Text = SafeReadAllText(ErrorsPath);
            labelSold.Text = "Sold total: " + (SafeReadAllText(SoldPath).Trim().Length == 0 ? "0" : SafeReadAllText(SoldPath).Trim());
            labelDonations.Text = "Donations total: " + (SafeReadAllText(DonationsPath).Trim().Length == 0 ? "0" : SafeReadAllText(DonationsPath).Trim());

            // autoscroll la final
            textLogs.SelectionStart = textLogs.Text.Length;
            textLogs.ScrollToCaret();
            textErrors.SelectionStart = textErrors.Text.Length;
            textErrors.ScrollToCaret();
        }

        private static string SafeReadAllText(string path)
        {
            try
            {
                return File.Exists(path) ? File.ReadAllText(path) : "";
            }
            catch { return ""; }
        }

        private static void MirrorDirectory(string src, string dst)
        {
            if (Directory.Exists(dst))
                Directory.Delete(dst, true);
            Directory.CreateDirectory(dst);

            foreach (var file in Directory.EnumerateFiles(src))
            {
                var name = Path.GetFileName(file);
                File.Copy(file, Path.Combine(dst, name), true);
            }
            foreach (var dir in Directory.EnumerateDirectories(src))
            {
                var name = Path.GetFileName(dir);
                MirrorDirectory(dir, Path.Combine(dst, name));
            }
        }

        private void textErrors_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
