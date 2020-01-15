// Created by Brock, Boris (visbbr), 2015-02-23
// Copyright (c) Vector Informatik GmbH. All rights reserved.

#region Usings

using System;
using System.Windows.Forms;
using ASAM.XIL.Implementation.Testbench.Common.ValueContainer;
using ASAM.XIL.Implementation.TestbenchFactory.Testbench;
using ASAM.XIL.Interfaces.Testbench;
using ASAM.XIL.Interfaces.Testbench.Common.ValueContainer;
using ASAM.XIL.Interfaces.Testbench.MAPort;

#endregion

namespace XILAPIClient
{
  internal class XILAPIWrapper
  {
    // MA Port instance
    private IMAPort mMaPort;

    // Test bench instance
    private ITestbench mTestbench;

    /// <summary>
    /// Gets a value indicating whether the MA port is connected.
    /// </summary>
    /// <value>
    /// <c>true</c> if this instance is connected; otherwise, <c>false</c>.
    /// </value>
    public bool IsConnected { get; private set; }

    /// <summary>
    /// Initializes the test bench and the MA port.
    /// </summary>
    /// <param name="vendorName">Name of the vendor.</param>
    /// <param name="productName">The productName.</param>
    /// <param name="productVersion">The productVersion.</param>
    /// <param name="portConfigurationFile">The port configuration file.</param>
    public void Init(string vendorName, string productName, string productVersion, string portConfigurationFile)
    {
      // Create the test bench
      try
      {
        var factory = new TestbenchFactory();
        mTestbench = factory.CreateVendorSpecificTestbench(vendorName, productName, productVersion);
      }
      catch (Exception ex)
      {
        string msg = "Could not create the test bench. Is the ASAM XIL API and CANoe XIL API correctly installed?";
        msg += Environment.NewLine + Environment.NewLine;
        msg += "Error details: " + ex.Message;

        throw new Exception(msg);
      }

      // Create the MA port
      try
      {
        mMaPort = mTestbench.MAPortFactory.CreateMAPort("Example MA Port");
        mMaPort.Configure(mMaPort.LoadConfiguration(portConfigurationFile), false);
        mMaPort.StartSimulation();
      }
      catch (Exception ex)
      {
        string msg = "Could not create the MA port. Reason: ";
        msg += Environment.NewLine + Environment.NewLine;
        msg += "Error details: " + ex.Message;

        throw new Exception(msg);
      }  

      IsConnected = true;
    }

    /// <summary>
    /// Reads the variable.
    /// </summary>
    /// <param name="variableName">Name of the variable.</param>
    /// <returns></returns>
    public double ReadVariable(string variableName)
    {
      var floatValue = mMaPort.Read(variableName) as IFloatValue;
      return floatValue?.Value ?? 0.0;
    }

    /// <summary>
    /// Shutdowns the test bench.
    /// </summary>
    public void Shutdown()
    {
      IsConnected = false;

      mMaPort.StopSimulation();
      mMaPort.Disconnect();
      mMaPort.Dispose();
    }

    /// <summary>
    /// Writes the variable.
    /// </summary>
    /// <param name="variableName">Name of the variable.</param>
    /// <param name="value">The value.</param>
    public void WriteVariable(string variableName, double value)
    {
      mMaPort.Write(variableName, new FloatValue(value));
    }
  }
}