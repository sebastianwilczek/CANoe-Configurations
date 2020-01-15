Public Class VBNetDisplay
    Inherits System.Windows.Forms.UserControl

    ' Declare types
    Dim WithEvents App As CANoe.Application
    Dim WithEvents Measurement As CANoe.Measurement

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Initialize types
        App = New CANoe.Application
        Measurement = App.Measurement

    End Sub

#Region " Windows Form Designer generated code "

    'VBNetDisplay overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents fraEngineSpeed As System.Windows.Forms.GroupBox
    Friend WithEvents fraMotor As System.Windows.Forms.GroupBox
    Friend WithEvents fraLight As System.Windows.Forms.GroupBox
    Friend WithEvents lblEngine As System.Windows.Forms.Label
    Friend WithEvents lblRunning As System.Windows.Forms.Label
    Friend WithEvents lblDriving As System.Windows.Forms.Label
    Friend WithEvents lblTurn As System.Windows.Forms.Label
    Friend WithEvents lblDrivingState As System.Windows.Forms.Label
    Friend WithEvents lblTurnState As System.Windows.Forms.Label
    Friend WithEvents tmrSignals As System.Windows.Forms.Timer
    Friend WithEvents lblCaption As System.Windows.Forms.Label
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
Me.components = New System.ComponentModel.Container
Me.fraEngineSpeed = New System.Windows.Forms.GroupBox
Me.lblEngine = New System.Windows.Forms.Label
Me.fraMotor = New System.Windows.Forms.GroupBox
Me.lblRunning = New System.Windows.Forms.Label
Me.fraLight = New System.Windows.Forms.GroupBox
Me.lblTurnState = New System.Windows.Forms.Label
Me.lblDrivingState = New System.Windows.Forms.Label
Me.lblTurn = New System.Windows.Forms.Label
Me.lblDriving = New System.Windows.Forms.Label
Me.tmrSignals = New System.Windows.Forms.Timer(Me.components)
Me.lblCaption = New System.Windows.Forms.Label
Me.fraEngineSpeed.SuspendLayout()
Me.fraMotor.SuspendLayout()
Me.fraLight.SuspendLayout()
Me.SuspendLayout()
'
'fraEngineSpeed
'
Me.fraEngineSpeed.Controls.Add(Me.lblEngine)
Me.fraEngineSpeed.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.fraEngineSpeed.Location = New System.Drawing.Point(8, 32)
Me.fraEngineSpeed.Name = "fraEngineSpeed"
Me.fraEngineSpeed.Size = New System.Drawing.Size(112, 48)
Me.fraEngineSpeed.TabIndex = 0
Me.fraEngineSpeed.TabStop = False
Me.fraEngineSpeed.Text = "Engine Speed"
'
'lblEngine
'
Me.lblEngine.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.lblEngine.Location = New System.Drawing.Point(8, 24)
Me.lblEngine.Name = "lblEngine"
Me.lblEngine.Size = New System.Drawing.Size(96, 13)
Me.lblEngine.TabIndex = 0
Me.lblEngine.Text = "0"
Me.lblEngine.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
'
'fraMotor
'
Me.fraMotor.Controls.Add(Me.lblRunning)
Me.fraMotor.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.fraMotor.Location = New System.Drawing.Point(128, 32)
Me.fraMotor.Name = "fraMotor"
Me.fraMotor.Size = New System.Drawing.Size(96, 48)
Me.fraMotor.TabIndex = 1
Me.fraMotor.TabStop = False
Me.fraMotor.Text = "Motor State"
'
'lblRunning
'
Me.lblRunning.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.lblRunning.Location = New System.Drawing.Point(8, 24)
Me.lblRunning.Name = "lblRunning"
Me.lblRunning.Size = New System.Drawing.Size(80, 16)
Me.lblRunning.TabIndex = 0
Me.lblRunning.Text = "Not Running"
Me.lblRunning.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
'
'fraLight
'
Me.fraLight.Controls.Add(Me.lblTurnState)
Me.fraLight.Controls.Add(Me.lblDrivingState)
Me.fraLight.Controls.Add(Me.lblTurn)
Me.fraLight.Controls.Add(Me.lblDriving)
Me.fraLight.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.fraLight.Location = New System.Drawing.Point(8, 88)
Me.fraLight.Name = "fraLight"
Me.fraLight.Size = New System.Drawing.Size(216, 48)
Me.fraLight.TabIndex = 2
Me.fraLight.TabStop = False
Me.fraLight.Text = "Driving Light"
'
'lblTurnState
'
Me.lblTurnState.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.lblTurnState.Location = New System.Drawing.Point(176, 24)
Me.lblTurnState.Name = "lblTurnState"
Me.lblTurnState.Size = New System.Drawing.Size(24, 16)
Me.lblTurnState.TabIndex = 3
Me.lblTurnState.Text = "Off"
Me.lblTurnState.TextAlign = System.Drawing.ContentAlignment.TopRight
'
'lblDrivingState
'
Me.lblDrivingState.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.lblDrivingState.Location = New System.Drawing.Point(64, 24)
Me.lblDrivingState.Name = "lblDrivingState"
Me.lblDrivingState.Size = New System.Drawing.Size(24, 16)
Me.lblDrivingState.TabIndex = 2
Me.lblDrivingState.Text = "Off"
Me.lblDrivingState.TextAlign = System.Drawing.ContentAlignment.TopRight
'
'lblTurn
'
Me.lblTurn.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.lblTurn.Location = New System.Drawing.Point(104, 24)
Me.lblTurn.Name = "lblTurn"
Me.lblTurn.Size = New System.Drawing.Size(72, 16)
Me.lblTurn.TabIndex = 1
Me.lblTurn.Text = "Turn signal:"
'
'lblDriving
'
Me.lblDriving.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.lblDriving.Location = New System.Drawing.Point(16, 24)
Me.lblDriving.Name = "lblDriving"
Me.lblDriving.Size = New System.Drawing.Size(56, 16)
Me.lblDriving.TabIndex = 0
Me.lblDriving.Text = "Driving:"
'
'tmrSignals
'
Me.tmrSignals.Enabled = True
'
'lblCaption
'
Me.lblCaption.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
Me.lblCaption.Location = New System.Drawing.Point(8, 8)
Me.lblCaption.Name = "lblCaption"
Me.lblCaption.Size = New System.Drawing.Size(216, 16)
Me.lblCaption.TabIndex = 3
Me.lblCaption.Text = "VB.NET Display"
Me.lblCaption.TextAlign = System.Drawing.ContentAlignment.TopCenter
'
'VBNetDisplay
'
Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
Me.Controls.Add(Me.lblCaption)
Me.Controls.Add(Me.fraLight)
Me.Controls.Add(Me.fraMotor)
Me.Controls.Add(Me.fraEngineSpeed)
Me.Name = "VBNetDisplay"
Me.Size = New System.Drawing.Size(232, 144)
Me.fraEngineSpeed.ResumeLayout(False)
Me.fraMotor.ResumeLayout(False)
Me.fraLight.ResumeLayout(False)
Me.ResumeLayout(False)

End Sub

#End Region
    Private Sub tmrSignals_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrSignals.Tick
        ' get all the signal values and actualize controls
        If Measurement.Running Then

            Dim configuration As CANoe.configuration
            Dim simulationSetup As CANoe.SimulationSetup
            Dim nodes As CANoe.Nodes
            Dim node As CANoe.Node
            Dim motorInputs As CANoe.Signals
            Dim motorControlInputs As CANoe.Signals
            Try
                configuration = App.configuration

                simulationSetup = configuration.SimulationSetup

                nodes = simulationSetup.Nodes

                node = nodes("Motor")

                motorInputs = node.Inputs

                If (motorInputs("OnOff").Value = 0) Then
                    lblRunning.Text = "Not Running"
                Else
                    lblRunning.Text = "Running"
                End If

                lblEngine.Text = motorInputs("EngineSpeed").Value.ToString()

                motorInputs = Nothing

                node = nodes("MotorControl")
                motorControlInputs = node.Inputs

                If (motorControlInputs("OnOff").Value = 0) Then
                    lblDrivingState.Text = "Off"
                Else
                    lblDrivingState.Text = "On"
                End If

                If (motorControlInputs("TurnSignal").Value = 0) Then
                    lblTurnState.Text = "Off"
                Else
                    lblTurnState.Text = "On"
                End If
            Catch
            Finally
                motorControlInputs = Nothing
                node = Nothing
                nodes = Nothing
                simulationSetup = Nothing
                configuration = Nothing
            End Try
        End If
    End Sub

End Class
