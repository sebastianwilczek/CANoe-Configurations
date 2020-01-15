namespace VariantCodingPanel
{
  partial class VariantCoding
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
      this.frmRequests = new System.Windows.Forms.GroupBox();
      this.btnWriteVariantCodingData = new System.Windows.Forms.Button();
      this.btnReadVariantCodingData = new System.Windows.Forms.Button();
      this.frmStatus = new System.Windows.Forms.GroupBox();
      this.lCOMStatus = new System.Windows.Forms.Label();
      this.frmVariantCodingData = new System.Windows.Forms.GroupBox();
      this.cobVehicleType = new System.Windows.Forms.ComboBox();
      this.cobCountryVariant = new System.Windows.Forms.ComboBox();
      this.txtSpecialSettings = new System.Windows.Forms.TextBox();
      this.lSpecualSettings = new System.Windows.Forms.Label();
      this.lVehicleType = new System.Windows.Forms.Label();
      this.lCountryVariant = new System.Windows.Forms.Label();
      this.lWriteStatus = new System.Windows.Forms.Label();
      this.btnResetCodingData = new System.Windows.Forms.Button();
      this.frmRequests.SuspendLayout();
      this.frmStatus.SuspendLayout();
      this.frmVariantCodingData.SuspendLayout();
      this.SuspendLayout();
      // 
      // frmRequests
      // 
      this.frmRequests.Controls.Add(this.btnWriteVariantCodingData);
      this.frmRequests.Controls.Add(this.btnReadVariantCodingData);
      this.frmRequests.Location = new System.Drawing.Point(3, 3);
      this.frmRequests.Name = "frmRequests";
      this.frmRequests.Size = new System.Drawing.Size(173, 84);
      this.frmRequests.TabIndex = 1;
      this.frmRequests.TabStop = false;
      this.frmRequests.Text = "Requests";
      // 
      // btnWriteVariantCodingData
      // 
      this.btnWriteVariantCodingData.Location = new System.Drawing.Point(16, 49);
      this.btnWriteVariantCodingData.Name = "btnWriteVariantCodingData";
      this.btnWriteVariantCodingData.Size = new System.Drawing.Size(142, 23);
      this.btnWriteVariantCodingData.TabIndex = 1;
      this.btnWriteVariantCodingData.Text = "Write Variant Coding Data";
      this.btnWriteVariantCodingData.UseVisualStyleBackColor = true;
      this.btnWriteVariantCodingData.Click += new System.EventHandler(this.btnWriteVariantCodingData_Click);
      // 
      // btnReadVariantCodingData
      // 
      this.btnReadVariantCodingData.Location = new System.Drawing.Point(16, 19);
      this.btnReadVariantCodingData.Name = "btnReadVariantCodingData";
      this.btnReadVariantCodingData.Size = new System.Drawing.Size(142, 23);
      this.btnReadVariantCodingData.TabIndex = 0;
      this.btnReadVariantCodingData.Text = "Read Variant Coding Data";
      this.btnReadVariantCodingData.UseVisualStyleBackColor = true;
      this.btnReadVariantCodingData.Click += new System.EventHandler(this.btnReadVariantCodingData_Click);
      // 
      // frmStatus
      // 
      this.frmStatus.Controls.Add(this.lCOMStatus);
      this.frmStatus.Location = new System.Drawing.Point(182, 117);
      this.frmStatus.Name = "frmStatus";
      this.frmStatus.Size = new System.Drawing.Size(268, 75);
      this.frmStatus.TabIndex = 4;
      this.frmStatus.TabStop = false;
      this.frmStatus.Text = "COM/ Diagnostic Status";
      // 
      // lCOMStatus
      // 
      this.lCOMStatus.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
      this.lCOMStatus.Location = new System.Drawing.Point(6, 16);
      this.lCOMStatus.Name = "lCOMStatus";
      this.lCOMStatus.Size = new System.Drawing.Size(256, 56);
      this.lCOMStatus.TabIndex = 0;
      this.lCOMStatus.Text = "...";
      // 
      // frmVariantCodingData
      // 
      this.frmVariantCodingData.Controls.Add(this.cobVehicleType);
      this.frmVariantCodingData.Controls.Add(this.cobCountryVariant);
      this.frmVariantCodingData.Controls.Add(this.txtSpecialSettings);
      this.frmVariantCodingData.Controls.Add(this.lSpecualSettings);
      this.frmVariantCodingData.Controls.Add(this.lVehicleType);
      this.frmVariantCodingData.Controls.Add(this.lCountryVariant);
      this.frmVariantCodingData.Location = new System.Drawing.Point(182, 3);
      this.frmVariantCodingData.Name = "frmVariantCodingData";
      this.frmVariantCodingData.Size = new System.Drawing.Size(268, 108);
      this.frmVariantCodingData.TabIndex = 5;
      this.frmVariantCodingData.TabStop = false;
      this.frmVariantCodingData.Text = "Variant Coding Data";
      // 
      // cobVehicleType
      // 
      this.cobVehicleType.FormattingEnabled = true;
      this.cobVehicleType.Items.AddRange(new object[] {
            "(undefined)",
            "Coupe",
            "Sedan",
            "Transporter"});
      this.cobVehicleType.Location = new System.Drawing.Point(98, 46);
      this.cobVehicleType.Name = "cobVehicleType";
      this.cobVehicleType.Size = new System.Drawing.Size(164, 21);
      this.cobVehicleType.TabIndex = 7;
      this.cobVehicleType.Text = "(undefined)";
      this.cobVehicleType.Leave += new System.EventHandler(this.cobVehicleType_Leave);
      // 
      // cobCountryVariant
      // 
      this.cobCountryVariant.FormattingEnabled = true;
      this.cobCountryVariant.Items.AddRange(new object[] {
            "(undefined)",
            "Europe",
            "USA",
            "Japan",
            "(others)"});
      this.cobCountryVariant.Location = new System.Drawing.Point(98, 19);
      this.cobCountryVariant.Name = "cobCountryVariant";
      this.cobCountryVariant.Size = new System.Drawing.Size(164, 21);
      this.cobCountryVariant.TabIndex = 6;
      this.cobCountryVariant.Text = "(undefined)";
      this.cobCountryVariant.Leave += new System.EventHandler(this.cobCountryVariant_Leave);
      // 
      // txtSpecialSettings
      // 
      this.txtSpecialSettings.Location = new System.Drawing.Point(98, 80);
      this.txtSpecialSettings.Name = "txtSpecialSettings";
      this.txtSpecialSettings.Size = new System.Drawing.Size(164, 20);
      this.txtSpecialSettings.TabIndex = 4;
      this.txtSpecialSettings.Text = "0";
      // 
      // lSpecualSettings
      // 
      this.lSpecualSettings.AutoSize = true;
      this.lSpecualSettings.Location = new System.Drawing.Point(6, 80);
      this.lSpecualSettings.Name = "lSpecualSettings";
      this.lSpecualSettings.Size = new System.Drawing.Size(86, 13);
      this.lSpecualSettings.TabIndex = 2;
      this.lSpecualSettings.Text = "Special Settings:";
      // 
      // lVehicleType
      // 
      this.lVehicleType.AutoSize = true;
      this.lVehicleType.Location = new System.Drawing.Point(6, 49);
      this.lVehicleType.Name = "lVehicleType";
      this.lVehicleType.Size = new System.Drawing.Size(72, 13);
      this.lVehicleType.TabIndex = 1;
      this.lVehicleType.Text = "Vehicle Type:";
      // 
      // lCountryVariant
      // 
      this.lCountryVariant.AutoSize = true;
      this.lCountryVariant.Location = new System.Drawing.Point(6, 19);
      this.lCountryVariant.Name = "lCountryVariant";
      this.lCountryVariant.Size = new System.Drawing.Size(82, 13);
      this.lCountryVariant.TabIndex = 0;
      this.lCountryVariant.Text = "Country Variant:";
      // 
      // lWriteStatus
      // 
      this.lWriteStatus.Location = new System.Drawing.Point(3, 101);
      this.lWriteStatus.Name = "lWriteStatus";
      this.lWriteStatus.Size = new System.Drawing.Size(173, 30);
      this.lWriteStatus.TabIndex = 5;
      // 
      // btnResetCodingData
      // 
      this.btnResetCodingData.Location = new System.Drawing.Point(19, 152);
      this.btnResetCodingData.Name = "btnResetCodingData";
      this.btnResetCodingData.Size = new System.Drawing.Size(142, 23);
      this.btnResetCodingData.TabIndex = 6;
      this.btnResetCodingData.Text = "Reset Coding Data";
      this.btnResetCodingData.UseVisualStyleBackColor = true;
      this.btnResetCodingData.Click += new System.EventHandler(this.btnResetCodingData_Click);
      // 
      // VariantCoding
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.btnResetCodingData);
      this.Controls.Add(this.lWriteStatus);
      this.Controls.Add(this.frmVariantCodingData);
      this.Controls.Add(this.frmStatus);
      this.Controls.Add(this.frmRequests);
      this.Name = "VariantCoding";
      this.Size = new System.Drawing.Size(457, 201);
      this.frmRequests.ResumeLayout(false);
      this.frmStatus.ResumeLayout(false);
      this.frmVariantCodingData.ResumeLayout(false);
      this.frmVariantCodingData.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.GroupBox frmRequests;
    private System.Windows.Forms.Button btnWriteVariantCodingData;
    private System.Windows.Forms.Button btnReadVariantCodingData;
    private System.Windows.Forms.GroupBox frmStatus;
    private System.Windows.Forms.Label lCOMStatus;
    private System.Windows.Forms.GroupBox frmVariantCodingData;
    private System.Windows.Forms.ComboBox cobVehicleType;
    private System.Windows.Forms.ComboBox cobCountryVariant;
    private System.Windows.Forms.TextBox txtSpecialSettings;
    private System.Windows.Forms.Label lSpecualSettings;
    private System.Windows.Forms.Label lVehicleType;
    private System.Windows.Forms.Label lCountryVariant;
    private System.Windows.Forms.Label lWriteStatus;
    private System.Windows.Forms.Button btnResetCodingData;
  }
}
