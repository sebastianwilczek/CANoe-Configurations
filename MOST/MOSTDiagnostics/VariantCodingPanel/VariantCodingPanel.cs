using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace VariantCodingPanel
{
  public partial class VariantCoding : UserControl
  {
    byte mVCCountryType;
    byte mVCVehicleType;
    byte mVCSpecial;
    // 0 for extended session
    // 1 for programming session
    int ext_or_pro;
    // 0 for reading
    // 1 for writing
    int read_or_write;

    int[] crc16Tab = new int[] {0x0, 0xC0C1, 0xC181, 0x140, 0xC301, 0x3C0, 0x280, 0xC241, 0xC601, 0x6C0, 0x780, 0xC741, 0x500, 0xC5C1, 0xC481, 0x440, 
                                 0xCC01, 0xCC0, 0xD80, 0xCD41, 0xF00, 0xCFC1, 0xCE81, 0xE40, 0xA00, 0xCAC1, 0xCB81, 0xB40, 0xC901, 0x9C0, 0x880, 0xC841,
                                 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 
                                 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 
                                 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
                                 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 
                                 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40, 
                                 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041, 
                                 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441, 
                                 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
                                 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
                                 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
                                 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
                                 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
                                 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
                                 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040};



    // If the diagnostics interface is released, the channel will be closed.
    // So the next time a request is sent, it will be re-opened and another
    // TesterPresent is sent, flooding the bus with TPs.
    // Therefore keep the diagnostics object.
    string gLastEcuQual;
    Diagnostic.Diagnostic gDiagnostic;

    // Declare types
    private CANoe.Application mCANoe;
    private CANoe.Measurement mMeasurement;
    private Diagnostic.DiagnosticRequest mRequestReadVariantCoding;
    private Diagnostic.DiagnosticRequest mRequestWriteVariantCoding;
    private Diagnostic.DiagnosticRequest mRequestDefaultSession;
    private Diagnostic.DiagnosticRequest mRequestExtendedSession;
    private Diagnostic.DiagnosticRequest mRequestProgrammingSession;
    private Diagnostic.DiagnosticRequest mRequestSeedRequest;
    private Diagnostic.DiagnosticRequest mRequestSendKey;

    //--------------------------
    //----- User Variables -----
    //--------------------------
    const string mECUName = "CDC1";
    const string mBusName = "CAN";
    const string mReadVariantCodingDataRequestString = "VariantCoding/Coding/Read";
    const string mWriteVariantCodingDataRequestString = "VariantCoding/Coding/Write";
    const string mDefaultSessionRequestString = "Sessions/DefaultSession/Start";
    const string mExtendedSessionRequestString = "Sessions/ExtendedDiagnosticSession/Start";
    const string mProgrammingSessionRequestString = "Sessions/ProgrammingSession/Start";
    const string mSendKeyRequestString = "SecurityAccess/KeyLevel1/Send";
    const string mSeedRequestRequestString = "SecurityAccess/SeedLevel1/Request";
      
    public VariantCoding()
    {
      InitializeComponent();
    }

    #region "*************************** GUI functions ****************************"
    private void btnReadVariantCodingData_Click(object sender, EventArgs e)
    {
      try{
        lWriteStatus.Text = "";
        lWriteStatus.BackColor = System.Drawing.SystemColors.Control;
        read_or_write = 0;
    
        SendExtendedSessionRequest();
      }
      catch
      {
        lCOMStatus.Text = "Invalid Parameter: Please give a value between 0 to 255!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      }
    }

    private void btnWriteVariantCodingData_Click(object sender, EventArgs e)
    {
      try{
        lWriteStatus.Text = "";
        lWriteStatus.BackColor = System.Drawing.SystemColors.Control;
        read_or_write = 1;
    
        SendExtendedSessionRequest();
      }
      catch
      {
        lCOMStatus.Text = "Invalid Parameter: Please give a value between 0 to 255!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      }
    }
    #endregion

    #region "****************************** CANoe Events ****************************"
    private void Measurement_OnInit()
    {
      MethodInvoker LabelUpdate = delegate
      {           
        mVCSpecial = 0;
        mVCCountryType = 0;
        mVCVehicleType = 0;
        AssignDataToGui();
        lCOMStatus.Text = "Measurement started";
        lCOMStatus.BackColor = System.Drawing.SystemColors.Control;
        lWriteStatus.Text = "";
        lWriteStatus.BackColor =  System.Drawing.SystemColors.Control;        
      };
      Invoke(LabelUpdate);
    }
    
    private void RequestDefaultSession_OnResponse(Object response)
    {
      Diagnostic.DiagnosticResponse res = (Diagnostic.DiagnosticResponse)response;
    
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.BackColor = System.Drawing.SystemColors.Control;
      
        if(res.Positive==true)
        {
        }
        else
        {
          lWriteStatus.Text = "Switching to default session failed!";
          lWriteStatus.BackColor = System.Drawing.Color.Red; 
        }
      };

      Invoke(LabelUpdate);    
    }
    
    private void RequestExtendedSession_OnResponse(Object response)
    {
      Diagnostic.DiagnosticResponse res = (Diagnostic.DiagnosticResponse)response;
    
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Extended Session: Response Received!";
        lCOMStatus.BackColor = System.Drawing.SystemColors.Control;  
           
        if(res.Positive==true)
        {
          SendSeedRequestRequest();
        }
        else if(res.ResponseCode==0x7E)
        {
          ext_or_pro = 1;
          PerformSecurityAccess();
        }
        else
        {
          lWriteStatus.Text = "Switching to extended session failed!";
          lWriteStatus.BackColor = System.Drawing.Color.Red; 
        }
      };

      Invoke(LabelUpdate);    
    }   
    
    private void RequestProgrammingSession_OnResponse(Object response)
    {
      Diagnostic.DiagnosticResponse res = (Diagnostic.DiagnosticResponse)response;
    
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Programming Session: Response Received!";      
        lCOMStatus.BackColor = System.Drawing.SystemColors.Control;
      
        if(res.Positive==true)
        {
          PerformSecurityAccess();
        }
        else
        {
          lWriteStatus.Text = "Request Programming Session failed!";
          lWriteStatus.BackColor = System.Drawing.Color.Red; 
        }
      };

      Invoke(LabelUpdate);    
    }    
    
    private void RequestReadVariantCoding_OnResponse(Object response)
    {
      Diagnostic.DiagnosticResponse res = (Diagnostic.DiagnosticResponse)response;
    
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Read Variant Coding Data: Response Received!";      
        lCOMStatus.BackColor = System.Drawing.SystemColors.Control;
      
        if(res.Positive==true)
        {  
          // set country variant bits depending on what country name was received
          mVCCountryType = System.Convert.ToByte(res.GetParameter("Codingstring.CountryType", Diagnostic.eValueType.cNumericValue));
          // set vehicle type bits depending on what vehicle type was received
          mVCVehicleType = System.Convert.ToByte(res.GetParameter("Codingstring.VehicleType", Diagnostic.eValueType.cNumericValue));
          // set special settings bits
          mVCSpecial = System.Convert.ToByte(res.GetParameter("Codingstring.SpecialAdjustment", Diagnostic.eValueType.cNumericValue));
                
          //assign recieved data to the correspondending GUI Elements
          AssignDataToGui();
          lWriteStatus.Text = "Reading succeeded!";
          lWriteStatus.BackColor = System.Drawing.Color.Lime;
        }
        else
        {
          lWriteStatus.Text = "Request Programming Session failed!";
          lWriteStatus.BackColor = System.Drawing.Color.Red; 
        }
      };

      SendDefaultSessionRequest();
      
      Invoke(LabelUpdate);    
    } 

    private void RequestSeedRequest_OnResponse(Object response)
    {
      Diagnostic.DiagnosticResponse res = (Diagnostic.DiagnosticResponse)response;
    
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Seed Request: Response Received!";      
        lCOMStatus.BackColor = System.Drawing.SystemColors.Control;
      
        if(res.Positive==true)
        {
          long seed, key;

          seed = System.Convert.ToInt32(res.GetParameter("SecuritySeed", Diagnostic.eValueType.cNumericValue));

          key = CalculateKey(seed);
          key = key & 65535;

          lWriteStatus.Text = "Seed " + String.Format("{0:X2}", (uint)System.Convert.ToUInt32(seed.ToString())) + " key " + String.Format("{0:X2}", (uint)System.Convert.ToUInt32(key.ToString()));
          SendSendKeyRequest(key);
        }
        else
        {
          lWriteStatus.Text = "Seed Request failed!";
          lWriteStatus.BackColor = System.Drawing.Color.Red; 
        }
      };
      Invoke(LabelUpdate);    
    }  

    private void RequestSendKey_OnResponse(Object response)
    {
      Diagnostic.DiagnosticResponse res = (Diagnostic.DiagnosticResponse)response;
    
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Send Key: Response Received!";      
        lCOMStatus.BackColor = System.Drawing.SystemColors.Control;
      
        if(res.Positive==true)
        {
          if(ext_or_pro == 0)
          {
            ext_or_pro = 1;
            SendProgrammingSessionRequest();
          }
          else
          {
            ext_or_pro = 0;
            
            if(read_or_write == 0)
            {
              SendReadVariantCodingRequest();
            }
            else
            {
              SendWriteVariantCodingRequest();
            }
          }
        }
        else
        {
          lWriteStatus.Text = "Send Key failed!";
          lWriteStatus.BackColor = System.Drawing.Color.Red; 
          SendDefaultSessionRequest();
        }
      };
      Invoke(LabelUpdate);    
    } 

    private void RequestWriteVariantCoding_OnResponse(Object response)
    {
      Diagnostic.DiagnosticResponse res = (Diagnostic.DiagnosticResponse)response;
    
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Write Variant Coding Data: Response Received!";      
        lCOMStatus.BackColor = System.Drawing.SystemColors.Control;
      
        if(res.Positive==true)
        {
          lWriteStatus.Text = "Writing succeeded!";
          lWriteStatus.BackColor = System.Drawing.Color.Lime; 
        }
        else
        {
          lWriteStatus.Text = "Writing failed!";
          lWriteStatus.BackColor = System.Drawing.Color.Red; 
        }
      };
      
      SendDefaultSessionRequest();
      
      Invoke(LabelUpdate);    
    }

    private void RequestDefaultSession_OnTimeout()
    {
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Default Session: Timeout!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      };

      Invoke(LabelUpdate);
    }
     
    private void RequestExtendedSession_OnTimeout()
    {
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Extended Session: Timeout!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      };

      Invoke(LabelUpdate);
    }   

    private void RequestProgrammingSession_OnTimeout()
    {
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Programming Session: Timeout!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      };

      Invoke(LabelUpdate);
    }  
    
    private void RequestSeedRequest_OnTimeout()
    {
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Seed Request: Timeout!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      };

      Invoke(LabelUpdate);
    }
    
    private void RequestSendKey_OnTimeout()
    {
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Send Key: Timeout!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      };

      Invoke(LabelUpdate);
    }

    private void RequestWriteVariantCoding_OnTimeout()
    {
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Write Variant Coding: Timeout!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      };

      Invoke(LabelUpdate);
    }

    private void RequestReadVariantCoding_OnTimeout()
    {
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request Read Variant Coding: Timeout!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      };

      Invoke(LabelUpdate);
    }

    private void RequestEcuReset_OnTimeout()
    {
      MethodInvoker LabelUpdate = delegate
      {
        lCOMStatus.Text = "Request ECU Reset: Timeout!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
      };

      Invoke(LabelUpdate);
    }
    #endregion

    #region "*************************** General functions ****************************"
    //creates a request from a given request string
    private Diagnostic.DiagnosticRequest GetRequest(string aQualPath, string aEcuQual, string aBusName)
    {
      try
      {
        if (mCANoe == null)
        {
          mCANoe = new CANoe.Application();
        }

        //get Measurement instance
        if (mMeasurement == null)
        { 
          mMeasurement = (CANoe.Measurement)mCANoe.Measurement;
          //create on event handles
          mMeasurement.OnInit += new CANoe._IMeasurementEvents_OnInitEventHandler(Measurement_OnInit);
        }

        //is measurement running?
        if (mMeasurement.Running == false)
        {
          lCOMStatus.Text = "Measurement not running!";
          lCOMStatus.BackColor = System.Drawing.Color.Yellow;      
          return null;
        }

        //get the diagnostics interface
        if (gDiagnostic == null || gLastEcuQual != aEcuQual)
        {
          gLastEcuQual = aEcuQual;
          
          //Diagnostic.Diagnostic diag;
          CANoe.Network network;
          CANoe.Device device;
          CANoe.Devices devices;

          gDiagnostic = null;

          network = (CANoe.Network)mCANoe.get_Networks(aBusName);

          if (network != null)
          {
            devices = (CANoe.Devices)network.Devices;
            device = (CANoe.Device)devices[mECUName];

            if (device != null)
            {
              gDiagnostic = (Diagnostic.Diagnostic)device.Diagnostic;
            }
          }
        }
        return (Diagnostic.DiagnosticRequest)gDiagnostic.CreateRequest(aQualPath);          
      }
      catch (Exception ex)
      {
        ShowError(ex);
        return null;
      }
    }

    // ----------------------------------
    // ----- send request functions -----
    // ----------------------------------
    void SendProgrammingSessionRequest()
    {
      mRequestProgrammingSession = GetRequest(mProgrammingSessionRequestString, mECUName, mBusName);
      
      if( mRequestProgrammingSession != null)
      {
        mRequestProgrammingSession.OnResponse += new Diagnostic._IDiagnosticRequestEvents_OnResponseEventHandler(RequestProgrammingSession_OnResponse);
        mRequestProgrammingSession.OnTimeout += new Diagnostic._IDiagnosticRequestEvents_OnTimeoutEventHandler(RequestProgrammingSession_OnTimeout);
     
        mRequestProgrammingSession.Send(true);
      }
    }

    void SendDefaultSessionRequest()
    {
      mRequestDefaultSession = GetRequest(mDefaultSessionRequestString, mECUName, mBusName);

      if (mRequestDefaultSession != null)
      {
        mRequestDefaultSession.OnResponse += new Diagnostic._IDiagnosticRequestEvents_OnResponseEventHandler(RequestDefaultSession_OnResponse);
        mRequestDefaultSession.OnTimeout += new Diagnostic._IDiagnosticRequestEvents_OnTimeoutEventHandler(RequestDefaultSession_OnTimeout);

        mRequestDefaultSession.Send(true);
      }
    }

    void SendExtendedSessionRequest()
    {
      mRequestExtendedSession = GetRequest(mExtendedSessionRequestString, mECUName, mBusName);

      if (mRequestExtendedSession != null)
      {
        mRequestExtendedSession.OnResponse += new Diagnostic._IDiagnosticRequestEvents_OnResponseEventHandler(RequestExtendedSession_OnResponse);
        mRequestExtendedSession.OnTimeout += new Diagnostic._IDiagnosticRequestEvents_OnTimeoutEventHandler(RequestExtendedSession_OnTimeout);

        mRequestExtendedSession.Send(true);
      }
    }

    void SendSeedRequestRequest()
    {
      mRequestSeedRequest = GetRequest(mSeedRequestRequestString, mECUName, mBusName);

      if (mRequestSeedRequest != null)
      {
        mRequestSeedRequest.OnResponse += new Diagnostic._IDiagnosticRequestEvents_OnResponseEventHandler(RequestSeedRequest_OnResponse);
        mRequestSeedRequest.OnTimeout += new Diagnostic._IDiagnosticRequestEvents_OnTimeoutEventHandler(RequestSeedRequest_OnTimeout);

        mRequestSeedRequest.Send(true);
      }
    }

    void SendSendKeyRequest(long aKey)
    {
      mRequestSendKey = GetRequest(mSendKeyRequestString, mECUName, mBusName);

      if (mRequestSendKey != null)
      {
        mRequestSendKey.OnResponse += new Diagnostic._IDiagnosticRequestEvents_OnResponseEventHandler(RequestSendKey_OnResponse);
        mRequestSendKey.OnTimeout += new Diagnostic._IDiagnosticRequestEvents_OnTimeoutEventHandler(RequestSendKey_OnTimeout);

        mRequestSendKey.SetParameter("SecurityKey", aKey);
        mRequestSendKey.Send(true);
      }
    }

    void SendWriteVariantCodingRequest()
    {
      mRequestWriteVariantCoding = GetRequest(mWriteVariantCodingDataRequestString, mECUName, mBusName);

      if (mRequestWriteVariantCoding != null)
      {
        if (AssignDataToRequest() == true)
        {
          mRequestWriteVariantCoding.OnResponse += new Diagnostic._IDiagnosticRequestEvents_OnResponseEventHandler(RequestWriteVariantCoding_OnResponse);
          mRequestWriteVariantCoding.OnTimeout += new Diagnostic._IDiagnosticRequestEvents_OnTimeoutEventHandler(RequestWriteVariantCoding_OnTimeout);

          mRequestWriteVariantCoding.Send(true);
        }
        else
        {
          lWriteStatus.Text = "Creation of Request failed!";
          lWriteStatus.BackColor = System.Drawing.Color.Red;
        }
      }
    }

    void SendReadVariantCodingRequest()
    {
      mRequestReadVariantCoding = GetRequest(mReadVariantCodingDataRequestString, mECUName, mBusName);

      if (mRequestReadVariantCoding != null)
      {
        mRequestReadVariantCoding.OnResponse += new Diagnostic._IDiagnosticRequestEvents_OnResponseEventHandler(RequestReadVariantCoding_OnResponse);
        mRequestReadVariantCoding.OnTimeout += new Diagnostic._IDiagnosticRequestEvents_OnTimeoutEventHandler(RequestReadVariantCoding_OnTimeout);

        mRequestReadVariantCoding.Send(true);
      }
    }

    void PerformSecurityAccess()
    {
      SendSeedRequestRequest();
    }

    long CalculateKey(long aSeed)
    {
      // Simple number sequence for demonstration
      aSeed = aSeed + 1;
      aSeed = -aSeed;
      return aSeed;
    }
        
    bool AssignDataToRequest()
    {
      try
      {
        //first check the validity of the values
        byte specialSettings;
        specialSettings = System.Convert.ToByte(txtSpecialSettings.Text);
        
        if(specialSettings < 0 || specialSettings > 255)
        {
          lCOMStatus.Text = "Invalid Parameter for \"Special Settings\": Please give a value between 0 to 255!";
          lCOMStatus.BackColor = System.Drawing.Color.Red; 
          txtSpecialSettings.Select();
          return false;
          
        }
        mVCSpecial = specialSettings;

        mRequestWriteVariantCoding.SetParameter("Codingstring.CountryType", mVCCountryType);
        mRequestWriteVariantCoding.SetParameter("Codingstring.VehicleType", mVCVehicleType);
        mRequestWriteVariantCoding.SetParameter("Codingstring.SpecialAdjustment", mVCSpecial);
        return true;
      }
      catch
      {
        lCOMStatus.Text = "Invalid Parameters!";
        lCOMStatus.BackColor = System.Drawing.Color.Red; 
        return false;
      }
    }
        
    // assign the elements from to data variable to the right GUI Element
    void AssignDataToGui()
    {
       txtSpecialSettings.Text = mVCSpecial.ToString();
       cobCountryVariant.SelectedIndex = mVCCountryType;
       cobVehicleType.SelectedIndex = mVCVehicleType;
    }

    void ShowError(Exception ex)
    {
      string msg;
      msg = "The object returned the following error: \r\r" + "Source: " + ex.Source + "\r" + "Description: " + ex.Message;
      MessageBox.Show(msg, "Fehler", MessageBoxButtons.OK, MessageBoxIcon.Error);
    }

    private void cobCountryVariant_Leave(object sender, EventArgs e)
    {
      try
      {
        mVCCountryType = (byte)cobCountryVariant.SelectedIndex;
        lCOMStatus.Text = "Country=" +  mVCCountryType.ToString();
      }
      catch
      {
        lCOMStatus.Text = "Invalid Parameter: Please get a valid value from the list!";
        lCOMStatus.BackColor = System.Drawing.Color.Red;       
      }
    }

    private void cobVehicleType_Leave(object sender, EventArgs e)
    {
      try
      {
        mVCVehicleType = (byte)cobVehicleType.SelectedIndex;
        lCOMStatus.Text = "VehicleType=" +  mVCVehicleType.ToString();
      }
      catch
      {
        lCOMStatus.Text = "Invalid Parameter: Please get a valid value from the list!";
        lCOMStatus.BackColor = System.Drawing.Color.Red;       
      }
    }
    #endregion

    private void btnResetCodingData_Click(object sender, EventArgs e)
    {
      mVCVehicleType = 0;
      mVCCountryType = 0;
      mVCSpecial = 0;
      AssignDataToGui();
    }
  }
}