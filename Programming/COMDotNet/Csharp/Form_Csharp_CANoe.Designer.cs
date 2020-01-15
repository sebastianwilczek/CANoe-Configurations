namespace COM_Csharp_CANoe_Demo
{
    partial class Form_Csharp_CANoe
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_Csharp_CANoe));
            this.mGroupboxConfigSettings = new System.Windows.Forms.GroupBox();
            this.mLabelInfoConfig = new System.Windows.Forms.Label();
            this.mButtonOpenConfiguration = new System.Windows.Forms.Button();
            this.mLabelDirCheck = new System.Windows.Forms.Label();
            this.mTextboxConfigDir = new System.Windows.Forms.TextBox();
            this.mGroupboxMeasurementControl = new System.Windows.Forms.GroupBox();
            this.mLabelMeasurementStatus = new System.Windows.Forms.Label();
            this.mButtonStartStop = new System.Windows.Forms.Button();
            this.mGroupboxCAPLWriteWindowOut = new System.Windows.Forms.GroupBox();
            this.mTextboxOperationResult = new System.Windows.Forms.TextBox();
            this.mTextboxOperand1 = new System.Windows.Forms.TextBox();
            this.mLabelEquals = new System.Windows.Forms.Label();
            this.mLabelMultiply = new System.Windows.Forms.Label();
            this.mLabelInfoCAPLFunc = new System.Windows.Forms.Label();
            this.mButtonCalculate = new System.Windows.Forms.Button();
            this.mTextboxOperand2 = new System.Windows.Forms.TextBox();
            this.mGroupboxSignalValues = new System.Windows.Forms.GroupBox();
            this.mLabelEngTempOut = new System.Windows.Forms.Label();
            this.mProgressbarEngTempOut = new System.Windows.Forms.ProgressBar();
            this.mTextboxEngStatusOut = new System.Windows.Forms.TextBox();
            this.mTextboxEngSpeedOut = new System.Windows.Forms.TextBox();
            this.mLabelmsCycle = new System.Windows.Forms.Label();
            this.mNumericupdownCycleTime = new System.Windows.Forms.NumericUpDown();
            this.mLabelCycleTime = new System.Windows.Forms.Label();
            this.mLabelEngTempMax = new System.Windows.Forms.Label();
            this.mLabelEngTempMin = new System.Windows.Forms.Label();
            this.mComboboxEngineStatusIn = new System.Windows.Forms.ComboBox();
            this.mTrackbarEngineTempIn = new System.Windows.Forms.TrackBar();
            this.mTextboxEngineSpeedIn = new System.Windows.Forms.TextBox();
            this.mLabelEngineStatus = new System.Windows.Forms.Label();
            this.mLabelEngineTemp = new System.Windows.Forms.Label();
            this.mLabelEngineSpeed = new System.Windows.Forms.Label();
            this.mLabelInfoSignalValues = new System.Windows.Forms.Label();
            this.mGroupboxSystemVariables = new System.Windows.Forms.GroupBox();
            this.mLabelSysVarsValue = new System.Windows.Forms.Label();
            this.mLabelSysVarMax = new System.Windows.Forms.Label();
            this.mComboboxSysVars = new System.Windows.Forms.ComboBox();
            this.mProgressbarSysVars = new System.Windows.Forms.ProgressBar();
            this.mLabelSysVarMin = new System.Windows.Forms.Label();
            this.mLabelInfoSysVars = new System.Windows.Forms.Label();
            this.mTrackbarSysVars = new System.Windows.Forms.TrackBar();
            this.mTimerPolling = new System.Windows.Forms.Timer(this.components);
            this.mGroupboxConfigSettings.SuspendLayout();
            this.mGroupboxMeasurementControl.SuspendLayout();
            this.mGroupboxCAPLWriteWindowOut.SuspendLayout();
            this.mGroupboxSignalValues.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.mNumericupdownCycleTime)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.mTrackbarEngineTempIn)).BeginInit();
            this.mGroupboxSystemVariables.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.mTrackbarSysVars)).BeginInit();
            this.SuspendLayout();
            // 
            // mGroupboxConfigSettings
            // 
            this.mGroupboxConfigSettings.Controls.Add(this.mLabelInfoConfig);
            this.mGroupboxConfigSettings.Controls.Add(this.mButtonOpenConfiguration);
            this.mGroupboxConfigSettings.Controls.Add(this.mLabelDirCheck);
            this.mGroupboxConfigSettings.Controls.Add(this.mTextboxConfigDir);
            this.mGroupboxConfigSettings.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.mGroupboxConfigSettings.Location = new System.Drawing.Point(10, 8);
            this.mGroupboxConfigSettings.Name = "mGroupboxConfigSettings";
            this.mGroupboxConfigSettings.Size = new System.Drawing.Size(464, 119);
            this.mGroupboxConfigSettings.TabIndex = 0;
            this.mGroupboxConfigSettings.TabStop = false;
            this.mGroupboxConfigSettings.Text = "Configuration Settings";
            // 
            // mLabelInfoConfig
            // 
            this.mLabelInfoConfig.AutoSize = true;
            this.mLabelInfoConfig.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelInfoConfig.Location = new System.Drawing.Point(6, 23);
            this.mLabelInfoConfig.Name = "mLabelInfoConfig";
            this.mLabelInfoConfig.Size = new System.Drawing.Size(445, 13);
            this.mLabelInfoConfig.TabIndex = 4;
            this.mLabelInfoConfig.Text = "Please make sure the provided demo configuration is located in the directory spec" +
    "ified below!";
            // 
            // mButtonOpenConfiguration
            // 
            this.mButtonOpenConfiguration.Enabled = false;
            this.mButtonOpenConfiguration.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mButtonOpenConfiguration.Location = new System.Drawing.Point(286, 82);
            this.mButtonOpenConfiguration.Name = "mButtonOpenConfiguration";
            this.mButtonOpenConfiguration.Size = new System.Drawing.Size(170, 23);
            this.mButtonOpenConfiguration.TabIndex = 0;
            this.mButtonOpenConfiguration.Text = "Open CANoe configuration";
            this.mButtonOpenConfiguration.UseVisualStyleBackColor = true;
            this.mButtonOpenConfiguration.Click += new System.EventHandler(this.mButtonOpenConfiguration_Click);
            // 
            // mLabelDirCheck
            // 
            this.mLabelDirCheck.AutoSize = true;
            this.mLabelDirCheck.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelDirCheck.Location = new System.Drawing.Point(6, 82);
            this.mLabelDirCheck.Name = "mLabelDirCheck";
            this.mLabelDirCheck.Size = new System.Drawing.Size(221, 26);
            this.mLabelDirCheck.TabIndex = 2;
            this.mLabelDirCheck.Text = "Configuration file found.\r\nClick to open the configuration within CANoe.";
            // 
            // mTextboxConfigDir
            // 
            this.mTextboxConfigDir.Location = new System.Drawing.Point(9, 44);
            this.mTextboxConfigDir.Name = "mTextboxConfigDir";
            this.mTextboxConfigDir.ReadOnly = true;
            this.mTextboxConfigDir.Size = new System.Drawing.Size(447, 20);
            this.mTextboxConfigDir.TabIndex = 1;
            this.mTextboxConfigDir.MouseHover += new System.EventHandler(this.mTextboxConfigDir_MouseHover);
            // 
            // mGroupboxMeasurementControl
            // 
            this.mGroupboxMeasurementControl.Controls.Add(this.mLabelMeasurementStatus);
            this.mGroupboxMeasurementControl.Controls.Add(this.mButtonStartStop);
            this.mGroupboxMeasurementControl.Enabled = false;
            this.mGroupboxMeasurementControl.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.mGroupboxMeasurementControl.Location = new System.Drawing.Point(10, 136);
            this.mGroupboxMeasurementControl.Name = "mGroupboxMeasurementControl";
            this.mGroupboxMeasurementControl.Size = new System.Drawing.Size(464, 63);
            this.mGroupboxMeasurementControl.TabIndex = 1;
            this.mGroupboxMeasurementControl.TabStop = false;
            this.mGroupboxMeasurementControl.Text = "Measurement Control";
            // 
            // mLabelMeasurementStatus
            // 
            this.mLabelMeasurementStatus.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelMeasurementStatus.Location = new System.Drawing.Point(202, 30);
            this.mLabelMeasurementStatus.Name = "mLabelMeasurementStatus";
            this.mLabelMeasurementStatus.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.mLabelMeasurementStatus.Size = new System.Drawing.Size(257, 18);
            this.mLabelMeasurementStatus.TabIndex = 2;
            this.mLabelMeasurementStatus.Text = "Status: No measurement is currently running";
            // 
            // mButtonStartStop
            // 
            this.mButtonStartStop.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mButtonStartStop.Location = new System.Drawing.Point(9, 25);
            this.mButtonStartStop.Name = "mButtonStartStop";
            this.mButtonStartStop.Size = new System.Drawing.Size(171, 23);
            this.mButtonStartStop.TabIndex = 1;
            this.mButtonStartStop.Text = "Start measurement";
            this.mButtonStartStop.UseVisualStyleBackColor = true;
            this.mButtonStartStop.Click += new System.EventHandler(this.mButtonStartStop_Click);
            // 
            // mGroupboxCAPLWriteWindowOut
            // 
            this.mGroupboxCAPLWriteWindowOut.Controls.Add(this.mTextboxOperationResult);
            this.mGroupboxCAPLWriteWindowOut.Controls.Add(this.mTextboxOperand1);
            this.mGroupboxCAPLWriteWindowOut.Controls.Add(this.mLabelEquals);
            this.mGroupboxCAPLWriteWindowOut.Controls.Add(this.mLabelMultiply);
            this.mGroupboxCAPLWriteWindowOut.Controls.Add(this.mLabelInfoCAPLFunc);
            this.mGroupboxCAPLWriteWindowOut.Controls.Add(this.mButtonCalculate);
            this.mGroupboxCAPLWriteWindowOut.Controls.Add(this.mTextboxOperand2);
            this.mGroupboxCAPLWriteWindowOut.Enabled = false;
            this.mGroupboxCAPLWriteWindowOut.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.mGroupboxCAPLWriteWindowOut.Location = new System.Drawing.Point(10, 209);
            this.mGroupboxCAPLWriteWindowOut.Name = "mGroupboxCAPLWriteWindowOut";
            this.mGroupboxCAPLWriteWindowOut.Size = new System.Drawing.Size(464, 94);
            this.mGroupboxCAPLWriteWindowOut.TabIndex = 2;
            this.mGroupboxCAPLWriteWindowOut.TabStop = false;
            this.mGroupboxCAPLWriteWindowOut.Text = "CAPL Function";
            // 
            // mTextboxOperationResult
            // 
            this.mTextboxOperationResult.Location = new System.Drawing.Point(242, 62);
            this.mTextboxOperationResult.Name = "mTextboxOperationResult";
            this.mTextboxOperationResult.ReadOnly = true;
            this.mTextboxOperationResult.Size = new System.Drawing.Size(78, 20);
            this.mTextboxOperationResult.TabIndex = 3;
            this.mTextboxOperationResult.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // mTextboxOperand1
            // 
            this.mTextboxOperand1.Location = new System.Drawing.Point(9, 62);
            this.mTextboxOperand1.Name = "mTextboxOperand1";
            this.mTextboxOperand1.Size = new System.Drawing.Size(78, 20);
            this.mTextboxOperand1.TabIndex = 0;
            this.mTextboxOperand1.Text = "50";
            this.mTextboxOperand1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // mLabelEquals
            // 
            this.mLabelEquals.AutoSize = true;
            this.mLabelEquals.ForeColor = System.Drawing.SystemColors.Desktop;
            this.mLabelEquals.Location = new System.Drawing.Point(213, 66);
            this.mLabelEquals.Name = "mLabelEquals";
            this.mLabelEquals.Size = new System.Drawing.Size(13, 13);
            this.mLabelEquals.TabIndex = 8;
            this.mLabelEquals.Text = "=";
            // 
            // mLabelMultiply
            // 
            this.mLabelMultiply.AutoSize = true;
            this.mLabelMultiply.ForeColor = System.Drawing.SystemColors.Desktop;
            this.mLabelMultiply.Location = new System.Drawing.Point(97, 66);
            this.mLabelMultiply.Name = "mLabelMultiply";
            this.mLabelMultiply.Size = new System.Drawing.Size(11, 13);
            this.mLabelMultiply.TabIndex = 7;
            this.mLabelMultiply.Text = "*";
            // 
            // mLabelInfoCAPLFunc
            // 
            this.mLabelInfoCAPLFunc.AutoSize = true;
            this.mLabelInfoCAPLFunc.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelInfoCAPLFunc.Location = new System.Drawing.Point(6, 22);
            this.mLabelInfoCAPLFunc.Name = "mLabelInfoCAPLFunc";
            this.mLabelInfoCAPLFunc.Size = new System.Drawing.Size(407, 26);
            this.mLabelInfoCAPLFunc.TabIndex = 4;
            this.mLabelInfoCAPLFunc.Text = "Enter multiplication operands and get the result using a parameterized CAPL funct" +
    "ion.\r\nThe function will also make an output of the calculation in CANoe\'s write " +
    "window.";
            // 
            // mButtonCalculate
            // 
            this.mButtonCalculate.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mButtonCalculate.Location = new System.Drawing.Point(366, 60);
            this.mButtonCalculate.Name = "mButtonCalculate";
            this.mButtonCalculate.Size = new System.Drawing.Size(89, 23);
            this.mButtonCalculate.TabIndex = 2;
            this.mButtonCalculate.Text = "Calculate";
            this.mButtonCalculate.UseVisualStyleBackColor = true;
            this.mButtonCalculate.Click += new System.EventHandler(this.mButtonCalculate_Click);
            // 
            // mTextboxOperand2
            // 
            this.mTextboxOperand2.Location = new System.Drawing.Point(121, 63);
            this.mTextboxOperand2.Name = "mTextboxOperand2";
            this.mTextboxOperand2.Size = new System.Drawing.Size(78, 20);
            this.mTextboxOperand2.TabIndex = 1;
            this.mTextboxOperand2.Text = "25";
            this.mTextboxOperand2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // mGroupboxSignalValues
            // 
            this.mGroupboxSignalValues.Controls.Add(this.mLabelEngTempOut);
            this.mGroupboxSignalValues.Controls.Add(this.mProgressbarEngTempOut);
            this.mGroupboxSignalValues.Controls.Add(this.mTextboxEngStatusOut);
            this.mGroupboxSignalValues.Controls.Add(this.mTextboxEngSpeedOut);
            this.mGroupboxSignalValues.Controls.Add(this.mLabelmsCycle);
            this.mGroupboxSignalValues.Controls.Add(this.mNumericupdownCycleTime);
            this.mGroupboxSignalValues.Controls.Add(this.mLabelCycleTime);
            this.mGroupboxSignalValues.Controls.Add(this.mLabelEngTempMax);
            this.mGroupboxSignalValues.Controls.Add(this.mLabelEngTempMin);
            this.mGroupboxSignalValues.Controls.Add(this.mComboboxEngineStatusIn);
            this.mGroupboxSignalValues.Controls.Add(this.mTrackbarEngineTempIn);
            this.mGroupboxSignalValues.Controls.Add(this.mTextboxEngineSpeedIn);
            this.mGroupboxSignalValues.Controls.Add(this.mLabelEngineStatus);
            this.mGroupboxSignalValues.Controls.Add(this.mLabelEngineTemp);
            this.mGroupboxSignalValues.Controls.Add(this.mLabelEngineSpeed);
            this.mGroupboxSignalValues.Controls.Add(this.mLabelInfoSignalValues);
            this.mGroupboxSignalValues.Enabled = false;
            this.mGroupboxSignalValues.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.mGroupboxSignalValues.Location = new System.Drawing.Point(10, 430);
            this.mGroupboxSignalValues.Name = "mGroupboxSignalValues";
            this.mGroupboxSignalValues.Size = new System.Drawing.Size(464, 228);
            this.mGroupboxSignalValues.TabIndex = 3;
            this.mGroupboxSignalValues.TabStop = false;
            this.mGroupboxSignalValues.Text = "Signal Values";
            // 
            // mLabelEngTempOut
            // 
            this.mLabelEngTempOut.AutoSize = true;
            this.mLabelEngTempOut.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelEngTempOut.Location = new System.Drawing.Point(283, 182);
            this.mLabelEngTempOut.Name = "mLabelEngTempOut";
            this.mLabelEngTempOut.Size = new System.Drawing.Size(13, 13);
            this.mLabelEngTempOut.TabIndex = 7;
            this.mLabelEngTempOut.Text = "0";
            // 
            // mProgressbarEngTempOut
            // 
            this.mProgressbarEngTempOut.Location = new System.Drawing.Point(160, 177);
            this.mProgressbarEngTempOut.Name = "mProgressbarEngTempOut";
            this.mProgressbarEngTempOut.Size = new System.Drawing.Size(120, 22);
            this.mProgressbarEngTempOut.TabIndex = 36;
            // 
            // mTextboxEngStatusOut
            // 
            this.mTextboxEngStatusOut.Location = new System.Drawing.Point(160, 147);
            this.mTextboxEngStatusOut.Name = "mTextboxEngStatusOut";
            this.mTextboxEngStatusOut.ReadOnly = true;
            this.mTextboxEngStatusOut.Size = new System.Drawing.Size(120, 20);
            this.mTextboxEngStatusOut.TabIndex = 35;
            this.mTextboxEngStatusOut.Text = "Stopped";
            this.mTextboxEngStatusOut.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // mTextboxEngSpeedOut
            // 
            this.mTextboxEngSpeedOut.Location = new System.Drawing.Point(160, 117);
            this.mTextboxEngSpeedOut.Name = "mTextboxEngSpeedOut";
            this.mTextboxEngSpeedOut.ReadOnly = true;
            this.mTextboxEngSpeedOut.Size = new System.Drawing.Size(120, 20);
            this.mTextboxEngSpeedOut.TabIndex = 32;
            this.mTextboxEngSpeedOut.Text = "0";
            this.mTextboxEngSpeedOut.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // mLabelmsCycle
            // 
            this.mLabelmsCycle.AutoSize = true;
            this.mLabelmsCycle.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelmsCycle.Location = new System.Drawing.Point(221, 78);
            this.mLabelmsCycle.Name = "mLabelmsCycle";
            this.mLabelmsCycle.Size = new System.Drawing.Size(20, 13);
            this.mLabelmsCycle.TabIndex = 31;
            this.mLabelmsCycle.Text = "ms";
            // 
            // mNumericupdownCycleTime
            // 
            this.mNumericupdownCycleTime.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.mNumericupdownCycleTime.Location = new System.Drawing.Point(160, 76);
            this.mNumericupdownCycleTime.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.mNumericupdownCycleTime.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.mNumericupdownCycleTime.Name = "mNumericupdownCycleTime";
            this.mNumericupdownCycleTime.Size = new System.Drawing.Size(55, 20);
            this.mNumericupdownCycleTime.TabIndex = 8;
            this.mNumericupdownCycleTime.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.mNumericupdownCycleTime.ValueChanged += new System.EventHandler(this.mNumericupdownCycleTime_ValueChanged);
            // 
            // mLabelCycleTime
            // 
            this.mLabelCycleTime.AutoSize = true;
            this.mLabelCycleTime.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelCycleTime.Location = new System.Drawing.Point(6, 78);
            this.mLabelCycleTime.Name = "mLabelCycleTime";
            this.mLabelCycleTime.Size = new System.Drawing.Size(95, 13);
            this.mLabelCycleTime.TabIndex = 29;
            this.mLabelCycleTime.Text = "Update cycle time:\r\n";
            // 
            // mLabelEngTempMax
            // 
            this.mLabelEngTempMax.AutoSize = true;
            this.mLabelEngTempMax.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelEngTempMax.Location = new System.Drawing.Point(435, 204);
            this.mLabelEngTempMax.Name = "mLabelEngTempMax";
            this.mLabelEngTempMax.Size = new System.Drawing.Size(25, 13);
            this.mLabelEngTempMax.TabIndex = 27;
            this.mLabelEngTempMax.Text = "100";
            // 
            // mLabelEngTempMin
            // 
            this.mLabelEngTempMin.AutoSize = true;
            this.mLabelEngTempMin.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelEngTempMin.Location = new System.Drawing.Point(337, 204);
            this.mLabelEngTempMin.Name = "mLabelEngTempMin";
            this.mLabelEngTempMin.Size = new System.Drawing.Size(13, 13);
            this.mLabelEngTempMin.TabIndex = 7;
            this.mLabelEngTempMin.Text = "0";
            // 
            // mComboboxEngineStatusIn
            // 
            this.mComboboxEngineStatusIn.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
            this.mComboboxEngineStatusIn.FormattingEnabled = true;
            this.mComboboxEngineStatusIn.Items.AddRange(new object[] {
            "Stopped",
            "Running"});
            this.mComboboxEngineStatusIn.Location = new System.Drawing.Point(338, 147);
            this.mComboboxEngineStatusIn.Name = "mComboboxEngineStatusIn";
            this.mComboboxEngineStatusIn.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.mComboboxEngineStatusIn.Size = new System.Drawing.Size(118, 21);
            this.mComboboxEngineStatusIn.TabIndex = 10;
            this.mComboboxEngineStatusIn.Text = "Stopped";
            this.mComboboxEngineStatusIn.SelectedIndexChanged += new System.EventHandler(this.mComboboxEngStatusIn_SelectedIndexChanged);
            // 
            // mTrackbarEngineTempIn
            // 
            this.mTrackbarEngineTempIn.LargeChange = 10;
            this.mTrackbarEngineTempIn.Location = new System.Drawing.Point(331, 172);
            this.mTrackbarEngineTempIn.Maximum = 100;
            this.mTrackbarEngineTempIn.Name = "mTrackbarEngineTempIn";
            this.mTrackbarEngineTempIn.Size = new System.Drawing.Size(132, 45);
            this.mTrackbarEngineTempIn.TabIndex = 11;
            this.mTrackbarEngineTempIn.TickFrequency = 10;
            this.mTrackbarEngineTempIn.Scroll += new System.EventHandler(this.mTrackbarEngTempIn_Scroll);
            // 
            // mTextboxEngineSpeedIn
            // 
            this.mTextboxEngineSpeedIn.Location = new System.Drawing.Point(338, 117);
            this.mTextboxEngineSpeedIn.Name = "mTextboxEngineSpeedIn";
            this.mTextboxEngineSpeedIn.Size = new System.Drawing.Size(118, 20);
            this.mTextboxEngineSpeedIn.TabIndex = 9;
            this.mTextboxEngineSpeedIn.Text = "0";
            this.mTextboxEngineSpeedIn.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.mTextboxEngineSpeedIn.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.mTextboxEngSpeedIn_KeyPress);
            this.mTextboxEngineSpeedIn.Leave += new System.EventHandler(this.mTextboxEngSpeedIn_Leave);
            // 
            // mLabelEngineStatus
            // 
            this.mLabelEngineStatus.AutoSize = true;
            this.mLabelEngineStatus.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelEngineStatus.Location = new System.Drawing.Point(6, 150);
            this.mLabelEngineStatus.Name = "mLabelEngineStatus";
            this.mLabelEngineStatus.Size = new System.Drawing.Size(70, 13);
            this.mLabelEngineStatus.TabIndex = 14;
            this.mLabelEngineStatus.Text = "EngineStatus";
            // 
            // mLabelEngineTemp
            // 
            this.mLabelEngineTemp.AutoSize = true;
            this.mLabelEngineTemp.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelEngineTemp.Location = new System.Drawing.Point(6, 180);
            this.mLabelEngineTemp.Name = "mLabelEngineTemp";
            this.mLabelEngineTemp.Size = new System.Drawing.Size(101, 13);
            this.mLabelEngineTemp.TabIndex = 12;
            this.mLabelEngineTemp.Text = "EngineTemp (degC)";
            // 
            // mLabelEngineSpeed
            // 
            this.mLabelEngineSpeed.AutoSize = true;
            this.mLabelEngineSpeed.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelEngineSpeed.Location = new System.Drawing.Point(6, 120);
            this.mLabelEngineSpeed.Name = "mLabelEngineSpeed";
            this.mLabelEngineSpeed.Size = new System.Drawing.Size(104, 13);
            this.mLabelEngineSpeed.TabIndex = 11;
            this.mLabelEngineSpeed.Text = "EngineSpeed (RPM)";
            // 
            // mLabelInfoSignalValues
            // 
            this.mLabelInfoSignalValues.AutoSize = true;
            this.mLabelInfoSignalValues.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelInfoSignalValues.Location = new System.Drawing.Point(6, 23);
            this.mLabelInfoSignalValues.Name = "mLabelInfoSignalValues";
            this.mLabelInfoSignalValues.Size = new System.Drawing.Size(449, 39);
            this.mLabelInfoSignalValues.TabIndex = 0;
            this.mLabelInfoSignalValues.Text = resources.GetString("mLabelInfoSignalValues.Text");
            // 
            // mGroupboxSystemVariables
            // 
            this.mGroupboxSystemVariables.Controls.Add(this.mLabelSysVarsValue);
            this.mGroupboxSystemVariables.Controls.Add(this.mLabelSysVarMax);
            this.mGroupboxSystemVariables.Controls.Add(this.mComboboxSysVars);
            this.mGroupboxSystemVariables.Controls.Add(this.mProgressbarSysVars);
            this.mGroupboxSystemVariables.Controls.Add(this.mLabelSysVarMin);
            this.mGroupboxSystemVariables.Controls.Add(this.mLabelInfoSysVars);
            this.mGroupboxSystemVariables.Controls.Add(this.mTrackbarSysVars);
            this.mGroupboxSystemVariables.Enabled = false;
            this.mGroupboxSystemVariables.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.mGroupboxSystemVariables.Location = new System.Drawing.Point(10, 313);
            this.mGroupboxSystemVariables.Name = "mGroupboxSystemVariables";
            this.mGroupboxSystemVariables.Size = new System.Drawing.Size(464, 108);
            this.mGroupboxSystemVariables.TabIndex = 4;
            this.mGroupboxSystemVariables.TabStop = false;
            this.mGroupboxSystemVariables.Text = "System Variables";
            // 
            // mLabelSysVarsValue
            // 
            this.mLabelSysVarsValue.AutoSize = true;
            this.mLabelSysVarsValue.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelSysVarsValue.Location = new System.Drawing.Point(283, 73);
            this.mLabelSysVarsValue.Name = "mLabelSysVarsValue";
            this.mLabelSysVarsValue.Size = new System.Drawing.Size(13, 13);
            this.mLabelSysVarsValue.TabIndex = 6;
            this.mLabelSysVarsValue.Text = "0";
            // 
            // mLabelSysVarMax
            // 
            this.mLabelSysVarMax.AutoSize = true;
            this.mLabelSysVarMax.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelSysVarMax.Location = new System.Drawing.Point(435, 89);
            this.mLabelSysVarMax.Name = "mLabelSysVarMax";
            this.mLabelSysVarMax.Size = new System.Drawing.Size(25, 13);
            this.mLabelSysVarMax.TabIndex = 5;
            this.mLabelSysVarMax.Text = "255";
            // 
            // mComboboxSysVars
            // 
            this.mComboboxSysVars.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
            this.mComboboxSysVars.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mComboboxSysVars.FormattingEnabled = true;
            this.mComboboxSysVars.Items.AddRange(new object[] {
            "SysVar1",
            "SysVar2"});
            this.mComboboxSysVars.Location = new System.Drawing.Point(9, 69);
            this.mComboboxSysVars.Name = "mComboboxSysVars";
            this.mComboboxSysVars.Size = new System.Drawing.Size(104, 21);
            this.mComboboxSysVars.TabIndex = 6;
            this.mComboboxSysVars.Text = "SysVar1";
            this.mComboboxSysVars.SelectedIndexChanged += new System.EventHandler(this.mComboboxSysVars_SelectedIndexChanged);
            // 
            // mProgressbarSysVars
            // 
            this.mProgressbarSysVars.Location = new System.Drawing.Point(160, 68);
            this.mProgressbarSysVars.Maximum = 255;
            this.mProgressbarSysVars.Name = "mProgressbarSysVars";
            this.mProgressbarSysVars.Size = new System.Drawing.Size(120, 22);
            this.mProgressbarSysVars.Step = 15;
            this.mProgressbarSysVars.TabIndex = 2;
            // 
            // mLabelSysVarMin
            // 
            this.mLabelSysVarMin.AutoSize = true;
            this.mLabelSysVarMin.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelSysVarMin.Location = new System.Drawing.Point(337, 89);
            this.mLabelSysVarMin.Name = "mLabelSysVarMin";
            this.mLabelSysVarMin.Size = new System.Drawing.Size(13, 13);
            this.mLabelSysVarMin.TabIndex = 4;
            this.mLabelSysVarMin.Text = "0";
            // 
            // mLabelInfoSysVars
            // 
            this.mLabelInfoSysVars.AutoSize = true;
            this.mLabelInfoSysVars.ForeColor = System.Drawing.SystemColors.ControlText;
            this.mLabelInfoSysVars.Location = new System.Drawing.Point(6, 23);
            this.mLabelInfoSysVars.Name = "mLabelInfoSysVars";
            this.mLabelInfoSysVars.Size = new System.Drawing.Size(452, 26);
            this.mLabelInfoSysVars.TabIndex = 0;
            this.mLabelInfoSysVars.Text = "Choose a system variable and read its current value out of the progress bar. Writ" +
    "e a new value\r\nby using the slider. Live update of the system variable value is " +
    "provided by COM server events.";
            // 
            // mTrackbarSysVars
            // 
            this.mTrackbarSysVars.LargeChange = 15;
            this.mTrackbarSysVars.Location = new System.Drawing.Point(331, 57);
            this.mTrackbarSysVars.Maximum = 255;
            this.mTrackbarSysVars.Name = "mTrackbarSysVars";
            this.mTrackbarSysVars.Size = new System.Drawing.Size(132, 45);
            this.mTrackbarSysVars.SmallChange = 5;
            this.mTrackbarSysVars.TabIndex = 7;
            this.mTrackbarSysVars.TickFrequency = 15;
            this.mTrackbarSysVars.Scroll += new System.EventHandler(this.mTrackbarSysVars_Scroll);
            // 
            // Form_Csharp_CANoe
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(485, 665);
            this.Controls.Add(this.mGroupboxSystemVariables);
            this.Controls.Add(this.mGroupboxSignalValues);
            this.Controls.Add(this.mGroupboxCAPLWriteWindowOut);
            this.Controls.Add(this.mGroupboxMeasurementControl);
            this.Controls.Add(this.mGroupboxConfigSettings);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "Form_Csharp_CANoe";
            this.Text = "CANoe as a COM Server - C# Sample Application";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form_Csharp_CANoe_FormClosing);
            this.mGroupboxConfigSettings.ResumeLayout(false);
            this.mGroupboxConfigSettings.PerformLayout();
            this.mGroupboxMeasurementControl.ResumeLayout(false);
            this.mGroupboxCAPLWriteWindowOut.ResumeLayout(false);
            this.mGroupboxCAPLWriteWindowOut.PerformLayout();
            this.mGroupboxSignalValues.ResumeLayout(false);
            this.mGroupboxSignalValues.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.mNumericupdownCycleTime)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.mTrackbarEngineTempIn)).EndInit();
            this.mGroupboxSystemVariables.ResumeLayout(false);
            this.mGroupboxSystemVariables.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.mTrackbarSysVars)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox mGroupboxConfigSettings;
        private System.Windows.Forms.TextBox mTextboxConfigDir;
        private System.Windows.Forms.Button mButtonOpenConfiguration;
        private System.Windows.Forms.Label mLabelDirCheck;
        private System.Windows.Forms.GroupBox mGroupboxMeasurementControl;
        private System.Windows.Forms.Button mButtonStartStop;
        private System.Windows.Forms.GroupBox mGroupboxCAPLWriteWindowOut;
        private System.Windows.Forms.Button mButtonCalculate;
        private System.Windows.Forms.TextBox mTextboxOperand2;
        private System.Windows.Forms.GroupBox mGroupboxSignalValues;
        private System.Windows.Forms.Label mLabelInfoSignalValues;
        private System.Windows.Forms.GroupBox mGroupboxSystemVariables;
        private System.Windows.Forms.Label mLabelInfoSysVars;
        private System.Windows.Forms.ComboBox mComboboxSysVars;
        private System.Windows.Forms.ProgressBar mProgressbarSysVars;
        private System.Windows.Forms.TrackBar mTrackbarSysVars;
        private System.Windows.Forms.Label mLabelSysVarMax;
        private System.Windows.Forms.Label mLabelSysVarMin;
        private System.Windows.Forms.Label mLabelMeasurementStatus;
        private System.Windows.Forms.Label mLabelInfoConfig;
        private System.Windows.Forms.Label mLabelInfoCAPLFunc;
        private System.Windows.Forms.Label mLabelEngineStatus;
        private System.Windows.Forms.Label mLabelEngineTemp;
        private System.Windows.Forms.Label mLabelEngineSpeed;
        private System.Windows.Forms.Label mLabelSysVarsValue;
        private System.Windows.Forms.TextBox mTextboxEngineSpeedIn;
        private System.Windows.Forms.ComboBox mComboboxEngineStatusIn;
        private System.Windows.Forms.TrackBar mTrackbarEngineTempIn;
        private System.Windows.Forms.Label mLabelEngTempMax;
        private System.Windows.Forms.Label mLabelEngTempMin;
        private System.Windows.Forms.Label mLabelmsCycle;
        private System.Windows.Forms.NumericUpDown mNumericupdownCycleTime;
        private System.Windows.Forms.Label mLabelCycleTime;
        private System.Windows.Forms.TextBox mTextboxEngStatusOut;
        private System.Windows.Forms.TextBox mTextboxEngSpeedOut;
        private System.Windows.Forms.ProgressBar mProgressbarEngTempOut;
        private System.Windows.Forms.Label mLabelEngTempOut;
        private System.Windows.Forms.Timer mTimerPolling;
        private System.Windows.Forms.TextBox mTextboxOperationResult;
        private System.Windows.Forms.TextBox mTextboxOperand1;
        private System.Windows.Forms.Label mLabelEquals;
        private System.Windows.Forms.Label mLabelMultiply;
    }
}

