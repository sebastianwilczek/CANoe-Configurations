/*-----------------------------------------------------------------------------
Vector.CANoe.MOST COM Demo C#
-------------------------------------------------------------------------------
NOTES
This class demonstrates how to connect a client GUI to the CANoe COM server.

SPECIAL ATTENTION should be put on the methods
- MainForm_Load()
- MainForm_FormClosing()
- CANoeAccess_DeckStatusReceived()
- buttonOperationGet_Click()
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/

using System;
using System.Windows.Forms;

namespace MostComClientCS
{
  public partial class MainForm : Form
  {
    #region -------------------- Members --------------------

    private CANoeAccess mCANoeAccess;

    private bool mPerformingInternalUpdate = false;

    #endregion

    #region -------------------- Construction / Initialization / Cleanup --------------------

    public MainForm()
    {
      InitializeComponent();

      checkBoxMeasurement.Enabled = false;
      buttonOperationGet.Enabled = false;
    }

    private void MainForm_Load( object sender, EventArgs e )
    {
      // Create CANoeAccess object
      mCANoeAccess = new CANoeAccess();

      // Register for events of CANoeAccess object
      mCANoeAccess.DeckStatusReceived += new EventHandler( CANoeAccess_DeckStatusReceived );
    }

    private void MainForm_FormClosing( object sender, FormClosingEventArgs e )
    {
      // Unregister from events of CANoeAccess object
      mCANoeAccess.DeckStatusReceived -= new EventHandler( CANoeAccess_DeckStatusReceived );

      // Release COM interfaces and perform cleanup in CANoeAccess object
      mCANoeAccess.Dispose();
      mCANoeAccess = null;
    }

    #endregion

    #region -------------------- Events from CANoeAccess --------------------

    private void CANoeAccess_DeckStatusReceived( object sender, EventArgs e )
    {
      // Use Invoke() function to switch execution context from CANoe to this client.
      // This is required, because CANoe must not access the client's GUI.
      Invoke( new EventHandler( UpdateDeckStatus ), this, new EventArgs() );
    }

    private void UpdateDeckStatus( object sender, EventArgs e )
    {
      // Display the received values
      textBoxSymbolicValue.Text = mCANoeAccess.DeckStatusSymbolicValue;
      textBoxNumericValue.Text = String.Format( "0x{0}", mCANoeAccess.DeckStatusNumericValue.ToString( "X2" ) );
    }

    #endregion

    #region -------------------- User Interaction --------------------

    private void checkBoxConnect_CheckedChanged( object sender, EventArgs e )
    {
      if( checkBoxConnect.Checked )
      {
        mCANoeAccess.ConnectToCANoe();
        checkBoxConnect.Text = "Disconnect from CANoe";

        mPerformingInternalUpdate = true;
        checkBoxMeasurement.Enabled = true;
        checkBoxMeasurement.Checked = mCANoeAccess.MeasurementRunning;
        mPerformingInternalUpdate = false;
      }
      else
      {
        mPerformingInternalUpdate = true;
        checkBoxMeasurement.Enabled = false;
        checkBoxMeasurement.Checked = false;
        mPerformingInternalUpdate = false;

        mCANoeAccess.DisconnectFromCANoe();
        checkBoxConnect.Text = "Connect to CANoe";
      }
    }

    private void checkBoxMeasurement_CheckedChanged( object sender, EventArgs e )
    {
      if( checkBoxMeasurement.Checked )
      {
        if( !mPerformingInternalUpdate )
          mCANoeAccess.StartMeasurement();
        checkBoxMeasurement.Text = "Stop Measurement";
        buttonOperationGet.Enabled = true;
      }
      else
      {
        buttonOperationGet.Enabled = false;
        checkBoxMeasurement.Text = "Start Measurement";
        if( !mPerformingInternalUpdate )
          mCANoeAccess.StopMeasurement();
      }
    }

    private void buttonOperationGet_Click( object sender, EventArgs e )
    {
      // Tell CANoeAccess object to request the DeckStatus Property
      mCANoeAccess.GetPropertyDeckStatus();
    }

    #endregion
  }
}