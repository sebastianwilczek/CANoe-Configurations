// Created by Brock, Boris (visbbr), 2015-02-23
// Copyright (c) Vector Informatik GmbH. All rights reserved.

#region Usings

using System;
using System.Windows.Forms;

#endregion

namespace XILAPIClient
{
  public partial class MainForm : Form
  {
    #region Configuration Strings

    // Config file used when using the LightRPC backend
    private const string ConfigMAPortConfigLightRPC = "../../../XIL API Configuration/VectorMAPortConfigLightRPC.xml";

    // Config file used when using the FDX backend
    private const string ConfigMAPortConfigFDX = "../../../XIL API Configuration/VectorMAPortConfigFDX.xml";

    // This variable will be read
    private const string ConfigVariableName1 = "Test::Variable1";

    // This variable will be written
    private const string ConfigVariableName2 = "Test::Variable2";

    #endregion


    #region Members

    private readonly XILAPIWrapper mXil;

    #endregion


    #region Methods

    /// <summary>
    ///   Initializes a new instance of the <see cref="MainForm" /> class.
    /// </summary>
    public MainForm()
    {
      InitializeComponent();
      mXil = new XILAPIWrapper();
    }

    /// <summary>
    /// Called when [load].
    /// </summary>
    /// <param name="sender">The sender.</param>
    /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
    private void OnLoad(object sender, EventArgs e)
    {
      // Initialize combo boxes
      cbxVendor.SelectedIndex = 0;
      cbxProduct.SelectedIndex = Environment.Is64BitProcess ? 1 : 0; // Select proper product based on bitness
      cbxProductVersion.SelectedIndex = 1;
      cbxBackend.SelectedIndex = 0;
    }

    /// <summary>
    ///   Connects to CANoe
    /// </summary>
    /// <param name="sender">The sender.</param>
    /// <param name="e">The <see cref="EventArgs" /> instance containing the event data.</param>
    private void OnBtnConnectClick(object sender, EventArgs e)
    {
      try
      {
        // Initialize the XIL API client with the settings made in the GUI
        bool useLightRPC = cbxBackend.SelectedIndex == 0;
        mXil.Init(
          cbxVendor.SelectedItem.ToString(),
          cbxProduct.SelectedItem.ToString(),
          cbxProductVersion.SelectedItem.ToString(),
          useLightRPC ? ConfigMAPortConfigLightRPC : ConfigMAPortConfigFDX);

        // Start the update timer
        mTimer.Enabled = true;

        // Enable/disable GUI elements
        mBtnConnect.Enabled = false;
        mBtnDisconnect.Enabled = true;
        mBtnWrite.Enabled = true;
        numSendValue.Enabled = true;
      }
      catch (Exception ex)
      {
        MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }

    /// <summary>
    ///   Called when [form closing].
    /// </summary>
    /// <param name="sender">The sender.</param>
    /// <param name="e">The <see cref="FormClosingEventArgs" /> instance containing the event data.</param>
    private void OnFormClosing(object sender, FormClosingEventArgs e)
    {
      if (mXil.IsConnected)
        mXil.Shutdown();
    }

    /// <summary>
    ///   Disconnects from CANoe
    /// </summary>
    /// <param name="sender">The sender.</param>
    /// <param name="e">The <see cref="EventArgs" /> instance containing the event data.</param>
    private void OnMBtnDisconnectClick(object sender, EventArgs e)
    {
      // Disable the update timer
      mTimer.Enabled = false;

      // Shut down the XIL API client instance
      mXil.Shutdown();

      // Enable/disable GUI elements
      mBtnConnect.Enabled = true;
      mBtnDisconnect.Enabled = false;
      mBtnWrite.Enabled = false;
      numSendValue.Enabled = false;
    }

    /// <summary>
    ///   Updates the GUI with current variable values
    /// </summary>
    /// <param name="sender">The sender.</param>
    /// <param name="e">The <see cref="EventArgs" /> instance containing the event data.</param>
    private void OnTimerTick(object sender, EventArgs e)
    {
      // Read the current value from CANoe
      var curValue = mXil.ReadVariable(ConfigVariableName1);

      // Show the value in the GUI
      numCurValue.Value = (decimal) Math.Round(curValue, 2);
      mBarCurValue.Value = (int)curValue;
    }

    /// <summary>
    /// Called when the Quit button is pressed
    /// </summary>
    /// <param name="sender">The sender.</param>
    /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
    private void OnBtnQuitClick(object sender, EventArgs e)
    {
      this.Close();
    }

    /// <summary>
    /// Sends the value from the GUI to CANoe.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnBtnWriteClick(object sender, EventArgs e)
    {
      // Write the value from the GUI to CANoe
      mXil.WriteVariable(ConfigVariableName2, (double)numSendValue.Value);
    }

    #endregion
  }
}