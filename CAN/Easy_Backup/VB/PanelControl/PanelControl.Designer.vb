<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class PanelControl
    Inherits System.Windows.Forms.Form

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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(PanelControl))
        Me.Label1 = New System.Windows.Forms.Label()
        Me.btnOpenControlPanel = New System.Windows.Forms.Button()
        Me.btnCloseControlPanel = New System.Windows.Forms.Button()
        Me.btnStartMeasurement = New System.Windows.Forms.Button()
        Me.btnStopMeasurement = New System.Windows.Forms.Button()
        Me.btnExit = New System.Windows.Forms.Button()
        Me.btnCloseDisplayPanel = New System.Windows.Forms.Button()
        Me.btnOpenDisplayPanel = New System.Windows.Forms.Button()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.btnCloseAllPanels = New System.Windows.Forms.Button()
        Me.btnOpenAllPanels = New System.Windows.Forms.Button()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.btnStartCANoe = New System.Windows.Forms.Button()
        Me.GroupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label1.Location = New System.Drawing.Point(16, 26)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(73, 13)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "Control Panel:"
        '
        'btnOpenControlPanel
        '
        Me.btnOpenControlPanel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnOpenControlPanel.Location = New System.Drawing.Point(115, 21)
        Me.btnOpenControlPanel.Name = "btnOpenControlPanel"
        Me.btnOpenControlPanel.Size = New System.Drawing.Size(75, 23)
        Me.btnOpenControlPanel.TabIndex = 1
        Me.btnOpenControlPanel.Text = "Open"
        Me.btnOpenControlPanel.UseVisualStyleBackColor = True
        '
        'btnCloseControlPanel
        '
        Me.btnCloseControlPanel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnCloseControlPanel.Location = New System.Drawing.Point(196, 21)
        Me.btnCloseControlPanel.Name = "btnCloseControlPanel"
        Me.btnCloseControlPanel.Size = New System.Drawing.Size(75, 23)
        Me.btnCloseControlPanel.TabIndex = 2
        Me.btnCloseControlPanel.Text = "Close"
        Me.btnCloseControlPanel.UseVisualStyleBackColor = True
        '
        'btnStartMeasurement
        '
        Me.btnStartMeasurement.Location = New System.Drawing.Point(331, 56)
        Me.btnStartMeasurement.Name = "btnStartMeasurement"
        Me.btnStartMeasurement.Size = New System.Drawing.Size(128, 23)
        Me.btnStartMeasurement.TabIndex = 1
        Me.btnStartMeasurement.Text = "Start Measurement"
        Me.btnStartMeasurement.UseVisualStyleBackColor = True
        '
        'btnStopMeasurement
        '
        Me.btnStopMeasurement.Location = New System.Drawing.Point(331, 85)
        Me.btnStopMeasurement.Name = "btnStopMeasurement"
        Me.btnStopMeasurement.Size = New System.Drawing.Size(128, 23)
        Me.btnStopMeasurement.TabIndex = 2
        Me.btnStopMeasurement.Text = "Stop Measurement"
        Me.btnStopMeasurement.UseVisualStyleBackColor = True
        '
        'btnExit
        '
        Me.btnExit.Location = New System.Drawing.Point(331, 114)
        Me.btnExit.Name = "btnExit"
        Me.btnExit.Size = New System.Drawing.Size(128, 23)
        Me.btnExit.TabIndex = 3
        Me.btnExit.Text = "Exit CANoe"
        Me.btnExit.UseVisualStyleBackColor = True
        '
        'btnCloseDisplayPanel
        '
        Me.btnCloseDisplayPanel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnCloseDisplayPanel.Location = New System.Drawing.Point(196, 50)
        Me.btnCloseDisplayPanel.Name = "btnCloseDisplayPanel"
        Me.btnCloseDisplayPanel.Size = New System.Drawing.Size(75, 23)
        Me.btnCloseDisplayPanel.TabIndex = 5
        Me.btnCloseDisplayPanel.Text = "Close"
        Me.btnCloseDisplayPanel.UseVisualStyleBackColor = True
        '
        'btnOpenDisplayPanel
        '
        Me.btnOpenDisplayPanel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnOpenDisplayPanel.Location = New System.Drawing.Point(115, 50)
        Me.btnOpenDisplayPanel.Name = "btnOpenDisplayPanel"
        Me.btnOpenDisplayPanel.Size = New System.Drawing.Size(75, 23)
        Me.btnOpenDisplayPanel.TabIndex = 4
        Me.btnOpenDisplayPanel.Text = "Open"
        Me.btnOpenDisplayPanel.UseVisualStyleBackColor = True
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label2.Location = New System.Drawing.Point(16, 55)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(74, 13)
        Me.Label2.TabIndex = 3
        Me.Label2.Text = "Display Panel:"
        '
        'btnCloseAllPanels
        '
        Me.btnCloseAllPanels.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnCloseAllPanels.Location = New System.Drawing.Point(196, 89)
        Me.btnCloseAllPanels.Name = "btnCloseAllPanels"
        Me.btnCloseAllPanels.Size = New System.Drawing.Size(75, 23)
        Me.btnCloseAllPanels.TabIndex = 8
        Me.btnCloseAllPanels.Text = "Close"
        Me.btnCloseAllPanels.UseVisualStyleBackColor = True
        '
        'btnOpenAllPanels
        '
        Me.btnOpenAllPanels.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnOpenAllPanels.Location = New System.Drawing.Point(115, 89)
        Me.btnOpenAllPanels.Name = "btnOpenAllPanels"
        Me.btnOpenAllPanels.Size = New System.Drawing.Size(75, 23)
        Me.btnOpenAllPanels.TabIndex = 7
        Me.btnOpenAllPanels.Text = "Open"
        Me.btnOpenAllPanels.UseVisualStyleBackColor = True
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label3.Location = New System.Drawing.Point(16, 94)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(56, 13)
        Me.Label3.TabIndex = 6
        Me.Label3.Text = "All Panels:"
        '
        'GroupBox1
        '
        Me.GroupBox1.BackColor = System.Drawing.SystemColors.ControlLight
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Controls.Add(Me.btnCloseAllPanels)
        Me.GroupBox1.Controls.Add(Me.btnCloseControlPanel)
        Me.GroupBox1.Controls.Add(Me.btnOpenControlPanel)
        Me.GroupBox1.Controls.Add(Me.btnOpenAllPanels)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Controls.Add(Me.btnOpenDisplayPanel)
        Me.GroupBox1.Controls.Add(Me.Label3)
        Me.GroupBox1.Controls.Add(Me.btnCloseDisplayPanel)
        Me.GroupBox1.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(12, 12)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(284, 124)
        Me.GroupBox1.TabIndex = 9
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Panel Control"
        '
        'btnStartCANoe
        '
        Me.btnStartCANoe.Location = New System.Drawing.Point(331, 28)
        Me.btnStartCANoe.Name = "btnStartCANoe"
        Me.btnStartCANoe.Size = New System.Drawing.Size(128, 23)
        Me.btnStartCANoe.TabIndex = 10
        Me.btnStartCANoe.Text = "Start CANoe"
        Me.btnStartCANoe.UseVisualStyleBackColor = True
        '
        'PanelControl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(486, 149)
        Me.Controls.Add(Me.btnStartCANoe)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.btnExit)
        Me.Controls.Add(Me.btnStopMeasurement)
        Me.Controls.Add(Me.btnStartMeasurement)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Name = "PanelControl"
        Me.Text = "CANoe Automot Control"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents btnOpenControlPanel As System.Windows.Forms.Button
    Friend WithEvents btnCloseControlPanel As System.Windows.Forms.Button
    Friend WithEvents btnStartMeasurement As System.Windows.Forms.Button
    Friend WithEvents btnStopMeasurement As System.Windows.Forms.Button
    Friend WithEvents btnExit As System.Windows.Forms.Button
    Friend WithEvents btnCloseDisplayPanel As System.Windows.Forms.Button
    Friend WithEvents btnOpenDisplayPanel As System.Windows.Forms.Button
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents btnCloseAllPanels As System.Windows.Forms.Button
    Friend WithEvents btnOpenAllPanels As System.Windows.Forms.Button
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents btnStartCANoe As System.Windows.Forms.Button

End Class
