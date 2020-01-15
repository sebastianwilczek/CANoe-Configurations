Public Class VariantCodingControl

    Dim mVCCountryType As Byte
    Dim mVCVehicleType As Byte
    Dim mVCSpecial As Byte
    ' 0 for extended session
    ' 1 for programming session
    Dim ext_or_pro As Integer
    ' 0 for reading
    ' 1 for writing
    Dim read_or_write As Integer

    Dim WithEvents mCANoe As CANoe.Application = CreateObject("CANoe.Application")
    Dim WithEvents mMeasurement As CANoe.Measurement = mCANoe.Measurement
    Dim WithEvents mRequestReadVariantCoding As Diagnostic.DiagnosticRequest
    Dim WithEvents mRequestWriteVariantCoding As Diagnostic.DiagnosticRequest
    Dim WithEvents mRequestDefaultSession As Diagnostic.DiagnosticRequest
    Dim WithEvents mRequestExtendedSession As Diagnostic.DiagnosticRequest
    Dim WithEvents mRequestProgrammingSession As Diagnostic.DiagnosticRequest
    Dim WithEvents mRequestSeedRequest As Diagnostic.DiagnosticRequest
    Dim WithEvents mRequestSendKey As Diagnostic.DiagnosticRequest

    ' If the diagnostics interface is released, the channel will be closed.
    ' So the next time a request is sent, it will be re-opened and another
    ' TesterPresent is sent, flooding the bus with TPs.
    ' Therefore keep the diagnostics object.
    Dim gLastEcuQual As String
    Dim gDiagnostic As Diagnostic.Diagnostic

    Enum ELevel
        eError
        eWarning
        eSuccess
        eClicked
        eResponse
    End Enum

    ' --------------------------
    ' ----- User Variables -----
    ' --------------------------

    Const mECUName As String = "Door"
    Const mBusName As String = "FlexRay"
    Const mReadVariantCodingDataRequestString As String = "VariantCoding/Coding/Read"
    Const mWriteVariantCodingDataRequestString As String = "VariantCoding/Coding/Write"
    Const mDefaultSessionRequestString As String = "Sessions/DefaultSession/Start"
    Const mExtendedSessionRequestString As String = "Sessions/ExtendedDiagnosticSession/Start"
    Const mProgrammingSessionRequestString As String = "Sessions/ProgrammingSession/Start"
    Const mSendKeyRequestString As String = "SecurityAccess/KeyLevel1/Send"
    Const mSeedRequestRequestString As String = "SecurityAccess/SeedLevel1/Request"

    ' creates a request from a given request string
    Private Function GetRequest(ByVal qualPath As String, Optional ByVal ecuQual As String = mECUName _
       , Optional ByVal busName As String = mBusName) As Diagnostic.DiagnosticRequest

        'is measurement running?
        If Not mMeasurement.Running Then
            GetRequest = Nothing
            ComStatus( ELevel.eWarning, "Measurement not running!")
            Exit Function
        End If

        'get the diagnostics interface
        If gDiagnostic Is Nothing Or gLastEcuQual <> ecuQual Then
            gDiagnostic = Nothing
            gLastEcuQual = ecuQual
            Dim network As CANoe.Network
            Dim device As CANoe.Device
            On Error GoTo DiagNotAvailableError

            network = mCANoe.Networks(busName)
            If Not network Is Nothing Then
                device = network.Devices(ecuQual)
                If Not device Is Nothing Then
                    gDiagnostic = device.Diagnostic()
                End If
            End If
        End If

        On Error GoTo CouldNotCreateRequestError
        GetRequest = gDiagnostic.CreateRequest(qualPath)
        Exit Function

DiagNotAvailableError:
        ComStatus(ELevel.eError, "Diagnostics is not available!")
        Exit Function

CouldNotCreateRequestError:
        ComStatus(ELevel.eError, "Could not create Request!")
        Exit Function
    End Function

    ' ----------------------------------
    ' ----- send request functions -----
    ' ----------------------------------

    Private Sub SendProgrammingSessionRequest()
        mRequestProgrammingSession = GetRequest(mProgrammingSessionRequestString)
        If Not mRequestProgrammingSession Is Nothing Then
            mRequestProgrammingSession.Send()
        End If
    End Sub

    Private Sub SendDefaultSessionRequest()
        mRequestDefaultSession = GetRequest(mDefaultSessionRequestString)
        If Not mRequestDefaultSession Is Nothing Then
            mRequestDefaultSession.Send()
        End If
    End Sub

    Private Sub SendExtendedSessionRequest()
        mRequestExtendedSession = GetRequest(mExtendedSessionRequestString)
        If Not mRequestExtendedSession Is Nothing Then
            mRequestExtendedSession.Send()
        End If
    End Sub

    Private Sub SendSeedRequestRequest()
        mRequestSeedRequest = GetRequest(mSeedRequestRequestString)
        If Not mRequestSeedRequest Is Nothing Then
            mRequestSeedRequest.Send()
        End If
    End Sub

    Private Sub SendSendKeyRequest(ByVal key As Long)
        mRequestSendKey = GetRequest(mSendKeyRequestString)
        If Not mRequestSendKey Is Nothing Then
            mRequestSendKey.SetParameter("SecurityKey", key)
            mRequestSendKey.Send()
        End If
    End Sub

    Private Sub SendWriteVariantCodingRequest()
        mRequestWriteVariantCoding = GetRequest(mWriteVariantCodingDataRequestString)
        If Not mRequestWriteVariantCoding Is Nothing Then
            If AssignDataToRequest() Then
                mRequestWriteVariantCoding.Send()
            Else
                InfoStatus(ELevel.eError, "Creation of Request failed!")
            End If
        End If
    End Sub

    Private Sub SendReadVariantCodingRequest()
        mRequestReadVariantCoding = GetRequest(mReadVariantCodingDataRequestString)
        If Not mRequestReadVariantCoding Is Nothing Then
            mRequestReadVariantCoding.Send()
        End If
    End Sub

    Private Sub PerformSecurityAccess()
        SendSeedRequestRequest()
    End Sub

    Private Function CalculateKey(ByVal seed As Long) As Long
        ' Simple number sequence for demonstration
        seed = seed + 1
        seed = -seed
        CalculateKey = seed
    End Function

    Private Sub btnReadVariantCodingData_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnReadVariantCoding.Click
        InfoStatus(ELevel.eClicked, "")
        read_or_write = 0

        On Error GoTo DiagInvalidParameterError
        SendExtendedSessionRequest()
        Exit Sub

DiagInvalidParameterError:
        ComStatus(ELevel.eError, "Invalid Parameter: Please give a value between 0 to 255!")
        Exit Sub
    End Sub

    Private Sub btnWriteVariantCodingData_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnWriteVariantCoding.Click
        InfoStatus(ELevel.eClicked, "")
        read_or_write = 1

        On Error GoTo DiagInvalidParameterError
        SendExtendedSessionRequest()
        Exit Sub

DiagInvalidParameterError:
        ComStatus(ELevel.eError, "Invalid Parameter: Please give a value between 0 to 255!")
        Exit Sub
    End Sub

    Private Sub cbCountryVariant_LostFocus(ByVal sender As Object, ByVal e As System.EventArgs) Handles cbCountryCode.LostFocus
        On Error GoTo DiagInvalidParameterError
        mVCCountryType = cbCountryCode.SelectedIndex
        ComStatus(ELevel.eClicked, "Country=" & mVCCountryType)
        Exit Sub

DiagInvalidParameterError:
        ComStatus(ELevel.eError, "Invalid Parameter: Please get a valid value from the list!")
        Exit Sub
    End Sub

    Private Sub cbVehicleType_LostFocus(ByVal sender As Object, ByVal e As System.EventArgs) Handles cbVehicleType.LostFocus
        On Error GoTo DiagInvalidParameterError
        mVCVehicleType = cbVehicleType.SelectedIndex
        ComStatus(ELevel.eClicked, "VehicleType=" & mVCVehicleType)
        Exit Sub

DiagInvalidParameterError:
        ComStatus(ELevel.eError, "Invalid Parameter: Please get a valid value from the list!")
        Exit Sub
    End Sub

    Private Sub mMeasurement_OnInit() Handles mMeasurement.OnInit
        mVCSpecial = 0
        mVCCountryType = 0
        mVCVehicleType = 0
        AssignDataToGui()
        ComStatus(ELevel.eClicked, "Measurement started")
        InfoStatus(ELevel.eClicked, "")
    End Sub

    Private Sub mRequestDefaultSession_OnConfirmation() Handles mRequestDefaultSession.OnConfirmation
        '    ComStatus(ELevel.eError, "Request Default Session: Confirmed!")
    End Sub

    Private Sub mRequestDefaultSession_OnResponse(ByVal response As Object) Handles mRequestDefaultSession.OnResponse
        ComStatus(ELevel.eResponse, "Request Default Session: Response Received!")
        If response.Positive Then
        Else
            InfoStatus(ELevel.eError, "Switching to default session failed!")
        End If
    End Sub

    Private Sub mRequestDefaultSession_OnTimeout() Handles mRequestDefaultSession.OnTimeout
        ComStatus(ELevel.eError, "Request Default Session: Timeout!")
    End Sub

    Private Sub mRequestExtendedSession_OnConfirmation() Handles mRequestExtendedSession.OnConfirmation
        ComStatus(ELevel.eResponse, "Request Extended Session: Confirmed!")
    End Sub

    Private Sub mRequestExtendedSession_OnResponse(ByVal response As Object) Handles mRequestExtendedSession.OnResponse
        ComStatus(ELevel.eResponse, "Request Extended Session: Response Received!")
        If response.Positive Then
            SendSeedRequestRequest()
        ElseIf response.ResponseCode = &H7E Then
            ext_or_pro = 1
            PerformSecurityAccess()
        Else
            InfoStatus(ELevel.eError, "Switching to extended session failed!")
        End If
    End Sub

    Private Sub mRequestExtendedSession_OnTimeout() Handles mRequestExtendedSession.OnTimeout
        ComStatus(ELevel.eError, "Request Extended Session: Timeout!")
    End Sub

    Private Sub mRequestProgrammingSession_OnConfirmation() Handles mRequestProgrammingSession.OnConfirmation
        ComStatus(ELevel.eResponse, "Request Extended Session: Confirmed!")
    End Sub

    Private Sub mRequestProgrammingSession_OnResponse(ByVal response As Object) Handles mRequestProgrammingSession.OnResponse
        ComStatus(ELevel.eResponse, "Request Programming Session: Response Received!")
        If response.Positive Then
            PerformSecurityAccess()
        Else
            InfoStatus(ELevel.eError, "Request Programming Session failed!")
        End If
    End Sub

    Private Sub mRequestProgrammingSession_OnTimeout() Handles mRequestProgrammingSession.OnTimeout
        ComStatus(ELevel.eError, "Request Programming Session: Timeout!")
    End Sub

    Private Sub mRequestReadVariantCoding_OnConfirmation() Handles mRequestReadVariantCoding.OnConfirmation
        ComStatus(ELevel.eResponse, "Request Read Variant Coding Data: Confirmed!")
    End Sub

    Private Sub mRequestReadVariantCoding_OnResponse(ByVal response As Object) Handles mRequestReadVariantCoding.OnResponse
        ComStatus(ELevel.eResponse, "Request Read Variant Coding Data: Response Received!")
        On Error GoTo ReadingFailedError
        If response.Positive Then
            ' country variant bits depending on what country name was received
            mVCCountryType = response.GetParameter("Codingstring.CountryType", Diagnostic.eValueType.cNumericValue)
            ' vehicle type bits depending on what vehicle type was received
            mVCVehicleType = response.GetParameter("Codingstring.VehicleType", Diagnostic.eValueType.cNumericValue)
            'special settings bits
            mVCSpecial = response.GetParameter("Codingstring.SpecialAdjustment", Diagnostic.eValueType.cNumericValue)
            ' assign recieved data to the correspondending GUI Elements
            AssignDataToGui()
            InfoStatus(ELevel.eSuccess, "Reading succeeded!")
        Else
ReadingFailedError:
            InfoStatus(ELevel.eError, "Reading failed!")
        End If
        SendDefaultSessionRequest()
    End Sub

    Private Sub mRequestReadVariantCoding_OnTimeout() Handles mRequestReadVariantCoding.OnTimeout
        ComStatus(ELevel.eError, "Request Read Variant Coding Data: Timeout!")
    End Sub

    Private Sub mRequestSeedRequest_OnConfirmation() Handles mRequestSeedRequest.OnConfirmation
        ComStatus(ELevel.eResponse, "Request Seed Request: Confirmed!")
    End Sub

    Private Sub mRequestSeedRequest_OnResponse(ByVal response As Object) Handles mRequestSeedRequest.OnResponse
        ComStatus(ELevel.eResponse, "Request Seed Request: Response Received!")
        If response.Positive Then
            Dim seed, key As Long
            seed = response.GetParameter("SecuritySeed", Diagnostic.eValueType.cNumericValue)
            If Err.Number <> 0 Then
                InfoStatus(ELevel.eError, "Access to seed value failed!")
            Else
                key = CalculateKey(seed)
                key = key And 65535
                InfoStatus(ELevel.eResponse, vbNewLine & "Seed " & Hex(seed) & " key " & Hex(key))
                SendSendKeyRequest(key)
            End If
        Else
            InfoStatus(ELevel.eError, "Seed Request failed!")
        End If
    End Sub

    Private Sub mRequestSeedRequest_OnTimeout() Handles mRequestSeedRequest.OnTimeout
        ComStatus(ELevel.eError, "Request Seed Request: Timeout!")
    End Sub

    Private Sub mRequestSendKey_OnConfirmation() Handles mRequestSendKey.OnConfirmation
        ComStatus(ELevel.eResponse, "Request Send Key: Confirmed!")
    End Sub

    Private Sub mRequestSendKey_OnResponse(ByVal response As Object) Handles mRequestSendKey.OnResponse
        ComStatus(ELevel.eResponse, "Request Send Key: Response Received!")
        If Not response.Positive Then
            InfoStatus(ELevel.eError, "Send Key failed!")
            SendDefaultSessionRequest()
        Else
            If ext_or_pro = 0 Then
                ext_or_pro = 1
                SendProgrammingSessionRequest()
            Else
                ext_or_pro = 0
                If read_or_write = 0 Then
                    SendReadVariantCodingRequest()
                Else
                    SendWriteVariantCodingRequest()
                End If
            End If
        End If
    End Sub

    Private Sub mRequestSendKey_OnTimeout() Handles mRequestSendKey.OnTimeout
        ComStatus(ELevel.eError, "Request Send Key: Timeout!")
    End Sub

    Private Sub mRequestWriteVariantCoding_OnConfirmation() Handles mRequestWriteVariantCoding.OnConfirmation
        ComStatus(ELevel.eSuccess, "Request Write Variant Coding Data: Confirmed!")
    End Sub

    Private Sub mRequestWriteVariantCoding_OnResponse(ByVal response As Object) Handles mRequestWriteVariantCoding.OnResponse
        ComStatus(ELevel.eResponse, "Request Write Variant Coding Data: Response Received!")
        If response.Positive Then
            InfoStatus(ELevel.eSuccess, "Writing succeeded!")
        Else
            InfoStatus(ELevel.eError, "Writing failed!")
        End If
        SendDefaultSessionRequest()
    End Sub

    Private Sub mRequestWriteVariantCoding_OnTimeout() Handles mRequestWriteVariantCoding.OnTimeout
        ComStatus(ELevel.eError, "Request Write Variant Coding Data: Timeout!")
    End Sub

    ' ----------------------------------------
    ' ----- several assignment functions -----
    ' ----------------------------------------

    ' Assigns target values to the request
    Private Function AssignDataToRequest() As Boolean
        On Error GoTo DiagInvalidParameterError
        ' first check the validity of the values
        mVCSpecial = numSpecialSetting.Value

        mRequestWriteVariantCoding.SetParameter("Codingstring.CountryType", "" & mVCCountryType)
        mRequestWriteVariantCoding.SetParameter("Codingstring.VehicleType", "" & mVCVehicleType)
        mRequestWriteVariantCoding.SetParameter("Codingstring.SpecialAdjustment", mVCSpecial)
        AssignDataToRequest = True
        Exit Function

DiagInvalidParameterError:
        ComStatus(ELevel.eError, "Invalid Parameters!")
        AssignDataToRequest = False
        Exit Function
    End Function

    ' assign the elements from to data variable to the right GUI Element
    Private Sub AssignDataToGui()
        numSpecialSetting.Value = mVCSpecial
        cbCountryCode.SelectedIndex = mVCCountryType
        cbVehicleType.SelectedIndex = mVCVehicleType
    End Sub

    Private Sub btnReset_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnReset.Click
        mVCSpecial = 0
        mVCCountryType = 0
        mVCVehicleType = 0
        AssignDataToGui()
    End Sub

    Private Sub ComStatus(ByVal level As ELevel, ByVal text As String)
        Select Case level
            Case ELevel.eError
                tbComStatus.BackColor = Color.OrangeRed
            Case ELevel.eSuccess
                tbComStatus.BackColor = Color.Green
            Case ELevel.eWarning
                tbComStatus.BackColor = Color.Yellow
            Case Else
                tbComStatus.BackColor = Color.FromArgb(255, 224, 223, 227)
        End Select
        tbComStatus.Text = text
    End Sub

    Private Sub InfoStatus(ByVal level As ELevel, ByVal text As String)
        Select Case level
            Case ELevel.eError
                tbWriteStatus.BackColor = Color.Red
            Case ELevel.eSuccess
                tbWriteStatus.BackColor = Color.Green
            Case ELevel.eWarning
                tbWriteStatus.BackColor = Color.Yellow
            Case Else ' clicked and response
                tbWriteStatus.BackColor = Color.FromArgb(255, 224, 223, 227)
        End Select
        tbWriteStatus.Text = text
    End Sub
End Class