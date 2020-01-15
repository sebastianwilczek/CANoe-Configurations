using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace CSharpPanels
{
    public class CSharpDisplay : System.Windows.Forms.UserControl
    {
        // Declare types
        private CANoe.Application App;
      private CANoe.Measurement Measurement;
      internal GroupBox fraEngineSpeed;
      internal GroupBox fraMotor;
      internal GroupBox fraLight;
      internal Label lblEngine;
      internal Label lblRunning;
      internal Label lblDriving;
      internal Label lblTurn;
      internal Label lblDrivingState;
      internal Label lblTurnState;
      private System.Windows.Forms.Timer tmrSignals;
      internal Label lblCaption;
        private System.ComponentModel.IContainer components;

        public CSharpDisplay()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            // Initialize types
            App         = new CANoe.Application();
            Measurement = (CANoe.Measurement)App.Measurement;
        }

        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
          this.components = new System.ComponentModel.Container();
          this.fraEngineSpeed = new System.Windows.Forms.GroupBox();
          this.lblEngine = new System.Windows.Forms.Label();
          this.fraMotor = new System.Windows.Forms.GroupBox();
          this.lblRunning = new System.Windows.Forms.Label();
          this.fraLight = new System.Windows.Forms.GroupBox();
          this.lblTurnState = new System.Windows.Forms.Label();
          this.lblDrivingState = new System.Windows.Forms.Label();
          this.lblTurn = new System.Windows.Forms.Label();
          this.lblDriving = new System.Windows.Forms.Label();
          this.tmrSignals = new System.Windows.Forms.Timer(this.components);
          this.lblCaption = new System.Windows.Forms.Label();
          this.fraEngineSpeed.SuspendLayout();
          this.fraMotor.SuspendLayout();
          this.fraLight.SuspendLayout();
          this.SuspendLayout();
          // 
          // fraEngineSpeed
          // 
          this.fraEngineSpeed.Controls.Add(this.lblEngine);
          this.fraEngineSpeed.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.fraEngineSpeed.Location = new System.Drawing.Point(6, 28);
          this.fraEngineSpeed.Name = "fraEngineSpeed";
          this.fraEngineSpeed.Size = new System.Drawing.Size(111, 42);
          this.fraEngineSpeed.TabIndex = 0;
          this.fraEngineSpeed.TabStop = false;
          this.fraEngineSpeed.Text = "Engine Speed";
          // 
          // lblEngine
          // 
          this.lblEngine.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.lblEngine.Location = new System.Drawing.Point(6, 15);
          this.lblEngine.Name = "lblEngine";
          this.lblEngine.Size = new System.Drawing.Size(101, 21);
          this.lblEngine.TabIndex = 0;
          this.lblEngine.Text = "0";
          this.lblEngine.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
          // 
          // fraMotor
          // 
          this.fraMotor.Controls.Add(this.lblRunning);
          this.fraMotor.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.fraMotor.Location = new System.Drawing.Point(121, 28);
          this.fraMotor.Name = "fraMotor";
          this.fraMotor.Size = new System.Drawing.Size(99, 42);
          this.fraMotor.TabIndex = 1;
          this.fraMotor.TabStop = false;
          this.fraMotor.Text = "Motor State";
          // 
          // lblRunning
          // 
          this.lblRunning.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.lblRunning.Location = new System.Drawing.Point(6, 15);
          this.lblRunning.Name = "lblRunning";
          this.lblRunning.Size = new System.Drawing.Size(89, 21);
          this.lblRunning.TabIndex = 0;
          this.lblRunning.Text = "Not Running";
          this.lblRunning.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
          // 
          // fraLight
          // 
          this.fraLight.Controls.Add(this.lblTurnState);
          this.fraLight.Controls.Add(this.lblDrivingState);
          this.fraLight.Controls.Add(this.lblTurn);
          this.fraLight.Controls.Add(this.lblDriving);
          this.fraLight.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.fraLight.Location = new System.Drawing.Point(6, 76);
          this.fraLight.Name = "fraLight";
          this.fraLight.Size = new System.Drawing.Size(214, 42);
          this.fraLight.TabIndex = 2;
          this.fraLight.TabStop = false;
          this.fraLight.Text = "Driving Light";
          // 
          // lblTurnState
          // 
          this.lblTurnState.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.lblTurnState.Location = new System.Drawing.Point(184, 18);
          this.lblTurnState.Name = "lblTurnState";
          this.lblTurnState.Size = new System.Drawing.Size(26, 16);
          this.lblTurnState.TabIndex = 3;
          this.lblTurnState.Text = "Off";
          this.lblTurnState.TextAlign = System.Drawing.ContentAlignment.TopRight;
          // 
          // lblDrivingState
          // 
          this.lblDrivingState.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.lblDrivingState.Location = new System.Drawing.Point(80, 18);
          this.lblDrivingState.Name = "lblDrivingState";
          this.lblDrivingState.Size = new System.Drawing.Size(26, 16);
          this.lblDrivingState.TabIndex = 2;
          this.lblDrivingState.Text = "Off";
          this.lblDrivingState.TextAlign = System.Drawing.ContentAlignment.TopRight;
          // 
          // lblTurn
          // 
          this.lblTurn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.lblTurn.Location = new System.Drawing.Point(110, 18);
          this.lblTurn.Name = "lblTurn";
          this.lblTurn.Size = new System.Drawing.Size(76, 21);
          this.lblTurn.TabIndex = 1;
          this.lblTurn.Text = "Turn signal:";
          // 
          // lblDriving
          // 
          this.lblDriving.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.lblDriving.Location = new System.Drawing.Point(12, 18);
          this.lblDriving.Name = "lblDriving";
          this.lblDriving.Size = new System.Drawing.Size(63, 22);
          this.lblDriving.TabIndex = 0;
          this.lblDriving.Text = "Driving:";
          // 
          // tmrSignals
          // 
          this.tmrSignals.Enabled = true;
          this.tmrSignals.Tick += new System.EventHandler(this.tmrSignals_Tick);
          // 
          // lblCaption
          // 
          this.lblCaption.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold);
          this.lblCaption.Location = new System.Drawing.Point(6, 7);
          this.lblCaption.Name = "lblCaption";
          this.lblCaption.Size = new System.Drawing.Size(214, 18);
          this.lblCaption.TabIndex = 3;
          this.lblCaption.Text = "C# Display";
          this.lblCaption.TextAlign = System.Drawing.ContentAlignment.TopCenter;
          // 
          // CSharpDisplay
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.Controls.Add(this.lblCaption);
          this.Controls.Add(this.fraLight);
          this.Controls.Add(this.fraMotor);
          this.Controls.Add(this.fraEngineSpeed);
          this.Name = "CSharpDisplay";
          this.Size = new System.Drawing.Size(226, 127);
          this.fraEngineSpeed.ResumeLayout(false);
          this.fraMotor.ResumeLayout(false);
          this.fraLight.ResumeLayout(false);
          this.ResumeLayout(false);

        }
        #endregion

        private void tmrSignals_Tick(object sender, System.EventArgs e)
        {
            // get all the signal values and actualize controls
            if(Measurement.Running)
            {
                CANoe.Configuration configuration     = null;
                CANoe.SimulationSetup simulationSetup = null; 
                CANoe.Nodes nodes                     = null;
                CANoe.Node node                       = null;
                CANoe.Signals motorInputs             = null;
                CANoe.Signals motorControlInputs      = null;
                try
                {
                    configuration   = (CANoe.Configuration)App.Configuration;
                    simulationSetup = (CANoe.SimulationSetup)configuration.SimulationSetup;
                    nodes           = (CANoe.Nodes)simulationSetup.Nodes;
                    node            = (CANoe.Node)nodes["Motor"];
                    motorInputs     = (CANoe.Signals)node.Inputs;
                
                    if (motorInputs["OnOff"].Value == 0)
                    {
                        lblRunning.Text = "Not Running";
                    }
                    else
                    {
                        lblRunning.Text = "Running";
                    }

                    lblEngine.Text = motorInputs["EngineSpeed"].Value.ToString();
        
                    motorInputs = null;
                
                    node = (CANoe.Node)nodes["MotorControl"];
                    motorControlInputs = (CANoe.Signals)node.Inputs;
        
                    if(motorControlInputs["OnOff"].Value == 0)
                    {
                        lblDrivingState.Text = "Off";
                    }
                    else
                    {
                        lblDrivingState.Text = "On";
                    }

                    if(motorControlInputs["TurnSignal"].Value == 0)
                    {
                        lblTurnState.Text = "Off";
                    }
                    else
                    {
                        lblTurnState.Text = "On";
                    }
                }
                catch
                {
                }
                finally
                {
                    motorControlInputs = null;
                    node = null;
                    nodes = null;
                    simulationSetup = null;
                    configuration = null;
                }
            }
        }
    }
}