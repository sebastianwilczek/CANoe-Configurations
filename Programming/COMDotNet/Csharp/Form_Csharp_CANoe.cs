using System;
using System.Windows.Forms;
using System.IO;

namespace COM_Csharp_CANoe_Demo
{
    public partial class Form_Csharp_CANoe : Form
    {
        #region Declarations
        // Literals
        private const String mRelativeConfigPath = @"..\..\..\COM.Net\COM.Net.cfg";
        // Fields
        private String mAbsoluteConfigPath;
        private ToolTip mConfigDirToolTip = new ToolTip();
        private bool mCANoeInstanceRunning;
        // Delegates
        private delegate void DelSafeInvocation();
        private delegate void DelSafeInvocationWithParam(String sysvarName, object value);
        // CANoe objects
        private CANoe.Application mCANoeApp;
        private CANoe.Measurement mCANoeMeasurement;
        // CANoe CAPL functions
        private CANoe.CAPLFunction mCANoeMultiply;
        // CANoe system variables
        private CANoe.Variable mCANoeSysVar1;
        private CANoe.Variable mCANoeSysVar2;
        // CANoe signals
        private CANoe.Signal mCANoeEngineStatus;
        private CANoe.Signal mCANoeEngineSpeed;
        private CANoe.Signal mCANoeEngineTemp;
        #endregion

        #region Construction & Initialization
        public Form_Csharp_CANoe()
        {
            InitializeComponent();

            // Check if the necessary configuration is at the correct location.
            PrepareOpenConfiguration();                        
        }                    
        #endregion

        #region Opening Tasks
        /// <summary>
        /// Shows the expected directory of the related configuration file and checks if it's located there.
        /// </summary>
        private void PrepareOpenConfiguration()
        {
            // Creates an absolute path out of the provided relative path.
            mAbsoluteConfigPath = Path.GetFullPath(mRelativeConfigPath);

            mTextboxConfigDir.Text = mAbsoluteConfigPath;

            // Sets the selection after the last character in the textbox.
            mTextboxConfigDir.SelectionStart = mTextboxConfigDir.Text.Length;

            if (File.Exists(mAbsoluteConfigPath))
            {
                mLabelDirCheck.Text = Properties.Resources.ConfigFileFound;
                mButtonOpenConfiguration.Enabled = true;
            }
            else
            {
                mLabelDirCheck.Text = Properties.Resources.ConfigFileNotFound;
            }
        }

        /// <summary>
        /// Occurs when a configuration was successfully opened.
        /// </summary>
        private void ConfigurationOpened()
        {
            try
            {
                // Assign system variables from namespaces.
                CANoe.System CANoeSystem = (CANoe.System)mCANoeApp.System;
                CANoe.Namespaces CANoeNamespaces = (CANoe.Namespaces)CANoeSystem.Namespaces;
                CANoe.Namespace CANoeNamespaceGeneral = (CANoe.Namespace)CANoeNamespaces["General"];
                CANoe.Variables CANoeVariablesGeneral = (CANoe.Variables)CANoeNamespaceGeneral.Variables;
                mCANoeSysVar1 = (CANoe.Variable)CANoeVariablesGeneral["SysVar1"];
                mCANoeSysVar2 = (CANoe.Variable)CANoeVariablesGeneral["SysVar2"];

                // Assign signals.
                CANoe.Bus CANoeBus = (CANoe.Bus)mCANoeApp.get_Bus("CAN");
                mCANoeEngineStatus = (CANoe.Signal)CANoeBus.GetSignal(1, "EngineData", "EngineStatus");
                mCANoeEngineSpeed = (CANoe.Signal)CANoeBus.GetSignal(1, "EngineData", "EngineSpeed");
                mCANoeEngineTemp = (CANoe.Signal)CANoeBus.GetSignal(1, "EngineData", "EngineTemp");
            }
            catch (System.Exception)
            {
                MessageBox.Show("Possible cause: Wrong namespace names, bus name, system variable names or signal names in source code or configuration.",
                    "Error while assigning system variables and signals", MessageBoxButtons.OK, MessageBoxIcon.Error);

                return;
            }

            // Enables the start/stop measurement button.
            mGroupboxConfigSettings.Enabled = false;
            mGroupboxMeasurementControl.Enabled = true;

            mButtonStartStop.Focus();
            
            if (mCANoeApp != null)
            {
                // Wire OnQuit event handler.
                mCANoeApp.OnQuit += new CANoe._IApplicationEvents_OnQuitEventHandler(CANoeQuit);
            }

            if (mCANoeMeasurement != null)
            {
                // Create on event handlers.
                mCANoeMeasurement.OnInit += new CANoe._IMeasurementEvents_OnInitEventHandler(MeasurementInitiated);
                mCANoeMeasurement.OnExit += new CANoe._IMeasurementEvents_OnExitEventHandler(MeasurementExited);
            }            

            // Indicate that an instance of CANoe is running.
            mCANoeInstanceRunning = true;
        }
        #endregion

        #region Measurement Init/Exit
        /// <summary>
        /// Occurs when the measurement has been initiated.
        /// </summary>
        private void MeasurementInitiated()
        {
            // Compile CAPL code of the CANoe configuration.
            CANoe.CAPL CANoeCAPL = (CANoe.CAPL)mCANoeApp.CAPL;
            if (CANoeCAPL != null)
            {
                CANoeCAPL.Compile(null);
            }

            try
            {
                // Assign CAPL functions.
                mCANoeMultiply = (CANoe.CAPLFunction)CANoeCAPL.GetFunction("Multiply");
            }
            catch (System.Exception)
            {
                MessageBox.Show("Possible cause: Wrong CAPL function name in source code or configuration.",
                    "Error while assigning CAPL functions", MessageBoxButtons.OK, MessageBoxIcon.Error);

                return;
            }

            // Pay attention to thread safety, as the accessed controls were created in the GUI thread!
            DelSafeInvocation safeinvocation = new DelSafeInvocation(MeasurementInitiatedInternal);
            this.Invoke(safeinvocation);

            // Create system variable on event handlers.
            if (mCANoeSysVar1 != null)
            {
                mCANoeSysVar1.OnChange += new CANoe._IVariableEvents_OnChangeEventHandler(SysVar1Changed);
            }

            if (mCANoeSysVar2 != null)
            {
                mCANoeSysVar2.OnChange += new CANoe._IVariableEvents_OnChangeEventHandler(SysVar2Changed);
            }
        }

        /// <summary>
        /// Used for thread safe call to manipulate GUI controls when measurement has been initiated.
        /// </summary>
        private void MeasurementInitiatedInternal()
        {
            mGroupboxCAPLWriteWindowOut.Enabled = true;
            mGroupboxSystemVariables.Enabled = true;
            mGroupboxSignalValues.Enabled = true;
            mButtonStartStop.Text = Properties.Resources.StopMeasurement;
            mLabelMeasurementStatus.Text = Properties.Resources.StatusMeasurementStarted;

            // SysVar1 is chosen by default at startup.
            mProgressbarSysVars.Value = mCANoeSysVar1.Value;
            mLabelSysVarsValue.Text = mCANoeSysVar1.Value.ToString();
            mLabelSysVarMin.Text = mCANoeSysVar1.MinValue.ToString();
            mLabelSysVarMax.Text = mCANoeSysVar1.MaxValue.ToString();

            //Activate polling of signal values.
            mTimerPolling.Interval = (int)mNumericupdownCycleTime.Value;
            mTimerPolling.Tick += new EventHandler(PollSignalValues);
            mTimerPolling.Start();            
        }                
        
        /// <summary>
        /// Occurs when the measurement has exited.
        /// </summary>
        private void MeasurementExited()
        {
            // Pay attention to thread safety, as the accessed controls were created in the GUI thread!
            DelSafeInvocation safeinvocation = new DelSafeInvocation(MeasurementExitedInternal);
            this.Invoke(safeinvocation);

            // Unregister system variables on event handlers.
            if (mCANoeSysVar1 != null)
            {
                mCANoeSysVar1.OnChange -= new CANoe._IVariableEvents_OnChangeEventHandler(SysVar1Changed);
            }

            if (mCANoeSysVar2 != null)
            {
                mCANoeSysVar2.OnChange -= new CANoe._IVariableEvents_OnChangeEventHandler(SysVar2Changed);
            }
        }

        /// <summary>
        /// Used for thread safe call to manipulate GUI controls when measurement has been exited.
        /// </summary>
        private void MeasurementExitedInternal()
        {
            mGroupboxCAPLWriteWindowOut.Enabled = false;
            mGroupboxSystemVariables.Enabled = false;
            mGroupboxSignalValues.Enabled = false;
            mButtonStartStop.Text = Properties.Resources.StartMeasurement;
            mComboboxSysVars.SelectedIndex = 0;
            mComboboxEngineStatusIn.SelectedIndex = 0;
            mLabelMeasurementStatus.Text = Properties.Resources.StatusMeasurementStopped;
            mLabelEngTempOut.Text = "0";
            mLabelSysVarsValue.Text = "0";
            mTextboxOperand2.Clear();
            mTextboxEngineSpeedIn.Text = "0";
            mTextboxEngSpeedOut.Text = "0";
            mTextboxEngStatusOut.Text = "Stopped";
            mTextboxOperand2.Clear();
            mTextboxOperand1.Clear();
            mTextboxOperationResult.Clear();
            mProgressbarSysVars.Value = 0;
            mProgressbarEngTempOut.Value = 0;
            mTrackbarSysVars.Value = 0;
            mTrackbarEngineTempIn.Value = 0;

            // Deactivate polling of signal values.
            mTimerPolling.Stop();
            mTimerPolling.Tick -= new EventHandler(PollSignalValues);
        }
        #endregion

        #region SysVar and Signal values
        /// <summary>
        ///  Occurs when the polling timer elapses.
        /// </summary>
        private void PollSignalValues(object sender, EventArgs e)
        {
            // Display the current signal values in the output controls.            
            mTextboxEngSpeedOut.Text = mCANoeEngineSpeed.Value.ToString();
            mTextboxEngStatusOut.Text = (mCANoeEngineStatus.Value == 1) ? "Running" : "Stopped";
            mProgressbarEngTempOut.Value = (int)mCANoeEngineTemp.Value;
            mLabelEngTempOut.Text = mCANoeEngineTemp.Value.ToString();
        }        

        /// <summary>
        /// Occurs when the value of a particular system variable changes.
        /// </summary>
        /// <param name="value">New value</param>
        private void SysVar1Changed(object value)
        {
            // Pay attention to thread safety, as the accessed controls were created in the GUI thread!
            DelSafeInvocationWithParam safeinvocationwithparam = new DelSafeInvocationWithParam(SysVarsChangedInternal);
            this.Invoke(safeinvocationwithparam, "SysVar1", value);
        }

        /// <summary>
        /// Occurs when the value of a particular system variable changes.
        /// </summary>
        /// <param name="value">New value</param>
        private void SysVar2Changed(object value)
        {
            // Pay attention to thread safety, as the accessed controls were created in the GUI thread!
            DelSafeInvocationWithParam safeinvocationwithparam = new DelSafeInvocationWithParam(SysVarsChangedInternal);
            this.Invoke(safeinvocationwithparam, "SysVar2", value);
        }

        /// <summary>
        /// Used for thread safe call to manipulate GUI controls when the value of a system variable has changed.
        /// </summary>
        private void SysVarsChangedInternal(String sysvarName, object value)
        {
            if (mCANoeMeasurement != null && mCANoeMeasurement.Running)
            {
                // Set new value of the currently selected system variable.
                if (mComboboxSysVars.Text.Equals(sysvarName))
                {
                    mProgressbarSysVars.Value = (int)value;
                    mLabelSysVarsValue.Text = value.ToString();
                }
            }
        }
        #endregion

        #region Closing Tasks
        /// <summary>
        /// Occurs when CANoe has quit.
        /// </summary>
        private void CANoeQuit()
        {
            // Pay attention to thread safety, as the accessed controls were created in the GUI thread!
            DelSafeInvocation safeinvocation = new DelSafeInvocation(CANoeQuitInternal);
            this.Invoke(safeinvocation);

            UnregisterCANoeEventHandlers();

            // Indicate that the instance of CANoe was closed.
            mCANoeInstanceRunning = false;
        }

        /// <summary>
        /// Used for thread safe call to manipulate GUI controls when CANoe has quit.
        /// </summary>
        private void CANoeQuitInternal()
        {
            // Disables the start/stop measurement button.
            mGroupboxMeasurementControl.Enabled = false;

            // Set the correct caption of the start/stop button.
            mButtonStartStop.Text = Properties.Resources.StartMeasurement;

            // Set the correct caption of the status label.
            mLabelMeasurementStatus.Text = Properties.Resources.StatusMeasurementStopped;

            // Disable the measurement related controls.
            MeasurementExited();
        }

        /// <summary>
        /// Releases all wired event handlers.
        /// </summary>
        private void UnregisterCANoeEventHandlers()
        {
            if (mCANoeApp != null)
            {
                mCANoeApp.OnQuit -= new CANoe._IApplicationEvents_OnQuitEventHandler(CANoeQuit);
            }

            if (mCANoeMeasurement != null)
            {
                mCANoeMeasurement.OnInit -= new CANoe._IMeasurementEvents_OnInitEventHandler(MeasurementInitiated);       
                mCANoeMeasurement.OnExit -= new CANoe._IMeasurementEvents_OnExitEventHandler(MeasurementExited);
            }            
        }
        #endregion

        #region .Net Control EventHandlers
        /// <summary>
        ///  Occurs when the user clicks the button to open the configuration.
        /// </summary>
        private void mButtonOpenConfiguration_Click(object sender, EventArgs e)
        {
            // Init new CANoe application.
            mCANoeApp = new CANoe.Application();

            // Init measurement object.
            mCANoeMeasurement = (CANoe.Measurement)mCANoeApp.Measurement;

            // Stopps a running measurement.
            if (mCANoeMeasurement.Running)
            {
                mCANoeMeasurement.Stop();
            }
       
            if (mCANoeApp != null)
            {                              
                // Open the demo configuration.
                mCANoeApp.Open(mAbsoluteConfigPath, true, true);

                // Make sure the configuration was successfully loaded.
                CANoe.OpenConfigurationResult ocresult = mCANoeApp.Configuration.OpenConfigurationResult;
                if (ocresult.result == 0)
                {
                    ConfigurationOpened();
                }
            }
        }

        /// <summary>
        /// Occurs when the user clicks the button to start or stop a measurement.
        /// </summary>
        private void mButtonStartStop_Click(object sender, EventArgs e)
        {
            if (mCANoeMeasurement != null)
            {
                if (mCANoeMeasurement.Running)
                {
                    mCANoeMeasurement.Stop();
                }
                else
                {
                    mCANoeMeasurement.Start();
                }
            }
        }

        /// <summary>
        /// Occurs when the user wants to calculate the multiplication result.
        /// </summary>
        private void mButtonCalculate_Click(object sender, EventArgs e)
        {
            // Call "Multiply" CAPL function and pass parameters, also get return value.
            if (mCANoeMultiply != null)
            {
                int operand1, operand2;
                Int32.TryParse(mTextboxOperand1.Text, out operand1);
                Int32.TryParse(mTextboxOperand2.Text, out operand2);

                // Return from a CAPL function works in evaluation branch of measurement set up only.
                int result = (int)mCANoeMultiply.Call(operand1, operand2);

                mTextboxOperationResult.Text = result.ToString();
            }
        }

        /// <summary>
        /// Occurs when the mouse pointer hovers over the textbox.
        /// </summary>
        private void mTextboxConfigDir_MouseHover(object sender, EventArgs e)
        {
            // Show tooltip with the absolute configuration path.
            mConfigDirToolTip.Show(mAbsoluteConfigPath, mTextboxConfigDir);            
        }

        /// <summary>
        /// Occurs when the textbox lost focus.
        /// </summary>
        private void mTextboxEngSpeedIn_Leave(object sender, EventArgs e)
        {
            // Set new value of signal EngineSpeed.
            double newEngineSpeed;
            if (Double.TryParse(mTextboxEngineSpeedIn.Text, out newEngineSpeed))
            {
                mCANoeEngineSpeed.Value = newEngineSpeed;
            }
        }

        /// <summary>
        /// Occurs when a key was pressed.
        /// </summary>
        private void mTextboxEngSpeedIn_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Return)
            {
                // Set new value of signal EngineSpeed.
                double newEngineSpeed;
                if (Double.TryParse(mTextboxEngineSpeedIn.Text, out newEngineSpeed))
                {
                    mCANoeEngineSpeed.Value = newEngineSpeed;
                }
            }
        }        

        /// <summary>
        /// Occurs when the selection of the combobox has changed.
        /// </summary>
        private void mComboboxSysVars_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Sets the content of controls depending on the selected entry.
            switch (mComboboxSysVars.Text)
            {
                case "SysVar1":
                    mProgressbarSysVars.Value = mCANoeSysVar1.Value;
                    mLabelSysVarsValue.Text = mCANoeSysVar1.Value.ToString();
                    mLabelSysVarMin.Text = mCANoeSysVar1.MinValue.ToString();
                    mLabelSysVarMax.Text = mCANoeSysVar1.MaxValue.ToString();

                    break;

                case "SysVar2":
                    mProgressbarSysVars.Value = mCANoeSysVar2.Value;
                    mLabelSysVarsValue.Text = mCANoeSysVar2.Value.ToString();
                    mLabelSysVarMin.Text = mCANoeSysVar2.MinValue.ToString();
                    mLabelSysVarMax.Text = mCANoeSysVar2.MaxValue.ToString();

                    break;

                default:
                    break;
            }

            mTrackbarSysVars.Value = 0;
        }

        /// <summary>
        /// Occurs when the selection of the combobox has changed.
        /// </summary>
        private void mComboboxEngStatusIn_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Set new value of signal EngineStatus.
            switch (mComboboxEngineStatusIn.Text)
            {
                case "Running":
                    mCANoeEngineStatus.Value = 1;

                    break;

                case "Stopped":
                    mCANoeEngineStatus.Value = 0;

                    break;

                default:
                    break;
            }
        }

        /// <summary>
        /// Occurs when the slider of the trackbar is scrolled.
        /// </summary>
        private void mTrackbarSysVars_Scroll(object sender, EventArgs e)
        {
            // Communicates new SysVarX value to CANoe.
            switch (mComboboxSysVars.Text)
            {
                case "SysVar1":
                    mCANoeSysVar1.Value = mTrackbarSysVars.Value;

                    break;

                case "SysVar2":
                    mCANoeSysVar2.Value = mTrackbarSysVars.Value;

                    break;

                default:
                    break;
            }                       
        }

        /// <summary>
        /// Occurs when the slider of the trackbar is scrolled.
        /// </summary>
        private void mTrackbarEngTempIn_Scroll(object sender, EventArgs e)
        {
            // Set new value of signal EngineTemp.
            mCANoeEngineTemp.Value = mTrackbarEngineTempIn.Value;
        }

        /// <summary>
        /// Occurs when the update cycle time is intended to be changed.
        /// </summary>
        private void mNumericupdownCycleTime_ValueChanged(object sender, EventArgs e)
        {
            // Set a new update cycle time.
            mTimerPolling.Interval = (int)mNumericupdownCycleTime.Value;
        }

        /// <summary>
        /// Occurs when the form is closing.
        /// </summary>
        private void Form_Csharp_CANoe_FormClosing(object sender, FormClosingEventArgs e)
        {
            UnregisterCANoeEventHandlers();

            // If an instance of CANoe is running and a measurement is running assure that it is stopped before the demo closes.
            if (mCANoeInstanceRunning && mCANoeMeasurement != null && mCANoeMeasurement.Running)
            {
                mCANoeMeasurement.Stop();
            }                        
        }        
        #endregion
    }
}