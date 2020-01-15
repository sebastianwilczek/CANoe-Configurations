<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class VariantCodingControl
    Inherits System.Windows.Forms.UserControl

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
    Me.btnReadVariantCoding = New System.Windows.Forms.Button
    Me.btnWriteVariantCoding = New System.Windows.Forms.Button
    Me.cbCountryCode = New System.Windows.Forms.ComboBox
    Me.cbVehicleType = New System.Windows.Forms.ComboBox
    Me.Label1 = New System.Windows.Forms.Label
    Me.Label2 = New System.Windows.Forms.Label
    Me.numSpecialSetting = New System.Windows.Forms.NumericUpDown
    Me.Label3 = New System.Windows.Forms.Label
    Me.tbComStatus = New System.Windows.Forms.TextBox
    Me.tbWriteStatus = New System.Windows.Forms.TextBox
    Me.GroupBox1 = New System.Windows.Forms.GroupBox
    Me.btnReset = New System.Windows.Forms.Button
    Me.Requests = New System.Windows.Forms.GroupBox
    Me.GroupBox2 = New System.Windows.Forms.GroupBox
    CType(Me.numSpecialSetting, System.ComponentModel.ISupportInitialize).BeginInit()
    Me.GroupBox1.SuspendLayout()
    Me.Requests.SuspendLayout()
    Me.GroupBox2.SuspendLayout()
    Me.SuspendLayout()
    '
    'btnReadVariantCoding
    '
    Me.btnReadVariantCoding.Location = New System.Drawing.Point(8, 17)
    Me.btnReadVariantCoding.Name = "btnReadVariantCoding"
    Me.btnReadVariantCoding.Size = New System.Drawing.Size(140, 30)
    Me.btnReadVariantCoding.TabIndex = 0
    Me.btnReadVariantCoding.Text = "Read Variant Coding"
    Me.btnReadVariantCoding.UseVisualStyleBackColor = True
    '
    'btnWriteVariantCoding
    '
    Me.btnWriteVariantCoding.Location = New System.Drawing.Point(8, 56)
    Me.btnWriteVariantCoding.Name = "btnWriteVariantCoding"
    Me.btnWriteVariantCoding.Size = New System.Drawing.Size(140, 30)
    Me.btnWriteVariantCoding.TabIndex = 1
    Me.btnWriteVariantCoding.Text = "Write Variant Coding"
    Me.btnWriteVariantCoding.UseVisualStyleBackColor = True
    '
    'cbCountryCode
    '
    Me.cbCountryCode.FormattingEnabled = True
    Me.cbCountryCode.Items.AddRange(New Object() {"(undefined)", "Europe", "USA", "Japan", "(others)"})
    Me.cbCountryCode.Location = New System.Drawing.Point(97, 20)
    Me.cbCountryCode.Name = "cbCountryCode"
    Me.cbCountryCode.Size = New System.Drawing.Size(214, 21)
    Me.cbCountryCode.TabIndex = 2
    '
    'cbVehicleType
    '
    Me.cbVehicleType.FormattingEnabled = True
    Me.cbVehicleType.Items.AddRange(New Object() {"(undefined)", "Coupé", "Limousine", "Station Wagon"})
    Me.cbVehicleType.Location = New System.Drawing.Point(97, 53)
    Me.cbVehicleType.Name = "cbVehicleType"
    Me.cbVehicleType.Size = New System.Drawing.Size(214, 21)
    Me.cbVehicleType.TabIndex = 3
    '
    'Label1
    '
    Me.Label1.AutoSize = True
    Me.Label1.Location = New System.Drawing.Point(17, 23)
    Me.Label1.Name = "Label1"
    Me.Label1.Size = New System.Drawing.Size(74, 13)
    Me.Label1.TabIndex = 4
    Me.Label1.Text = "Country Code:"
    Me.Label1.TextAlign = System.Drawing.ContentAlignment.TopRight
    '
    'Label2
    '
    Me.Label2.AutoSize = True
    Me.Label2.Location = New System.Drawing.Point(19, 56)
    Me.Label2.Name = "Label2"
    Me.Label2.Size = New System.Drawing.Size(72, 13)
    Me.Label2.TabIndex = 5
    Me.Label2.Text = "Vehicle Type:"
    Me.Label2.TextAlign = System.Drawing.ContentAlignment.TopRight
    '
    'numSpecialSetting
    '
    Me.numSpecialSetting.Location = New System.Drawing.Point(97, 90)
    Me.numSpecialSetting.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
    Me.numSpecialSetting.Name = "numSpecialSetting"
    Me.numSpecialSetting.Size = New System.Drawing.Size(50, 20)
    Me.numSpecialSetting.TabIndex = 6
    '
    'Label3
    '
    Me.Label3.AutoSize = True
    Me.Label3.Location = New System.Drawing.Point(10, 90)
    Me.Label3.Name = "Label3"
    Me.Label3.Size = New System.Drawing.Size(81, 13)
    Me.Label3.TabIndex = 7
    Me.Label3.Text = "Special Setting:"
    Me.Label3.TextAlign = System.Drawing.ContentAlignment.TopRight
    '
    'tbComStatus
    '
    Me.tbComStatus.BackColor = System.Drawing.SystemColors.Control
    Me.tbComStatus.BorderStyle = System.Windows.Forms.BorderStyle.None
    Me.tbComStatus.Location = New System.Drawing.Point(6, 19)
    Me.tbComStatus.Multiline = True
    Me.tbComStatus.Name = "tbComStatus"
    Me.tbComStatus.Size = New System.Drawing.Size(302, 45)
    Me.tbComStatus.TabIndex = 8
    '
    'tbWriteStatus
    '
    Me.tbWriteStatus.BackColor = System.Drawing.SystemColors.Control
    Me.tbWriteStatus.Location = New System.Drawing.Point(12, 160)
    Me.tbWriteStatus.Multiline = True
    Me.tbWriteStatus.Name = "tbWriteStatus"
    Me.tbWriteStatus.Size = New System.Drawing.Size(148, 45)
    Me.tbWriteStatus.TabIndex = 9
    '
    'GroupBox1
    '
    Me.GroupBox1.Controls.Add(Me.btnReset)
    Me.GroupBox1.Controls.Add(Me.Label3)
    Me.GroupBox1.Controls.Add(Me.numSpecialSetting)
    Me.GroupBox1.Controls.Add(Me.Label2)
    Me.GroupBox1.Controls.Add(Me.Label1)
    Me.GroupBox1.Controls.Add(Me.cbVehicleType)
    Me.GroupBox1.Controls.Add(Me.cbCountryCode)
    Me.GroupBox1.Location = New System.Drawing.Point(169, 12)
    Me.GroupBox1.Name = "GroupBox1"
    Me.GroupBox1.Size = New System.Drawing.Size(318, 122)
    Me.GroupBox1.TabIndex = 10
    Me.GroupBox1.TabStop = False
    Me.GroupBox1.Text = "Variant Coding Data"
    '
    'btnReset
    '
    Me.btnReset.Location = New System.Drawing.Point(242, 87)
    Me.btnReset.Name = "btnReset"
    Me.btnReset.Size = New System.Drawing.Size(66, 23)
    Me.btnReset.TabIndex = 8
    Me.btnReset.Text = "Reset"
    Me.btnReset.UseVisualStyleBackColor = True
    '
    'Requests
    '
    Me.Requests.Controls.Add(Me.btnWriteVariantCoding)
    Me.Requests.Controls.Add(Me.btnReadVariantCoding)
    Me.Requests.Location = New System.Drawing.Point(4, 6)
    Me.Requests.Name = "Requests"
    Me.Requests.Size = New System.Drawing.Size(156, 96)
    Me.Requests.TabIndex = 11
    Me.Requests.TabStop = False
    Me.Requests.Text = "Requests"
    '
    'GroupBox2
    '
    Me.GroupBox2.Controls.Add(Me.tbComStatus)
    Me.GroupBox2.Location = New System.Drawing.Point(169, 141)
    Me.GroupBox2.Name = "GroupBox2"
    Me.GroupBox2.Size = New System.Drawing.Size(317, 71)
    Me.GroupBox2.TabIndex = 12
    Me.GroupBox2.TabStop = False
    Me.GroupBox2.Text = "COM/Diagnostics Status"
    '
    'VariantCodingControl
    '
    Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
    Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
    Me.Controls.Add(Me.GroupBox2)
    Me.Controls.Add(Me.Requests)
    Me.Controls.Add(Me.GroupBox1)
    Me.Controls.Add(Me.tbWriteStatus)
    Me.Name = "VariantCodingControl"
    Me.Size = New System.Drawing.Size(545, 223)
    CType(Me.numSpecialSetting, System.ComponentModel.ISupportInitialize).EndInit()
    Me.GroupBox1.ResumeLayout(False)
    Me.GroupBox1.PerformLayout()
    Me.Requests.ResumeLayout(False)
    Me.GroupBox2.ResumeLayout(False)
    Me.GroupBox2.PerformLayout()
    Me.ResumeLayout(False)
    Me.PerformLayout()

  End Sub
    Friend WithEvents btnReadVariantCoding As System.Windows.Forms.Button
    Friend WithEvents btnWriteVariantCoding As System.Windows.Forms.Button
    Friend WithEvents cbCountryCode As System.Windows.Forms.ComboBox
    Friend WithEvents cbVehicleType As System.Windows.Forms.ComboBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents numSpecialSetting As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents tbComStatus As System.Windows.Forms.TextBox

    Public Sub New()

        ' This call is required by the Windows Form Designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.

    End Sub
    Friend WithEvents tbWriteStatus As System.Windows.Forms.TextBox
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Requests As System.Windows.Forms.GroupBox
    Friend WithEvents btnReset As System.Windows.Forms.Button
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
End Class
