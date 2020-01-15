using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;

namespace CSharpPanels
{
    public class CSharpControl : System.Windows.Forms.UserControl
    {
        // Declare types
        private CANoe.Application         App;
        private CANoe.Measurement         Measurement;
        private CANoe.Variable            svMotorSwitch = null;
        private CANoe.Variable            svTurnSignal = null;
        private CANoe.Variable            svEngineSpeedEntry = null;
        private System.Windows.Forms.Timer tmrSignals;
                
        private System.Windows.Forms.GroupBox fraMotor;
        private System.Windows.Forms.GroupBox fraLight;
        private System.Windows.Forms.Label lblDriving;
        private System.Windows.Forms.Label lblTurn;
        private System.Windows.Forms.Button btnLightSwitch;
        private System.Windows.Forms.Button btnTurnSignal;
        private System.Windows.Forms.GroupBox fraEntry;
        private System.Windows.Forms.TrackBar sliSpeedEntry;
        private System.Windows.Forms.Button btnMotorSwitch;
        private System.Windows.Forms.Label lblCaption;
        
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public CSharpControl()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            // Initialize timer to get Signal changes
            tmrSignals = new System.Windows.Forms.Timer(components);
            if (tmrSignals != null)
            {
                tmrSignals.Enabled = true;
                tmrSignals.Tick += new System.EventHandler(tmrSignals_Tick);
            }

            // Initialize types
            App            = new CANoe.Application();
            Measurement    = (CANoe.Measurement)App.Measurement;
            CANoe.System system = (CANoe.System)App.System;

            if (system != null)
            {
                CANoe.Namespaces namespaces = (CANoe.Namespaces)system.Namespaces;
                if (namespaces != null)
                {
                    CANoe.Namespace nsEngine = (CANoe.Namespace)namespaces["Engine"];
                    if (nsEngine != null)
                    {
                        CANoe.Variables engineVars = (CANoe.Variables)nsEngine.Variables;
                        if (engineVars != null)
                        {
                            svMotorSwitch = (CANoe.Variable)engineVars["MotorSwitch"];
                            svEngineSpeedEntry = (CANoe.Variable)engineVars["EngineSpeedEntry"];

                            if (svMotorSwitch != null)
                                svMotorSwitch.OnChange += new CANoe._IVariableEvents_OnChangeEventHandler(svMotorSwitch_OnChange);
                            if (svEngineSpeedEntry != null)
                                svEngineSpeedEntry.OnChange += new CANoe._IVariableEvents_OnChangeEventHandler(svEngineSpeedEntry_OnChange);
                        }
                    }
                    CANoe.Namespace nsLights = (CANoe.Namespace)namespaces["Lights"];
                    if (nsLights != null)
                    {
                        CANoe.Variables lightsVars = (CANoe.Variables)nsLights.Variables;
                        if (lightsVars != null)
                        {
                            svTurnSignal = (CANoe.Variable)lightsVars["TurnSignal"];

                            if (svTurnSignal != null)
                                svTurnSignal.OnChange += new CANoe._IVariableEvents_OnChangeEventHandler(svTurnSignal_OnChange);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if( components != null )
                    components.Dispose();

                if (tmrSignals != null)
                    tmrSignals.Dispose();
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
          this.fraMotor = new System.Windows.Forms.GroupBox();
          this.btnMotorSwitch = new System.Windows.Forms.Button();
          this.fraLight = new System.Windows.Forms.GroupBox();
          this.btnTurnSignal = new System.Windows.Forms.Button();
          this.btnLightSwitch = new System.Windows.Forms.Button();
          this.lblTurn = new System.Windows.Forms.Label();
          this.lblDriving = new System.Windows.Forms.Label();
          this.fraEntry = new System.Windows.Forms.GroupBox();
          this.sliSpeedEntry = new System.Windows.Forms.TrackBar();
          this.lblCaption = new System.Windows.Forms.Label();
          this.fraMotor.SuspendLayout();
          this.fraLight.SuspendLayout();
          this.fraEntry.SuspendLayout();
          ((System.ComponentModel.ISupportInitialize)(this.sliSpeedEntry)).BeginInit();
          this.SuspendLayout();
          // 
          // fraMotor
          // 
          this.fraMotor.Controls.Add(this.btnMotorSwitch);
          this.fraMotor.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.fraMotor.Location = new System.Drawing.Point(8, 32);
          this.fraMotor.Name = "fraMotor";
          this.fraMotor.Size = new System.Drawing.Size(104, 72);
          this.fraMotor.TabIndex = 0;
          this.fraMotor.TabStop = false;
          this.fraMotor.Text = "Motor Switch";
          // 
          // btnMotorSwitch
          // 
          this.btnMotorSwitch.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.btnMotorSwitch.Location = new System.Drawing.Point(24, 32);
          this.btnMotorSwitch.Name = "btnMotorSwitch";
          this.btnMotorSwitch.Size = new System.Drawing.Size(56, 25);
          this.btnMotorSwitch.TabIndex = 0;
          this.btnMotorSwitch.Text = "Off";
          this.btnMotorSwitch.Click += new System.EventHandler(this.btnMotorSwitch_Click);
          // 
          // fraLight
          // 
          this.fraLight.Controls.Add(this.btnTurnSignal);
          this.fraLight.Controls.Add(this.btnLightSwitch);
          this.fraLight.Controls.Add(this.lblTurn);
          this.fraLight.Controls.Add(this.lblDriving);
          this.fraLight.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.fraLight.Location = new System.Drawing.Point(120, 32);
          this.fraLight.Name = "fraLight";
          this.fraLight.Size = new System.Drawing.Size(136, 72);
          this.fraLight.TabIndex = 1;
          this.fraLight.TabStop = false;
          this.fraLight.Text = "Light Switch";
          // 
          // btnTurnSignal
          // 
          this.btnTurnSignal.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.btnTurnSignal.Location = new System.Drawing.Point(78, 38);
          this.btnTurnSignal.Name = "btnTurnSignal";
          this.btnTurnSignal.Size = new System.Drawing.Size(40, 25);
          this.btnTurnSignal.TabIndex = 3;
          this.btnTurnSignal.Text = "Off";
          this.btnTurnSignal.Click += new System.EventHandler(this.btnTurnSignal_Click);
          // 
          // btnLightSwitch
          // 
          this.btnLightSwitch.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.btnLightSwitch.Location = new System.Drawing.Point(14, 38);
          this.btnLightSwitch.Name = "btnLightSwitch";
          this.btnLightSwitch.Size = new System.Drawing.Size(40, 25);
          this.btnLightSwitch.TabIndex = 2;
          this.btnLightSwitch.Text = "Off";
          this.btnLightSwitch.Click += new System.EventHandler(this.btnLightSwitch_Click);
          // 
          // lblTurn
          // 
          this.lblTurn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.lblTurn.Location = new System.Drawing.Point(66, 20);
          this.lblTurn.Name = "lblTurn";
          this.lblTurn.Size = new System.Drawing.Size(66, 16);
          this.lblTurn.TabIndex = 1;
          this.lblTurn.Text = "Turn signal";
          // 
          // lblDriving
          // 
          this.lblDriving.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.lblDriving.Location = new System.Drawing.Point(14, 20);
          this.lblDriving.Name = "lblDriving";
          this.lblDriving.Size = new System.Drawing.Size(48, 16);
          this.lblDriving.TabIndex = 0;
          this.lblDriving.Text = "Driving";
          // 
          // fraEntry
          // 
          this.fraEntry.Controls.Add(this.sliSpeedEntry);
          this.fraEntry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.fraEntry.Location = new System.Drawing.Point(8, 112);
          this.fraEntry.Name = "fraEntry";
          this.fraEntry.Size = new System.Drawing.Size(248, 72);
          this.fraEntry.TabIndex = 2;
          this.fraEntry.TabStop = false;
          this.fraEntry.Text = "Motor Entry";
          // 
          // sliSpeedEntry
          // 
          this.sliSpeedEntry.Location = new System.Drawing.Point(16, 24);
          this.sliSpeedEntry.Maximum = 3500;
          this.sliSpeedEntry.Name = "sliSpeedEntry";
          this.sliSpeedEntry.Size = new System.Drawing.Size(216, 45);
          this.sliSpeedEntry.TabIndex = 0;
          this.sliSpeedEntry.TickFrequency = 350;
          this.sliSpeedEntry.Scroll += new System.EventHandler(this.sliSpeedEntry_Scroll);
          // 
          // lblCaption
          // 
          this.lblCaption.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.lblCaption.Location = new System.Drawing.Point(8, 8);
          this.lblCaption.Name = "lblCaption";
          this.lblCaption.Size = new System.Drawing.Size(256, 16);
          this.lblCaption.TabIndex = 3;
          this.lblCaption.Text = "C# Control";
          this.lblCaption.TextAlign = System.Drawing.ContentAlignment.TopCenter;
          // 
          // CSharpControl
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.Controls.Add(this.lblCaption);
          this.Controls.Add(this.fraEntry);
          this.Controls.Add(this.fraLight);
          this.Controls.Add(this.fraMotor);
          this.Name = "CSharpControl";
          this.Size = new System.Drawing.Size(264, 192);
          this.fraMotor.ResumeLayout(false);
          this.fraLight.ResumeLayout(false);
          this.fraEntry.ResumeLayout(false);
          this.fraEntry.PerformLayout();
          ((System.ComponentModel.ISupportInitialize)(this.sliSpeedEntry)).EndInit();
          this.ResumeLayout(false);

        }
        #endregion

        private void btnMotorSwitch_Click(object sender, System.EventArgs e)
        {
            if (Measurement.Running)
            {
                if (svMotorSwitch != null)
                {
                    if (btnMotorSwitch.Text == "Off")
                    {
                        svMotorSwitch.Value = 1;
                        btnMotorSwitch.Text = "On";
                    }
                    else
                    {
                        svMotorSwitch.Value = 0;
                        btnMotorSwitch.Text = "Off";
                    }
                }
            }
        }

        private void svMotorSwitch_OnChange(object Value)
        {
            int tmp = (int)Value;
            if (tmp == 0)
            {
                btnMotorSwitch.Text = "Off";
            }
            else
            {
                btnMotorSwitch.Text = "On";
            }
        }

        private void btnLightSwitch_Click(object sender, System.EventArgs e)
        {
            if (Measurement.Running)
            {
                CANoe.Bus bus = null;
                try
                {
                    bus = (CANoe.Bus)App.get_Bus("CAN");
                    if (bus != null)
                    {
                        CANoe.Signal signal = (CANoe.Signal)bus.GetSignal(1, "LightState", "OnOff");
                        if (signal != null)
                        {
                            if (btnLightSwitch.Text == "Off")
                            {
                                signal.Value = 1;
                                btnLightSwitch.Text = "On";
                            }
                            else
                            {
                                signal.Value = 0;
                                btnLightSwitch.Text = "Off";
                            }
                        }
                    }
                }
                catch
                {
                }
            }
        }
   
        private void tmrSignals_Tick(object sender, System.EventArgs e)
        {
            // get all the signal values and actualize controls
            if (Measurement.Running)
            {
                CANoe.Bus bus = null;
                try
                {
                    bus = (CANoe.Bus)App.get_Bus("CAN");
                    if (bus != null)
                    {
                        CANoe.Signal signal = (CANoe.Signal)bus.GetSignal(1, "LightState", "OnOff");
                        if (signal != null)
                        {
                            if (signal.Value == 0)
                                btnLightSwitch.Text = "Off";
                            else
                                btnLightSwitch.Text = "On";
                        }
                    }
                }
                catch
                {
                }
            }
        }

        private void btnTurnSignal_Click(object sender, System.EventArgs e)
        {
            if (Measurement.Running)
            {
                if (btnTurnSignal.Text == "Off") 
                {
                    if (svTurnSignal != null)
                    {
                        svTurnSignal.Value = 1;
                        btnTurnSignal.Text = "On";
                    }
                }
                else
                {
                    if (svTurnSignal != null)
                    {
                        svTurnSignal.Value = 0;
                        btnTurnSignal.Text = "Off";
                    }
                }
            }
        }

        private void svTurnSignal_OnChange(object Value)
        {
            int tmp = (int)Value;
            if (tmp == 0)
            {
                btnTurnSignal.Text = "Off";
            }
            else
            {
                btnTurnSignal.Text = "On";
            }
        }

        private void sliSpeedEntry_Scroll(object sender, System.EventArgs e)
        {
            if(Measurement.Running)
            {
                if (svEngineSpeedEntry != null)
                {
                    double tmp = sliSpeedEntry.Value;
                    svEngineSpeedEntry.Value = tmp;
                }
            }
        }

        private void svEngineSpeedEntry_OnChange(object Value)
        {
            sliSpeedEntry.Value = (int)((double)Value);
        }
    }
}
