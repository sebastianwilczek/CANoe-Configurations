namespace VariantIdentifier
{
  partial class VariantIdentMainForm
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
      this.label1 = new System.Windows.Forms.Label();
      this.mBt_StartIdent = new System.Windows.Forms.Button();
      this.mTB_Status = new System.Windows.Forms.TextBox();
      this.label3 = new System.Windows.Forms.Label();
      this.mBt_Connect = new System.Windows.Forms.Button();
      this.mLV_Targets = new System.Windows.Forms.ListView();
      this.chDeviceAndCheckmark = new System.Windows.Forms.ColumnHeader();
      this.chNetwork = new System.Windows.Forms.ColumnHeader();
      this.chConfiguredVariant = new System.Windows.Forms.ColumnHeader();
      this.chIdentifiedVariant = new System.Windows.Forms.ColumnHeader();
      this.btExit = new System.Windows.Forms.Button();
      this.mBt_ConfigureVariants = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label1.Location = new System.Drawing.Point(12, 9);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(278, 24);
      this.label1.TabIndex = 0;
      this.label1.Text = "Diagnostic Variant Identifier V1.0";
      // 
      // mBt_StartIdent
      // 
      this.mBt_StartIdent.Enabled = false;
      this.mBt_StartIdent.Location = new System.Drawing.Point(114, 265);
      this.mBt_StartIdent.Name = "mBt_StartIdent";
      this.mBt_StartIdent.Size = new System.Drawing.Size(109, 23);
      this.mBt_StartIdent.TabIndex = 3;
      this.mBt_StartIdent.Text = "Start Identification";
      this.mBt_StartIdent.UseVisualStyleBackColor = true;
      this.mBt_StartIdent.Click += new System.EventHandler(this.mBt_StartIdent_Click);
      // 
      // mTB_Status
      // 
      this.mTB_Status.BackColor = System.Drawing.SystemColors.ControlLight;
      this.mTB_Status.Location = new System.Drawing.Point(16, 176);
      this.mTB_Status.Multiline = true;
      this.mTB_Status.Name = "mTB_Status";
      this.mTB_Status.ReadOnly = true;
      this.mTB_Status.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.mTB_Status.Size = new System.Drawing.Size(445, 83);
      this.mTB_Status.TabIndex = 4;
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(15, 160);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(40, 13);
      this.label3.TabIndex = 5;
      this.label3.Text = "Status:";
      // 
      // mBt_Connect
      // 
      this.mBt_Connect.Location = new System.Drawing.Point(16, 265);
      this.mBt_Connect.Name = "mBt_Connect";
      this.mBt_Connect.Size = new System.Drawing.Size(90, 23);
      this.mBt_Connect.TabIndex = 6;
      this.mBt_Connect.Text = "(Re-) Connect";
      this.mBt_Connect.UseVisualStyleBackColor = true;
      this.mBt_Connect.Click += new System.EventHandler(this.mBt_Connect_Click);
      // 
      // mLV_Targets
      // 
      this.mLV_Targets.CheckBoxes = true;
      this.mLV_Targets.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chDeviceAndCheckmark,
            this.chNetwork,
            this.chConfiguredVariant,
            this.chIdentifiedVariant});
      this.mLV_Targets.FullRowSelect = true;
      this.mLV_Targets.Location = new System.Drawing.Point(17, 36);
      this.mLV_Targets.Name = "mLV_Targets";
      this.mLV_Targets.Size = new System.Drawing.Size(444, 121);
      this.mLV_Targets.TabIndex = 7;
      this.mLV_Targets.UseCompatibleStateImageBehavior = false;
      this.mLV_Targets.View = System.Windows.Forms.View.Details;
      // 
      // chDeviceAndCheckmark
      // 
      this.chDeviceAndCheckmark.Text = "Target";
      this.chDeviceAndCheckmark.Width = 99;
      // 
      // chNetwork
      // 
      this.chNetwork.Text = "Network";
      this.chNetwork.Width = 66;
      // 
      // chConfiguredVariant
      // 
      this.chConfiguredVariant.Text = "Configured Variant";
      this.chConfiguredVariant.Width = 132;
      // 
      // chIdentifiedVariant
      // 
      this.chIdentifiedVariant.Text = "Identified Variant";
      this.chIdentifiedVariant.Width = 142;
      // 
      // btExit
      // 
      this.btExit.Location = new System.Drawing.Point(385, 265);
      this.btExit.Name = "btExit";
      this.btExit.Size = new System.Drawing.Size(75, 23);
      this.btExit.TabIndex = 8;
      this.btExit.Text = "Exit";
      this.btExit.UseVisualStyleBackColor = true;
      this.btExit.Click += new System.EventHandler(this.button1_Click);
      // 
      // mBt_ConfigureVariants
      // 
      this.mBt_ConfigureVariants.Enabled = false;
      this.mBt_ConfigureVariants.Location = new System.Drawing.Point(229, 265);
      this.mBt_ConfigureVariants.Name = "mBt_ConfigureVariants";
      this.mBt_ConfigureVariants.Size = new System.Drawing.Size(113, 23);
      this.mBt_ConfigureVariants.TabIndex = 9;
      this.mBt_ConfigureVariants.Text = "Configure Variants";
      this.mBt_ConfigureVariants.UseVisualStyleBackColor = true;
      this.mBt_ConfigureVariants.Click += new System.EventHandler(this.mBt_ConfigureVariants_Click);
      // 
      // VariantIdentMainForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(481, 312);
      this.Controls.Add(this.mBt_ConfigureVariants);
      this.Controls.Add(this.btExit);
      this.Controls.Add(this.mLV_Targets);
      this.Controls.Add(this.mBt_Connect);
      this.Controls.Add(this.label3);
      this.Controls.Add(this.mTB_Status);
      this.Controls.Add(this.mBt_StartIdent);
      this.Controls.Add(this.label1);
      this.Name = "VariantIdentMainForm";
      this.Text = "Variant Identifier";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button mBt_StartIdent;
    private System.Windows.Forms.TextBox mTB_Status;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Button mBt_Connect;
    private System.Windows.Forms.ListView mLV_Targets;
    private System.Windows.Forms.ColumnHeader chNetwork;
    private System.Windows.Forms.ColumnHeader chConfiguredVariant;
    private System.Windows.Forms.ColumnHeader chIdentifiedVariant;
    private System.Windows.Forms.ColumnHeader chDeviceAndCheckmark;
    private System.Windows.Forms.Button btExit;
    private System.Windows.Forms.Button mBt_ConfigureVariants;
  }
}

