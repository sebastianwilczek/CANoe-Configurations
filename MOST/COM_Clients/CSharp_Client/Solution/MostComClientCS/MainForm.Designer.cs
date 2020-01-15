namespace MostComClientCS
{
  partial class MainForm
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose( bool disposing )
    {
      if( disposing && (components != null) )
      {
        components.Dispose();
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
      this.checkBoxConnect = new System.Windows.Forms.CheckBox();
      this.checkBoxMeasurement = new System.Windows.Forms.CheckBox();
      this.label1 = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.label4 = new System.Windows.Forms.Label();
      this.textBoxSymbolicValue = new System.Windows.Forms.TextBox();
      this.textBoxNumericValue = new System.Windows.Forms.TextBox();
      this.buttonOperationGet = new System.Windows.Forms.Button();
      this.groupBoxDeckStatus = new System.Windows.Forms.GroupBox();
      this.groupBoxDeckStatus.SuspendLayout();
      this.SuspendLayout();
      // 
      // checkBoxConnect
      // 
      this.checkBoxConnect.Appearance = System.Windows.Forms.Appearance.Button;
      this.checkBoxConnect.Location = new System.Drawing.Point( 12, 12 );
      this.checkBoxConnect.Name = "checkBoxConnect";
      this.checkBoxConnect.Size = new System.Drawing.Size( 150, 24 );
      this.checkBoxConnect.TabIndex = 0;
      this.checkBoxConnect.Text = "Connect to CANoe";
      this.checkBoxConnect.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      this.checkBoxConnect.UseVisualStyleBackColor = true;
      this.checkBoxConnect.CheckedChanged += new System.EventHandler( this.checkBoxConnect_CheckedChanged );
      // 
      // checkBoxMeasurement
      // 
      this.checkBoxMeasurement.Appearance = System.Windows.Forms.Appearance.Button;
      this.checkBoxMeasurement.Location = new System.Drawing.Point( 168, 12 );
      this.checkBoxMeasurement.Name = "checkBoxMeasurement";
      this.checkBoxMeasurement.Size = new System.Drawing.Size( 150, 24 );
      this.checkBoxMeasurement.TabIndex = 1;
      this.checkBoxMeasurement.Text = "Start Measurement";
      this.checkBoxMeasurement.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      this.checkBoxMeasurement.UseVisualStyleBackColor = true;
      this.checkBoxMeasurement.CheckedChanged += new System.EventHandler( this.checkBoxMeasurement_CheckedChanged );
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point( 11, 21 );
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size( 211, 13 );
      this.label1.TabIndex = 2;
      this.label1.Text = "Function:   AudioDiskPlayer.01.DeckStatus";
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point( 11, 45 );
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size( 82, 13 );
      this.label3.TabIndex = 4;
      this.label3.Text = "Symbolic Value:";
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point( 11, 71 );
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size( 79, 13 );
      this.label4.TabIndex = 6;
      this.label4.Text = "Numeric Value:";
      // 
      // textBoxSymbolicValue
      // 
      this.textBoxSymbolicValue.Location = new System.Drawing.Point( 109, 42 );
      this.textBoxSymbolicValue.Name = "textBoxSymbolicValue";
      this.textBoxSymbolicValue.ReadOnly = true;
      this.textBoxSymbolicValue.Size = new System.Drawing.Size( 100, 20 );
      this.textBoxSymbolicValue.TabIndex = 5;
      // 
      // textBoxNumericValue
      // 
      this.textBoxNumericValue.Location = new System.Drawing.Point( 109, 68 );
      this.textBoxNumericValue.Name = "textBoxNumericValue";
      this.textBoxNumericValue.ReadOnly = true;
      this.textBoxNumericValue.Size = new System.Drawing.Size( 100, 20 );
      this.textBoxNumericValue.TabIndex = 7;
      // 
      // buttonOperationGet
      // 
      this.buttonOperationGet.Location = new System.Drawing.Point( 14, 94 );
      this.buttonOperationGet.Name = "buttonOperationGet";
      this.buttonOperationGet.Size = new System.Drawing.Size( 195, 23 );
      this.buttonOperationGet.TabIndex = 8;
      this.buttonOperationGet.Text = "Get Property Value";
      this.buttonOperationGet.UseVisualStyleBackColor = true;
      this.buttonOperationGet.Click += new System.EventHandler( this.buttonOperationGet_Click );
      // 
      // groupBoxDeckStatus
      // 
      this.groupBoxDeckStatus.Controls.Add( this.buttonOperationGet );
      this.groupBoxDeckStatus.Controls.Add( this.label1 );
      this.groupBoxDeckStatus.Controls.Add( this.textBoxNumericValue );
      this.groupBoxDeckStatus.Controls.Add( this.textBoxSymbolicValue );
      this.groupBoxDeckStatus.Controls.Add( this.label3 );
      this.groupBoxDeckStatus.Controls.Add( this.label4 );
      this.groupBoxDeckStatus.Location = new System.Drawing.Point( 12, 42 );
      this.groupBoxDeckStatus.Name = "groupBoxDeckStatus";
      this.groupBoxDeckStatus.Size = new System.Drawing.Size( 306, 128 );
      this.groupBoxDeckStatus.TabIndex = 9;
      this.groupBoxDeckStatus.TabStop = false;
      this.groupBoxDeckStatus.Text = "Property: DeckStatus";
      // 
      // MainForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size( 330, 181 );
      this.Controls.Add( this.groupBoxDeckStatus );
      this.Controls.Add( this.checkBoxMeasurement );
      this.Controls.Add( this.checkBoxConnect );
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Name = "MainForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Vector.CANoe.MOST: COM Demo C#";
      this.Load += new System.EventHandler( this.MainForm_Load );
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler( this.MainForm_FormClosing );
      this.groupBoxDeckStatus.ResumeLayout( false );
      this.groupBoxDeckStatus.PerformLayout();
      this.ResumeLayout( false );

    }

    #endregion

    private System.Windows.Forms.CheckBox checkBoxConnect;
    private System.Windows.Forms.CheckBox checkBoxMeasurement;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.TextBox textBoxSymbolicValue;
    private System.Windows.Forms.TextBox textBoxNumericValue;
    private System.Windows.Forms.Button buttonOperationGet;
    private System.Windows.Forms.GroupBox groupBoxDeckStatus;
  }
}

