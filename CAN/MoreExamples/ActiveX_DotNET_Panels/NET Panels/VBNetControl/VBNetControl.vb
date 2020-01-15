Public Class VBNetControl
    Inherits System.Windows.Forms.UserControl

    'Declare types
    Dim WithEvents App As CANoe.Application
    Dim WithEvents Measurement As CANoe.Measurement
    Dim systemCAN As CANoe.System
    Dim WithEvents svMotorSwitch As CANoe.Variable
    Dim WithEvents svTurnSignal As CANoe.Variable
    Dim WithEvents svEngineSpeedEntry As CANoe.Variable

    Friend WithEvents tmrSignals As System.Windows.Forms.Timer

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Initialize types
        App = New CANoe.Application
        Measurement = App.Measurement
        systemCAN = App.System

        If Not (systemCAN Is Nothing) Then
            Dim namespaces As CANoe.Namespaces
            namespaces = systemCAN.Namespaces

            If Not (namespaces Is Nothing) Then
                Dim nsEngine As CANoe.Namespace
                nsEngine = namespaces("Engine")

                If Not (nsEngine Is Nothing) Then
                    Dim engineVars As CANoe.Variables
                    engineVars = nsEngine.Variables

                    If Not (engineVars Is Nothing) Then
                        svMotorSwitch = engineVars("MotorSwitch")
                        svEngineSpeedEntry = engineVars("EngineSpeedEntry")
                    End If
                End If

                Dim nsLights As CANoe.Namespace
                nsLights = namespaces("Lights")

                If Not (nsLights Is Nothing) Then
                    Dim lightsVars As CANoe.Variables
                    lightsVars = nsLights.Variables

                    If Not (lightsVars Is Nothing) Then
                        svTurnSignal = lightsVars("TurnSignal")
                    End If
                End If
            End If
        End If

    End Sub

    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If

            If Not (tmrSignals Is Nothing) Then
                tmrSignals.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

#Region " Windows Form Designer generated code "

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents fraMotor As System.Windows.Forms.GroupBox
    Friend WithEvents fraLight As System.Windows.Forms.GroupBox
    Friend WithEvents lblDriving As System.Windows.Forms.Label
    Friend WithEvents lblTurn As System.Windows.Forms.Label
    Friend WithEvents btnLightSwitch As System.Windows.Forms.Button
    Friend WithEvents btnTurnSignal As System.Windows.Forms.Button
    Friend WithEvents fraEntry As System.Windows.Forms.GroupBox
    Friend WithEvents sliSpeedEntry As System.Windows.Forms.TrackBar
    Friend WithEvents btnMotorSwitch As System.Windows.Forms.Button
    Friend WithEvents lblCaption As System.Windows.Forms.Label
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.fraMotor = New System.Windows.Forms.GroupBox
        Me.btnMotorSwitch = New System.Windows.Forms.Button
        Me.fraLight = New System.Windows.Forms.GroupBox
        Me.btnTurnSignal = New System.Windows.Forms.Button
        Me.btnLightSwitch = New System.Windows.Forms.Button
        Me.lblTurn = New System.Windows.Forms.Label
        Me.lblDriving = New System.Windows.Forms.Label
        Me.fraEntry = New System.Windows.Forms.GroupBox
        Me.sliSpeedEntry = New System.Windows.Forms.TrackBar
        Me.lblCaption = New System.Windows.Forms.Label
        Me.tmrSignals = New System.Windows.Forms.Timer(Me.components)
        Me.fraMotor.SuspendLayout()
        Me.fraLight.SuspendLayout()
        Me.fraEntry.SuspendLayout()
        CType(Me.sliSpeedEntry, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'fraMotor
        '
        Me.fraMotor.Controls.Add(Me.btnMotorSwitch)
        Me.fraMotor.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.fraMotor.Location = New System.Drawing.Point(8, 32)
        Me.fraMotor.Name = "fraMotor"
        Me.fraMotor.Size = New System.Drawing.Size(104, 72)
        Me.fraMotor.TabIndex = 0
        Me.fraMotor.TabStop = False
        Me.fraMotor.Text = "Motor Switch"
        '
        'btnMotorSwitch
        '
        Me.btnMotorSwitch.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnMotorSwitch.Location = New System.Drawing.Point(24, 32)
        Me.btnMotorSwitch.Name = "btnMotorSwitch"
        Me.btnMotorSwitch.Size = New System.Drawing.Size(56, 25)
        Me.btnMotorSwitch.TabIndex = 0
        Me.btnMotorSwitch.Text = "Off"
        '
        'fraLight
        '
        Me.fraLight.Controls.Add(Me.btnTurnSignal)
        Me.fraLight.Controls.Add(Me.btnLightSwitch)
        Me.fraLight.Controls.Add(Me.lblTurn)
        Me.fraLight.Controls.Add(Me.lblDriving)
        Me.fraLight.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.fraLight.Location = New System.Drawing.Point(120, 32)
        Me.fraLight.Name = "fraLight"
        Me.fraLight.Size = New System.Drawing.Size(136, 72)
        Me.fraLight.TabIndex = 1
        Me.fraLight.TabStop = False
        Me.fraLight.Text = "Light Switch"
        '
        'btnTurnSignal
        '
        Me.btnTurnSignal.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnTurnSignal.Location = New System.Drawing.Point(78, 38)
        Me.btnTurnSignal.Name = "btnTurnSignal"
        Me.btnTurnSignal.Size = New System.Drawing.Size(40, 25)
        Me.btnTurnSignal.TabIndex = 3
        Me.btnTurnSignal.Text = "Off"
        '
        'btnLightSwitch
        '
        Me.btnLightSwitch.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnLightSwitch.Location = New System.Drawing.Point(14, 38)
        Me.btnLightSwitch.Name = "btnLightSwitch"
        Me.btnLightSwitch.Size = New System.Drawing.Size(40, 25)
        Me.btnLightSwitch.TabIndex = 2
        Me.btnLightSwitch.Text = "Off"
        '
        'lblTurn
        '
        Me.lblTurn.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblTurn.Location = New System.Drawing.Point(66, 20)
        Me.lblTurn.Name = "lblTurn"
        Me.lblTurn.Size = New System.Drawing.Size(66, 16)
        Me.lblTurn.TabIndex = 1
        Me.lblTurn.Text = "Turn signal"
        '
        'lblDriving
        '
        Me.lblDriving.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblDriving.Location = New System.Drawing.Point(14, 20)
        Me.lblDriving.Name = "lblDriving"
        Me.lblDriving.Size = New System.Drawing.Size(48, 16)
        Me.lblDriving.TabIndex = 0
        Me.lblDriving.Text = "Driving"
        '
        'fraEntry
        '
        Me.fraEntry.Controls.Add(Me.sliSpeedEntry)
        Me.fraEntry.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.fraEntry.Location = New System.Drawing.Point(8, 112)
        Me.fraEntry.Name = "fraEntry"
        Me.fraEntry.Size = New System.Drawing.Size(248, 72)
        Me.fraEntry.TabIndex = 2
        Me.fraEntry.TabStop = False
        Me.fraEntry.Text = "Motor Entry"
        '
        'sliSpeedEntry
        '
        Me.sliSpeedEntry.Location = New System.Drawing.Point(16, 24)
        Me.sliSpeedEntry.Maximum = 3500
        Me.sliSpeedEntry.Name = "sliSpeedEntry"
        Me.sliSpeedEntry.Size = New System.Drawing.Size(216, 45)
        Me.sliSpeedEntry.TabIndex = 0
        Me.sliSpeedEntry.TickFrequency = 350
        '
        'lblCaption
        '
        Me.lblCaption.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblCaption.Location = New System.Drawing.Point(8, 8)
        Me.lblCaption.Name = "lblCaption"
        Me.lblCaption.Size = New System.Drawing.Size(256, 16)
        Me.lblCaption.TabIndex = 3
        Me.lblCaption.Text = "VB.NET Control"
        Me.lblCaption.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'tmrSignals
        '
        Me.tmrSignals.Enabled = True
        '
        'VBNetControl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.lblCaption)
        Me.Controls.Add(Me.fraEntry)
        Me.Controls.Add(Me.fraLight)
        Me.Controls.Add(Me.fraMotor)
        Me.Name = "VBNetControl"
        Me.Size = New System.Drawing.Size(264, 192)
        Me.fraMotor.ResumeLayout(False)
        Me.fraLight.ResumeLayout(False)
        Me.fraEntry.ResumeLayout(False)
        Me.fraEntry.PerformLayout()
        CType(Me.sliSpeedEntry, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private Sub btnMotorSwitch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMotorSwitch.Click

        'toggle motor switch
        If Measurement.Running = True Then
            If Not (svMotorSwitch Is Nothing) Then
                If btnMotorSwitch.Text = "Off" Then
                    svMotorSwitch.Value = 1
                    btnMotorSwitch.Text = "On"
                Else
                    svMotorSwitch.Value = 0
                    btnMotorSwitch.Text = "Off"
                End If
            End If
        End If

    End Sub

    Private Sub svMotorSwitch_OnChange(ByVal Value As Object) Handles svMotorSwitch.OnChange

        If Value = 0 Then
            btnMotorSwitch.Text = "Off"
        Else
            btnMotorSwitch.Text = "On"
        End If

    End Sub

    Private Sub btnLightSwitch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLightSwitch.Click

        If Measurement.Running = True Then
            Dim signal As CANoe.Signal
            signal = App.Bus.GetSignal(1, "LightState", "OnOff")

            If Not (signal Is Nothing) Then
                If btnLightSwitch.Text = "Off" Then
                    signal.Value = 1
                    btnLightSwitch.Text = "On"
                Else
                    signal.Value = 0
                    btnLightSwitch.Text = "Off"
                End If
            End If
        End If

    End Sub

    Private Sub tmrSignals_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrSignals.Tick
        ' get all the signal values and actualize controls
        If Measurement.Running Then

            Dim bus As CANoe.Bus
            Dim signalCAN As CANoe.Signal
            Try
                bus = App.Bus("CAN")
                If Not (bus Is Nothing) Then
                    signalCAN = bus.GetSignal(1, "LightState", "OnOff")

                    If Not (signalCAN Is Nothing) Then
                        If (signalCAN.Value = 0) Then
                            btnLightSwitch.Text = "Off"
                        Else
                            btnLightSwitch.Text = "On"
                        End If
                    End If
                End If
            Catch
            Finally
                signalCAN = Nothing
                bus = Nothing
            End Try
        End If
    End Sub

    Private Sub btnTurnSignal_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTurnSignal.Click

        If Measurement.Running = True Then
            If Not (svTurnSignal Is Nothing) Then
                If btnTurnSignal.Text = "Off" Then
                    svTurnSignal.Value = 1
                    btnTurnSignal.Text = "On"
                Else
                    svTurnSignal.Value = 0
                    btnTurnSignal.Text = "Off"
                End If
            End If
        End If

    End Sub

    Private Sub svTurnSignal_OnChange(ByVal Value As Object) Handles svTurnSignal.OnChange

        If Value = 0 Then
            btnTurnSignal.Text = "Off"
        Else
            btnTurnSignal.Text = "On"
        End If

    End Sub

    Private Sub sliSpeedEntry_Scroll(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles sliSpeedEntry.Scroll

        If Measurement.Running = True Then
            If Not (svEngineSpeedEntry Is Nothing) Then
                Dim tmp As Double
                tmp = sliSpeedEntry.Value
                svEngineSpeedEntry.Value = tmp
            End If
        End If

    End Sub

    Private Sub svEngineSpeedEntry_OnChange(ByVal Value As Object) Handles svEngineSpeedEntry.OnChange

        sliSpeedEntry.Value = Value

    End Sub

End Class
