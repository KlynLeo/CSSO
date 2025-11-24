namespace MarketUI
{
    partial class Form1
    {
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.TextBox textDeposit;
        private System.Windows.Forms.TextBox textSold;
        private System.Windows.Forms.Button btnBrowseDeposit;
        private System.Windows.Forms.Button btnBrowseSold;
        private System.Windows.Forms.CheckBox chkMirror;
        private System.Windows.Forms.Button btnRun;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.Label labelSold;
        private System.Windows.Forms.Label labelDonations;
        private System.Windows.Forms.TextBox textLogs;
        private System.Windows.Forms.TextBox textErrors;
        private System.Windows.Forms.Timer refreshTimer;
        private System.Windows.Forms.Label lbl1;
        private System.Windows.Forms.Label lbl2;
        private System.Windows.Forms.Label lblLogs;
        private System.Windows.Forms.Label lblErrs;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null)) components.Dispose();
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.textDeposit = new System.Windows.Forms.TextBox();
            this.textSold = new System.Windows.Forms.TextBox();
            this.btnBrowseDeposit = new System.Windows.Forms.Button();
            this.btnBrowseSold = new System.Windows.Forms.Button();
            this.chkMirror = new System.Windows.Forms.CheckBox();
            this.btnRun = new System.Windows.Forms.Button();
            this.btnStop = new System.Windows.Forms.Button();
            this.labelSold = new System.Windows.Forms.Label();
            this.labelDonations = new System.Windows.Forms.Label();
            this.textLogs = new System.Windows.Forms.TextBox();
            this.textErrors = new System.Windows.Forms.TextBox();
            this.refreshTimer = new System.Windows.Forms.Timer(this.components);
            this.lbl1 = new System.Windows.Forms.Label();
            this.lbl2 = new System.Windows.Forms.Label();
            this.lblLogs = new System.Windows.Forms.Label();
            this.lblErrs = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // textDeposit
            // 
            this.textDeposit.Location = new System.Drawing.Point(130, 18);
            this.textDeposit.Name = "textDeposit";
            this.textDeposit.ReadOnly = true;
            this.textDeposit.Size = new System.Drawing.Size(420, 22);
            this.textDeposit.TabIndex = 0;
            // 
            // textSold
            // 
            this.textSold.Location = new System.Drawing.Point(130, 50);
            this.textSold.Name = "textSold";
            this.textSold.ReadOnly = true;
            this.textSold.Size = new System.Drawing.Size(420, 22);
            this.textSold.TabIndex = 1;
            // 
            // btnBrowseDeposit
            // 
            this.btnBrowseDeposit.Location = new System.Drawing.Point(560, 16);
            this.btnBrowseDeposit.Name = "btnBrowseDeposit";
            this.btnBrowseDeposit.Size = new System.Drawing.Size(80, 26);
            this.btnBrowseDeposit.TabIndex = 2;
            this.btnBrowseDeposit.Text = "Browse…";
            this.btnBrowseDeposit.Click += new System.EventHandler(this.btnBrowseDeposit_Click);
            // 
            // btnBrowseSold
            // 
            this.btnBrowseSold.Location = new System.Drawing.Point(560, 48);
            this.btnBrowseSold.Name = "btnBrowseSold";
            this.btnBrowseSold.Size = new System.Drawing.Size(80, 26);
            this.btnBrowseSold.TabIndex = 3;
            this.btnBrowseSold.Text = "Browse…";
            this.btnBrowseSold.Click += new System.EventHandler(this.btnBrowseSold_Click);
            // 
            // chkMirror
            // 
            this.chkMirror.Location = new System.Drawing.Point(660, 18);
            this.chkMirror.Name = "chkMirror";
            this.chkMirror.Size = new System.Drawing.Size(190, 24);
            this.chkMirror.TabIndex = 4;
            this.chkMirror.Text = "Mirror to working dir";
            // 
            // btnRun
            // 
            this.btnRun.Location = new System.Drawing.Point(660, 48);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(90, 26);
            this.btnRun.TabIndex = 5;
            this.btnRun.Text = "Run";
            this.btnRun.Click += new System.EventHandler(this.btnRun_Click);
            // 
            // btnStop
            // 
            this.btnStop.Location = new System.Drawing.Point(760, 48);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(90, 26);
            this.btnStop.TabIndex = 6;
            this.btnStop.Text = "Stop";
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // labelSold
            // 
            this.labelSold.Location = new System.Drawing.Point(20, 85);
            this.labelSold.Name = "labelSold";
            this.labelSold.Size = new System.Drawing.Size(300, 20);
            this.labelSold.TabIndex = 7;
            this.labelSold.Text = "Sold total: 0";
            // 
            // labelDonations
            // 
            this.labelDonations.Location = new System.Drawing.Point(340, 85);
            this.labelDonations.Name = "labelDonations";
            this.labelDonations.Size = new System.Drawing.Size(300, 20);
            this.labelDonations.TabIndex = 8;
            this.labelDonations.Text = "Donations total: 0";
            // 
            // textLogs
            // 
            this.textLogs.Location = new System.Drawing.Point(24, 140);
            this.textLogs.Multiline = true;
            this.textLogs.Name = "textLogs";
            this.textLogs.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textLogs.Size = new System.Drawing.Size(410, 380);
            this.textLogs.TabIndex = 13;
            // 
            // textErrors
            // 
            this.textErrors.Location = new System.Drawing.Point(460, 140);
            this.textErrors.Multiline = true;
            this.textErrors.Name = "textErrors";
            this.textErrors.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textErrors.Size = new System.Drawing.Size(410, 380);
            this.textErrors.TabIndex = 14;
            this.textErrors.TextChanged += new System.EventHandler(this.textErrors_TextChanged);
            // 
            // lbl1
            // 
            this.lbl1.Location = new System.Drawing.Point(24, 20);
            this.lbl1.Name = "lbl1";
            this.lbl1.Size = new System.Drawing.Size(100, 23);
            this.lbl1.TabIndex = 9;
            this.lbl1.Text = "Deposit Folder:";
            // 
            // lbl2
            // 
            this.lbl2.Location = new System.Drawing.Point(24, 52);
            this.lbl2.Name = "lbl2";
            this.lbl2.Size = new System.Drawing.Size(100, 23);
            this.lbl2.TabIndex = 10;
            this.lbl2.Text = "Sold Folder:";
            // 
            // lblLogs
            // 
            this.lblLogs.Location = new System.Drawing.Point(24, 120);
            this.lblLogs.Name = "lblLogs";
            this.lblLogs.Size = new System.Drawing.Size(100, 23);
            this.lblLogs.TabIndex = 11;
            this.lblLogs.Text = "Logs";
            // 
            // lblErrs
            // 
            this.lblErrs.Location = new System.Drawing.Point(460, 120);
            this.lblErrs.Name = "lblErrs";
            this.lblErrs.Size = new System.Drawing.Size(100, 23);
            this.lblErrs.TabIndex = 12;
            this.lblErrs.Text = "Errors";
            // 
            // Form1
            // 
            this.ClientSize = new System.Drawing.Size(900, 550);
            this.Controls.Add(this.textDeposit);
            this.Controls.Add(this.textSold);
            this.Controls.Add(this.btnBrowseDeposit);
            this.Controls.Add(this.btnBrowseSold);
            this.Controls.Add(this.chkMirror);
            this.Controls.Add(this.btnRun);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.labelSold);
            this.Controls.Add(this.labelDonations);
            this.Controls.Add(this.lbl1);
            this.Controls.Add(this.lbl2);
            this.Controls.Add(this.lblLogs);
            this.Controls.Add(this.lblErrs);
            this.Controls.Add(this.textLogs);
            this.Controls.Add(this.textErrors);
            this.Name = "Form1";
            this.Text = "Market Management UI";
            this.ResumeLayout(false);
            this.PerformLayout();

        }
    }
}
